#include "tcpconnection.h"

TCPConnection::TCPConnection(QObject *parent) : QObject(parent)
{
    Q_UNUSED(parent);
    qDebug() << this << "created";
    active();
}

TCPConnection::~TCPConnection()
{
    qDebug() << this << "destroyed";
}

int TCPConnection::idleTime()
{
    return activity.secsTo(QTime::currentTime());
}

void TCPConnection::quit()
{
    qDebug() << this << "Quit called, closing client";

    foreach(QTcpSocket *socket, sockets)
    {
        socket->close();
    }

    emit closed();
}

void TCPConnection::accept(qintptr descriptor)
{
    qDebug() << this << " accepting " << descriptor;

    QTcpSocket* socket = createSocket();
    if(!socket)
    {
        qWarning() << this << "could not find created socket!";
    }

    sockets.append(socket);

    if(!socket->setSocketDescriptor(descriptor))
    {
        qDebug() << this << " setSocketDescriptor failed " << descriptor;
        return;
    }

    qDebug() << this << " accepted " << descriptor;
    activity = QTime::currentTime();
    emit opened();
}

void TCPConnection::connected()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    if(!socket) return;

    qDebug() << this << " connected";
    activity = QTime::currentTime();
    emit opened();
}

void TCPConnection::disconnected()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    if(!socket) return;

    qDebug() << this << socket << " disconnected";

    sockets.removeAll(socket);
    socket->deleteLater();
    //delete socket;

    qDebug() << this << "Notify Runnable of closure";
    activity = QTime::currentTime();
    emit closed();
}

void TCPConnection::readyRead()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    if(!socket) return;

    //QByteArray data = socket->readAll();

    qDebug() << this << socket << " TCPConnection::readyRead" << socket->bytesAvailable();
    active();
}

void TCPConnection::bytesWritten(qint64 bytes)
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    if(!socket) return;

    qDebug() << socket << " bytesWritten " << bytes;
    active();
}

void TCPConnection::stateChanged(QAbstractSocket::SocketState socketState)
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    if(!socket) return;

    qDebug() << socket << " stateChanged " << socketState;
    active();
}

void TCPConnection::error(QAbstractSocket::SocketError socketError)
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    if(!socket) return;

    qDebug() << socket << " error " << socketError;
    active();
}

QTcpSocket *TCPConnection::createSocket()
{
    qDebug() << this << " creating socket";
    QTcpSocket *socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected,this, &TCPConnection::connected, Qt::QueuedConnection);
    connect(socket, &QTcpSocket::disconnected,this, &TCPConnection::disconnected, Qt::QueuedConnection);
    connect(socket, &QTcpSocket::readyRead,this, &TCPConnection::readyRead, Qt::QueuedConnection);
    connect(socket, &QTcpSocket::bytesWritten,this, &TCPConnection::bytesWritten, Qt::QueuedConnection);
    connect(socket, &QTcpSocket::stateChanged,this, &TCPConnection::stateChanged, Qt::QueuedConnection);
    connect(socket,static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),this,&TCPConnection::error, Qt::QueuedConnection);

    return socket;
}

void TCPConnection::active()
{
    qDebug() << this << "socket active";
    activity = QTime::currentTime();
}


