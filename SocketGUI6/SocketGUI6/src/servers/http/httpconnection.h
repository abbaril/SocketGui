#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include <QObject>
#include <src/servers/tcp/tcpconnection.h>

class HTTPConnection : public TCPConnection
{
    Q_OBJECT
public:
    explicit HTTPConnection(QObject *parent = 0);

signals:

public slots:

protected:
    void readyRead();
};

#endif // HTTPCONNECTION_H
