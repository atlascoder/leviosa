#ifndef MDNSDISCOTHREAD_H
#define MDNSDISCOTHREAD_H

#include <QThread>
#include <QString>

class MDNSDiscoThread : public QThread
{
    Q_OBJECT
    void run();
public:
    MDNSDiscoThread(QObject* parent = 0);
signals:
    void discovered(const QString& mac, const QString& ip);
};

#endif // MDNSDISCOTHREAD_H
