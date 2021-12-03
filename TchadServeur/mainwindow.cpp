#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gestionfile.h"
#include <QThread>

#define SERVEUR "[SERVEUR]"
#define CONNEXION "[CONNEXION]"
#define LOGIN "[LOGIN]"
#define UTILS "[UTILS]"
#define MESSAGE "[MESSAGE]"
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
    QString address = peerAddress == "::1" ? "localhost" : peerAddress.replace("::ffff:", "");

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

bool MainWindow::isLoginExist(QString login){

    GestionFile gf("logins.json");
    QJsonArray logins = gf.getJsonArray();
    for (const QJsonValue &obj: qAsConst(logins)) {
        QJsonObject currentObject = obj.toObject();

        if (currentObject.value("login").toString() == login) {

            return true;
        }
    }
    return false;
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
        else if(action == "isloginexist"){
            QString login = obj.value("login").toString();
            QJsonObject response;
            response.insert("action", "answerisloginexist");
            if(this->isLoginExist(login)){
                response.insert("state", "ok");
            }
            else{
                response.insert("state", "fail");
            }
            QJsonDocument doc(response);
            incomingSocket->write(doc.toJson());

            this->writeLogs(UTILS, "Vérification de l'existence de l'utilisateur : " + login, Qt::darkGray);
        }
        else if(action == "send"){
            QJsonArray to = obj.value("to").toArray();
            QString content = obj.value("content").toString();
            QString sendAt = QDateTime::currentDateTime().toString("dd/MM/yy hh:mm:ss");
            QString from = clientsTcp[incomingSocket];

            this->writeLogs(MESSAGE, "L'utilisateur " + from + " a envoyé un message.", Qt::black);

            for(const QJsonValue& user : qAsConst(to)){
                QJsonObject newMsg
                {
                    {"to", user.toString()},
                    {"from", from},
                    {"content", content},
                    {"datetime", sendAt}
                };

                if(clientsTcp.key(user.toString()) == nullptr){
                    GestionFile gf("messages.json");
                    QJsonArray messages = gf.getJsonArray();
                    messages.append(newMsg);
                    gf.write(messages);
                }
                else{
                    QTcpSocket *socket = clientsTcp.key(user.toString());
                    newMsg.insert("action", "receive");
                    QJsonDocument doc(newMsg);
                    socket->write(doc.toJson());
                }
            }
        }
        else if(action == "get_waiting_messages"){
            this->sendWaitingMsg(clientsTcp[incomingSocket]);
        }
    }

    qDebug() << "ACTION : " << action;
}

void MainWindow::sendWaitingMsg(QString login){
    QTcpSocket *socket = clientsTcp.key(login);
    GestionFile gf("messages.json");
    QJsonArray messages = gf.getJsonArray();
    QList<int> indexToRemove = {};

    for (int i = 0; i < messages.count(); i++) {
        QJsonObject currentObject = messages.at(i).toObject();

        if (currentObject.value("to").toString() == login) {
            QJsonObject json
            {
              {"action", "receive"},
              {"from", currentObject.value("from").toString()},
              {"content", currentObject.value("content").toString()},
              {"datetime", currentObject.value("datetime").toString()},
            };
            QJsonDocument doc(json);
            qDebug() << json;
            socket->write(doc.toJson());
            indexToRemove.append(i);
        }
    }
    //TODO : remove correctly
    // PK TU TE RETIRES PAS
    // FAIRE CODE PLUS BEAU
    for(int i : indexToRemove){
        messages.removeAt(i);
    }
    gf.write(messages);
}

void MainWindow::addClient(){
    QDialog dialog(this);
    QFormLayout form(&dialog);

    form.addRow(new QLabel("Connectez-vous"));

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

