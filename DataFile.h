#ifndef DATAFILE_H
#define DATAFILE_H

#include <QThread>
#include <QFile>
#include <iostream>
#include <ostream>
#include <fstream>
#include <string>
#include <QString>
#include <QDebug>
#include <QTextStream>
#include <iomanip>
#include <QMap>
#include <QMetaType>
#include <QTimer>
#include <QVector>
#include <atomic>
#include <locale>
#include <QStringList>
#include <QRegExp>

typedef QMap<double, double> t_dataMap;

typedef struct
{
    QString appName;
    QString measType;
    QString startTime;
    QString measTime;
    QString params;
    QString raw;
    QStringList lines;
} t_dataFileInfo;

class DataFile : public QThread
{
    Q_OBJECT

public:
    explicit DataFile(QObject *parent, QString fileName);
    ~DataFile();

private:
    void run();
    bool parseFileHead();

    QString fineName;
    std::ifstream *ifstr;
    t_dataMap data;
    double max, min;
    int samples_count;
    QTimer *timer;
    std::atomic<bool> bNeedToWork;
    t_dataFileInfo dataFileInfo;


signals:
    void signal_dataLoadComplete(t_dataMap data);
    void signal_dataLoadingStatus(QString msg);
    void signal_fileInfo(t_dataFileInfo);
    void signal_showError(QString);

public slots:
    void slot_timer();

private slots:
    void slot_started(void);

};

#endif // DATAFILE_H
