#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QMouseEvent>
#include <QString>
#include <QThread>
#include <QImage>
#include <QVBoxLayout>
#include <QWidget>
#include <QSplitter>
#include <QtGlobal>
#include <QAction>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QSizePolicy>
#include <QTextEdit>
#include <QSharedPointer>
#include "screen.h"
#include "DataFile.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    //void mouseMoveEvent(QMouseEvent *event);
    ~MainWindow();

private:
    void createMenu();
    void createUi();
    void createActions();

    Ui::MainWindow *ui;
    QLabel coords;
    QImage buffer;
    Screen screen;
    QMenu* menu_about;
    QMenu *fileMenu;
    QAction *openAct;
    QAction *fileInfoAct;
    QVBoxLayout* layout;
    DataFile *dataFile;
    t_dataFileInfo fileInfo;

protected:

signals:
    void signal_statusBarMessage(QString msg);

public slots:
    void slot_workCompleted(QImage);
    void slot_statusBarMessage(QString msg);
    void slot_setMouseCoords(double x, double y);
    void slot_setFileInfo(t_dataFileInfo fileInfo);

private slots:
    void slot_open();
    void slot_fileInfo();
    void slot_showError(QString);

};

#endif // MAINWINDOW_H
