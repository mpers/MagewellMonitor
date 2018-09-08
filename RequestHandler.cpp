#include <QByteArray>
#include <QCoreApplication>
#include <QHostAddress>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSysInfo>
#include <QUrl>

#include "WebSockServer.h"
#include "RequestHandler.h"

//#define FORMAT_HTML

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

RequestHandler::RequestHandler (WebServer* wsServer, QObject* parent)
    : HttpRequestHandler(parent)
    , m_wsServer(wsServer)
{
//    qDebug() << "RequestHandler: created";

} // RequestHandler

////////////////////////////////////////////////////////////////////////////////

RequestHandler::~RequestHandler ()
{
//    qDebug() << "RequestHandler: deleted";
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void RequestHandler::service (HttpRequest& request, HttpResponse& response)
{
    QByteArray path = request.getPath();

    qDebug() << "*** RequestHandler: Servicing path =" << path/*.data()*/;

    bool formatHtml = false;
    QList<QByteArray> formats = request.getParameters("format");

    if (!formats.isEmpty())
        formatHtml = (0 == QString::compare(QString("html"), QString(formats.at(0)),
                                            Qt::CaseInsensitive));

    if (formatHtml)
        response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");
    else
        response.setHeader("Content-Type", "application/json");

    QByteArray body;

    if (path.startsWith("/version"))
        getVersion(body, formatHtml);
    else
        dumpRequest(request, body, formatHtml);

    response.write(body, true);

    qDebug("RequestHandler: finished request");

} // service

////////////////////////////////////////////////////////////////////////////////

void RequestHandler::getVersion (QByteArray& body, bool html)
{
    QString osStr = QSysInfo::prettyProductName();
//    quint16 wsPort = m_wsServer->serverPort();
//    QHostAddress wsIP = m_wsServer->serverAddress();
//    QString strIP  = wsIP.toString();
    QString strUrl = m_wsServer->serverUrl().toDisplayString();

    if (html) {
        body.append("<html><body>");

        body.append("Version: ");
        body.append(qApp->applicationVersion().toLocal8Bit());

        body.append("<br>OS: ");
        body.append(osStr.toLocal8Bit());

        body.append("<br>WebSocket URL: ");
        body.append(strUrl.toLocal8Bit());

        body.append("</body></html>");
    }
    else {
        QJsonObject jsonBody;

        jsonBody.insert("Version", qApp->applicationVersion());
        jsonBody.insert("OS", osStr);
        jsonBody.insert("WebSocket URL:", strUrl);

        body = QJsonDocument(jsonBody).toJson();
    }

} // getVersion

////////////////////////////////////////////////////////////////////////////////

void RequestHandler::dumpRequest (HttpRequest& request, QByteArray& body, bool html)
{
    if (html) {
        body.append("<html><body>");

        body.append("<b>Request:</b>");

        body.append("<br>Method: ");
        body.append(request.getMethod());

        body.append("<br>Path: ");
        body.append(request.getPath());

        body.append("<br>Version: ");
        body.append(request.getVersion());

        body.append("<p><b>Headers:</b>");
        QMapIterator<QByteArray,QByteArray> i(request.getHeaderMap());
        while (i.hasNext()) {
            i.next();
            body.append("<br>");
            body.append(i.key());
            body.append("=");
            body.append(i.value());
        }

        body.append("<p><b>Parameters:</b>");
        i = QMapIterator<QByteArray,QByteArray>(request.getParameterMap());
        while (i.hasNext()) {
            i.next();
            body.append("<br>");
            body.append(i.key());
            body.append("=");
            body.append(i.value());
        }

        body.append("<p><b>Cookies:</b>");
        i = QMapIterator<QByteArray,QByteArray>(request.getCookieMap());
        while (i.hasNext())
        {
            i.next();
            body.append("<br>");
            body.append(i.key());
            body.append("=");
            body.append(i.value());
        }

        body.append("<p><b>Body:</b><br>");
        body.append(request.getBody());

        body.append("</body></html>");
    }
    else {

        QJsonObject jsonBody;
//        QJsonArray jsonReq;

//        QJsonObject jsMethod{{"Method", QString(request.getMethod())}};
//        jsonReq.append(jsMethod);
        jsonBody.insert("Method", QString(request.getMethod()));

//        QJsonObject jsPath{{"Path", QString(request.getPath())}};
//        jsonReq.append(jsPath);
        jsonBody.insert("Path", QString(request.getPath()));

//        QJsonObject jsVersion{{"Version", QString(request.getVersion())}};
//        jsonReq.append(jsVersion);
        jsonBody.insert("Version", QString(request.getVersion()));

        QJsonArray jsHeadArray;
        QMapIterator<QByteArray,QByteArray> i(request.getHeaderMap());
        while (i.hasNext()) {
            i.next();
            jsHeadArray.append(QString("%1=%2").arg(QString(i.key())).arg(QString(i.value())));
        }
//        QJsonObject jsHeaders{{"Headers", jsHeadArray}};
//        jsonReq.append(jsHeaders);
        jsonBody.insert("Headers", jsHeadArray);

        QJsonArray jsParamArray;
        i = QMapIterator<QByteArray,QByteArray>(request.getParameterMap());
        while (i.hasNext()) {
            i.next();
            jsParamArray.append(QString("%1=%2").arg(QString(i.key())).arg(QString(i.value())));
        }
//        QJsonObject jsParameters{{"Parameters", jsParamArray}};
//        jsonReq.append(jsParameters);
        jsonBody.insert("Parameters", jsParamArray);

        QJsonArray jsCookArray;
        i = QMapIterator<QByteArray,QByteArray>(request.getCookieMap());
        while (i.hasNext()) {
            i.next();
            jsCookArray.append(QString("%1=%2").arg(QString(i.key())).arg(QString(i.value())));
        }
//        QJsonObject jsCookies{{"Cookies", jsCookArray}};
//        jsonReq.append(jsCookies);
        jsonBody.insert("Cookies", jsCookArray);

//        QJsonObject jsBody{{"Body", QString(request.getBody())}};
//        jsonReq.append(jsBody);
        jsonBody.insert("Body", QString(request.getBody()));

//        jsonBody.insert("Request", jsonReq);

        body = QJsonDocument(jsonBody).toJson();
    }

} // dumpRequest

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
