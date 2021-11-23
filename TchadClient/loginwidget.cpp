#include "loginwidget.h"
#include "ui_loginwidget.h"

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWidget)
{
    ui->setupUi(this);
    connect(ui->Blogin, SIGNAL(pressed()), this, SLOT(BloginPressed()));
    c = nullptr;
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
            {"password", QString(pwd)}
        };

        // TODO send json and receive JSON :
        // {"action":"answerlogin","state":"ok"}

        if(c == nullptr){
            c = new ClientWindow(this);
        }
        connect(c, SIGNAL(logout()), this, SLOT(ClientLogout()));
        this->hide();
        c->show();
    }
}

void LoginWidget::ClientLogout()
{
    disconnect(c, SIGNAL(logout()), this, SLOT(ClientLogout()));
    ui->LEpwd->setText("");
    c->close();
    this->show();
}

LoginWidget::~LoginWidget()
{
    delete ui;
    delete c;
}
