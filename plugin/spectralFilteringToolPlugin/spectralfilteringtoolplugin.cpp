#include "spectralFilteringToolPlugin.h"
#include <QDebug>
spectralFilteringToolPlugin::~spectralFilteringToolPlugin()
{

}

void spectralFilteringToolPlugin::receiveImageRequest()
{
    requestImage();
}

void spectralFilteringToolPlugin::receiveSendImage(cv::Mat &img, bool isChanged, bool isNew)
{
    sendDataOnClose(img, isChanged, isNew, ui->getOpenedFileName());
}

void spectralFilteringToolPlugin::receiveWindowClose()
{
    disconnect(ui, SIGNAL(sendSig()),this,SLOT(receiveWindowClose()));
    disconnect(ui, SIGNAL(sendExportImg(cv::Mat&, bool, bool)),this,SLOT(receiveSendImage(cv::Mat&, bool, bool)));
    disconnect(ui, SIGNAL(sendImageRequest()),this,SLOT(receiveImageRequest()));
    delete ui;
    ui = 0;
}

QStringList spectralFilteringToolPlugin::toolsIndex() const
{
    return QStringList() << tr("Spectral Filtereing Tool");
}

void spectralFilteringToolPlugin::showUI(QString &actiontitle)
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

void spectralFilteringToolPlugin::closeUI()
{
    ui->close();
}

void spectralFilteringToolPlugin::setImage(QImage &img)
{
}

QImage spectralFilteringToolPlugin::returnImage()
{
    return QImage();
}

#ifdef HAVE_OPENCV
void spectralFilteringToolPlugin::setImage(cv::Mat &img)
{
    //_img = img.clone();
    ui->setImageFromMain(img);
    ui->receiveRequestedImage();
}

cv::Mat spectralFilteringToolPlugin::returncvImage()
{
    return cv::Mat();
}
#endif

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(spectralFiltering, spectralFilteringToolPlugin)
#endif // QT_VERSION < 0x050000
