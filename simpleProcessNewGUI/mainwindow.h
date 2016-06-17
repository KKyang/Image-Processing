#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include <QDir>
#include <QFileDialog>
#include "plugininterfaces.h"
#include "subwindowwidget.h"



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
    void subWinChanged(QMdiSubWindow* mdiWin);
    void imgEffects();
    void imgEffectProcess();
    void imgTools();
    void on_actionOpen_image_triggered();

private:
    void loadPlugins();
    void populateMenus(QObject *plugin);
    void addToMenu(QObject *plugin, const QStringList &texts, QMenu *menu,
                       const char *member, QActionGroup *actionGroup = 0);

    Ui::MainWindow *ui;

    QDir pluginsDir;
    QStringList pluginFileNames;
    imageProcessInterface *_imgProc = 0;

};

#endif // MAINWINDOW_H
