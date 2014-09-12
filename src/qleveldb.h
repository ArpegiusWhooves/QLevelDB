#ifndef QLEVELDB_H
#define QLEVELDB_H

#include <QObject>
#include <QMap>
#include <QWeakPointer>
#include <QReadWriteLock>
#include <QJsonDocument>

namespace leveldb {
    class DB;
}

class QLevelDB : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString filepath READ filepath WRITE setFilepath NOTIFY filepathChanged)
    QString m_filepath;
    QSharedPointer<leveldb::DB> db;

    typedef QMap<QString, QWeakPointer<leveldb::DB> > OpenDBFiles;
    static OpenDBFiles openDBFiles;
    static QReadWriteLock openDBFilesLock;

    void open();
    static void close(leveldb::DB*);

public:
    explicit QLevelDB(QObject *parent = 0);

    QString filepath() const
    {
        return m_filepath;
    }

    Q_INVOKABLE void set( QJsonDocument const& key, QJsonDocument const& value );
    Q_INVOKABLE QJsonDocument get( QJsonDocument const& key );

    signals:

    void filepathChanged(QString arg);

    public slots:

    void setFilepath(QString arg)
    {
        if (m_filepath != arg) {
            m_filepath = arg;
            open();
            emit filepathChanged(arg);
        }
    }
};

#endif // QLEVELDB_H
