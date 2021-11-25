#ifndef GESTIONFILE_H
#define GESTIONFILE_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QIODevice>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTextStream>

class GestionFile
{
public:
    GestionFile(QString);
    QJsonArray getJsonArray();
    void write(QJsonArray);
    ~GestionFile();

private:
    QString filename;
};

#endif // GESTIONFILE_H
