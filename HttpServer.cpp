#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>

#include <QDebug>
#include <QTimer>

#include "HttpServer.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

HttpServer::HttpServer (bool debug, QObject *parent)
    : QTcpServer(parent)
    , m_client(Q_NULLPTR)
    , m_webSocketPort(0)
    , m_debug(debug)
{
    QTimer::singleShot(0, this, &HttpServer::init);

} // HttpServer

////////////////////////////////////////////////////////////////////////////////

HttpServer::~HttpServer()
{
    if (m_client)
        m_client->close();

} // ~HttpServer

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void HttpServer::init ()
{
    if (listen(QHostAddress::Any, 2323)) {
        connect(this, &QTcpServer::newConnection,
                this, &HttpServer::onNewConnection);

        emit ready();
    }

} // init

////////////////////////////////////////////////////////////////////////////////

void HttpServer::onNewConnection ()
{
    m_client = nextPendingConnection();

    connect(m_client, &QTcpSocket::readyRead, this, &HttpServer::onReadyRead);
    connect(m_client, &QTcpSocket::disconnected, this, [&]() {
        m_client->deleteLater();
        m_client = Q_NULLPTR;
    });

    if (m_debug)
        qDebug() << "New HTTP connection:" << m_client;

} // onNewConnection

////////////////////////////////////////////////////////////////////////////////

void HttpServer::onReadyRead ()
{
    while (m_client->bytesAvailable()) {
        QByteArray request = m_client->readLine(1024).trimmed();
        QList<QByteArray> list = request.split(' ');

        if (list.count() != 3 || !list.at(2).contains("HTTP")) {
            qWarning("HttpRequest: received broken HTTP request, invalid first line");
//            status=abort;
        }
        else {
            QByteArray method  = list.at(0).trimmed();
            QByteArray path    = list.at(1);
            QByteArray version = list.at(2);

            QHostAddress peerAddress = m_client->peerAddress();
//            status=waitForHeader;
            int n = 17;
        }

        QMultiMap<QByteArray,QByteArray> headers;

        QByteArray header  = m_client->readLine(1024).trimmed();
        int colon = header.indexOf(':');
        if (colon > 0) {
            // Received a line with a colon - a header
            QByteArray currentHeader = header.left(colon).toLower();
            QByteArray value = header.mid(colon+1).trimmed();
            headers.insert(currentHeader,value);
            qDebug("HttpRequest: received header %s: %s", currentHeader.data(), value.data());
        }
        else if (!header.isEmpty()) {
            // received another line - belongs to the previous header
            qDebug("HttpRequest: read additional line of header");

            // Received additional line of previous header
//            if (headers.contains(currentHeader)) {
//                headers.insert(currentHeader, headers.value(currentHeader) + " " + newData);
//            }
        }
        else {
            // received an empty line - end of headers reached
            qDebug("HttpRequest: headers completed");

            // Empty line received, that means all headers have been received
            // Check for multipart/form-data
            QByteArray contentType = headers.value("content-type");
            QByteArray boundary;

            if (contentType.startsWith("multipart/form-data"))
            {
                int posi=contentType.indexOf("boundary=");
                if (posi>=0) {
                    boundary = contentType.mid(posi+9);
                    if  (boundary.startsWith('"') && boundary.endsWith('"')) {
                       boundary = boundary.mid(1,boundary.length()-2);
                    }
                }
            }

            int expectedBodySize;
            QByteArray contentLength = headers.value("content-length");
            if (!contentLength.isEmpty()) {
                expectedBodySize = contentLength.toInt();
            }
            if (expectedBodySize == 0) {
                qDebug("HttpRequest: expect no body");
//                status = complete;
            }
            else if (boundary.isEmpty()/* && expectedBodySize+currentSize>maxSize*/)
            {
                qWarning("HttpRequest: expected body is too large");
//                status=abort;
            }
//            else if (!boundary.isEmpty() && expectedBodySize>maxMultiPartSize)
//            {
//                qWarning("HttpRequest: expected multipart body is too large");
//                status=abort;
//            }
            else {
                qDebug("HttpRequest: expect %i bytes body",expectedBodySize);
//                status=waitForBody;
            }
        }

        QByteArray body    = m_client->readAll();

//        if (m_debug)
//            qDebug() << "HTTP request received:" << request;
    }

} // onReadyRead

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
