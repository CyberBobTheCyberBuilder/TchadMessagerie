#ifndef DATARESSOURCE_H
#define DATARESSOURCE_H

#include <QObject>
#include <QList>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTcpSocket>

class DataRessource : public QObject
{
    Q_OBJECT
public:
    QTcpSocket *socket;
    explicit DataRessource(QObject *parent = nullptr);
    void sendData(QJsonObject);
    ~DataRessource();


public slots:
    void sendMsg(QString, QList<QString>);
    void receiveData();

signals:
    void connected();
    void disconnected();
    void messageReceived(QString, QString, QDateTime);
    void responseLogin(bool);
    void responseIsLoginExist(bool);

};

#endif // DATARESSOURCE_H
