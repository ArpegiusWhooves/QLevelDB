#include "qleveldb.h"
#include <QFileInfo>

#include <leveldb/db.h>
#include <leveldb/options.h>
#include "qleveldbjsonlexicalcomparator.h"


#include <cmath>

#include <QDebug>

QMap<QString, QWeakPointer<leveldb::DB> > QLevelDB::openDBFiles;
QReadWriteLock QLevelDB::openDBFilesLock;

void QLevelDB::open()
{
    QString fpath = m_filepath;
    QFileInfo fi(m_filepath);
    bool createNewDatabase = true;
    if(fi.exists()) {
        createNewDatabase=false;
        fpath= fi.canonicalFilePath();
        qDebug() << "Openeing " << fpath;
    }
    else {
        qDebug() << "Creating " << fpath;
    }

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
        options.comparator = new QLevelDBJsonLexicalComparator;
        options.create_if_missing =  createNewDatabase;
        leveldb::DB * ptr;
        leveldb::Status s= leveldb::DB::Open(options,fpath.toStdString(),&ptr);

        qDebug() << "Opened status " << QString::fromStdString(s.ToString());

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

void QLevelDB::set(QJsonDocument const& key, QJsonDocument const& value)
{
    assert(!!db);
    leveldb::WriteOptions op;
    QByteArray k= key.toBinaryData();
    QByteArray v= value.toBinaryData();
    leveldb::Status s= db->Put(op, leveldb::Slice(k.data(),k.size()), leveldb::Slice( v.data(),v.size()) );
    qDebug() << "Write status " << QString::fromStdString(s.ToString());
}

QJsonDocument QLevelDB::get(QJsonDocument const& key)
{
    assert(!!db);
    leveldb::ReadOptions op;
    QByteArray k= key.toBinaryData();
    std::string s;
    leveldb::Status st= db->Get(op,leveldb::Slice(k.data(),k.size()),&s);
    qDebug() << "Read status " << QString::fromStdString(st.ToString());
    return QJsonDocument::fromBinaryData( QByteArray(s.data(), s.size()) );

}
