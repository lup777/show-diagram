#include "screen.h"

Screen::Screen(QWidget *parent) : QWidget(parent)
{
    thread.start();
    this->setMinimumSize(220, 220);
    setMouseTracking(true);
}

void Screen::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawImage(this->rect(), buffer, buffer.rect());
    //qDebug() << "screen size: " << this->rect().height() << ":" << this->rect().width() << "\n";
    //qDebug() << "buffer size: " << buffer.rect().height() << ":" << buffer.rect().width() << "\n";

    QPen pen(Qt::black);
    pen.setCapStyle(Qt::RoundCap);
    pen.setWidth(1);
    pen.setColor(Qt::red);
    p.setPen(pen);
    p.drawLine(0, mouseCoord.y(), buffer.width(), mouseCoord.y());
    p.drawLine(mouseCoord.x(), 0, mouseCoord.x(), buffer.height());
}

void Screen::resizeEvent(QResizeEvent *)
{
    if (this->rect() != buffer.rect())
        thread.startWork(this->rect());
}

void Screen::mouseMoveEvent(QMouseEvent *event)
{
    TK tk = thread.getTk();

    double x = (mouseCoord.x() - tk.Kmx) / tk.Kpx;
    double y = (this->rect().x() - mouseCoord.y() - tk.Kmy) / tk.Kpy;

    emit signal_setMouseCoord(x, y);
    mouseCoord = QPoint(event->x(), event->y());
    this->update();
}

void Screen::slot_workCompleted(QImage buffer)
{
    qDebug() << "Screen::slot_workCompleted >>> \n";
    this->buffer = buffer;
    this->update();
    signal_statusBarMessage(QString("loading completed"));
}

void Screen::slot_dataLoadComplete(t_dataMap data)
{
    qDebug() << "Screen::slot_dataLoadComplete >>>\n";
    thread.startWork(this->rect(), data);
}

WorkThread* Screen::getWorkThread()
{
    return &thread;
}
