#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include <QHostAddress>
#include <QList>
#include <QNetworkInterface>
#include <QSettings>

////////////////////////////////////////////////////////////////////////////////

#include "ConfigMgr.h"
//#include "HttpServer.h"
#include "WebSockServer.h"
#include "RequestHandler.h"
#include "MWMonitor.h"

#include "httplistener.h"

////////////////////////////////////////////////////////////////////////////////

#define APP_VERSION_STR     "0.0.1"

////////////////////////////////////////////////////////////////////////////////

using namespace stefanfrings;

////////////////////////////////////////////////////////////////////////////////

int main (int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    app.setApplicationName("Magewell Monitor");
    app.setOrganizationName("Vulcan");

    app.setApplicationVersion(APP_VERSION_STR);

    QCommandLineParser parser;
    parser.setApplicationDescription(app.applicationName() + " " +
                                     app.organizationName());
    parser.addHelpOption();

    parser.addPositionalArgument("config", "The configuration file to open.");

    QCommandLineOption dbgOption({"d", "debug"}, "Debug output [default: off].");
    parser.addOption(dbgOption);

    parser.process(app);

    bool debug = parser.isSet(dbgOption);

    // config file override?
    QStringList posArgs = parser.positionalArguments();

    QList<QHostAddress> ipAddresses = QNetworkInterface::allAddresses();

    ConfigMgr* configMgr = new ConfigMgr(posArgs.value(0), debug);

    MWMonitor* mwMonitor = new MWMonitor(&app);

    WebServer* webServer = new WebServer(/*configMgr*/ipAddresses, debug);
    QObject::connect(webServer, &WebServer::closed, &app, &QCoreApplication::quit);
    QObject::connect(webServer, &WebServer::ready, [&]() {
//        if (debug)
            qDebug() << "WebSocketServer: Listening on port" << webServer->serverPort();
    });

    QObject::connect(mwMonitor, &MWMonitor::mwEvent, webServer, &WebServer::signalEvent);

    RequestHandler* requestHandler = new RequestHandler(webServer, &app);

    // TODO: Read HTTP Port from Config file and write it to Settings file
    // (ALTERNATIVE): Rewrite Listener and ConnectionHandlerPool to remove dependency on Settings

    QSettings* listenerSettings = new QSettings("MWMonitor.ini", QSettings::IniFormat, &app);
    listenerSettings->beginGroup("listener");
    new HttpListener(listenerSettings, requestHandler, &app);

    if (!ipAddresses.isEmpty()) {
        puts("Available IP4 addresses:");
        for (auto ip : ipAddresses)
            if (ip.toIPv4Address())
                printf("\t%s\n", ip.toString().toStdString().c_str());
    }


//    HttpServer* httpServer = new HttpServer(debug);
//    QObject::connect(httpServer, &HttpServer::ready, [&]() {
//        if (debug)
//            qDebug() << "HttpServer is listening on port" << httpServer->serverPort();
//    });

//    httpServer->setWebSocketPort(webServer->serverPort());

    return app.exec();

} // main

////////////////////////////////////////////////////////////////////////////////

void handleError (std::exception_ptr error)
{
    try {
        if (error)
            std::rethrow_exception(error);
    }
    catch (const std::exception &e) {
        qDebug() << "MagewellMonitor::ERROR -" << e.what();
    }

} // handleError

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

