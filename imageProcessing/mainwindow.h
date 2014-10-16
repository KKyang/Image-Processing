#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <QFileDialog>
#include <QString>

#include "lib/imgcore.h"
#include "lib/imageprocessing.h"
#include "smartgraphicsview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_Image_triggered();

    void on_actionGrayscale_average_triggered();

    void on_actionGrayscale_luminosity_triggered();

private:
    Ui::MainWindow *ui;
    QString fileName;
    cv::Mat image;


};

#endif // MAINWINDOW_H
