#include "clientwindow.h"
#include "ui_clientwindow.h"
#include <QDebug>
#include <QInputDialog>
#include <QErrorMessage>
#include "tabitemlistconv.h"

QString text;
ClientWindow::ClientWindow(QWidget *parent, DataRessource *dr)
    : QMainWindow(parent)
    , ui(new Ui::ClientWindow)
{
    ui->setupUi(this);
    this->dr = dr;
    //Connect ui
    connect(ui->BnewConv, SIGNAL(pressed()), this, SLOT(BnewConv()));
    connect(ui->Bsend, SIGNAL(pressed()), this, SLOT(sendMessage()));
    connect(ui->tabConv, SIGNAL(tabCloseRequested(int)), this, SLOT(closeMyTab(int)));
    connect(ui->MAlogout, SIGNAL(triggered()), this, SLOT(menuLogoutPressed()));

}

void ClientWindow::receiveMsg(){

}

void ClientWindow::BnewConv(){
    qDebug() << "BnewConv pressed";
    bool ok;
    text = QInputDialog::getText(this, tr("Nouvelle conversation"),tr("User name :"), QLineEdit::Normal, "", &ok).simplified();
    if (ok && !text.isEmpty()){
        disconnect(ui->BnewConv, SIGNAL(pressed()), this, SLOT(BnewConv()));
        QJsonObject json
        {
            {"action", "isloginexist"},
            {"login", text}
        };
        connect(dr, SIGNAL(responseIsLoginExist(bool)), this, SLOT(createConv(bool)));
        dr->sendData(json);
    }
}

void ClientWindow::createConv(bool state){

    if(state){
        ui->tabConv->addTab(new TabItemListConv(), text);
        ui->Bsend->setDisabled(false);
        ui->Bsend->setAutoDefault(true);
        ui->BexportPDF->setDisabled(false);
        ui->LEinputMsg->setDisabled(false);
    }
    else{
        QErrorMessage errorDialog(this);
        errorDialog.showMessage("Cet utilisateur n'existe pas.");
    }
    connect(ui->BnewConv, SIGNAL(pressed()), this, SLOT(BnewConv()));
}

void ClientWindow::closeMyTab(int i){
    qDebug() << "Tab closed";
    delete ui->tabConv->widget(i);
    ui->tabConv->removeTab(i);
    if(ui->tabConv->count() <= 0){
        ui->Bsend->setDisabled(true);
        ui->Bsend->setAutoDefault(false);
        ui->tabConv->setDisabled(true);
        ui->tabConv->setDisabled(true);
    }
}

void ClientWindow::sendMessage(){
    qDebug() << "Bsend pressed";
    QString message = ui->LEinputMsg->text().simplified();
    if(message.isEmpty()){
        return;
    }
    TabItemListConv *currentTab = qobject_cast<TabItemListConv*>(ui->tabConv->currentWidget());
    if(currentTab != nullptr){
        currentTab->addSendMsg(message);
        ui->LEinputMsg->setText("");
    }
}

void ClientWindow::menuLogoutPressed(){
    qDebug() << "logout";
    emit logout();
}

ClientWindow::~ClientWindow()
{
    delete ui;
    qDebug() << "client window destroyed";
}

