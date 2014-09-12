#include "qleveldb.h"
#include <QFileInfo>

#include <leveldb/db.h>
#include <leveldb/options.h>
#include <QJsonDocument>

QMap<QString, QWeakPointer<leveldb::DB> > QLevelDB::openDBFiles;
QReadWriteLock QLevelDB::openDBFilesLock;

class JSONComparator: public leveldb::Comparator {

public:
    Compare(const leveldb::Slice& a, const leveldb::Slice& b) const {
        QJsonDocument.fromBinaryData(QByteArray(a.data(),a.sze()))

    };
    const char* Name() const { return "JSONComparator"; }
};

void QLevelDB::open()
{
    QFileInfo fi(m_filepath);
    QString fpath= fi.canonicalFilePath();
    if(!fi.exists()) return;
    openDBFilesLock.lockForRead();
    OpenDBFiles::iterator i= openDBFiles.lowerBound(fpath);
    if( i != openDBFiles.end() && i.key() == fpath ) {
        db= i.value().toStrongRef();
    }
    if( !db ) {
        if(!openDBFilesLock.tryLockForWrite()) { //upgrade for write
            openDBFilesLock.unlock();            //unclock to prevent deadlocks
            openDBFilesLock.lockForWrite();
            i= openDBFiles.lowerBound(fpath);    //concurent open may change as we unlock
            if( i != openDBFiles.end() && i.key() == fpath ) {
                db= i.value().toStrongRef();
                if(!db.isNull()) {
                    openDBFilesLock.unlock();
                    return;
                }
            }
        }
        leveldb::Options options;
        leveldb::DB * ptr;
        leveldb::Status s= leveldb::DB::Open(options,fpath.toStdString(),&ptr);

        if(!s.ok()) {
            openDBFilesLock.unlock();
            return;
        }

        db= QSharedPointer<leveldb::DB>( ptr, &close );

        if( i != openDBFiles.end() ) {
            openDBFiles.insert(i,fpath,db.toWeakRef());
        } else {
            openDBFiles.insert(fpath,db.toWeakRef());
        }

    }
    openDBFilesLock.unlock();
}

void QLevelDB::close(leveldb::DB* db)
{
    delete db;
}

QLevelDB::QLevelDB(QObject *parent) :
    QObject(parent)
{
}
