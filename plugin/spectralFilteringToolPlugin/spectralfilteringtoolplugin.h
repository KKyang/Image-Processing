#ifndef SPECTRALFILTERINGTOOL_H
#define SPECTRALFILTERINGTOOL_H

#include <QObject>
#include <QtPlugin>
#include <QStringList>
#include <QImage>

#include "mainwindow.h"
#include "../../simpleProcessNewGUI/plugininterfaces.h"


class spectralFilteringToolPlugin : public QObject,
                                    public toolsInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.KKyang-project.SimpleProcess.toolsInterface" FILE "spectralFilteringToolPlugin.json")
#endif // QT_VERSION >= 0x050000
    Q_INTERFACES(toolsInterface)

public:
    ~spectralFilteringToolPlugin();
    QStringList toolsIndex() const Q_DECL_OVERRIDE;
    void showUI(QString &actiontitle) Q_DECL_OVERRIDE;
    void setImage(QImage &img) Q_DECL_OVERRIDE;
    QImage returnImage() Q_DECL_OVERRIDE;
#ifdef HAVE_OPENCV
    void setImage(cv::Mat &img) Q_DECL_OVERRIDE;
    cv::Mat returncvImage() Q_DECL_OVERRIDE;
#endif
signals:
    void requestImage();
    void sendDataOnClose(cv::Mat result_img, bool isChanged = false, bool isNew = false, QString filename = QString());
public slots:
    void closeUI() Q_DECL_OVERRIDE;
private slots:
    void receiveImageRequest();
    void receiveSendImage(cv::Mat &img, bool isChanged, bool isNew);
    void receiveWindowClose();
private:
    MainWindow *ui = 0;
};

#endif // SPECTRALFILTERINGTOOL_H
