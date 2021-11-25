#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gestionfile.h"

#define SERVEUR "[SERVEUR]"
#define CONNEXION "[CONNEXION]"
#define LOGIN "[LOGIN]"
#define ERROR "[ERROR]"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->writeLogs(SERVEUR, "Démarrage...", Qt::black);

    this->tcpServer = new QTcpServer(this);

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(serverConnected()));
    tcpServer->listen(QHostAddress::Any, 44444);

    connect(ui->BaddClient, SIGNAL(pressed()), this, SLOT(addClient()));
    connect(ui->BdeleteClient, SIGNAL(pressed()), this, SLOT(deleteClient()));

    GestionFile gf("logins.json");
    QJsonArray logins = gf.getJsonArray();
    qDebug() << "Nombre de login dans le fichier : " << logins.count();
    for (const QJsonValue &obj: qAsConst(logins)) {
        QJsonObject currentObject = obj.toObject();

        ui->listClient->addItem(currentObject.value("login").toString());
    }


    this->writeLogs(SERVEUR, "Initialisation terminée, en attente de connexion.", Qt::black);
}

void MainWindow::writeLogs(QString type, QString msg, Qt::GlobalColor color)
{
    QString log = type + " ";
    log += QDateTime::currentDateTime().toString("dd/MM/yy hh:mm:ss");
    log += " -> " + msg;
    QTextCharFormat tf = ui->PTElogs->currentCharFormat();
    tf.setForeground(QBrush(color));
    ui->PTElogs->setCurrentCharFormat(tf);

    ui->PTElogs->appendPlainText(log);
}

void MainWindow::serverConnected(){
    QTcpSocket* connection = tcpServer->nextPendingConnection();
    connect(connection, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    connect(connection, SIGNAL(readyRead()), this, SLOT(readyToRead()));
    clientsNotLogin.append(connection);

    QString peerAddress = connection->peerAddress().toString();

    this->writeLogs(CONNEXION, "Nouvelle connexion (" + address + ") !", Qt::blue);
}

bool MainWindow::clientLogin(QString name, QString password)
{
    this->writeLogs(LOGIN, "Tentative de login pour l'utilisateur `" + name + "`.", Qt::green);
    
    GestionFile gf("logins.json");
    QJsonArray logins = gf.getJsonArray();

    for (const QJsonValue &obj: qAsConst(logins)) {
        QJsonObject currentObject = obj.toObject();

        if (currentObject.value("login").toString() == name) {

            if (currentObject.value("password").toString() == password) {
                return true;
            }
        }
    }
    return false;
}

void MainWindow::socketDisconnected(){

    QTcpSocket* disconnectingClient = (QTcpSocket*) sender();

    QString peerAddress = disconnectingClient->peerAddress().toString();
    QString address = peerAddress == "::1" ? "localhost" : peerAddress.replace("::ffff:", "");

    this->clientLogout(disconnectingClient);

    this->writeLogs(CONNEXION, "Fin de connexion (" + address + ") s'est déconnecté.", Qt::blue);

    disconnectingClient->deleteLater();
}

void MainWindow::clientLogout(QTcpSocket * incomingSocket){

    QString username = "Anonymous";
    if (clientsTcp.contains(incomingSocket))
    {
        username = clientsTcp[incomingSocket];
        clientsTcp.remove(incomingSocket);
        clientsNotLogin.append(incomingSocket);
    }
    QList<QListWidgetItem *> items = ui->listConnected->findItems(username, Qt::MatchExactly);

    for (QListWidgetItem *item: qAsConst(items)) {
        ui->listConnected->removeItemWidget(item);
        delete item;
    }
    this->writeLogs(LOGIN, "Le client " + username + " s'est déconnecté.", Qt::green);
}

void MainWindow::readyToRead(){

    QTcpSocket* incomingSocket = (QTcpSocket*) sender();
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

        if (action == "login") {
            QString login = obj.value("login").toString();
            QString password = obj.value("password").toString();

            QJsonObject response;
            response.insert("action", "answerlogin");

            if (this->clientLogin(login, password)) {
                clientsTcp.insert(incomingSocket, login);
                clientsNotLogin.removeAll(incomingSocket);

                response.insert("state", "ok");
                this->writeLogs(LOGIN, "L'utilisateur `" + login + "` est connecté.", Qt::green);
                ui->listConnected->addItem(login);

            } else {
                response.insert("state", "fail");
            }
            QJsonDocument doc(response);

            incomingSocket->write(doc.toJson());
        }
        else if(action == "logout"){
            this->clientLogout(incomingSocket);
        }
        else if(action == "send"){
            /*
            Envoi message du client
            Trame :
            {
                "action":"send",
                "to":["login1","login2"....],
                "content":"message"
            }
            Si un message est envoyé à un client non connecté, il est stocké dans un fichier du serveur.
            Un message peut être envoyé à plusieurs destinataires.
            */
            //SAVE THE CURRENT DATETIME
        }
        else if(action == "receive"){
            /*
            Réception message
            Trame :
            {
                "action":"receive",
                "from":"login1",
                "content":"message",
                "datetime":"12/11/2021 14 :56" }
            */
        }
    }

    qDebug() << "ACTION : " << action;
}

void MainWindow::addClient(){
    QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);

    // Add some text above the fields
    form.addRow(new QLabel("Connectez-vous"));

    // Add the lineEdits with their respective labels
    QList<QLineEdit *> fields;
    QLineEdit *idEdit = new QLineEdit(&dialog);
    QString idLabel = "Identifiant";
    form.addRow(idLabel, idEdit);
    fields << idEdit;

    QLineEdit *pwEdit = new QLineEdit(&dialog);
    pwEdit->setEchoMode(QLineEdit::Password);
    QString pwLabel = "Mot de passe : ";
    form.addRow(pwLabel, pwEdit);
    fields << pwEdit;

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted && !idEdit->text().isEmpty() && !pwEdit->text().isEmpty()) {
        QByteArray p = QCryptographicHash::hash(pwEdit->text().toUtf8(), QCryptographicHash::Sha3_512);
        qDebug() << QDir::currentPath();

        GestionFile gf("logins.json");
        QJsonArray logins = gf.getJsonArray();

        QJsonObject newLogin;
        newLogin.insert("login", idEdit->text());
        newLogin.insert("password", QString(p.toHex()));
        logins.append(newLogin);

        gf.write(logins);

        ui->listClient->addItem(idEdit->text());
    }
}

void MainWindow::deleteClient(){
    QList<QListWidgetItem*> items = ui->listClient->selectedItems();
    foreach(QListWidgetItem* item, items){
        QTcpSocket *find = clientsTcp.key(item->text());
        if(find == nullptr){

            GestionFile gestionFile("logins.json");
            QJsonArray logins = gestionFile.getJsonArray();

            for (int i = 0; i < logins.count(); i++) {
                QJsonObject currentObject = logins.at(i).toObject();

                if (currentObject.value("login").toString() == item->text()) {
                    qDebug() << i;
                    logins.removeAt(i);
                }
            }

            gestionFile.write(logins);

            ui->listClient->removeItemWidget(item);
            delete item;
        }
    }
}

MainWindow::~MainWindow()
{
    this->writeLogs(SERVEUR, "Arrêt du serveur...", Qt::red);
    delete ui;
    delete tcpServer;
    //qDeleteAll(sockets);
    //sockets.clear();
}

