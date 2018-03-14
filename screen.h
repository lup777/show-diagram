#ifndef SCREEN_H
#define SCREEN_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QDebug>
#include <QString>
#include <QMouseEvent>
#include <QLabel>
#include "WorkThread.h"
#include "DataFile.h"

class Screen : public QWidget
{
    Q_OBJECT
public:
    explicit Screen(QWidget *parent = 0);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    WorkThread* getWorkThread();

private:
    QImage buffer;
    WorkThread thread;
    QMap<double, double> _data;
    QPoint mouseCoord;

signals:
    void signal_statusBarMessage(QString msg);
    void signal_setMouseCoord(double x, double y);

public slots:
    void slot_workCompleted(QImage buffer);
    void slot_dataLoadComplete(t_dataMap data);

};

#endif // SCREEN_H
