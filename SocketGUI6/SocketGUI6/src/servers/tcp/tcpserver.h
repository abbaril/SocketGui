#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QThreadPool>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QTimer>

#include "tcprunnable.h"

/**
 * @brief TCP Server class
 * @details Runs in three modes
 * 1 - Single - The connectoions share a single thread
 * 2 - Pooled  -The connections share a comon pool of threads
 * 3 - Threaded - The connections run in their own thread
 *
 * Seige test = siege 127.0.0.1:2000 -c 1000
 */
class TCPServer : public QTcpServer
{
    Q_OBJECT
public:

    /**
     * @brief TCPServer constructor
     * @param parent The QObject parent
     */
    explicit TCPServer(QObject *parent = 0);

    /**
     * @brief TCPServer deconstructor
     */
    ~TCPServer();

    /**
     * @brief The ThreadMode enum
     */
    enum ThreadMode {
        MODE_SINGLE = 0,    /**< All connections run on a single thread */
        MODE_POOLED = 1,    /**< All connections run in a pool of threads */
        MODE_THREADED = 2   /**< All connections run in their own thread */
    };

    /**     * @brief Sets the maximum threads allowed
     * @param value Max count
     */
    void setMaxThreads(int value);

    /**
     * @brief Sets the maximum connections allowed
     * @param value Max count
     */
    void setMaxConnections(int value);

    /**
     * @brief Sets the connection timeout for idle connections
     * @param value The max time in seconds
     */
    void setConnectionTimeout(int value);

    /**
     * @brief Sets the threading mode
     * @param value ThreadMode enum
     */
    void setMode(ThreadMode value);

    /**
     * @brief Listens for incomming connections
     * @param address The address to listen on
     * @param port The port to listen on
     */
    void listen(const QHostAddress &address, quint16 port);

    /**
     * @brief Closes the server, any connections will quit
     */
    void close();


    /**
     * @brief Return a list of possible IP addresses
     * @return QStringList
     */
    QStringList getAddresses();

signals:

    /**
     * @brief Emitted when an incomming connection is waiting to be accepted
     * @param handle The socket descriptor
     * @param runnable The runnable to accept the connection
     * @param connection The connection to accept
     */
    void connecting(qintptr handle, TCPRunnable *runnable, TCPConnection* connection);

    /**
     * @brief Emitted when the server is closing
     */
    void closing();

    /**
     * @brief Emitted when idle connections should be removed
     * @param value
     */
    void idle(int value);


public slots:
    /**
     * @brief Called when a runnable is started on a thread
     */
    void started();

    /**
     * @brief Called when a runnable has finished running on a thread
     */
    void finished();

    /**
     * @brief timeout Called when the servers timers timeout is emitted
     */
    void timeout();

protected:

    /**
     * @brief The thread mode to run in
     */
    ThreadMode threadMode = MODE_SINGLE;

    /**
     * @brief The maximum number of connections
     */
    int maxConnections = 0;

    /**
     * @brief Time out for idle connections, Zero is never
     */
    int connectionTimeout = 0;

    /**
     * @brief List of runnables that are on different threads
     */
    QList<TCPRunnable*> runnables;

    /**
     * @brief Timer for server operations
     */
    QTimer timer;

    /**
     * @brief Called when a connection is ready to be accepted
     * @param handle
     */
    virtual void incomingConnection(qintptr handle);

    /**
     * @brief Creates a runnable on a different thread
     * @return TCPRunnable*
     */
    virtual TCPRunnable *createRunnable();

    /**
     * @brief Connects the runnable and starts it
     * @param runnable
     */
    virtual void startRunnable(TCPRunnable *runnable);

    /**
     * @brief Rejects a connection
     * @param handle The socket descriptor of the connection
     */
    virtual void reject(qintptr handle);

    /**
     * @brief Accepts a connection in MODE_SINGLE
     * @param handle The socket descriptor of the connection
     */
    virtual void acceptSingle(qintptr handle);

    /**
     * @brief Accepts a connection in MODE_POOLED
     * @param handle The socket descriptor of the connection
     */
    virtual void acceptPooled(qintptr handle);

    /**
     * @brief Accepts a connection in MODE_THREADED
     * @param handle The socket descriptor of the connection
     */
    virtual void acceptThreaded(qintptr handle);

    /**
     * @brief Starts the runnables in MODE_SINGLE
     */
    virtual void startSingle();

    /**
     * @brief Starts the runnabled in MODE_POOLED
     */
    virtual void startPooled();

    /**
     * @brief Starts the runnabled in MODE_THREADED
     */
    virtual void startThreaded();

    /**
     * @brief Gets the number of connections on all runnables
     * @return int The connection count
     */
    virtual int connections();

    /**
     * @brief Emitted when an incomming connection is waiting to be accepted
     * @param handle The socket descriptor
     * @param runnable The runnable to accept the connection
     */
    virtual void accept(qintptr handle, TCPRunnable *runnable);

};

#endif // TCPSERVER_H
