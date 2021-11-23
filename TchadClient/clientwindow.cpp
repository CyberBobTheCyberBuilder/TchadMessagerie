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
}

void ClientWindow::newConversation(){
    qDebug() << "BnewConv pressed";
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),tr("User name :"), QLineEdit::Normal, "", &ok);
    if (ok && !text.isEmpty()){
        ui->tabConv->addTab(new TabItemListConv(), text);
        ui->Bsend->setDisabled(false);
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
        ui->tabConv->setDisabled(true);
        ui->tabConv->setDisabled(true);
    }
}


ClientWindow::~ClientWindow()
{
    delete ui;
}

