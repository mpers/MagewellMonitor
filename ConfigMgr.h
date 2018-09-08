#ifndef CONFIGMGR_H
#define CONFIGMGR_H

#include <QObject>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
class QJsonDocument;
class QJsonObject;
QT_END_NAMESPACE

class ConfigMgr : public QObject
{
    Q_OBJECT

public:
    explicit ConfigMgr (const QString& fileName, bool debug = false,
                        QObject *parent = nullptr);

    bool        isValid ()  { return m_valid; }

    quint16     webSocketPort ();


signals:

public slots:

private:
    QJsonObject     m_jsonConfig;
    bool            m_valid;
};

#endif // CONFIGMGR_H
