#include "qleveldbjsonlexicalcomparator.h"

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>

#include <leveldb/slice.h>

#include <limits>

int QLevelDBJsonLexicalComparator::Compare(const QJsonValue l, const QJsonValue r) {
    if( l.type() < r.type() ) return -1;
    if( l.type() > r.type() ) return 1;
    switch (l.type()) {
    case QJsonValue::Null:
        return 0;
    case QJsonValue::Bool:
        if( l.toBool() ) return r.toBool()?0:-1;
        return r.toBool()?1:0;
    case QJsonValue::Double:
        {
            double val= l.toDouble() - r.toDouble();
            if(val < -std::numeric_limits<float>::epsilon() ) return -1;
            if(val > std::numeric_limits<float>::epsilon() ) return 1;
            return 0;
        }
    case QJsonValue::String:
        return QString::compare( l.toString(), r.toString() );
    case QJsonValue::Array:
        return Compare( l.toArray(), r.toArray() );
    case QJsonValue::Object:
        return Compare( l.toObject(), r.toObject() );
    default:
        return 0;
    }
}


int QLevelDBJsonLexicalComparator::Compare(QJsonArray l, QJsonArray r) {
    for(int i= 0, s= std::min(l.size(),r.size());i<s;++i){
        if(int ret= Compare( l[i],r[i] )) return ret;
    }
    if( l.size() < r.size() ) return -1; // l < r
    if( l.size() > r.size() ) return  1; // l > r
    return 0;
}


int QLevelDBJsonLexicalComparator::Compare(QJsonObject l, QJsonObject r) {
    QJsonObject::const_iterator i = l.begin();
    QJsonObject::const_iterator j = r.begin();

    for(;;++i,++j){
        if(i == l.end()) return j==r.end() ? 0:-1; // l <= r
        if(j == r.end()) return 1;                 // l > r
        if(int ret= QString::compare( i.key() , j.key() )) return ret;
        if(int ret= Compare( i.value() , j.value() )) return ret;
    }
}

int QLevelDBJsonLexicalComparator::Compare(const QJsonDocument &l, const QJsonDocument &r)
{
    if(l.isEmpty()) return r.isEmpty() ? 0 :-1; // l <= r
    if(r.isEmpty()) return 1;                   // l > r
    if(l.isNull()) return r.isNull() ? 0 :-1;   // l <= r
    if(r.isNull()) return 1;                    // l > r
    if(l.isArray()) {
        if(!r.isArray()) return -1;             // l < r
        return Compare(l.array(),r.array());
    }
    if(r.isArray()) return 1;                   // l > r

    if(l.isObject()) {
        if(!r.isObject()) return -1;            // l < r
        return Compare(l.object(),r.object());
    }

    if(r.isObject()) return 1;
    return 0;
}


int QLevelDBJsonLexicalComparator::Compare(const leveldb::Slice &a, const leveldb::Slice &b) const {
    return Compare( QJsonDocument::fromBinaryData(QByteArray(a.data(),a.size())),
                    QJsonDocument::fromBinaryData(QByteArray(b.data(),b.size())));
}

int QLevelDBJsonLexicalComparator::CompareJson(const QByteArray &a, const QByteArray &b) {
    int v= Compare( QJsonDocument::fromJson(a),QJsonDocument::fromJson(b));
    return (v > 0) - (v < 0);
}

const char *QLevelDBJsonLexicalComparator::Name() const { return "JSONComparator"; }

void QLevelDBJsonLexicalComparator::FindShortestSeparator(std::string *start, const leveldb::Slice &limit) const{

}

void QLevelDBJsonLexicalComparator::FindShortSuccessor(std::string *key) const {

}
