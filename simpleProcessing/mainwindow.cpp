#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ist = 0;
    pref = new DialogPreference();
    this->setFixedSize(1280, 720);
    QPixmap init(QSize(256,100));
    init.fill(Qt::black);
    ui->histogram->setPixmap(init);

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

    ui->actionBack->setEnabled(false);
    ui->actionForward->setEnabled(false);

    connect(ui->graphicsView, SIGNAL(sendMousePress()),this,SLOT(receiveMousePress()));
    connect(ui->graphicsView_preview, SIGNAL(sendMousePress()),this,SLOT(receiveMousePressPreview()));

    //Set up QtNetwork
    m_client = 0;
    m_server = new LocalSocketIpcServer("simpleProcessing", this);
    connect(m_server, SIGNAL(messageReceived(QString)), this, SLOT(socketIcpMessage(QString)));
    mem = new shareMemory();

    setUIEnable(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    disconnect(m_server, SIGNAL(messageReceived(QString)), this, SLOT(socketIcpMessage(QString)));
    if(m_client)
        disconnect(m_client, SIGNAL(socketClientStatus(int)), this, SLOT(socketClientStatus(int)));
    if(ist)
        receiveSubWindowClose(0);
    if(pref)
    {
        disconnect(ui->graphicsView, SIGNAL(sendMousePress()),this,SLOT(receiveMousePress()));
        disconnect(ui->graphicsView_preview, SIGNAL(sendMousePress()),this,SLOT(receiveMousePressPreview()));
        pref->deleteLater();
    }
    if(mem){delete mem;}
    if(m_server){m_server->deleteLater();}
        if(m_client){m_client->deleteLater();}
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

void MainWindow::setUIEnable(bool status)
{
    ui->menuEdit->setEnabled(status);
    ui->actionCustom_Filter->setEnabled(status);
    ui->actionImage_Subtractor->setEnabled(status);
    ui->actionPrint->setEnabled(status);
    ui->menuExport->setEnabled(status);
}

void MainWindow::receiveMousePress() //qgraphicsView mouse press event. change histogram.
{
    if(!image.empty())
        setShowHistogram(image);
}

void MainWindow::receiveMousePressPreview()
{
    if(!image.empty())
        setShowHistogram(imagePreview);
}

void MainWindow::setShowImage(cv::Mat &img)
{
    setShowHistogram(img);

    if(img.type()==CV_8UC3)
    {
       ui->graphicsView->setImage(img);
    }
    else if(img.type() == CV_8UC1)
    {
        cv::Mat show;
        myCV::myCvtColor(img, show, myCV::GRAY2GBR);
        ui->graphicsView->setImage(show);
    }
}
void MainWindow::setShowImagePreview(cv::Mat &img)
{
    setShowHistogram(img);
    QImage qshow;
    if(img.type()==CV_8UC3)
    {
        qshow = QImage(img.data, img.size().width, img.size().height, img.step, QImage::Format_RGB888).rgbSwapped();
    }
    else if(img.type() == CV_8UC1)
    {
        cv::Mat show;
        myCV::myCvtColor(img, show, myCV::GRAY2GBR);
        qshow = QImage(show.data, show.size().width, show.size().height, show.step, QImage::Format_RGB888).rgbSwapped();
    }
    qshow = qshow.scaled(280,200,Qt::KeepAspectRatio);
    ui->graphicsView_preview->setImagefromQImage(qshow);
}

void MainWindow::setShowHistogram(cv::Mat &img)
{
    cv::Mat his;
    myCV::histogram(img, his);
    ui->histogram->setPixmap(QPixmap::fromImage(QImage(his.data, his.cols, his.rows, his.step, QImage::Format_RGB888).rgbSwapped()));
}

void MainWindow::backupImage(cv::Mat &img)  //let user undo
{
    forwardImg.clear();
    ui->actionForward->setEnabled(false);
    if(recoverImg.size() > recoverLimit)
    {
        recoverImg.erase(recoverImg.begin());
    }
    recoverImg.push_back(img);
}

void MainWindow::on_actionOpen_Image_triggered()
{
    fileName = QFileDialog::getOpenFileName(this,tr("Open File"),0,"Image files (*.png *.bmp *.jpg);;PNG (*.png);;BMP (*.bmp);;JPG (*.jpg)");
    if(fileName.isEmpty())
        return;
    if(!image.empty())
    {
        backupImage(image);
        ui->actionBack->setEnabled(true);
    }
    image = cv::imread(fileName.toStdString());
    if(!image.empty())
    {
        initialViewer();
    }
    ui->statusBar->showMessage("Image: " + fileName + " opened!");
}

void MainWindow::initialViewer()
{
    imagePreview = image.clone();
    ui->graphicsView->initialize(1, image.cols, image.rows);  //initial qgraphics view
    ui->graphicsView_preview->initialize(1, 280, 200);
    setShowImage(image);
    setShowImagePreview(imagePreview);
    setUIEnable(true);
}

void MainWindow::on_actionGrayscale_average_triggered()
{
    if(!image.empty())
    {
        backupImage(image);
        myCV::myCvtColor(image, image, myCV::BGR2GRAY, myCV::GRAY_AVG);
        setShowImage(image);
        ui->actionBack->setEnabled(true);
        ui->statusBar->showMessage("The image is now grayscaled (average).");
    }
}

void MainWindow::on_actionGrayscale_luminosity_triggered()
{
    if(!image.empty())
    {
        backupImage(image);
        myCV::myCvtColor(image, image, myCV::BGR2GRAY, myCV::GRAY_LUMI);
        setShowImage(image);
        ui->actionBack->setEnabled(true);
        ui->statusBar->showMessage("The image is now grayscaled (luminosity).");
    }
}

void MainWindow::on_actionResize_triggered()
{
    if(!image.empty())
    {      
        DialogResize r;  //open dialog
        r.getCurrentImageProperties(image.size().width, image.size().height);
        if(r.exec()== QDialog::Accepted) //return when the key is pressed.
        {
            backupImage(image);
            myCV::myResize(image, image,r.getWidth(),r.getHeight(),r.isAspect(), r.getMethod());
            setShowImage(image);
            ui->actionBack->setEnabled(true);
            ui->statusBar->showMessage("Image resized to: " + QString::number(r.getWidth()) + ", " + QString::number(r.getHeight()) + ".");
        }
    }
}

void MainWindow::on_actionBack_triggered() //undo function
{
    forwardImg.push_back(image);
    image = recoverImg[recoverImg.size()-1].clone();
    recoverImg.erase(recoverImg.end()-1);
    setShowImage(image);

    if(recoverImg.empty())
        ui->actionBack->setEnabled(false);
    ui->actionForward->setEnabled(true);
    ui->statusBar->showMessage("Undo");
}

void MainWindow::on_actionForward_triggered()  //redo function
{
    recoverImg.push_back(image);
    image = forwardImg[forwardImg.size()-1].clone();
    forwardImg.erase(forwardImg.end()-1);
    setShowImage(image);

    if(forwardImg.empty())
        ui->actionForward->setEnabled(false);
    ui->actionBack->setEnabled(true);
    ui->statusBar->showMessage("Redo");
}

void MainWindow::on_actionThreshold_triggered()
{
    if(!image.empty())
    {
        backupImage(image);
        myCV::myThreshold(image, image);
        setShowImage(image);
        ui->actionBack->setEnabled(true);
        ui->statusBar->showMessage("Image Thresholding completed.");
    }
}

void MainWindow::on_actionHistogram_Equalization_triggered()
{
    if(!image.empty())
    {
        backupImage(image);
        myCV::EqualizeHist(image, image);
        setShowImage(image);
        ui->actionBack->setEnabled(true);
        ui->statusBar->showMessage("Image is now equalized.");
    }
}

void MainWindow::on_horizontalSlider_contrast_sliderReleased()
{
    if(!image.empty())
    {
        std::vector<std::vector<int>> data;
        myCV::histogram(image, cv::Mat(),data);

        int&& val = myCV::findHistLargestPos(data);
        double&& con = (ui->horizontalSlider_contrast->value()-50)/10.0;
        int&& max_r = 255 - val;
        int&& min_r = val - 0;

        if(con > 0)
        {
            max_r = max_r * (1 - (std::abs(con)/5.0));
            min_r = min_r * (1 - (std::abs(con)/5.0));
        }
        if(con < 0)
        {
            max_r = max_r * (1 + (std::abs(con)/5.0));
            min_r = min_r * (1 + (std::abs(con)/5.0));
        }

        myCV::myContrast(image, imagePreview, (val - min_r), (val + max_r));
        setShowImagePreview(imagePreview);
    }
}

void MainWindow::on_horizontalSlider_contrast_valueChanged(int value)
{
    ui->label_contrastInfo->setText(QString::number((double)((value - 50)/10.0),'f',1));
}

void MainWindow::on_actionContrast_triggered()  //Let user modify contrast from -5.0 to 5.0.
{
    if(!image.empty())
    {
        backupImage(image);
        std::vector<std::vector<int>> data;
        myCV::histogram(image, cv::Mat(),data);

        int&& val = myCV::findHistLargestPos(data);
        double&& con = (ui->horizontalSlider_contrast->value()-50)/10.0;
        int&& max_r = 255 - val;
        int&& min_r = val - 0;

        if(con > 0)
        {
            max_r = max_r * (1 - (std::abs(con)/5.0));
            min_r = min_r * (1 - (std::abs(con)/5.0));
        }
        if(con < 0)
        {
            max_r = max_r * (1 + (std::abs(con)/5.0));
            min_r = min_r * (1 + (std::abs(con)/5.0));
        }

        myCV::myContrast(image, image, (val - min_r), (val + max_r));
        setShowImage(image);
        ui->actionBack->setEnabled(true);
        ui->statusBar->showMessage("Image contrast increased " + QString::number(con,'f',1)+ ".");
    }
}

void MainWindow::on_actionBlur_triggered()
{
    if(!image.empty())
    {
        DialogBlur b;
        if(b.exec() == QDialog::Accepted)
        {
            backupImage(image);
            b.useGaussian() == true ? myCV::Blur::Gaussian(image, image, b.getMaskSize(), b.getSigma(), b.getSigma()) :
                                      myCV::Blur::simple(image, image, b.getMaskSize());
            setShowImage(image);
            ui->actionBack->setEnabled(true);
            ui->statusBar->showMessage("Image blurred.");
        }
    }
}

void MainWindow::on_actionPreference_triggered()
{
    if(pref->exec() == QDialog::Accepted)
    {
        saveSettings();
    }
}

void MainWindow::on_actionOpenImage_triggered()
{
    ui->actionOpen_Image->trigger();
}

void MainWindow::on_actionOptions_triggered()
{
     ui->actionPreference->trigger();
}

void MainWindow::on_horizontalSlider_grayResolution_sliderReleased()
{
    if(!image.empty())
    {
        myCV::grayResolutionScale(image, imagePreview, ui->horizontalSlider_grayResolution->value());
        setShowImagePreview(imagePreview);
    }
}

void MainWindow::on_horizontalSlider_grayResolution_valueChanged(int value)
{
    ui->label_grayResolutionInfo->setText(QString::number(value,'f',1));
}

void MainWindow::on_actionGray_Resolution_Scale_triggered()
{
    if(!image.empty())
    {
        backupImage(image);
        myCV::grayResolutionScale(image, image, ui->horizontalSlider_grayResolution->value());
        setShowImage(image);
        ui->actionBack->setEnabled(true);
        ui->statusBar->showMessage("Image grayscale resolution is now 2^" + QString::number(ui->horizontalSlider_grayResolution->value()) + ".");
    }
}

//Noise Reduction start//
void MainWindow::on_actionMedian_Filter_triggered()
{
    if(!image.empty())
    {
        backupImage(image);
        DialogSize f;
        f.setWindowTitle("Median Filter Size");
        if(f.exec() == QDialog::Accepted)
        {
            spendT = clock();
            int sizes = f.getSize()%2 == 0 ? f.getSize()-1 : f.getSize();
            myCV::medianFilter(image, image, sizes);
            spendT = clock() - spendT;
            setShowImage(image);
            ui->actionBack->setEnabled(true);
            ui->statusBar->showMessage("Noise reduced by median filter. Compute time: "+QString::number((float)spendT/CLOCKS_PER_SEC)+" sec.");
        }
    }
}

void MainWindow::on_actionMean_Filter_triggered()
{
    if(!image.empty())
    {
        backupImage(image);
        DialogSize f;
        f.setWindowTitle("Mean Filter Size");
        if(f.exec() == QDialog::Accepted)
        {
            spendT = clock();
            int sizes = f.getSize()%2 == 0 ? f.getSize()-1 : f.getSize();
            myCV::myCvtColor(image, image, myCV::BGR2GRAY);
            myCV::Blur::simple(image, image, sizes);
            spendT = clock() - spendT;
            setShowImage(image);
            ui->actionBack->setEnabled(true);
            ui->statusBar->showMessage("Noise reduced by mean filter. Compute time: "+QString::number((float)spendT/CLOCKS_PER_SEC)+" sec.");
        }
    }
}
//Noise Reduction end//

void MainWindow::on_actionFuzzy_Logic_triggered()
{
    if(!image.empty())
    {
        DialogFuzzyLogic fuzzy;
        fuzzy.readImage(image);
        if(fuzzy.exec() == QDialog::Accepted)
        {
            backupImage(image);
            fuzzy.computeResult(image);
            setShowImage(image);
            ui->actionBack->setEnabled(true);
            ui->statusBar->showMessage("Fuzzy edge detected.");
        }
    }
}

void MainWindow::on_actionLaplacian_Filter_triggered()
{
    if(!image.empty())
    {
        backupImage(image);
        myCV::laplacianFilter(image, image);
        setShowImage(image);
        ui->actionBack->setEnabled(true);
        ui->statusBar->showMessage("Laplcian Filter Applied.");
    }
}

void MainWindow::on_actionCustom_Filter_triggered()
{
    if(!image.empty())
    {
        DialogCustomFilter cf;
        if(cf.exec() == QDialog::Accepted)
        {
            std::vector<int> data;
            int w, h;
            cf.getVectorData(data, w, h);

#ifdef _DEBUG
            for(int j = 0 ; j< h ; j++)
            {
                for(int i =0 ; i< w; i ++)
                {
                    std::cout << data[j*w+i] << " ";
                }
                std::cout << std::endl;
            }
#endif
            backupImage(image);
            myCV::customFilter(image, image, w, h, data);
            setShowImage(image);
            ui->actionBack->setEnabled(true);
            ui->statusBar->showMessage("Custom Filter Applied.");
        }
    }
}

void MainWindow::on_actionImage_Subtractor_triggered()
{
    if(!image.empty())
    {
        if(!ist)
        {
            ist = new dialogimagesubtracttool();
            connect(ist, SIGNAL(windowClosed(int)), this, SLOT(receiveSubWindowClose(int)));
        }
        ist->setOriginImage(image);
        ist->show();
    }
}

void MainWindow::on_actionSobel_Filter_triggered()
{
    if(!image.empty())
    {
        backupImage(image);
        myCV::sobelFilter(image, image);
        setShowImage(image);
        ui->actionBack->setEnabled(true);
        ui->statusBar->showMessage("Sobel Filter Applied.");
    }
}

void MainWindow::on_actionPrint_triggered()
{    
    if(!image.empty())
    {
        QPrinter printer(QPrinter::HighResolution);
        //printer.setOutputFileName("print.ps");
        printer.setOutputFormat(QPrinter::NativeFormat);
        QPrintPreviewDialog preview(&printer, this);
        preview.setWindowFlags(Qt::Window);
        connect(&preview, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printPreview(QPrinter*)));
        if(preview.exec() != QDialog::Accepted)
            return;
    }
}

