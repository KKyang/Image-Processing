#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QPageSetupDialog>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QProcess>
#include <QSettings>
#include <QThread>

#include "plugininterfaces.h"
#include "subwindowwidget.h"

#include "dialog/dialogblur.h"
#include "dialog/dialogfuzzylogic.h"
#include "dialog/dialogpreference.h"
#include "dialog/dialogresize.h"
#include "dialog/dialogsize.h"

#include "imageprocessing/fft.h"
#include "imageprocessing/fuzzylogic.h"

#include "imgcore.h"
#include "imageprocessing.h"

#include "qsmartgraphicsview.h"

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

    void on_actionSave_image_triggered();

    void on_actionPrint_triggered();

    void on_actionTo_PDF_triggered();

    void on_actionPreference_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionResize_triggered();

    void on_actionBlur_triggered();

    void on_actionAverage_triggered();

    void on_actionLuminosity_triggered();

private:
    void loadSettings();
    void saveSettings();
    void loadPlugins();
    void populateMenus(QObject *plugin);
    void addToMenu(QObject *plugin, const QStringList &texts, QMenu *menu,
                       const char *member, QActionGroup *actionGroup = 0);

    void backupImage(subWindow *subwin);
    void setShowImage(subWindow *subwin);
    void setShowHistogram(cv::Mat &img);

    void setUIEnable(bool status);

    Ui::MainWindow *ui;
    DialogPreference *pref;
    QString m_sSettingsFile;
    int recoverLimit = 5;

    QDir pluginsDir;
    QStringList pluginFileNames;
    imageProcessInterface *_imgProc = 0;

};

#endif // MAINWINDOW_H
