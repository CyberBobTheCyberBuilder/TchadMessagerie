#include "loginwidget.h"
#include "ui_loginwidget.h"

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWidget)
{
    ui->setupUi(this);
    client = nullptr;
    dr = new DataRessource();
    connect(ui->Blogin, SIGNAL(pressed()), this, SLOT(BloginPressed()));
}

void LoginWidget::BloginPressed()
{
    if(!ui->LEid->text().isEmpty() || !ui->LEpwd->text().isEmpty()){
        disconnect(ui->Blogin, SIGNAL(pressed()), this, SLOT(BloginPressed()));
        QString id = ui->LEid->text();
        QByteArray pwd = QCryptographicHash::hash(ui->LEpwd->text().toUtf8(), QCryptographicHash::Sha3_512);
        QJsonObject json
        {
            {"action", "login"},
            {"login", id},
            {"password", QString(pwd.toBase64())}
        };
        connect(dr, SIGNAL(responseLogin(bool)), this, SLOT(login(bool)));
        dr->sendData(json);

    }
}

void LoginWidget::login(bool state){
    disconnect(dr, SIGNAL(responseLogin(bool)), this, SLOT(login(bool)));
    if(state){
        client = new ClientWindow(this, dr);
        connect(client, SIGNAL(logout()), this, SLOT(ClientLogout()));
        this->hide();
        client->show();
    }
}

void LoginWidget::ClientLogout()
{
    disconnect(client, SIGNAL(logout()), this, SLOT(ClientLogout()));

    ui->LEpwd->setText("");
    client->close();
    this->show();
    QJsonObject json
    {
        {"action", "logout"}
    };
    dr->sendData(json);
    connect(ui->Blogin, SIGNAL(pressed()), this, SLOT(BloginPressed()));

}

LoginWidget::~LoginWidget()
{
    delete ui;
    delete client;
}
