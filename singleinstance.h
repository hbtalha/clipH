#ifndef SINGLEINSTANCE_H
#define SINGLEINSTANCE_H

/*
 * Code from this Stack Overflow answer: https://stackoverflow.com/a/28172162/11337328
*/

#include <QObject>
#include <QSharedMemory>
#include <QSystemSemaphore>


class SingleInstance
{

public:
    SingleInstance( const QString& key );
    ~SingleInstance();

    bool isAnotherRunning();
    bool run();
    void release();

private:
    const QString key;
    const QString memLockKey;
    const QString sharedmemKey;

    QSharedMemory sharedMem;
    QSystemSemaphore memLock;

    Q_DISABLE_COPY( SingleInstance )
};

#endif // SINGLEINSTANCE_H
