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

    QImage::Format qimageFormat = img.type() == CV_8UC3 ? QImage::Format_RGB888 : QImage::Format_Indexed8;
    QImage image = QImage(img.data, img.cols, img.rows, img.step, qimageFormat);

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

bool shareMemory::readFromSharedMemory(cv::Mat &img, int imageType)
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

    int channels = imageType == CV_8UC3 ? 3 : 1;
    img.release();
    img = cv::Mat(image.height(), image.width(), imageType).clone();

    for(int j = 0; j < temp.rows; j++)
    {
        for(int i = 0; i < temp.cols; i++)
        {
            for(int k = 0; k < channels; k++)
                img.at<cv::Vec3b>(j,i)[k] = temp.at<cv::Vec4b>(j,i)[k];
        }
    }
}
