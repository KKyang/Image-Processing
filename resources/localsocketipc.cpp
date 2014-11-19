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
    m_socket->connectToServer(m_serverName);
}


void LocalSocketIpcClient::socket_connected(){
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);
    out << m_message;
    out.device()->seek(0);
    m_socket->write(block);
    m_socket->flush();
}

void LocalSocketIpcClient::socket_disconnected() {
#ifdef _DEBUG
    qDebug() << "socket_disconnected";
#endif
    emit socketClientStatus(1);
}


void LocalSocketIpcClient::socket_readReady() {
#ifdef _DEBUG
    qDebug() << "socket_readReady";
#endif
    emit socketClientStatus(0);
}

void LocalSocketIpcClient::socket_error(QLocalSocket::LocalSocketError) {
#ifdef _DEBUG
    qDebug() << "socket_error";
#endif
    emit socketClientStatus(2);
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
    {
       clientConnection->waitForReadyRead();
    }

    connect(clientConnection, SIGNAL(disconnected()),
            clientConnection, SLOT(deleteLater()));

    QDataStream in(clientConnection);
    in.setVersion(QDataStream::Qt_5_3);
    if (clientConnection->bytesAvailable() < (int)sizeof(quint64)) {
        return;
    }
    QString message;
    in >> message;
    emit messageReceived(message);
}
