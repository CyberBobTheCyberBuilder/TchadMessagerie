#include "gestionfile.h"

GestionFile::GestionFile(QString filename)
{
    this->filename = filename;
}

QJsonArray GestionFile::getJsonArray(){
    QString json;
    QFile file(QDir::currentPath() + "/" + this->filename);
    if(!file.exists()){
        file.open(QIODevice::NewOnly | QIODevice::ReadWrite | QIODevice::Text);
        file.write("[]");
        file.close();
    }
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    json = file.readAll();
    file.close();
    return QJsonDocument::fromJson(json.toUtf8()).array();
}

void GestionFile::write(QJsonArray jsonarray){

    QFile file(QDir::currentPath()+ "/" + this->filename);
    if(!file.exists()){
        file.open(QIODevice::NewOnly | QIODevice::ReadWrite | QIODevice::Text);
    }
    else{
        file.open(QIODevice::WriteOnly | QIODevice::Text);
    }
    QJsonDocument doc(jsonarray);
    QByteArray json = doc.toJson();
    file.write(json);
    file.close();
}

GestionFile::~GestionFile(){

}