void MainWindow::printPreview(QPrinter* printer)
{
    QPainter *painter = new QPainter(printer);
    QImage pImg = QImage(image.data, image.size().width, image.size().height, image.step, QImage::Format_RGB888).rgbSwapped();
    if(pImg.width()  > printer->pageRect().width() ||
       pImg.height() > printer->pageRect().height())
        pImg = pImg.scaled(printer->pageRect().width(),
                           printer->pageRect().height(), Qt::KeepAspectRatio);
    painter->drawImage(0, 0, pImg);
    painter->end();
}

void MainWindow::on_actionTo_PDF_triggered()
{
    if(!image.empty())
    {
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        QPrintPreviewDialog preview(&printer, this);
        preview.setWindowFlags(Qt::Window);
        connect(&preview, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printPreview(QPrinter*)));
        if(preview.exec() != QDialog::Accepted)
            return;
    }
}

void MainWindow::on_actionFourier_Transform_triggered()
{
    if(checkPID::isRunning("spectralfilteringtool.exe"))
    {
        QMessageBox::warning(0,"Warning", "Application already opened.");
        return;
    }
    QProcess::startDetached("spectralfilteringtool.exe");
}

void MainWindow::on_actionSpectralFilteringToolMenubar_triggered()
{
    on_actionFourier_Transform_triggered();
}

