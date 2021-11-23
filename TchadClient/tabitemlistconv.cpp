#include "tabitemlistconv.h"
#include <QDebug>

TabItemListConv::TabItemListConv(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    list = new QListWidget();
    layout->insertWidget(0, list);
}

void TabItemListConv::addSendMsg(QString msg){
    QDateTime date = QDateTime::currentDateTime();
    QString fullText = date.toString("dd/MM/yy hh:mm");
    fullText += " : -> " + msg;
    qDebug() << fullText;
    QListWidgetItem *newItem = new QListWidgetItem(fullText);
    newItem->setForeground(Qt::blue);
    list->addItem(newItem);
}

void TabItemListConv::addReceiveMsg(QDateTime date, QString msg){
    QString fullText = date.toString("dd/MM/yy hh:mm");
    fullText += " : <- " + msg;
    QListWidgetItem *newItem = new QListWidgetItem(fullText);
    newItem->setForeground(Qt::red);
    list->addItem(newItem);
}

TabItemListConv::~TabItemListConv()
{
    delete list;
}
