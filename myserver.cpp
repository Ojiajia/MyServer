#include <QObject>
#include <QtWidgets>
#include <QtCore>
#include <QtNetwork>
#include <myserver.h>
#include <QList>
#include <stdio.h>
#include <stdlib.h>
#include <QListIterator>
#include <QtXml>
#include <QDebug>
#include <QtXml>

//nPort - номер порта
MyServer::MyServer(int nPort, QWidget* pwgt /*=0*/) : QWidget(pwgt)
                                                    , m_nNextBlockSize(0)
{
    m_ptcpServer = new QTcpServer(this);
    //вызов метода listen() для запуска сервера
    if (!m_ptcpServer->listen(QHostAddress::Any, nPort)) {
        // !m_ptcpServer->... ! дает результат переменная типа bool
        QMessageBox::critical(0,
                              "Server Error",
                              "Unable to start the server:"
                              + m_ptcpServer->errorString()
                              );
        m_ptcpServer->close();
        return;
    }
    connect(m_ptcpServer, SIGNAL(newConnection()),
    this,                 SLOT(slotNewConnection())
    );

    m_ptxt = new QTextEdit;    // виджет многострочного текстового поля
    m_ptxt->setReadOnly(true); // режим только просмотра информации

    //Layout setup

    // чтобы привязать компоновщик к виджету, нужно создать объект
    // компоновщика, а потом передать его в метод виджета setLayout().

    QVBoxLayout* pvbxLayout = new QVBoxLayout; //pvbxLayout устанавливает
                                               //себя компоновщиком для
                                               //QVBoxLayout
    pvbxLayout->addWidget(new QLabel("<H1>Server</H1>"));
    pvbxLayout->addWidget(m_ptxt);
    setLayout(pvbxLayout);

}

//***********************************************************************


// вызывается каждый раз при соединении с новым клиентом
/*virtual*/ void MyServer::slotNewConnection()
{
// для подтверждения соединения с клиентом вызывается метод
// nextPendingConnection(), который возвратит сокет
    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();



    socList.append(pClientSocket);
    numberOfUsers = numberOfUsers + 1;

    //занести в qmap ключ (сокет)

    //qDebug() << "nick from server slotNewConnection:" << str2;
    //qDebug() << "nick: " <<nickList.at(numberOfUsers) << "socket: " << socList;

   // qDebug() << "local port = " << pClientSocket->localPort();
    connect(pClientSocket, SIGNAL(disconnected()), //standart
            pClientSocket, SLOT(deleteLater())     //standart
           );

    connect(pClientSocket, SIGNAL(readyRead()),    //standart
            this,          SLOT(saveNick())  //описан ниже отдельно
           );



//  saveNick();
    // описан ниже отдельно
    sendToClient(pClientSocket, "Server Response:","You are connected!");

}

//***********************************************************************

void MyServer::slotReadClient()
{
//    printf("hi\n1\n2\n3\n");
//    fflush(stdout);

     // преобразование указателя,возвращаемого методом sender()
     // к типу QTcpSocket:
     QTcpSocket* pClientSocket = (QTcpSocket*)sender();
     if (usersList.contains(pClientSocket)) {
         qDebug() << "slotReadClient" << usersList.value(pClientSocket);
     }
     QDataStream in(pClientSocket);      // in ???

     in.setVersion(QDataStream::Qt_5_3); // ???
     for (;;) { // бесконечный цикл
         if (!m_nNextBlockSize) { // если 0 т.е. неизвестен
             if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
                 // sizeof(quint16)) = 2 байта
                 // размер полученных данных не меньше двух байтов.
                 // bytesAvailable() возвращает число байт,
                 // доступных для чтения.
                 break;
             }
             in >> m_nNextBlockSize; // считывается размер блока
         }
         // размер доступных для чтения данных >/= размеру блока
         if (pClientSocket->bytesAvailable() < m_nNextBlockSize) {
         break;
         }

         QTime   time;
         in >> time >> str1 >> str2; // переменная in - объект потока QDataStream         
         QString strMessage =
              time.toString() +  " Client " + str2 + " sent: " + str1;
         // time.toString() - время в строку
         m_ptxt->append(strMessage);
         // m_ptxt - (виджет) длина следующего текстового блока
         // strMessage добавляется в m_ptxt
         m_nNextBlockSize = 0; // размер очередного текстового блока
                                // неизвестен
         // сообщение клиенту о прочтении данных

        // sendToClient(pClientSocket,"Server Response: " + str2, " sent: " + str1);


         QList<QTcpSocket*>::iterator it = socList.begin();
         while (it != socList.end()) {       
         sendToClient(*it,str2 + ": ",str1);
         ++it;
         }
     }
}


//***********************************************************************

// формирование данных для отсылки клиенту

// заранее неизвестен размер блока -> нельзя сразу записать данные
// в сокет, т.к. размер блока должен быть записан в первую очередь

void MyServer::sendToClient(QTcpSocket* pSocket,QString str)
 {
    QByteArray  arrBlock; // массив байтов
    QDataStream out(&arrBlock, QIODevice::WriteOnly);

// Класс QDataStream обеспечивает сериализацию
// двоичных данных в QIODevice .
// cериализация это процесс сохранения состояния объекта
// в последовательность байт;
// Создает поток данных, который работает с массивом байтов arrBlock
// QIODevice::WriteOnly описывает,kак устройство должно быть
// использовано.

    out.setVersion(QDataStream::Qt_5_3);
    out << quint16(0) << QTime::currentTime() << str << str1;
    out.device()->seek(0); // перемещение указателя на начало блока
    out << quint16(arrBlock.size() - sizeof(quint16));
    // arrBlock.size() - размер блока - 2 байта,+запись в поток out
    pSocket->write(arrBlock);
}

void MyServer::saveNick()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_5_3);
    for (;;) {
        if (!m_nNextBlockSize) { // если 0 т.е. неизвестен
            if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
                return;
            }
            in >> m_nNextBlockSize; // считывается размер блока
        }
        // размер доступных для чтения данных >/= размеру блока
        if (pClientSocket->bytesAvailable() < m_nNextBlockSize) {
        return;
        }

        in >> str2;
        m_nNextBlockSize = 0;
       // qDebug() << "nick (str2): " <<  str2;
       nickList.append(str2);

       // каждый раз придобавлении нового клиента печатает список всех клиентов
       QListIterator<QString> iter(nickList);
        while(iter.hasNext()) {
        qDebug() << "Element:" << iter.next();
        }

//        QDomDocument domSaveNick;
        QString str= "<msg><domNickName>"+str2+"</domNickName></msg>";
//        QDomElement nickname = domSaveNick.createElement(str2);

        usersList.insert(pClientSocket, str2);

        disconnect(pClientSocket, SIGNAL(readyRead()),    //standart
                   this,          SLOT(saveNick()));
        connect(pClientSocket, SIGNAL(readyRead()),    //standart
                this,          SLOT(slotReadClient())  //чтение сообщения
                                                       //и отправка подтверждения
                                                       //о прочтении
               );
        break;
    }


}


