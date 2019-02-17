#include "tcpserver.h"

TCPServer::TCPServer(QObject *parent) : QTcpServer(parent)
{
    qDebug() << this << "created";

    //Connect the server timer
    connect(&timer,&QTimer::timeout,this,&TCPServer::timeout);
}

TCPServer::~TCPServer()
{
    qDebug() << this << "destroyed";
}

void TCPServer::setMaxThreads(int value)
{
    qDebug() << this << "Setting max threads to: " << value;
    QThreadPool::globalInstance()->setMaxThreadCount(value);
}

void TCPServer::setMaxConnections(int value)
{
    qDebug() << this << "Setting max connections to: " << value;
    maxConnections = value;
}

void TCPServer::setConnectionTimeout(int value)
{
    qDebug() << this << "Setting the connection timeout to: " << value;
    connectionTimeout = value;
}

void TCPServer::setMode(TCPServer::ThreadMode value)
{
    qDebug() << this << "Setting thread mode to: " << value;
    threadMode = value;
}

void TCPServer::listen(const QHostAddress &address, quint16 port)
{
    if(!QTcpServer::listen(address,port))
    {
        qCritical() << this << errorString();
        return;
    }

    qDebug() << this << "Listing on port:" << port;

    switch (threadMode) {
    case MODE_SINGLE:
        startSingle();
        break;
    case MODE_POOLED:
         startPooled();
        break;
    case MODE_THREADED:
        startThreaded();
        break;
    }

    timer.start(1000);

}

void TCPServer::close()
{
    qDebug() << this << "Closing Server, and all open connections.";
    emit closing();
    QTcpServer::close();
    timer.stop();
    qDebug() << this << "Server has closed.";
}

QStringList TCPServer::getAddresses()
{
    QStringList lst;

    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
        {
            lst.append(address.toString());
        }
    }

    return lst;

}

void TCPServer::incomingConnection(qintptr handle)
{

    //Determine if we can accept the connection
    int count = connections();
    if(maxConnections != 0 && count >= maxConnections)
    {
        //FIX IT NOOOOOOOOOOOOOOOWWWWWWW
        qDebug() << this << "************************ Rejecting connection:" << handle;
        qDebug() << this << " Count: " << count << " Max:" << maxConnections;
        reject(handle);
        return;
    }

    //Accept the connection
    switch (threadMode) {
    case MODE_SINGLE:
        acceptSingle(handle);
        break;
    case MODE_POOLED:
        acceptPooled(handle);
        break;
    case MODE_THREADED:
        acceptThreaded(handle);
        break;
    default:
        break;
    }
}

void TCPServer::started()
{
    TCPRunnable *runnable = static_cast<TCPRunnable*>(sender());
    if(!runnable) return;
    qDebug() << runnable << "has started";
}

void TCPServer::finished()
{
    qDebug() << this << "finished" << sender();
    TCPRunnable *runnable = static_cast<TCPRunnable*>(sender());
    if(!runnable) return;
    qDebug() << runnable << "has finished, removing from list";

    runnables.removeAll(runnable);
    runnable->deleteLater();
    //delete runnable;
}

void TCPServer::timeout()
{
    //int count = connections();
    //qDebug() << this <<  "Server is running " << count << " connections";

    if(connectionTimeout > 0) emit idle(connectionTimeout);

}

TCPRunnable *TCPServer::createRunnable()
{
    qDebug() << this << "Creating runnable...";

    TCPRunnable *runnable = new TCPRunnable();
    runnable->setAutoDelete(false);

    return runnable;
}

void TCPServer::startRunnable(TCPRunnable *runnable)
{
    if(!runnable)
    {
        qWarning() << this << "Runnable is null!";
        return;
    }

    qDebug() << this << "Starting " << runnable;

    runnable->setAutoDelete(false);

    runnables.append(runnable);

    qDebug() << this << "Connecting signal and slots";
   // connect(this,&TCPServer::accept, runnable,&TCPRunnable::accept, Qt::QueuedConnection);
    connect(this,&TCPServer::closing,runnable, &TCPRunnable::closing, Qt::QueuedConnection);
    connect(runnable,&TCPRunnable::started, this, &TCPServer::started, Qt::QueuedConnection);
    connect(runnable,&TCPRunnable::finished, this, &TCPServer::finished, Qt::QueuedConnection);
    connect(this,&TCPServer::connecting, runnable,&TCPRunnable::connecting, Qt::QueuedConnection);
    connect(this,&TCPServer::idle, runnable, &TCPRunnable::idle, Qt::QueuedConnection);

    qDebug() << this << "created " << runnable;

    if(threadMode == MODE_THREADED) runnable->setThreadedMode(true);

    QThreadPool::globalInstance()->start(runnable);
}

