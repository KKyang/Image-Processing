#ifndef LOCALSOCKETIPC_H
#define LOCALSOCKETIPC_H

#include <QObject>
#include <QImage>
#include <QtNetwork>
#include "opencv2/opencv.hpp"

class LocalSocketIpcClient : public QObject
{
    Q_OBJECT
public:
    LocalSocketIpcClient(QString remoteServername, QObject *parent = 0);
    ~LocalSocketIpcClient();

signals:

public slots:
    void sendMessageToServer(QString message);

    void socket_connected();
    void socket_disconnected();

    void socket_readReady();
    void socket_error(QLocalSocket::LocalSocketError);

private:
    QLocalSocket*   m_socket;
    quint16         m_blockSize;
    QString         m_serverName;

    //Save data
    QString         m_message;
};

class LocalSocketIpcServer: public QObject
{
    Q_OBJECT
public:
    LocalSocketIpcServer(QString servername, QObject *parent);
    ~LocalSocketIpcServer();

signals:
    void messageReceived(QString);

public slots:
    void socket_new_connection();

private:
    QLocalServer*       m_server;
};

#endif // LOCALSOCKETIPC_H
