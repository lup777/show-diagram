#ifndef WORKTHREAD_H
#define WORKTHREAD_H

#include <QThread>
#include <QRect>
#include <QSemaphore>
#include <QDebug>
#include <QMutex>
#include <QImage>
#include <QPainter>
#include <QString>
#include <QMetaType>
#include <QPen>
#include <QPoint>
#include <QTimer>
#include <QPair>
#include <QImage>

typedef struct struct_K
{
    double Kpx;
    double Kpy;
    double Kmx;
    double Kmy;
} TK;

class WorkThread : public QThread
{
    Q_OBJECT
public:
    explicit WorkThread(QObject *parent = 0);
    void startWork(const QRect rect, const QMap<double, double>& data);
    void startWork(const QRect rect);
    TK getTk();
    ~WorkThread();

private:
    volatile bool bNeedToWork;
    QSemaphore sem;
    QMutex rect_mutex;
    QMutex data_mutex;
    QRect rect;
    QMap<double, double> _data;
    QTimer *timer;
    int progress;
    TK tk;

    void run();

    QRect getRect();
    void setRect(const QRect&);
    QMap<double, double> getData();
    void setData(const QMap<double, double>& data);


signals:
    void signal_workCompleted(QImage buffer);
    void signal_statusBarMessage(QString msg);
    void signal_startTimer();


private slots:
    void slot_timer();
    void slot_stopTimer(QImage);
};

#endif // WORKTHREAD_H
