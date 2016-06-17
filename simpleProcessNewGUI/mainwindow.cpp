#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPluginLoader>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pref = new DialogPreference();
//    QPixmap init(QSize(256,100));
//    init.fill(Qt::black);
//    ui->histogram->setPixmap(init);
    qRegisterMetaType< cv::Mat >("cv::Mat");
//    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),this, SLOT(subWinChanged(QMdiSubWindow*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadSettings()
{
    QSettings settings(m_sSettingsFile, QSettings::IniFormat);
    QString sText = settings.value("recoverLimit","").toString();
    if(sText.toInt() > 0 && sText.toInt() <= 10)
    {
        recoverLimit = sText.toInt();
        pref->setRecoverLimit(sText.toInt());
        ui->statusBar->showMessage("Load settings successfully!!");
    }
    else
    {
        recoverLimit = 5;
        pref->setRecoverLimit(5);
        ui->statusBar->showMessage("Error loading undo limit, use default value 5.");
    }
}

void MainWindow::saveSettings()
{
    QSettings settings(m_sSettingsFile, QSettings::IniFormat);
    QString sText = QString::number(pref->getRecoverLimit());
    settings.setValue("recoverLimit", sText);
    settings.sync();

    ui->statusBar->showMessage("Save settings successfully!!");
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

    //_imgProc->dialog().show();
    if(_imgProc->dialog().exec()==QDialog::Accepted)
    {
        subWindow *tmp = qobject_cast<subWindow *>(ui->mdiArea->activeSubWindow());
        if(tmp)
            _imgProc->process(tmp->_img);
    }
}

void MainWindow::imgTools()
{

}

void MainWindow::imgEffectProcess()
{
//    if(_imgProc)
//    {
//        subWindow *tmp = qobject_cast<subWindow *>(ui->mdiArea->activeSubWindow());
//        if(tmp)
//            _imgProc->process(tmp->_img);
//    }
}

void MainWindow::setShowImage(subWindow *subwin)
{
    //set hist
    subwin->refreshImg();
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

void MainWindow::on_actionResize_triggered()
{
    subWindow *tmp = qobject_cast<subWindow *>(ui->mdiArea->activeSubWindow());
    if(tmp)
    {
        if(!tmp->_img.empty())
        {
            DialogResize r;
            r.getCurrentImageProperties(tmp->_img.size().width, tmp->_img.size().height);
            if(r.exec() == QDialog::Accepted)
            {
                tmp->backupImage();
                myCV::myResize(tmp->_img, tmp->_img, r.getWidth(),r.getHeight(),r.isAspect(),r.getMethod());
                setShowImage(tmp);
            }
        }
    }
}

void MainWindow::on_actionBlur_triggered()
{
    subWindow *tmp = qobject_cast<subWindow *>(ui->mdiArea->activeSubWindow());
    if(tmp)
    {
        if(!tmp->_img.empty())
        {
            DialogBlur b;
            if(b.exec() == QDialog::Accepted)
            {
                tmp->backupImage();
                b.useGaussian() == true ? myCV::Blur::Gaussian(tmp->_img, tmp->_img, b.getMaskSize(), b.getSigma(), b.getSigma()) :
                                          myCV::Blur::simple(tmp->_img, tmp->_img, b.getMaskSize());
                setShowImage(tmp);
            }
        }
    }
}
