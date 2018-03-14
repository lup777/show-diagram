#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createUi();
    setMouseTracking(true);

    QObject::connect(screen.getWorkThread(), SIGNAL(signal_workCompleted(QImage)), &screen, SLOT(slot_workCompleted(QImage)));
    QObject::connect(screen.getWorkThread(), SIGNAL(signal_statusBarMessage(QString)), this, SLOT(slot_statusBarMessage(QString)));
    QObject::connect(&screen, SIGNAL(signal_statusBarMessage(QString)), this, SLOT(slot_statusBarMessage(QString)));
    QObject::connect(&screen, SIGNAL(signal_setMouseCoord(double,double)), this, SLOT(slot_setMouseCoords(double,double)));    

    layout = new QVBoxLayout(this);
    Q_ASSERT(layout);
    centralWidget()->setLayout(layout);

    layout->addWidget(&screen);
    layout->addWidget(&coords);

    statusBar()->setMaximumHeight(20);
    coords.setMaximumHeight(20);
    dataFile = new DataFile(0, QString(""));
    Q_ASSERT(dataFile);
    this->show();
}

void MainWindow::createUi()
{
    createActions();
    coords.setText("XX:YY");

    statusBar()->addPermanentWidget(&coords);
    statusBar()->showMessage("statusBar");

    fileMenu = new QMenu(tr("&File"));
    Q_ASSERT(fileMenu);
    fileMenu->addAction(openAct);
    fileMenu->addAction(fileInfoAct);

    menuBar()->addMenu(fileMenu);
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open"), this);
    Q_ASSERT(openAct);
    //openAct->setStatusTip(tr("Open a file"));
    QObject::connect(openAct, SIGNAL(triggered()), this, SLOT(slot_open()));

    fileInfoAct = new QAction(tr("&FileInfo"), this);
    Q_ASSERT(fileInfoAct);
    QObject::connect(fileInfoAct, SIGNAL(triggered(bool)), this, SLOT(slot_fileInfo()));
}

void MainWindow::slot_setMouseCoords(double x, double y)
{
    QString str_x(QString::number(x, 'e', 6));
    QString str_y(QString::number(y, 'e', 6));
    coords.setText(str_x + QString(" : ") + str_y);
}

void MainWindow::slot_setFileInfo(t_dataFileInfo fileInfo)
{
    qDebug() << "MainWindow::slot_setFileInfo\n";
    this->fileInfo = fileInfo;
}

void MainWindow::slot_workCompleted(QImage buffer)
{
    qDebug() << "slot_workCompleted >>> \n";
    this->buffer = buffer;
    this->update();
}

void MainWindow::slot_statusBarMessage(QString msg)
{
    statusBar()->showMessage(msg);
}

void MainWindow::slot_fileInfo()
{
    qDebug() << "void MainWindow::slot_fileInfo()\n";
    QMessageBox* messageBox = new QMessageBox(this);
    Q_ASSERT(messageBox);

    messageBox->setFixedSize(500, 550);
    messageBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    messageBox->setText("file info");
    messageBox->setDetailedText(fileInfo.params);

    QString informativeText;
    informativeText += "App name: ";
    informativeText += fileInfo.appName;
    informativeText += "\n";
    informativeText += "Measure type: ";
    informativeText += fileInfo.measType;
    informativeText += "\n";
    informativeText += "Measure time: ";
    informativeText += fileInfo.measTime;
    informativeText += "\n";

    messageBox->setInformativeText(informativeText);
    QFont font = QFont();
    font.setFamily("Arial");
    messageBox->setFont(font);
    messageBox->exec();
    delete messageBox;
}

void MainWindow::slot_showError(QString msg)
{
    QMessageBox* messageBox = new QMessageBox();
    Q_ASSERT(messageBox);
    messageBox->setText("Error");
    messageBox->setInformativeText(msg);
    messageBox->exec();
    delete messageBox;
}

void MainWindow::slot_open()
{
    QString dataFileName;

    dataFileName = QFileDialog::getOpenFileName(this);

    if(dataFileName == NULL)
        return;

    qDebug() << "dataFileName: " << dataFileName<< "\n";

    delete dataFile;

    dataFile = new DataFile(this, dataFileName);
    Q_ASSERT(dataFile);

    QObject::connect(dataFile, SIGNAL(signal_dataLoadComplete(t_dataMap)), &screen, SLOT(slot_dataLoadComplete(t_dataMap)), Qt::QueuedConnection);
    QObject::connect(dataFile, SIGNAL(signal_dataLoadingStatus(QString)), this, SLOT(slot_statusBarMessage(QString)));
    QObject::connect(dataFile, SIGNAL(signal_fileInfo(t_dataFileInfo)), this, SLOT(slot_setFileInfo(t_dataFileInfo)));
    QObject::connect(dataFile, SIGNAL(signal_showError(QString)), this, SLOT(slot_showError(QString)));
    dataFile->start();
}


MainWindow::~MainWindow()
{
    delete dataFile;
    delete fileInfoAct;
    delete openAct;
    delete menu_about;
    delete layout;
    delete ui;
}
