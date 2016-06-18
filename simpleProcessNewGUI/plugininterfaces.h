#ifndef PLUGININTERFACES
#define PLUGININTERFACES

#include <QMainWindow>
#include <QtPlugin>
#include <QDialog>
#ifdef HAVE_OPENCV
#include "opencv2/opencv.hpp"
#endif


struct functionSettings
{
    QString func_name;
    int func_value_type; //0 for int, 1 for string
    int func_value_int;
    QString func_value_string;
};


class imageProcessInterface
{
public:
    virtual ~imageProcessInterface(){}
    virtual QStringList processIndex() const = 0;
    virtual QDialog dialog() const = 0;
    virtual std::vector<functionSettings> processSettings() const = 0;
    virtual void process(QImage &img) = 0;
#ifdef HAVE_OPENCV
    virtual void process(cv::Mat &img) = 0;
#endif
};

class toolsInterface
{
public:
    virtual ~toolsInterface(){}
    virtual QStringList toolsIndex() const = 0;
    virtual void showUI() = 0;
    virtual void setImage(QImage &img) = 0;
    virtual QImage returnImage() = 0;
#ifdef HAVE_OPENCV
    virtual void setImage(cv::Mat &img) = 0;
    virtual cv::Mat returncvImage() = 0;
#endif
signals:
    virtual void sendDataOnClose(cv::Mat result_img, bool isChanged = false, bool isNew = false) = 0;
public slots:
    virtual void closeUI() = 0;
};

QT_BEGIN_NAMESPACE

#define imageProcessInterface_iid "org.KKyang-project.SimpleProcess.imageProcessInterface"

Q_DECLARE_INTERFACE(imageProcessInterface, imageProcessInterface_iid)

#define toolsInterface_iid "org.KKyang-project.SimpleProcess.toolsInterface"

Q_DECLARE_INTERFACE(toolsInterface, toolsInterface_iid)

QT_END_NAMESPACE

#endif // PLUGININTERFACES

