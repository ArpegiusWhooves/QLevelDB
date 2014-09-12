#ifndef QLEVELDBJSONLEXICALCOMPARATOR_H
#define QLEVELDBJSONLEXICALCOMPARATOR_H

#include <leveldb/comparator.h>

class QJsonValue;
class QJsonArray;
class QJsonObject;
class QJsonDocument;
class QByteArray;

#include <string>

class QLevelDBJsonLexicalComparator: public leveldb::Comparator {

public:

    static int Compare( QJsonValue l, QJsonValue r );

    static int Compare( QJsonArray l, QJsonArray r );

    static int Compare( QJsonObject l, QJsonObject r );

    static int Compare( QJsonDocument const& l, QJsonDocument const& r );

    static int CompareJson(const QByteArray& a, const QByteArray& b);

    virtual int Compare(const leveldb::Slice& a, const leveldb::Slice& b) const;

    const char *Name() const;

    virtual void FindShortestSeparator(
        std::string* start,
        const leveldb::Slice& limit) const;

    virtual void FindShortSuccessor(std::string* key) const;

};



#endif // QLEVELDBJSONLEXICALCOMPARATOR_H
