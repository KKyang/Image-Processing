#include "transformerToolPlugin.h"
#include <QDebug>
transformerToolPlugin::~transformerToolPlugin()
{

}

void transformerToolPlugin::receiveImageRequest()
{
    requestImage();
}

void transformerToolPlugin::receiveSendImage(cv::Mat &img, bool isChanged, bool isNew)
{
    sendDataOnClose(img, isChanged, isNew, ui->getOpenedFileName());
}

void transformerToolPlugin::receiveWindowClose()
{
    disconnect(ui, SIGNAL(sendSig()),this,SLOT(receiveWindowClose()));
    disconnect(ui, SIGNAL(sendExportImg(cv::Mat&, bool, bool)),this,SLOT(receiveSendImage(cv::Mat&, bool, bool)));
    disconnect(ui, SIGNAL(sendImageRequest()),this,SLOT(receiveImageRequest()));
    delete ui;
    ui = 0;
}

QStringList transformerToolPlugin::toolsIndex() const
{
    return QStringList() << tr("Transformer Tool");
}

void transformerToolPlugin::showUI(QString &actiontitle)
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

void transformerToolPlugin::closeUI()
{
    ui->close();
}

void transformerToolPlugin::setImage(QImage &img)
{
}

QImage transformerToolPlugin::returnImage()
{
    return QImage();
}

#ifdef HAVE_OPENCV
void transformerToolPlugin::setImage(cv::Mat &img)
{
    //_img = img.clone();
    ui->setImageFromMain(img);
    ui->receiveRequestedImage();
}

cv::Mat transformerToolPlugin::returncvImage()
{
    return cv::Mat();
}
#endif

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(spectralFiltering, transformerToolPlugin)
#endif // QT_VERSION < 0x050000