void MainWindow::on_actionColor_Splitter_triggered()
{
    if(checkPID::isRunning("colorsplittool.exe"))
    {
        QMessageBox::warning(0,"Warning", "Application already opened.");
        return;
    }
    QProcess::startDetached("colorsplittool.exe");
}

void MainWindow::socketClientStatus(int status)
{
    // 0 - Ready, 1 - disconnected, 2 - error
    if(status == 2)
    {
        QMessageBox::warning(0, "Error", "Application is not opened.");
    }
}

void MainWindow::socketIcpMessage(QString message)
{
#ifdef _DEBUG
    qDebug() << message;
    qDebug() << message.left(1).toInt();
#endif
    // 1 - spectralFilterTool 2 - colorSplitTool
    std::string name;
    if(message.left(1).toInt() == 1) {name = "spectralFilterTool";}
    else if(message.left(1).toInt() == 2) {name = "colorSplitTool";}
    else
    {
        return;
    }
#ifdef _DEBUG
    qDebug() << QString::fromStdString(name);
#endif
    if(!m_client)
    {
        m_client = new LocalSocketIpcClient(name.c_str(), this); //spectralFilterTool
        connect(m_client, SIGNAL(socketClientStatus(int)), this, SLOT(socketClientStatus(int)));
    }
    else
    {
        m_client->setServerName(name.c_str());
    }

    if(message.contains("requestImage"))
    {
        if(image.empty())
        {
            m_client->sendMessageToServer("NoImage.");
            return;
        }
        if(mem->addToSharedMemory(image))
            m_client->sendMessageToServer("ok " + QString::number(image.type()));
        else
        {
            m_client->sendMessageToServer("Porting fail.");
        }
    }
    else if(message.contains("ok")) // "ok"
    {
        int imageType = message.right(3).toInt();
        cv::Mat tmp;
        if(!mem->readFromSharedMemory(tmp, imageType))
            return;
        if(!tmp.empty())
        {
            backupImage(image);
            image = tmp.clone();
            ui->actionBack->setEnabled(true);
        }
        initialViewer();
        m_client->sendMessageToServer("done");
    }
    else if(message.contains("done"))
    {
        mem->requestDetach();
    }
    else if(message.contains("ImportImage"))
    {
        m_client->sendMessageToServer("requestImage");
    }
}

void MainWindow::receiveSubWindowClose(int num)
{
    //0 - image subtractor tool
    //1 - Spectral Filtering tool
    if(num == 0)
    {
       disconnect(ist, SIGNAL(windowClosed(int)), this, SLOT(receiveSubWindowClose(int)));
       ist->deleteLater();
       ist = 0;
    }
}



void MainWindow::on_actionBi_histogram_Equalization_triggered()
{
    if(!image.empty())
    {
        if(image.type() != CV_8UC1)
        {
            ui->statusBar->showMessage("Currently only supports gray images.");
            return;
        }

        backupImage(image);
        myCV::BBHE(image, image);
        setShowImage(image);
        ui->actionBack->setEnabled(true);
        ui->statusBar->showMessage("Image is now equalized.");
    }
}
