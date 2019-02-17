#include "tcprunnable.h"

TCPRunnable::TCPRunnable(QObject *parent) : QObject(parent)
{
    Q_UNUSED(parent);
    qDebug() << this << "created";
    isThreaded = false;
}

TCPRunnable::~TCPRunnable()
{
    qDebug() << this << "destroyed";
}

void TCPRunnable::run()
{
    qDebug() << this << "started on " << QThread::currentThread();

    //Make an event loop to keep this alive on the thread
    loop = new QEventLoop();
    connect(this, &TCPRunnable::quit,loop, &QEventLoop::quit);
    loop->exec();

    qDebug() << this << "finished on " << QThread::currentThread();
    emit finished();

}

int TCPRunnable::count()
{
    QReadLocker locker(&lock);
    return connections.count();
}

void TCPRunnable::setThreadedMode(bool value)
{
    isThreaded = value;
}

//void TCPRunnable::accept(qintptr handle, TCPRunnable *runnable)
//{
//    //Dont accept if its not ment for this runnable
//    if(runnable != this) return;

//    TCPConnection *connection = createConnection();

//    connections.append(connection);
//    connection->accept(handle);
//}

void TCPRunnable::connecting(qintptr handle, TCPRunnable *runnable, TCPConnection *connection)
{
    //Dont accept if its not ment for this runnable
    if(runnable != this) return;

    qDebug() << this << "Connecting: " << handle << " on " << runnable << " with " << connection;

    connection->moveToThread(QThread::currentThread());

    connections.append(connection);
    addSignals(connection);
    connection->accept(handle);
}

void TCPRunnable::idle(int value)
{
    foreach(TCPConnection* connection, connections)
    {
        if(!connection) continue;

        int idle = connection->idleTime();
        qDebug() << this << connection << " idle for " << idle << " timeout is " << value;

        if(idle >= value)
        {
            qDebug() << this << "Closing idle connection" << connection;
            connection->quit();
        }
    }
}

void TCPRunnable::closing()
{
    emit quit();
}

void TCPRunnable::opened()
{
    TCPConnection *connection = static_cast<TCPConnection*>(sender());
    if(!connection) return;

    qDebug() << connection << "opened";
}

void TCPRunnable::closed()
{
    qDebug() << this << "Attempting closed";
    TCPConnection *connection = static_cast<TCPConnection*>(sender());
    if(!connection) return;

    qDebug() << connection << "closed";
    connections.removeAll(connection);

    qDebug() << this << "deleting" << connection;

    connection->deleteLater();
    //delete connection;

    if(isThreaded)
    {
        qDebug() << this << "**** Quitting runnable" << isThreaded;
        emit quit();
    }
}

TCPConnection *TCPRunnable::createConnection()
{
    TCPConnection *connection = new TCPConnection();

    qDebug() << this << "created" << connection;

    return connection;
}

void TCPRunnable::addSignals(TCPConnection *connection)
{
    qDebug() << this << "connecting signals and slots" << connection;
    connect(connection,&TCPConnection::opened,this,&TCPRunnable::opened, Qt::QueuedConnection);
    connect(connection,&TCPConnection::closed,this,&TCPRunnable::closed, Qt::QueuedConnection);
    connect(this,&TCPRunnable::quit,connection,&TCPConnection::quit, Qt::QueuedConnection);
}
