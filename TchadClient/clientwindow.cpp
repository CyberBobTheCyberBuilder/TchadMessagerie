#include "clientwindow.h"
#include "ui_clientwindow.h"
#include <QDebug>
#include <QInputDialog>
#include <QErrorMessage>
#include <QPdfWriter>
#include <QPainter>
#include <QPrinter>
#include <QPalette>
#include "tabitemlistconv.h"

QString text;
ClientWindow::ClientWindow(QWidget *parent, DataRessource *dr)
    : QMainWindow(parent)
    , ui(new Ui::ClientWindow)
{
    ui->setupUi(this);
    this->dr = dr;

    this->enabledDarkMode = false;
    this->whitePalette = this->palette();
    this->darkModePalette = QPalette();
    darkModePalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkModePalette.setColor(QPalette::WindowText, Qt::white);
    darkModePalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkModePalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkModePalette.setColor(QPalette::ToolTipBase, Qt::black);
    darkModePalette.setColor(QPalette::ToolTipText, Qt::white);
    darkModePalette.setColor(QPalette::Text, Qt::white);
    darkModePalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkModePalette.setColor(QPalette::ButtonText, Qt::white);
    darkModePalette.setColor(QPalette::BrightText, Qt::red);
    darkModePalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkModePalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkModePalette.setColor(QPalette::HighlightedText, Qt::black);

    //Connect ui
    connect(ui->BnewConv, SIGNAL(pressed()), this, SLOT(BnewConv()));
    connect(ui->Bsend, SIGNAL(pressed()), this, SLOT(sendMessage()));
    connect(ui->tabConv, SIGNAL(tabCloseRequested(int)), this, SLOT(closeMyTab(int)));
    connect(ui->MAlogout, SIGNAL(triggered()), this, SLOT(menuLogoutPressed()));
    connect(ui->actionEnable_dark_mode, SIGNAL(triggered()), this, SLOT(changeDarkMode()));
    connect(ui->BexportPDF, SIGNAL(pressed()), this, SLOT(BexportPDFPressed()));

    connect(dr, SIGNAL(messageReceived(QString, QString, QDateTime)), this, SLOT(receivedMessage(QString, QString, QDateTime)));

    // Sécurité : demander au serveur les messages en attente qu'après le chargement de la fenêtre
    // IMCOMPATIBLE AVEC LES AUTRES SERVEURS
    /*
    QJsonObject json
    {
        {"action", "get_waiting_messages"}
    };
    dr->sendData(json);
    */
}

void ClientWindow::receivedMessage(QString from, QString content, QDateTime sendAt){
    int index = -1;
    for(int i = 0; i < ui->tabConv->count(); i++){
        if(ui->tabConv->tabText(i) == from){
            index = i;
        }
    }
    if(index == -1){
        index = ui->tabConv->addTab(new TabItemListConv(), from);
        ui->Bsend->setDisabled(false);
        ui->Bsend->setAutoDefault(true);
        ui->BexportPDF->setDisabled(false);
        ui->LEinputMsg->setDisabled(false);
    }

    TabItemListConv *currentTab = qobject_cast<TabItemListConv*>(ui->tabConv->widget(index));
    currentTab->addReceiveMsg(sendAt, content);

}

void ClientWindow::BnewConv(){
    qDebug() << "BnewConv pressed";
    bool ok;
    text = QInputDialog::getText(this, tr("Nouvelle conversation"),tr("User name :"), QLineEdit::Normal, "", &ok).simplified();
    if (ok && !text.isEmpty()){
        disconnect(ui->BnewConv, SIGNAL(pressed()), this, SLOT(BnewConv()));

        this->createConv(true);

        // Sécurité : vérification de l'existence de l'utilisateur
        // IMCOMPATIBLE AVEC LES AUTRES SERVEURS
        /*
        QJsonObject json
        {
            {"action", "isloginexist"},
            {"login", text}
        };
        connect(dr, SIGNAL(responseIsLoginExist(bool)), this, SLOT(createConv(bool)));

        dr->sendData(json);
        */
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
        int currentIndex = ui->tabConv->currentIndex();
        QList<QString> to = {ui->tabConv->tabText(currentIndex)};
        QJsonObject json
        {
            {"action", "send"},
            {"to", QJsonArray::fromStringList(to)},
            {"content", message}
        };
        dr->sendData(json);
        currentTab->addSendMsg(message);
        ui->LEinputMsg->setText("");
    }
}

void ClientWindow::menuLogoutPressed(){
    qDebug() << "logout";
    emit logout();
}

void ClientWindow::changeDarkMode(){
    this->enabledDarkMode = !this->enabledDarkMode;
    if(this->enabledDarkMode){
        ui->actionEnable_dark_mode->setText("Disable dark mode");
        this->setPalette(darkModePalette);
    }
    else{
        ui->actionEnable_dark_mode->setText("Enable dark mode");
        this->setPalette(this->whitePalette);
    }
}

void ClientWindow::BexportPDFPressed()
{
    qDebug() << "Export";
    QPrinter printer;
    //TODO : QDialogFolder choice folder
    printer.setOutputFileName("/home/fabrice/Bureau/export.pdf");
    printer.setOutputFormat(QPrinter::PdfFormat);

    TabItemListConv *currentTab = qobject_cast<TabItemListConv*>(ui->tabConv->currentWidget());

    currentTab->PTEmsgs->document()->print(&printer);
}

ClientWindow::~ClientWindow()
{
    delete ui;
    qDebug() << "client window destroyed";
}

