#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(1280, 720);

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

void MainWindow::receiveMousePress() //qgraphicsView mouse press event. change histogram.
{
    setShowHistogram(image);
}

void MainWindow::receiveMousePressPreview()
{
    setShowHistogram(imagePreview);
}

void MainWindow::setShowImage(cv::Mat &img)
{
    setShowHistogram(img);

    if(img.type()==CV_8UC3)
    {
       ui->graphicsView->setImage2(img);
    }
    else if(img.type() == CV_8UC1)
    {
        cv::Mat show;
        myCV::myCvtColor(img, show, myCV::GRAY2GBR);
        ui->graphicsView->setImage2(show);
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
    fileName = QFileDialog::getOpenFileName(this,tr("Open File"));
    image = cv::imread(fileName.toStdString());
    if(!image.empty())
    {
        imagePreview = image.clone();
        ui->graphicsView->initialize(1, image.rows, image.cols);  //initial qgraphics view
        ui->graphicsView_preview->initialize(1, 280, 200);
        setShowImage(image);
        setShowImagePreview(image);
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
    }
}

void MainWindow::on_actionResize_triggered()
{
    if(!image.empty())
    {
        backupImage(image);
        DialogResize r;  //open dialog
        r.getCurrentImageProperties(image.size().width, image.size().height);
        if(r.exec()== QDialog::Accepted) //return when the key is pressed.
        {
            myCV::myResize(image, image,r.getWidth(),r.getHeight(),r.isAspect());
            setShowImage(image);
            ui->actionBack->setEnabled(true);
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
}

void MainWindow::on_actionThreshold_triggered()
{
    if(!image.empty())
    {
        backupImage(image);
        myCV::myThreshold(image, image);
        setShowImage(image);
        ui->actionBack->setEnabled(true);
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

void MainWindow::on_actionContrast_triggered()
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
    }
}
