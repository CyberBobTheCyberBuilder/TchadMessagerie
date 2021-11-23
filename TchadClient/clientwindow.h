#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class ClientWindow; }
QT_END_NAMESPACE

class ClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();

public slots:
    void newConversation();
    void closeMyTab(int);
    void sendMessage();

private:
    Ui::ClientWindow *ui;
};
#endif // CLIENTWINDOW_H
