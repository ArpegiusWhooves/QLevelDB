
#include <QCoreApplication>
#include "qleveldb.h"

#include <QDebug>

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>

QByteArray jsons[] = {
    "",
    "[]",
    "[null]",
    "[true]",
    "[false]",
    "[-0.5]",
    "[0]",
    "[1]",
    "[1,2,3]",
    "[1,2,3,-1,5,6]",
    "[1,2,3,4]",
    "[\"\"]",
    "[\"1\"]",
    "[\"ZAM\"]",
    "[\"ZAMK\"]",
    "[[]]",
    "[[],[null]]",
    "[[[[]]],[]]",
    "[[[[1]]],[]]",
    "{}",
    "{\"\":[]}",
    "{\"\":{}}",
    "{\"ZAM\":[]}",
    "{\"ZAMK\":null}",
    "{\"ZAMK\":10}",
    "{\"ZAMK\":{}}",
};

template<class T, size_t N>
size_t size(T (&)[N]) { return N; }

int main(int argc, char **argv)
{
    QCoreApplication core(argc,argv);

    QLevelDB lvldb;
    lvldb.setFilepath("newFile");

    for(int i=1; i!= size(jsons);++i)
    {
        QJsonArray a;
        a.append(QJsonValue(i) );
        lvldb.set( QJsonDocument::fromJson(jsons[i]), QJsonDocument(a) );
    }

    for(int i=1; i!= size(jsons);++i)
        qDebug() << i  << "=" << lvldb.get(  QJsonDocument::fromJson(jsons[i])  ).array().at(0);


    return 0;
    return core.exec();
}

