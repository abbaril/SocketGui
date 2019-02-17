#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QTime>

/**
 * @brief The TCPConnection class
 * @details The tcp connection wraps the socket and performs operations
 */
class TCPConnection : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief TCPConnection constructor
     * @param parent The parent QObject
     */
    explicit TCPConnection(QObject *parent = 0);

    /**
     * @brief TCPConnection deconstructor
     */
    ~TCPConnection();

    /**
     * @brief lastActivity
     * @return int The number of seconds the socket has been idle
     */
    int idleTime();

signals:
    /**
     * @brief Emitted when the connection is opened
     */
    void opened();

    /**
     * @brief Emitted when the connection has closed
     */
    void closed();

public slots:

    /**
     * @brief Called when the runnable quits
     */
    virtual void quit();

    /**
     * @brief Called when the client should be accepted
     * @param descriptor The socket descriptor
     */
    virtual void accept(qintptr descriptor);

    /**
     * @brief Called when the client is connected
     */
    virtual void connected();

    /**
     * @brief Called when the client has disconnected
     */
    virtual void disconnected();

    /**
     * @brief Called when the client has data ready to read
     */
    virtual void readyRead();

    /**
     * @brief Called when the client has written bytes
     * @param bytes The number of bytes written
     */
    virtual void bytesWritten(qint64 bytes);

    /**
     * @brief Called when the clients state has changed
     * @param socketState The state enumerator
     */
    virtual void stateChanged(QAbstractSocket::SocketState socketState);

    /**
     * @brief Called when there is a socket error on the client
     * @param socketError The socket error enumerator
     */
    virtual void error(QAbstractSocket::SocketError socketError);

protected:

    /**
     * @brief List of sockets used for this connection
     */
    QList<QTcpSocket*> sockets;

    /**
     * @brief Last time the socket was active
     */
    QTime activity;

    /**
     * @brief Creates the client socket
     * @return QTcpSocket*
     */
    QTcpSocket* createSocket();

    /**
     * @brief Updates the last activity time
     */
    void active();
};

#endif // TCPCONNECTION_H
