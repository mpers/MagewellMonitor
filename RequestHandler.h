#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <httprequesthandler.h>

using namespace stefanfrings;

// The request handler receives incoming HTTP requests
// and generates JSON responses.

QT_BEGIN_NAMESPACE
class QJsonDocument;
class QJsonObject;
QT_END_NAMESPACE

class WebServer;

class RequestHandler : public HttpRequestHandler
{
    Q_OBJECT
    Q_DISABLE_COPY(RequestHandler)

public:

    RequestHandler (WebServer* webServer, QObject* parent = nullptr);
    ~RequestHandler ();

    void service (HttpRequest& request, HttpResponse& response);

private:
    void getVersion (QByteArray& body, bool html);
    void dumpRequest (HttpRequest &request, QByteArray& body, bool html);

private:
    WebServer*      m_wsServer;

};

#endif // REQUESTHANDLER_H
