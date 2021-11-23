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

ClientWindow::~ClientWindow()
{
    delete ui;
}

