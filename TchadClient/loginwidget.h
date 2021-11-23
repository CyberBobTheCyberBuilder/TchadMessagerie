#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <QCryptographicHash>
#include <QJsonObject>
#include "clientwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWidget; }
QT_END_NAMESPACE

class LoginWidget : public QWidget
{
    Q_OBJECT
public:
    LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();
    ClientWindow *c;

public slots:
    void BloginPressed();
    void ClientLogout();
private:
    Ui::LoginWidget *ui;
};

#endif // LOGINWIDGET_H
