#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPluginLoader>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pref = new DialogPreference();
    QPixmap init(QSize(256,100));
    init.fill(Qt::black);
    ui->histogram_mainui->setPixmap(init);
    if(!QFile("settings.ini").exists())
    {
        m_sSettingsFile = "settings.ini";
        saveSettings();
    }
    else
    {
        m_sSettingsFile = "settings.ini";
        loadSettings();
    }

    ui->actionUndo->setEnabled(false);
    ui->actionRedo->setEnabled(false);

    qRegisterMetaType< cv::Mat >("cv::Mat");
    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),this, SLOT(subWinChanged(QMdiSubWindow*)));

    //Set up QtNetwork
//    m_client = 0;
//    m_server = new LocalSocketIpcServer("simpleProcessing", this);
//    connect(m_server, SIGNAL(messageReceived(QString)), this, SLOT(socketIcpMessage(QString)));
//    mem = new shareMemory();
    setUIEnable(false);
}

MainWindow::~MainWindow()
{
    delete ui;
//    disconnect(m_server, SIGNAL(messageReceived(QString)), this, SLOT(socketIcpMessage(QString)));
//    if(m_client)
//        disconnect(m_client, SIGNAL(socketClientStatus(int)), this, SLOT(socketClientStatus(int)));
//    if(ist)
//        receiveSubWindowClose(0);
    if(pref)
    {
        //disconnect(ui->graphicsView, SIGNAL(sendMousePress()),this,SLOT(receiveMousePress()));
        //disconnect(ui->graphicsView_preview, SIGNAL(sendMousePress()),this,SLOT(receiveMousePressPreview()));
        pref->deleteLater();
    }
//    if(mem){delete mem;}
//    if(m_server){m_server->deleteLater();}
//        if(m_client){m_client->deleteLater();}
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
    subWindow *tmp = qobject_cast<subWindow *>(ui->mdiArea->activeSubWindow());
    if(tmp)
        if(!tmp->_img.empty())
        {
            setShowHistogram(tmp->_img);
            setUIEnable(true);
        }
        else
        {
            setUIEnable(false);
        }

    if(tmp->recoverImg.empty())
        ui->actionUndo->setEnabled(false);
    else
        ui->actionUndo->setEnabled(true);

    if(tmp->forwardImg.empty())
        ui->actionRedo->setEnabled(false);
    else
        ui->actionRedo->setEnabled(true);
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

void MainWindow::backupImage(subWindow *subwin)
{
    subwin->forwardImg.clear();
    ui->actionRedo->setEnabled(false);
    while(subwin->recoverImg.size() > recoverLimit)
    {
        subwin->recoverImg.erase(subwin->recoverImg.begin());
    }
    subwin->recoverImg.push_back(subwin->_img);
}

void MainWindow::setShowImage(subWindow *subwin)
{
    //set hist
    setShowHistogram(subwin->_img);
    subwin->refreshImg();
}

void MainWindow::setShowHistogram(cv::Mat &img)
{
    cv::Mat his;
    myCV::histogram(img, his);
    ui->histogram_mainui->setPixmap(QPixmap::fromImage(QImage(his.data, his.cols, his.rows, his.step, QImage::Format_RGB888).rgbSwapped()));
}

void MainWindow::setUIEnable(bool status)
{
    ui->menuEdit->setEnabled(status);
//    ui->actionCustom_Filter->setEnabled(status);
//    ui->actionImage_Subtractor->setEnabled(status);
    ui->actionPrint->setEnabled(status);
    ui->menuExport->setEnabled(status);
}

void MainWindow::on_actionOpen_image_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open File"),0,"Image files (*.png *.bmp *.jpg);;PNG (*.png);;BMP (*.bmp);;JPG (*.jpg)");
    if(filename.isEmpty())
        return;

    cv::Mat tmp = cv::imread(filename.toStdString());

    if(tmp.empty())
        return;

    subWindow *newWindow = new subWindow(ui->mdiArea);
    setShowHistogram(tmp);
    newWindow->setWindowTitle(filename);
    newWindow->setGeometry(0,0,ui->mdiArea->width()/2,ui->mdiArea->height()/2);
    newWindow->setAttribute(Qt::WA_DeleteOnClose);
    newWindow->show();
    newWindow->setImage(tmp);
    setUIEnable(true);
    ui->statusBar->showMessage("Image: " + filename + " opened!");
}

void MainWindow::on_actionSave_image_triggered()
{
    subWindow *tmp = qobject_cast<subWindow *>(ui->mdiArea->activeSubWindow());
    if(tmp)
    {
        if(tmp->_img.empty())
        {
            QMessageBox::warning(this,"Warning","Nothing to save!!");
            return;
        }
        QString savefileName = QFileDialog::getSaveFileName(this,"Save image");
        cv::imwrite(savefileName.toStdString(),tmp->_img);
    }
}

