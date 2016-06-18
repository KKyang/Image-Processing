#include "colorsplitterplugin.h"
#include <QDebug>
colorSplitterPlugin::~colorSplitterPlugin()
{
    sendDataOnClose(_img,false,false);
    qDebug() <<"delete1";
    qDebug() <<"delete";
}

void colorSplitterPlugin::receiveWindowClose()
{
    disconnect(&ui, SIGNAL(sendSig()),this,SLOT(receiveWindowClose()));
    sendDataOnClose(_img,false,false);
}

QStringList colorSplitterPlugin::toolsIndex() const
{
    return QStringList() << tr("Color Splitter");
}

void colorSplitterPlugin::showUI()
{
    if(!ui.isVisible())
    {
        connect(&ui, SIGNAL(sendSig()),this,SLOT(receiveWindowClose()));
        ui.show();
    }

    //sendDataOnClose(_img,false,false);
}

void colorSplitterPlugin::closeUI()
{
    ui.close();
}

void colorSplitterPlugin::setImage(QImage &img)
{
}

QImage colorSplitterPlugin::returnImage()
{
    return QImage();
}

#ifdef HAVE_OPENCV
void colorSplitterPlugin::setImage(cv::Mat &img)
{
    _img = img.clone();
}

cv::Mat colorSplitterPlugin::returncvImage()
{
    return cv::Mat();
}
#endif

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(colorSplitter, colorSplitterPlugin)
#endif // QT_VERSION < 0x050000
