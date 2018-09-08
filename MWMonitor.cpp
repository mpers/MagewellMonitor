#include <QTime>
#include <QTimer>

#include "MWMonitor.h"

MWMonitor::MWMonitor (QObject *parent)
    : QObject(parent)
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [&]() {
        emit mwEvent(QVariant(QTime::currentTime()));
    });

    timer->start(1000);

} // MWMonitor
