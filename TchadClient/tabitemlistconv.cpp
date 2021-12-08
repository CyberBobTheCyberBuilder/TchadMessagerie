#include "tabitemlistconv.h"
#include <QDebug>

TabItemListConv::TabItemListConv(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    //list = new QListWidget();
    PTEmsgs = new QPlainTextEdit();
    PTEmsgs->setReadOnly(true);
    layout->insertWidget(0, PTEmsgs);
}

void TabItemListConv::addSendMsg(QString msg){
    QDateTime date = QDateTime::currentDateTime();
    QString fullText = date.toString("dd/MM/yyyy hh:mm:ss");
    fullText += " : -> " + msg;
    qDebug() << fullText;
    QTextCharFormat tf = PTEmsgs->currentCharFormat();
    tf.setForeground(QBrush(Qt::blue));
    PTEmsgs->setCurrentCharFormat(tf);

    PTEmsgs->appendPlainText(fullText);
}

void TabItemListConv::addReceiveMsg(QDateTime date, QString msg){
    QString fullText = date.toString("dd/MM/yyyy hh:mm:ss");
    fullText += " : <- " + msg;
    QTextCharFormat tf = PTEmsgs->currentCharFormat();
    tf.setForeground(QBrush(Qt::red));
    PTEmsgs->setCurrentCharFormat(tf);

    PTEmsgs->appendPlainText(fullText);
}

TabItemListConv::~TabItemListConv()
{
    delete list;
}
