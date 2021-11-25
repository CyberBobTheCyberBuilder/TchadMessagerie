#include "loginwidget.h"
#include "ui_loginwidget.h"

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWidget)
{
    ui->setupUi(this);
    c = nullptr;
    socket = new QTcpSocket(this);
    socket->connectToHost("localhost", 44444);
    if(socket->waitForConnected(5000)){
        qDebug() << "connected";
        connect(ui->Blogin, SIGNAL(pressed()), this, SLOT(BloginPressed()));
    }
    else{
        qDebug() << "Serveur indisponible";
    }

}

void LoginWidget::BloginPressed()
{
    if(!ui->LEid->text().isEmpty() || !ui->LEpwd->text().isEmpty()){
        QString id = ui->LEid->text();
        QByteArray pwd = QCryptographicHash::hash(ui->LEpwd->text().toUtf8(), QCryptographicHash::Sha3_512);
        QJsonObject json
        {
            {"action", "login"},
            {"login", id},
            {"password", QString(pwd.toHex())}
        };
        connect(socket, SIGNAL(readyRead()), this, SLOT(receiveResponse()));
        QJsonDocument doc(json);
        socket->write(doc.toJson());
        // TODO send json and receive JSON :
        // {"action":"answerlogin","state":"ok"}

    }
}

void LoginWidget::receiveResponse(){
    QTcpSocket* incomingSocket = (QTcpSocket*) sender();
    disconnect(socket, SIGNAL(readyRead()), this, SLOT(receiveResponse()));
    QByteArray data = incomingSocket->readAll();

    qDebug() << data;
    QJsonDocument doc = QJsonDocument::fromJson(data);

    QJsonObject obj;
    // check validity of the document
    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
            //qDebug() << "JSON : " << QString(data);
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
                c = new ClientWindow(this);
                connect(c, SIGNAL(logout()), this, SLOT(ClientLogout()));
                this->hide();
                c->show();
                disconnect(socket, SIGNAL(readyRead()), this, SLOT(receiveResponse()));
            }
        }
    }
}

void LoginWidget::ClientLogout()
{
    disconnect(c, SIGNAL(logout()), this, SLOT(ClientLogout()));
    ui->LEpwd->setText("");
    c->close();
    this->show();
    QJsonObject json
    {
        {"action", "logout"}
    };
    connect(socket, SIGNAL(readyRead()), this, SLOT(receiveResponse()));
    QJsonDocument doc(json);
    socket->write(doc.toJson());
}

LoginWidget::~LoginWidget()
{
    delete ui;
    delete c;
}
