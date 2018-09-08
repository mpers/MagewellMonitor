#ifndef MWMONITOR_H
#define MWMONITOR_H

#include <QObject>
#include <QVariant>

class MWMonitor : public QObject
{
    Q_OBJECT

public:
    explicit MWMonitor (QObject *parent = nullptr);

signals:
    void    mwEvent (QVariant event);

public slots:
};

#endif // MWMONITOR_H
