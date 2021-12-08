#include "dataressource.h"

DataRessource::DataRessource(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
    socket->connectToHost("localhost", 8585);
    if(socket->waitForConnected(5000)){
        qDebug() << "connected";
    }
    else{
        qDebug() << "Serveur indisponible";
    }


    connect(socket, SIGNAL(readyRead()), this, SLOT(receiveData()));
}

void DataRessource::receiveData(){
    QByteArray dataRead = socket->readAll();

    QList<QByteArray> dataList = dataRead.split('}');

    foreach( QByteArray data, dataList )
    {
        data.append('}');
        QJsonDocument doc = QJsonDocument::fromJson(data);

        QJsonObject obj;
        // check validity of the document
        if(!doc.isNull()){
            if(doc.isObject()){
                obj = doc.object();
                qDebug() << "JSON : " << QString(data);
            }
            else{
                qDebug() << "Document is not an object";
            }
        }
        else{
            qDebug() << "Invalid JSON...\n" << QString(data);
        }
        QString action = "";
        if (!obj.value("action").isUndefined()) {
            action = obj.value("action").toString();

            if (action == "answerlogin") {
                if(obj.value("state").toString() == "ok"){
                    qDebug() << "show client window";
                    emit responseLogin(true);
                }
                else{
                    qDebug() << "Bad id or password";
                    emit responseLogin(false);
                }
            }
            else if (action == "receive") {
                QString from = obj.value("from").toString();
                QString msg = obj.value("content").toString();
                QDateTime sendAt = QDateTime::fromString(obj.value("datetime").toString(), "dd/MM/yyyy hh:mm:ss");
                qDebug() << msg;
                emit messageReceived(from, msg, sendAt);
            }
            else if(action == "answerisloginexist"){
                if(obj.value("state").toString() == "ok"){
                    emit responseIsLoginExist(true);
                }
                else{
                    emit responseIsLoginExist(false);
                }

            }
        }
        qDebug() << "ACTION : " << action;
    }

}

void DataRessource::sendMsg(QString content, QList<QString> to){
    QJsonObject json
    {
        {"action", "send"},
        {"to", QJsonArray::fromStringList(to)},
        {"content", content}
    };
    QJsonDocument doc(json);
    socket->write(doc.toJson());
}

void DataRessource::sendData(QJsonObject data){
    QJsonDocument doc(data);
    socket->write(doc.toJson());
}

DataRessource::~DataRessource(){
    delete socket;
}
