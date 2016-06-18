#include "colorsplitterplugin.h"
#include <QDebug>
colorSplitterPlugin::~colorSplitterPlugin()
{

}

void colorSplitterPlugin::receiveWindowClose()
{
    disconnect(&ui, SIGNAL(sendSig()),this,SLOT(receiveWindowClose()));
    sendDataOnClose(cv::Mat(),false,false);
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
    //_img = img.clone();
    ui.setImageFromMain(img);
}

cv::Mat colorSplitterPlugin::returncvImage()
{
    return cv::Mat();
}
#endif

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(colorSplitter, colorSplitterPlugin)
#endif // QT_VERSION < 0x050000
