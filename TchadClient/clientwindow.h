#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QMainWindow>
#include <QCryptographicHash>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTcpSocket>
#include "dataressource.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ClientWindow; }
QT_END_NAMESPACE

class ClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    ClientWindow(QWidget *parent = nullptr, DataRessource *dr = nullptr);
    ~ClientWindow();
    DataRessource *dr;

public slots:
    void BnewConv();
    void createConv(bool state);
    void closeMyTab(int);
    void sendMessage();
    void menuLogoutPressed();
    void BexportPDFPressed();
    void receivedMessage(QString, QString, QDateTime);

signals:
    void logout();

private:
    Ui::ClientWindow *ui;
};
#endif // CLIENTWINDOW_H
