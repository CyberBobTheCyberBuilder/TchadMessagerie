#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <QCryptographicHash>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTcpSocket>
#include "clientwindow.h"
#include "dataressource.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWidget; }
QT_END_NAMESPACE

class LoginWidget : public QWidget
{
    Q_OBJECT
public:
    LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();
    ClientWindow *client;
    DataRessource *dr;

public slots:
    void BloginPressed();
    void ClientLogout();
    void login(bool);

private:
    Ui::LoginWidget *ui;
    QTcpSocket *socket;

};

#endif // LOGINWIDGET_H
