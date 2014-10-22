#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
}

MainWindow::~MainWindow()
{
    disconnect(ui->graphicsView, SIGNAL(sendMousePress()),this,SLOT(receiveMousePress()));
    disconnect(ui->graphicsView_preview, SIGNAL(sendMousePress()),this,SLOT(receiveMousePressPreview()));
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
    image = cv::imread(fileName.toStdString());
    if(!image.empty())
    {
        imagePreview = image.clone();
        ui->graphicsView->initialize(1, image.rows, image.cols);  //initial qgraphics view
        ui->graphicsView_preview->initialize(1, 280, 200);
        setShowImage(image);
        setShowImagePreview(image);
        ui->statusBar->showMessage("Image: " + fileName + " opened!");
    }
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
