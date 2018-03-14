#include "DataFile.h"

DataFile::DataFile(QObject *, QString fileName) :
    fineName(fileName), bNeedToWork(true)
{
    timer = new QTimer(this);
    Q_ASSERT(timer);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(slot_timer()));
    timer->setSingleShot(true);
    qRegisterMetaType<t_dataMap>("t_dataMap");
    qRegisterMetaType<t_dataFileInfo>("t_dataFileInfo");
    QObject::connect(this, SIGNAL(started()), this, SLOT(slot_started()));
}

DataFile::~DataFile()
{
    qDebug() << "waiting for thread ended\n";
    bNeedToWork = false;
    while(this->isRunning())
    {}
    timer->stop();
    delete timer;
}

void DataFile::run()
{
    qDebug() << "dataFile thead started\n";
    //std::locale loc("ru_RU.cp1251");
    //std::basic_ios::imbue(loc);
    std::ifstream ifstr(fineName.toStdString().data());
    std::cout.setf(std::ios::scientific);

    data.clear();
    samples_count = 0;

    while(ifstr.eof() == false && bNeedToWork == true)
    {
        std::string str;
        std::getline(ifstr, str);

        if(ifstr.fail())
        {
            emit signal_showError("File format is not valid.");
            return;
        }

        if(str.find("#") != std::string::npos)
        { //header
            qDebug() << QString(str.data());
            dataFileInfo.raw += str.data();
            dataFileInfo.lines.append(str.data());
        }
        else
        { //data

            double label, value;
            samples_count ++;

            ifstr >> label >> value;

            data[label] = value;
        }
    }

    if (parseFileHead() == true)
    {
        emit signal_dataLoadComplete(this->data);
        emit signal_fileInfo(dataFileInfo);
    }
}

bool DataFile::parseFileHead()
{
    QRegExp are_removeHash("#\\s*(.*)");
    if (dataFileInfo.lines.length() < 3)
    {
        qDebug() << "dataFileInfo.lines.length() = " << dataFileInfo.lines.length() << "\n";
        return false;
    }

    QRegExp qre_head("#\\s*(Pendulum\\s{1,}Instruments\\s{1,}AB.*)");
    if (dataFileInfo.lines[0].indexOf(qre_head) == -1)
    {
        qDebug() << "Applicatin name was not found\n";
        emit signal_showError("File format is not valid. Applicatin name was not found");
        return false;
    }
    dataFileInfo.appName = qre_head.cap(1);//dataFileInfo.lines[0];

    if (are_removeHash.indexIn(dataFileInfo.lines[1]) != -1)
        dataFileInfo.measType = are_removeHash.cap(1);

    QRegExp qre_measTime("#\\s*([A-Za-z]+\\s+[A-Za-z]+\\s+\\d+\\s+\\d+\\:\\d+\\:\\d+\\s+\\d{4,4}).*");
    if (dataFileInfo.lines[2].indexOf(qre_measTime) == -1)
    {
        qDebug() << "Measeure data nad time was not found\n";
        emit signal_showError("File format is not valid. Measeure data and time was not found");
        return false;
    }
    if (are_removeHash.indexIn(dataFileInfo.lines[2]) != -1)
        dataFileInfo.measTime = are_removeHash.cap(1);

    for (int i = 0; i < dataFileInfo.lines.length(); i++)
    {
        if (are_removeHash.indexIn(dataFileInfo.lines[i]) != -1)
        {
            dataFileInfo.params += are_removeHash.cap(1);
            dataFileInfo.params += "\n";
        }
    }

    qDebug() << "qre_head[0] returned: " << dataFileInfo.lines[0].indexOf(qre_head) << "\n";
    qDebug() << "qre_head[2] returned: " << dataFileInfo.lines[2].indexOf(qre_measTime) << "\n";

    return true;
}

void DataFile::slot_timer()
{
    if(this->isRunning())
    {
        emit signal_dataLoadingStatus(QString("Loading data file: ") + QString::number(samples_count, 10) + QString(" samples"));

        timer->start(100);
    }
}

void DataFile::slot_started(void)
{
    qDebug() << "slot_started >>>\n";
    timer->start(100);
}
