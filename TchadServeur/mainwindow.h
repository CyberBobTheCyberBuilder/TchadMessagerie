#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QCryptographicHash>
#include <QDir>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    bool clientLogin(QString name, QString password);
    ~MainWindow();
    QTcpServer *tcpServer;
    QMap<QTcpSocket*, QString> clientsTcp;
    QList<QTcpSocket*> clientsNotLogin;

private:
    void writeLogs(QString, QString, Qt::GlobalColor);
    void clientLogout(QTcpSocket *);
    bool isLoginExist(QString);
    void sendWaitingMsg(QString);

public slots:
    void serverConnected();
    void socketDisconnected();
    void readyToRead();
    void addClient();
    void deleteClient();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
