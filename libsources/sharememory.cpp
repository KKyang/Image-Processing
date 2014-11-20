#include "sharememory.h"

shareMemory::shareMemory()
{
}

bool shareMemory::addToSharedMemory(cv::Mat &img)
{
    if(img.empty())
        return false;

    if (sharedMemory.isAttached())
             sharedMemory.detach();

    cv::Mat tmp;
    if(img.type() == CV_8UC1)
    {
        myCV::myCvtColor(img, tmp, myCV::GRAY2GBR);
    }
    else
    {
        tmp = img.clone();
    }
    QImage image = QImage(tmp.data, tmp.cols, tmp.rows, tmp.step, QImage::Format_RGB888);

    QBuffer buffer;
    buffer.open(QBuffer::WriteOnly);
    QDataStream out(&buffer);
    out << image;
    int size = buffer.size();

    sharedMemory.setKey("0");
    if (!sharedMemory.create(size)) {
        std::cout << "Unable to create shared memory segment." << std::endl;
        return false;
    }
    sharedMemory.lock();
    char *to = (char*)sharedMemory.data();
    const char *from = buffer.data().data();
    memcpy(to, from, qMin(sharedMemory.size(), size));
    sharedMemory.unlock();
    return true;
}

bool shareMemory::readFromSharedMemory(cv::Mat &img, const int imageType)
{
    sharedMemory.setKey("0");
    if (!sharedMemory.attach()) {
        std::cout << "Unable to attach to shared memory segment." <<std::endl;
        //ui.label->setText(tr("Unable to attach to shared memory segment.\n" \
        //                     "Load an image first."));
        return false;
    }

    QBuffer buffer;
    QDataStream in(&buffer);
    QImage image;

    sharedMemory.lock();
    buffer.setData((char*)sharedMemory.constData(), sharedMemory.size());
    buffer.open(QBuffer::ReadOnly);
    in >> image;
    sharedMemory.unlock();
    sharedMemory.detach();

    image = image.rgbSwapped();
    cv::Mat temp(image.height(), image.width(), CV_8UC4, image.bits(), image.bytesPerLine());

    img.release();
    img = cv::Mat(image.height(), image.width(), imageType).clone();

    if(imageType == CV_8UC3)
        for(int j = 0; j < img.rows; j++)
        {
            for(int i = 0; i < img.cols; i++)
            {
                for(int k = 0; k < 3; k++)
                    img.at<cv::Vec3b>(j,i)[k] = temp.at<cv::Vec4b>(j,i)[k];
            }
        }
    else if(imageType == CV_8UC1)
    {
        for(int j = 0; j < img.rows; j++)
        {
            for(int i = 0; i < img.cols; i++)
            {
                img.at<uchar>(j,i) = temp.at<cv::Vec4b>(j,i)[0];
            }
        }
    }
}

void shareMemory::requestDetach()
{
    if(sharedMemory.isAttached())
        sharedMemory.detach();
}
