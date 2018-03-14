#include "WorkThread.h"


WorkThread::WorkThread(QObject *) :
    bNeedToWork(true)
{
    timer = new QTimer();
    Q_ASSERT(timer);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(slot_timer()));
    QObject::connect(this, SIGNAL(signal_startTimer()), this, SLOT(slot_timer()));
    QObject::connect(this, SIGNAL(signal_workCompleted(QImage)), this, SLOT(slot_stopTimer(QImage)));
    tk.Kpx = 0;
    tk.Kpy = 0;
    tk.Kmx = 0;
    tk.Kmy = 0;
}

void WorkThread::run()
{
    while(1)
    {
        qDebug() << "waiting for semaphore\n";
        sem.acquire();
        qDebug() << "semaphore released (" << sem.available() << ")";

        if(bNeedToWork == false)
        {
            qDebug() << "Exiting thread\n";
            return;
        }

        {
            emit signal_startTimer();
            progress = 0;
            QImage buffer(getRect().width(), getRect().height(), QImage::Format_Mono);

            QPainter p(&buffer);
            p.fillRect(getRect(), Qt::white );

            QMap<double, double> data = getData();
            int dataSize = data.size();
            QMap<double, double>::Iterator i = data.begin();

            double min_value = data[i.key()];
            double max_value = data[i.key()];
            double min_label = i.key();
            double max_label = i.key();

            double num = 0;

            for (i = data.begin(), num = 0; i != data.end() && sem.available() == 0; ++i, ++num)
            {
                if (data[i.key()] < min_value) min_value = data[i.key()];
                if (data[i.key()] > max_value) max_value = data[i.key()];
                if (i.key() < min_label) min_label = i.key();
                if (i.key() > max_label) max_label = i.key();
                progress = (num / dataSize * 50);
            }
            if(sem.available() != 0)
                continue;

            double delta_label = max_label- min_label;
            double delta_value = max_value- min_value;

            double Kpx = buffer.width() / delta_label;
            double Kpy = buffer.height() / delta_value;

            double Kmx = 0 - (min_label * Kpx);
            double Kmy = 0 - (min_value * Kpy);

            tk.Kpx = Kpx;
            tk.Kpy = Kpy;
            tk.Kmx = Kmx;
            tk.Kmy = Kmy;

            QPen pen(Qt::black);
            pen.setCapStyle(Qt::RoundCap);
            pen.setWidth(1);
            p.setPen(pen);

            i = data.begin();
            QPoint startP(
                        (i.key() * Kpx) + Kmx,
                        (data[i.key()] * Kpy) + Kmy);

            for(num = 0; i != data.end() && sem.available() == 0 && bNeedToWork == true; ++i, ++num)
            {
               //start painting
                int x = (i.key() * Kpx) + Kmx;
                int y = (data[i.key()] * Kpy) + Kmy;
                QPoint endP(x, y);

                p.drawLine(startP, endP);

                startP = endP;

                progress = (num / dataSize * 50) + 50;
               //end painting
            }
            qDebug() << "delta label = " << delta_label << "\n";
            qDebug() << "delta value = " << delta_value << "\n";
            qDebug() << "buffer height = " << buffer.height() << "\n";
            qDebug() << "buffer width = " << buffer.width() << "\n";
            qDebug() << "MIN: " << min_label * Kpx << ":" << min_value * Kpy << "\n";
            qDebug() << "MAX: " << max_label * Kpx << ":" << max_value * Kpy << "\n";
            qDebug() << "Kpx = " << Kpx << "\n";
            qDebug() << "Kpy = " << Kpy << "\n";

            if (sem.available() == 0 && bNeedToWork == true)
            {
                emit signal_workCompleted(buffer.mirrored(false, true));
            }
        }
    }
}

void WorkThread::startWork(const QRect rect, const QMap<double, double>& data)
{
    //qDebug() << "startWork >>>\n";
    signal_statusBarMessage(QString("loading .. "));

    setRect(rect);

    setData(data);

    if(sem.available() == 0)
        sem.release();
}

void WorkThread::WorkThread::startWork(const QRect rect)
{
    //qDebug() << "startWork >>>\n";
    signal_statusBarMessage(QString("loading .. "));

    setRect(rect);

    if(sem.available() == 0)
        sem.release();
}

TK WorkThread::getTk()
{
    return tk;
}

QRect WorkThread::getRect()
{
    rect_mutex.lock();
    QRect tmp = rect;
    rect_mutex.unlock();

    return tmp;
}

void WorkThread::setRect(const QRect& rect)
{
    rect_mutex.lock();
    this->rect = rect;
    rect_mutex.unlock();
}

QMap<double, double> WorkThread::getData()
{
    data_mutex.lock();
    QMap<double, double> tmp = _data;
    data_mutex.unlock();

    return tmp;
}

void WorkThread::setData(const QMap<double, double>& data)
{
    data_mutex.lock();
    this->_data = data;
    data_mutex.unlock();
}

void WorkThread::slot_timer()
{
    //qDebug() << "WorkThread::slot_timer\n";
    emit signal_statusBarMessage(QString("loading ") + QString::number(progress, 10) + QString("%"));
    timer->start(100);
}

void WorkThread::slot_stopTimer(QImage)
{
    qDebug() << "WorkThread slot_stopTimer >>>\n";
    timer->stop();
}

WorkThread::~WorkThread()
{
    bNeedToWork = false;
    sem.release(sem.available() + 1);
    while(this->isRunning()) {};
    delete timer;
    qDebug() << "~WorkThread <<<\n";
}
