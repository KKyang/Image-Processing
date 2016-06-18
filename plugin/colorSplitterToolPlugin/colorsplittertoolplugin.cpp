#include "colorsplittertoolplugin.h"
#include <QDebug>
colorSplitterToolPlugin::~colorSplitterToolPlugin()
{

}

void colorSplitterToolPlugin::receiveWindowClose()
{
    disconnect(ui, SIGNAL(sendSig()),this,SLOT(receiveWindowClose()));
    delete ui;
    ui = 0;
}

QStringList colorSplitterToolPlugin::toolsIndex() const
{
    return QStringList() << tr("Color Splitter");
}

void colorSplitterToolPlugin::showUI(QString &actiontitle)
{
    if(!ui)
    {
        ui = new MainWindow;
        if(!ui->isVisible())
        {
            connect(ui, SIGNAL(sendSig()),this,SLOT(receiveWindowClose()));
            ui->show();
        }
    }
}

void colorSplitterToolPlugin::closeUI()
{
    ui->close();
}

void colorSplitterToolPlugin::setImage(QImage &img)
{
}

QImage colorSplitterToolPlugin::returnImage()
{
    return QImage();
}

#ifdef HAVE_OPENCV
void colorSplitterToolPlugin::setImage(cv::Mat &img)
{
    //_img = img.clone();
    ui->setImageFromMain(img);
}

cv::Mat colorSplitterToolPlugin::returncvImage()
{
    return cv::Mat();
}
#endif

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(colorSplitter, colorSplitterToolPlugin)
#endif // QT_VERSION < 0x050000
