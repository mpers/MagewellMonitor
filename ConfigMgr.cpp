#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QSettings>

#include "ConfigMgr.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

ConfigMgr::ConfigMgr (const QString &fileName, bool debug, QObject *parent)
    : QObject(parent)
    , m_valid(false)
{
    Q_UNUSED(debug);

    QSettings settings;

    QString cfgFileName(fileName);
    if (cfgFileName.isEmpty()) {
        cfgFileName = settings.value("Config/File", "Visa4EncoderConfigOption.json").toString();
    }

    QString cfgDirName = settings.value("Config/Path").toString();
    QDir cfgDir = cfgDirName.isEmpty() ? QDir::current() : QDir(cfgDirName);

    if (cfgDir.exists()) {
        QFileInfo cfgFileInfo(cfgDir, cfgFileName);

        if (cfgFileInfo.exists()) {
            QFile cfgFile(cfgFileInfo.absoluteFilePath());
            if (cfgFile.open(QFile::ReadOnly)) {
                QJsonParseError parseError;
                QJsonDocument cfgDoc = QJsonDocument::fromJson(cfgFile.readAll(), &parseError);
                if (parseError.error == QJsonParseError::NoError)
                    m_jsonConfig = cfgDoc.object();
                else
                    qDebug() << "JSON parsing error:" << parseError.errorString()
                             << "at offset" << parseError.offset;
            }
        }
        else {
            qDebug() << "File" << cfgFileInfo.absoluteFilePath() << "not found";
        }
    }
    else {
        qDebug() << "Directory" << cfgDir.absolutePath() << "not found";
    }

    m_valid = !m_jsonConfig.isEmpty();

} // ConfigMgr

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

quint16 ConfigMgr::webSocketPort ()
{
    QSettings settings;

    quint16 webPort = (quint16) settings.value("WebSocket/Port", 1234).toInt();

    if (m_valid)
        webPort = (quint16) m_jsonConfig.value("WebSocketPort").toInt(webPort);

    return webPort;

} // webSocketPort
