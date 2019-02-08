#ifndef MYSERVER_H
#define MYSERVER_H
#pragma once
#include <QObject>
#include <QtWidgets>
#include <QtCore>
#include <QtNetwork>
#include <QList>

class QTcpServer;
class QTextEdit;
class QTcpSocket;
// ======================================================================
class MyServer : public QWidget {
Q_OBJECT
private:
    QTcpServer* m_ptcpServer;     //атрибут
    QTextEdit*  m_ptxt;           //длина следующего текстового
                                  //блока от сокета
    quint16     m_nNextBlockSize; //многострочное текстовое поле
                                  //для информирования о происходящих
                                  //соединениях
private:

    void sendToClient(QTcpSocket* pSocket, QString str2, QString str1);

    QList<QTcpSocket*> socList;
    QList<QTcpSocket*> iterator;
    int it;
    QList<QString> nickList;
    int i;
    QMap<QTcpSocket*,QString> usersList;

public:

    MyServer(int nPort, QWidget* pwgt = 0);
    int numberOfUsers = 0;
    QString str1; // message
    QString str2; // nickname

public slots:

    virtual void slotNewConnection();
            void slotReadClient();
            void saveNick();
};
#endif // MYSERVER_H
