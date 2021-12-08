#ifndef TABITEMLISTCONV_H
#define TABITEMLISTCONV_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QDateTime>
#include <QPlainTextEdit>

class TabItemListConv : public QWidget
{
    Q_OBJECT
public:
    explicit TabItemListConv(QWidget *parent = nullptr);
    void addSendMsg(QString);
    void addReceiveMsg(QDateTime, QString);
    ~TabItemListConv();

public:
    QListWidget *list;
    QPlainTextEdit *PTEmsgs;
signals:

};

#endif // TABITEMLISTCONV_H
