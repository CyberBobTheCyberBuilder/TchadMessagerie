#include "tabitemlistconv.h"

TabItemListConv::TabItemListConv(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    list = new QListWidget();
    layout->insertWidget(0, list);
}

void TabItemListConv::addSendMsg(QString text){
    QDateTime date = QDateTime::currentDateTime();
    QString fullText = date.toString("dd/MM/yy hh:mm");
    fullText += ": -> " + text;
    QListWidgetItem *newItem = new QListWidgetItem(text);
    newItem->setForeground(Qt::blue);
    list->addItem(newItem);
}

void TabItemListConv::addReceiveMsg(QDateTime date, QString text){
    QString fullText = date.toString("dd/MM/yy hh:mm");
    fullText += ": <- " + text;
    QListWidgetItem *newItem = new QListWidgetItem(text);
    newItem->setForeground(Qt::red);
    list->addItem(newItem);
}

TabItemListConv::~TabItemListConv()
{
    delete list;
}
