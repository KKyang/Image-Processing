#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPluginLoader>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qRegisterMetaType< cv::Mat >("cv::Mat");
//    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),this, SLOT(subWinChanged(QMdiSubWindow*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::subWinChanged(QMdiSubWindow* mdiWin)
{

}

void MainWindow::loadPlugins()
{
    foreach(QObject *plugin, QPluginLoader::staticInstances())
        populateMenus(plugin);

    pluginsDir = QDir(qApp->applicationDirPath());

#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("plugins");

    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            populateMenus(plugin);
            pluginFileNames += fileName;

        }

    }

//    brushMenu->setEnabled(!brushActionGroup->actions().isEmpty());
//    shapesMenu->setEnabled(!shapesMenu->actions().isEmpty());
//    filterMenu->setEnabled(!filterMenu->actions().isEmpty());

}

void MainWindow::populateMenus(QObject *plugin)
{
    imageProcessInterface *imgProc = qobject_cast<imageProcessInterface *>(plugin);
    if(imgProc)
    {
        connect(&imgProc->dialog(),SIGNAL(accept()),this,SLOT(imgEffectProcess()));
        addToMenu(plugin, imgProc->processIndex(), ui->menuImage_Effect, SLOT(imgEffects()));
    }
}

void MainWindow::addToMenu(QObject *plugin, const QStringList &texts, QMenu *menu,
                                const char *member, QActionGroup *actionGroup)
{
    foreach (QString text, texts) {
        QAction *action = new QAction(text, plugin);
        connect(action, SIGNAL(triggered()), this, member);
        menu->addAction(action);

        if (actionGroup) {
            action->setCheckable(true);
            actionGroup->addAction(action);
        }
    }
}

void MainWindow::imgEffects()
{
    QAction *action = qobject_cast<QAction *>(sender());
    _imgProc = qobject_cast<imageProcessInterface *>(action->parent());

    _imgProc->dialog().show();
}

void MainWindow::imgTools()
{

}

void MainWindow::imgEffectProcess()
{
    if(_imgProc)
    {
        subWindow *tmp = qobject_cast<subWindow *>(ui->mdiArea->activeSubWindow());
        _imgProc->process(tmp->_img);
    }
}

void MainWindow::on_actionOpen_image_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,"Open image","");
    if(filename.isEmpty())
        return;

    cv::Mat tmp = cv::imread(filename.toStdString());


    subWindow *newWindow = new subWindow(ui->mdiArea);
    newWindow->setWindowTitle(filename);
    newWindow->setGeometry(0,0,ui->mdiArea->width()/2,ui->mdiArea->height()/2);
    newWindow->setAttribute(Qt::WA_DeleteOnClose);
    newWindow->show();
    newWindow->setImage(tmp);
}
