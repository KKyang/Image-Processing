#ifndef DIALOGFUZZYLOGIC_H
#define DIALOGFUZZYLOGIC_H

#include <QDialog>
#include "../lib/imgcore.h"
#include "../lib/imageprocessing/fuzzylogic.h"

namespace Ui {
class DialogFuzzyLogic;
}

class DialogFuzzyLogic : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFuzzyLogic(QWidget *parent = 0);
    ~DialogFuzzyLogic();
    void readImage(cv::Mat &img);
    void computeResult(cv::Mat &outputResult);



private slots:
    void on_horizontalSlider_bellThreshold_valueChanged(int value);

    void on_spinBox_bellThreshold_valueChanged(int arg1);

    void on_horizontalSlider_outputLeftBound_valueChanged(int value);

    void on_spinBox_outputLeftBound_valueChanged(int arg1);

    void on_horizontalSlider_outputRightBound_valueChanged(int value);

    void on_spinBox_outputRightBound_valueChanged(int arg1);

    void on_horizontalSlider_bellThreshold_sliderReleased();

    void on_spinBox_bellThreshold_editingFinished();

    void on_horizontalSlider_outputLeftBound_sliderReleased();

    void on_spinBox_outputLeftBound_editingFinished();

    void on_horizontalSlider_outputRightBound_sliderReleased();

    void on_spinBox_outputRightBound_editingFinished();

private:
    void drawBell();
    void drawTriang();
    void setShowImage(cv::Mat &img);
    double sShape(int x, int a, int b, int c);
    double bellCurve(int x, int b, int c);
    double triangShape(int x, int center, int a, int b);

    Ui::DialogFuzzyLogic *ui;
    myCV::fuzzyLogic fuzzy;
    cv::Mat originImg;

    QVector<double> bell;
    struct curve
    {
        int mean;
        int b;
    }Bell;

    QVector<double> triangWH;
    QVector<double> triangBL;
    struct line
    {
        int center;
        int left;
        int right;
    }TriangWH, TriangBL;
};

#endif // DIALOGFUZZYLOGIC_H