void MainWindow::on_actionPrint_triggered()
{
    subWindow *tmp = qobject_cast<subWindow *>(ui->mdiArea->activeSubWindow());
    if(tmp)
    {
        if(tmp->_img.empty())
        {
            QMessageBox::warning(this,"Warning","Nothing to print!!");
            return;
        }
        QPrinter printer(QPrinter::HighResolution);
        //printer.setOutputFileName("print.ps");
        printer.setOutputFormat(QPrinter::NativeFormat);
        QPrintPreviewDialog preview(&printer, this);
        preview.setWindowFlags(Qt::Window);
        QImage pImg = QImage(tmp->_img.data, tmp->_img.size().width, tmp->_img.size().height, tmp->_img.step, QImage::Format_RGB888).rgbSwapped();
        if(pImg.width()  > printer.pageRect().width() ||
           pImg.height() > printer.pageRect().height())
            pImg = pImg.scaled(printer.pageRect().width(),
                               printer.pageRect().height(), Qt::KeepAspectRatio);
        QPainter *painter = new QPainter(&printer);
        painter->drawImage(0, 0, pImg);
        painter->end();

        if(preview.exec() != QDialog::Accepted)
            return;
    }
}

void MainWindow::on_actionTo_PDF_triggered()
{
    subWindow *tmp = qobject_cast<subWindow *>(ui->mdiArea->activeSubWindow());
    if(tmp)
    {
        if(tmp->_img.empty())
        {
            QMessageBox::warning(this,"Warning","Nothing to export!!");
            return;
        }
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        QPrintPreviewDialog preview(&printer, this);
        preview.setWindowFlags(Qt::Window);
        QImage pImg = QImage(tmp->_img.data, tmp->_img.size().width, tmp->_img.size().height, tmp->_img.step, QImage::Format_RGB888).rgbSwapped();
        if(pImg.width()  > printer.pageRect().width() ||
           pImg.height() > printer.pageRect().height())
            pImg = pImg.scaled(printer.pageRect().width(),
                               printer.pageRect().height(), Qt::KeepAspectRatio);
        QPainter *painter = new QPainter(&printer);
        painter->drawImage(0, 0, pImg);
        painter->end();

        if(preview.exec() != QDialog::Accepted)
            return;
    }
}

void MainWindow::on_actionPreference_triggered()
{
    if(pref->exec() == QDialog::Accepted)
    {
        saveSettings();
    }
}

void MainWindow::on_actionUndo_triggered()
{
    subWindow *tmp = qobject_cast<subWindow *>(ui->mdiArea->activeSubWindow());
    if(tmp)
    {
        tmp->forwardImg.push_back(tmp->_img);
        tmp->_img = tmp->recoverImg[tmp->recoverImg.size()-1].clone();
        tmp->recoverImg.erase(tmp->recoverImg.end()-1);
        setShowImage(tmp);

        if(tmp->recoverImg.empty())
            ui->actionUndo->setEnabled(false);
        ui->actionRedo->setEnabled(true);
        ui->statusBar->showMessage("Undo");
    }
}

void MainWindow::on_actionRedo_triggered()
{
    subWindow *tmp = qobject_cast<subWindow *>(ui->mdiArea->activeSubWindow());
    if(tmp)
    {
        tmp->recoverImg.push_back(tmp->_img);
        tmp->_img = tmp->forwardImg[tmp->forwardImg.size()-1].clone();
        tmp->forwardImg.erase(tmp->forwardImg.end()-1);
        setShowImage(tmp);

        if(tmp->forwardImg.empty())
            ui->actionRedo->setEnabled(false);
        ui->actionUndo->setEnabled(true);
        ui->statusBar->showMessage("Redo");
    }
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
                backupImage(tmp);
                myCV::myResize(tmp->_img, tmp->_img, r.getWidth(),r.getHeight(),r.isAspect(),r.getMethod());
                setShowImage(tmp);
                ui->actionUndo->setEnabled(true);
                ui->statusBar->showMessage("Image resized to: " + QString::number(r.getWidth()) + ", " + QString::number(r.getHeight()) + ".");
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
                backupImage(tmp);
                b.useGaussian() == true ? myCV::Blur::Gaussian(tmp->_img, tmp->_img, b.getMaskSize(), b.getSigma(), b.getSigma()) :
                                          myCV::Blur::simple(tmp->_img, tmp->_img, b.getMaskSize());
                setShowImage(tmp);
                ui->actionUndo->setEnabled(true);
                ui->statusBar->showMessage("Image blurred.");
            }
        }
    }
}



void MainWindow::on_actionAverage_triggered()
{
    subWindow *tmp = qobject_cast<subWindow *>(ui->mdiArea->activeSubWindow());
    if(tmp)
    {
        if(!tmp->_img.empty())
        {
            backupImage(tmp);
            myCV::myCvtColor(tmp->_img, tmp->_img, myCV::BGR2GRAY, myCV::GRAY_AVG);
            setShowImage(tmp);
            ui->actionUndo->setEnabled(true);
            ui->statusBar->showMessage("The image is now grayscaled (average).");
        }
    }
}

void MainWindow::on_actionLuminosity_triggered()
{
    subWindow *tmp = qobject_cast<subWindow *>(ui->mdiArea->activeSubWindow());
    if(tmp)
    {
        if(!tmp->_img.empty())
        {
            backupImage(tmp);
            myCV::myCvtColor(tmp->_img, tmp->_img, myCV::BGR2GRAY, myCV::GRAY_LUMI);
            setShowImage(tmp);
            ui->actionUndo->setEnabled(true);
            ui->statusBar->showMessage("The image is now grayscaled (luminosity).");
        }
    }
}
