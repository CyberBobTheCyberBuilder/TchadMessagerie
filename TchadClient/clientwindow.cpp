#include "clientwindow.h"
#include "ui_clientwindow.h"
#include <QDebug>
#include <QInputDialog>
#include "tabitemlistconv.h"

ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientWindow)
{
    ui->setupUi(this);

    //Connect ui
    connect(ui->BnewConv, SIGNAL(pressed()), this, SLOT(newConversation()));
    connect(ui->Bsend, SIGNAL(pressed()), this, SLOT(sendMessage()));
    connect(ui->tabConv, SIGNAL(tabCloseRequested(int)), this, SLOT(closeMyTab(int)));
    connect(ui->MAlogout, SIGNAL(triggered()), this, SLOT(menuLogoutPressed()));
}

void ClientWindow::newConversation(){
    qDebug() << "BnewConv pressed";
    bool ok;
    QString text = QInputDialog::getText(this, tr("Nouvelle conversation"),tr("User name :"), QLineEdit::Normal, "", &ok).simplified();
    if (ok && !text.isEmpty()){
        ui->tabConv->addTab(new TabItemListConv(), text);
        ui->Bsend->setDisabled(false);
        ui->Bsend->setAutoDefault(true);
        ui->BexportPDF->setDisabled(false);
        ui->LEinputMsg->setDisabled(false);
    }
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

