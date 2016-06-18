#include "templateToolPlugin.h"
#include <QDebug>
templateToolPlugin::~templateToolPlugin()
{

}

void templateToolPlugin::receiveImageRequest()
{
    requestImage();
}

void templateToolPlugin::receiveSendImage(cv::Mat &img, bool isChanged, bool isNew)
{
    sendDataOnClose(img, isChanged, isNew, ui->getOpenedFileName());
}

void templateToolPlugin::receiveWindowClose()
{
    disconnect(ui, SIGNAL(sendSig()),this,SLOT(receiveWindowClose()));
    disconnect(ui, SIGNAL(sendExportImg(cv::Mat&, bool, bool)),this,SLOT(receiveSendImage(cv::Mat&, bool, bool)));
    disconnect(ui, SIGNAL(sendImageRequest()),this,SLOT(receiveImageRequest()));
    delete ui;
    ui = 0;
}

QStringList templateToolPlugin::toolsIndex() const
{
    return QStringList() << tr("Template Tool");
}

void templateToolPlugin::showUI(QString &actiontitle)
{
    if(!ui)
    {
        ui = new MainWindow;
        if(!ui->isVisible())
        {
            connect(ui, SIGNAL(sendSig()),this,SLOT(receiveWindowClose()));
            connect(ui, SIGNAL(sendExportImg(cv::Mat&, bool, bool)),this,SLOT(receiveSendImage(cv::Mat&, bool, bool)));
            connect(ui, SIGNAL(sendImageRequest()),this,SLOT(receiveImageRequest()));
            ui->show();
        }
    }
}

void templateToolPlugin::closeUI()
{
    ui->close();
}

void templateToolPlugin::setImage(QImage &img)
{
}

QImage templateToolPlugin::returnImage()
{
    return QImage();
}

#ifdef HAVE_OPENCV
void templateToolPlugin::setImage(cv::Mat &img)
{
    //_img = img.clone();
    ui->setImageFromMain(img);
    ui->receiveRequestedImage();
}

cv::Mat templateToolPlugin::returncvImage()
{
    return cv::Mat();
}
#endif

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(spectralFiltering, templateToolPlugin)
#endif // QT_VERSION < 0x050000
