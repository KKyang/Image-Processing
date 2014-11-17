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
    void changeClientStatus(int status){client_status = status;}

signals:

public slots:
    void sendMessageToServer(QString message);
    void sendImageToServer(cv::Mat &image);

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
    QImage          m_image;
    //Status 0 - on sending message, 1 - on sending image.
    int             client_status = 0;
};

class LocalSocketIpcServer: public QObject
{
    Q_OBJECT
public:
    LocalSocketIpcServer(QString servername, QObject *parent);
    ~LocalSocketIpcServer();
    void changeServerStatus(int status){server_status = status;}

signals:
    void messageReceived(QString);
    void imageReceived(QImage);

public slots:
    void socket_new_connection();

private:
    QLocalServer*       m_server;
    //Status 0 - on listening, 1 - on receiving image.
    int             server_status = 0;
};

#endif // LOCALSOCKETIPC_H