void TCPServer::reject(qintptr handle)
{
    qDebug() << this << "************************ Rejecting connection: " << handle;

    QTcpSocket *socket = new QTcpSocket(this);
    socket->setSocketDescriptor(handle);
    socket->close();
    socket->deleteLater();
}

void TCPServer::acceptSingle(qintptr handle)
{
    //All connections run on a single thread

    qDebug() << this << "Accepting in single mode:" << handle;

    if(runnables.count() == 0)
    {
        qCritical() << this << "no runnables to accept connection: " << handle;
        return;
    }

    TCPRunnable *runnable = runnables.at(0);
    if(!runnable)
    {
        qCritical() << this << "could not find runnable to accept connection: " << handle;
        return;
    }

    /*
    TCPConnection *connection = createConnection();
    if(!connection)
    {
        qCritical() << this << "could not find connection to accept connection: " << handle;
        return;
    }

    //emit accept(handle,runnable);
    emit connecting(handle,runnable,connection);
    */

     accept(handle,runnable);
}

void TCPServer::acceptPooled(qintptr handle)
{
    //All connection run in a pool of threads
    qDebug() << this << "Accepting in pooled mode:" << handle;

    int previous = 0;
    TCPRunnable *runnable = runnables.at(0);

    foreach(TCPRunnable *item, runnables)
    {
        int count = item->count();

        if(count == 0 || count < previous)
        {
            runnable = item;
            break;
        }

        previous = count;
    }

    if(!runnable)
    {
        qWarning() << this << "Could not find runable!";
        return;
    }

    /*
    TCPConnection *connection = createConnection();
    if(!connection)
    {
        qCritical() << this << "could not find connection to accept connection: " << handle;
        return;
    }

    //emit accept(handle,runnable);
    emit connecting(handle,runnable,connection);

    */

     accept(handle,runnable);
}

void TCPServer::acceptThreaded(qintptr handle)
{
    //Each connection runs in its own thread

    qDebug() << this << "Accepting in threaded mode:" << handle;

    TCPRunnable *runnable = createRunnable();
    if(!runnable)
    {
        qWarning() << this << "Could not find runable!";
        return;
    }

    startRunnable(runnable);

    /*
    TCPConnection *connection = createConnection();
    if(!connection)
    {
        qCritical() << this << "could not find connection to accept connection: " << handle;
        return;
    }

    //emit accept(handle,runnable);
    emit connecting(handle,runnable,connection);
    */

    accept(handle,runnable);
}

void TCPServer::startSingle()
{
    qDebug() << this << "MODE_SINGLE - starting one runnable";
    QThreadPool::globalInstance()->setMaxThreadCount(1);

    TCPRunnable *runnable = createRunnable();
    if(!runnable)
    {
        qWarning() << this << "Could not find runable!";
        return;
    }

    startRunnable(runnable);
}

void TCPServer::startPooled()
{
    qDebug() << this << "MODE_POOLED - starting all runnables";

    QThreadPool pool;
    QThreadPool::globalInstance()->setMaxThreadCount(pool.maxThreadCount());
    for(int i = 0; i < QThreadPool::globalInstance()->maxThreadCount(); i++)
    {
        TCPRunnable *runnable = createRunnable();
        if(!runnable)
        {
            qWarning() << this << "Could not find runable!";
            return;
        }

        startRunnable(runnable);
    }
}

void TCPServer::startThreaded()
{
    qDebug() << this << "MODE_THREADED - runnables will be created as connections are accepted";
    QThreadPool::globalInstance()->setMaxThreadCount(maxConnections);
}

int TCPServer::connections()
{
    int count = 0;
    foreach(TCPRunnable *item, runnables)
    {
        int value = item->count();
        //qDebug() << this << " On " << item << "Adding count: " << value << " += " << count;
        count += value;
       // qDebug() << this << " On " << item << "Total count: " << count;
    }

    return count;
}

void TCPServer::accept(qintptr handle, TCPRunnable *runnable)
{
    qDebug() << "Called in TCP";
    qDebug() << this << "accepting" << handle << "on" << runnable;

    TCPConnection *connection = new TCPConnection;
    if(!connection)
    {
        qCritical() << this << "could not find connection to accept connection: " << handle;
        return;
    }

    emit connecting(handle,runnable,connection);
}

