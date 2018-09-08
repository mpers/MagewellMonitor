#include "QtWebSockets/qwebsocket.h"

#include <QtConcurrent/QtConcurrentRun>

#include <QDebug>
#include <QTimer>

#include <algorithm>
#include <stdexcept>
#include <thread>

#include "WebSockServer.h"

QT_USE_NAMESPACE

extern void handleError (std::exception_ptr error);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

QString doProcess (const QString& message);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

WebServer::WebServer (/*ConfigMgr* configMgr*/ QList<QHostAddress> ipList,
                      bool debug, QObject* parent)
//    : QObject(parent)
    : QWebSocketServer(QStringLiteral("Vulcan Magewell Monitor"),
                       QWebSocketServer::NonSecureMode, parent)
    , m_ipList(ipList)
    , m_debug(debug)
{
    QTimer::singleShot(0, this, &WebServer::init);

} // WebServer

////////////////////////////////////////////////////////////////////////////////

WebServer::~WebServer ()
{
    close();

    qDeleteAll(m_clients.begin(), m_clients.end());

} // ~WebServer

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void WebServer::init ()
{
    QHostAddress serverAddress;

    for (auto ip : m_ipList) {
        if (ip.toIPv4Address() && ip != QHostAddress::LocalHost) {
            serverAddress = ip;
            break;
        }
    }

    if (listen(serverAddress, 0)) {
        connect(this, &QWebSocketServer::newConnection,
                this, &WebServer::onNewConnection);
        connect(this, &QWebSocketServer::closed,
                this, &WebServer::closed);

        connect(this, &WebServer::replyReady, this,
                    [&](QWebSocket* client, const QString& reply) {
                        client->sendTextMessage(reply);
                    }, Qt::QueuedConnection);

        emit ready();
    }

} // init

////////////////////////////////////////////////////////////////////////////////

void WebServer::onNewConnection ()
{
    QWebSocket* client = nextPendingConnection();

    connect(client, &QWebSocket::textMessageReceived,
            this, &WebServer::processTextMessage);
    connect(client, &QWebSocket::binaryMessageReceived,
            this, &WebServer::processBinaryMessage);

    connect(client, &QWebSocket::disconnected, this,
            [&]() {
                QWebSocket* client = qobject_cast<QWebSocket *>(sender());
                if (client) {
                    m_clients.removeAll(client);
                    client->deleteLater();
                    if (m_debug)
                        qDebug() << "WebSocket disconnected:" << client
                                 << "(total" << m_clients.length() << "clients)";
                }
            });

    m_clients << client;

    if (m_debug)
        qDebug() << "New WebSocket connection:" << client
                 << "(total" << m_clients.length() << "clients)";

} // onNewConnection

////////////////////////////////////////////////////////////////////////////////

void WebServer::signalEvent (QVariant event)
{
    QVariant::Type type = event.type();

    switch (type) {
        case QVariant::List: {
        }
        break;
        case QVariant::Time: {
            QTime currentTime = event.toTime();
            QString strTime = currentTime.toString();
            for (auto client : m_clients)
                emit replyReady(client, strTime);
        }
        break;
        default:
            qDebug() << "Unrecognized event - type" << type;
    }

} // signalEvent

////////////////////////////////////////////////////////////////////////////////

void WebServer::processTextMessage (const QString& message)
{
    QWebSocket* client = qobject_cast<QWebSocket*>(sender());

    if (m_debug)
        qDebug() << "Text message received:" << message << "from" << client;

    if (client)
        asyncProcessText(message, [&](QString& result, std::exception_ptr error) {
            if (error)
                handleError(error);
            else
                emit replyReady(client, result);
        });

} // processTextMessage

////////////////////////////////////////////////////////////////////////////////

void WebServer::processBinaryMessage (const QByteArray& message)
{
    QWebSocket* client = qobject_cast<QWebSocket*>(sender());

    if (m_debug)
        qDebug() << "Binary message received:" << message;

    if (client)
        asyncProcessBin(message, [&](QString& result, std::exception_ptr error) {
            if (error)
                handleError(error);
            else
                emit replyReady(client, result);
        });
//        client->sendBinaryMessage(message);

} // processBinaryMessage

////////////////////////////////////////////////////////////////////////////////

void WebServer::asyncProcessText (const QString& msg, ResponseCallback callback)
{
    std::thread( [&, msg, callback] {
        QString response;
        try
        {
            response = doProcess(msg);
            callback(response, nullptr);
        }
        catch ( ... )
        {
            callback(response, std::current_exception());
        }
    } ).detach();

} // asyncProcessText

////////////////////////////////////////////////////////////////////////////////

void WebServer::asyncProcessBin (const QByteArray& msg, ResponseCallback callback)
{
    std::thread( [&, msg, callback] {
        QString response;
        try
        {
            response = QString("Binary message processed"); // doProcessBin(msg);
            callback(response, nullptr);
        }
        catch ( ... )
        {
            callback(response, std::current_exception());
        }
    } ).detach();

} // asyncProcessBin

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

QString doProcess (const QString& message)
{
    QByteArray reverse = message.toUtf8();

    std::string reverseStr(reverse);
    std::reverse(reverseStr.begin(), reverseStr.end());

    return QString(reverseStr.c_str());

} // doProcess
