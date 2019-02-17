#ifndef TCPRUNNABLE_H
#define TCPRUNNABLE_H

#include <QObject>
#include <QDebug>
#include <QRunnable>
#include <QThread>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QEventLoop>
#include <QThread>
#include "tcpconnection.h"

/**
 * @brief The TCPRunnable class
 * @details Used in in thread operations
 */
class TCPRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:

    /**
     * @brief TCPRunnable constructor
     * @param parent The parent QObject
     */
    explicit TCPRunnable(QObject *parent = 0);

    /**
     * @brief TCPRunnable deconstructor
     */
    ~TCPRunnable();

    /**
     * @brief Called from the thread pool to start the runnable on a thread
     */
    void run();

    /**
     * @brief Returns the client count
     * @return int
     */
    int count();

    /**
     * @brief Sets the thread mode
     * @param value Bool determines if the runnable will finish when the socket closes
     */
    void setThreadedMode(bool value);

signals:
    /**
     * @brief Emitted when the runnable is started on a thread
     */
    void started();

    /**
     * @brief Emitted when the runnable is finished running and can be deleted
     */
    void finished();

    /**
     * @brief Emitted when the QEventLoop needs to quit
     */
    void quit();


public slots:

    /**
     * @brief Called when a client is waiting to be accepted and added to the runnables client list
     * @param handle
     * @param runnable
     */
    //void accept(qintptr handle, TCPRunnable *runnable);

    /**
     * @brief Called when a new client is connecting
     * @param handle The socket descriptor
     * @param runnable The runnable that should handle the connection
     * @param connection The connection object
     */
    void connecting(qintptr handle, TCPRunnable *runnable, TCPConnection* connection);

    /**
     * @brief Called when idle connections should be removed
     * @param value
     */
    void idle(int value);

    /**
     * @brief Called when the server is closing
     */
    void closing();

    /**
     * @brief Called when the client has opened a connection
     */
    void opened();

    /**
     * @brief Called when the client has closed
     */
    void closed();


protected:

    /**
     * @brief Determines if the runnable will finish when the socket is closed
     */
    bool isThreaded;

    /**
     * @brief List of TCPConnections running on this runnable
     */
    QList<TCPConnection*> connections;

    /**
     * @brief Event loop used to keep the runnable active on the thread
     */
    QEventLoop* loop;

    /**
     * @brief Used for cross thread operations
     */
    QReadWriteLock lock;

    /**
     * @brief Creates the client connection
     * @return
     */
    TCPConnection *createConnection();

    /**
     * @brief Connects the Signals and Slots
     * @param TCPConnection connection
     */
    void addSignals(TCPConnection* connection);
};

#endif // TCPRUNNABLE_H
