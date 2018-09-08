#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTcpServer>

#include <functional>

#include "ConfigMgr.h"

QT_FORWARD_DECLARE_CLASS(QTcpSocket)

////////////////////////////////////////////////////////////////////////////////

typedef std::function< void (QString&, std::exception_ptr) > ResponseCallback;

////////////////////////////////////////////////////////////////////////////////

class HttpServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit HttpServer (bool debug, QObject *parent = nullptr);
    ~HttpServer ();

    void    setWebSocketPort (quint16 port) { m_webSocketPort = port; }

signals:
    void    ready ();

public slots:

private slots:
    void    init ();

    void    onNewConnection ();
    void    onReadyRead ();

private:
    QTcpSocket*     m_client;

    quint16         m_webSocketPort;

    bool            m_debug;
};

#endif // HTTPSERVER_H
