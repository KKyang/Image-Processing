#include "localsocketipc.h"

#include <QtNetwork>
#include <QDebug>

LocalSocketIpcClient::LocalSocketIpcClient(QString remoteServername, QObject *parent) :
        QObject(parent) {

    m_socket = new QLocalSocket(this);
    m_serverName = remoteServername;

    connect(m_socket, SIGNAL(connected()), this, SLOT(socket_connected()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(socket_disconnected()));

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(socket_readReady()));
    connect(m_socket, SIGNAL(error(QLocalSocket::LocalSocketError)),
            this, SLOT(socket_error(QLocalSocket::LocalSocketError)));
}

LocalSocketIpcClient::~LocalSocketIpcClient() {
    m_socket->abort();
    delete m_socket;
    m_socket = NULL;
}

void LocalSocketIpcClient::sendMessageToServer(QString message) {
    m_socket->abort();
    m_message = message;
    client_status = 0;
    m_socket->connectToServer(m_serverName);
}

void LocalSocketIpcClient::sendImageToServer(cv::Mat &image)
{
    m_socket->abort();
    if(image.type()==CV_8UC3)
        m_image = QImage(image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
    else if(image.type()==CV_8UC1)
    {
        m_image = QImage(image.data, image.cols, image.rows, image.step, QImage::Format_Indexed8);
    }
    client_status = 1;
    m_socket->connectToServer(m_serverName);
}


void LocalSocketIpcClient::socket_connected(){
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    if(client_status == 0)
        out << m_message;
    else if(client_status == 1)
        out << m_image;

    out.device()->seek(0);
    m_socket->write(block);
    m_socket->flush();
}

void LocalSocketIpcClient::socket_disconnected() {
    qDebug() << "socket_disconnected";
}


void LocalSocketIpcClient::socket_readReady() {
    qDebug() << "socket_readReady";
}

void LocalSocketIpcClient::socket_error(QLocalSocket::LocalSocketError) {
    qDebug() << "socket_error";
}

//////////////////////////////////////////////////////////////////////////////////////////////

LocalSocketIpcServer::LocalSocketIpcServer(QString servername, QObject *parent)
    :QObject(parent) {
    m_server = new QLocalServer(this);
    if (!m_server->listen(servername)) {
        qDebug() << "Not able to start the Server";
    }

    connect(m_server, SIGNAL(newConnection()), this, SLOT(socket_new_connection()));
}

LocalSocketIpcServer::~LocalSocketIpcServer() {

}


void LocalSocketIpcServer::socket_new_connection() {

    QLocalSocket *clientConnection = m_server->nextPendingConnection();

    while (clientConnection->bytesAvailable() < (int)sizeof(quint32))
        clientConnection->waitForReadyRead();


    connect(clientConnection, SIGNAL(disconnected()),
            clientConnection, SLOT(deleteLater()));

    QDataStream in(clientConnection);
    in.setVersion(QDataStream::Qt_4_0);
    if (clientConnection->bytesAvailable() < (int)sizeof(quint16)) {
        return;
    }
    if(server_status == 0)
    {
        QString message;
        in >> message;
        emit messageReceived(message);
    }
    else if(server_status == 1)
    {
        QImage image;
        in >> image;
        emit imageReceived(image);
    }
}
