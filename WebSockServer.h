#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <QByteArray>
#include <QList>
#include <QObject>
#include <QWebSocketServer>
//#include "QtWebSockets/qwebsocketserver.h"

#include <functional>

#include "ConfigMgr.h"

QT_FORWARD_DECLARE_CLASS(QWebSocket)

////////////////////////////////////////////////////////////////////////////////

typedef std::function< void (QString&, std::exception_ptr) > ResponseCallback;

////////////////////////////////////////////////////////////////////////////////

class WebServer : public QWebSocketServer
{
    Q_OBJECT

public:
    explicit WebServer (/*ConfigMgr* configMgr*/ QList<QHostAddress> ipList,
                        bool debug = false,
                        QObject *parent = Q_NULLPTR);
    ~WebServer ();

signals:
    void    ready ();
    void    closed ();
    void    replyReady (QWebSocket* client, const QString& reply);

public slots:
    void    signalEvent (QVariant event);

private slots:
    void    init ();

    void    onNewConnection ();

    void    processTextMessage (const QString& message);
    void    processBinaryMessage (const QByteArray& message);

//    void    socketDisconnected ();


private:
    void    asyncProcessText (const QString& msg, ResponseCallback callback);
    void    asyncProcessBin (const QByteArray& msg, ResponseCallback callback);

private:
    QList<QHostAddress> m_ipList;

    QList<QWebSocket*>  m_clients;

    bool                m_debug;
};

#endif // WEBSERVER_H
