#ifndef FOGDETECT_H
#define FOGDETECT_H
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core/core.hpp"
#include <QImage>
#include <QList>
#include <iomanip>
#include "asmopencv.h"



struct object {
    int distanse;
    int x;
    int y;
    int w;
    int h;
};

class FogDetect {
private:
    //obraz testowy
    cv::Mat testImage;
    //Wczytane wspolrzedne do badania
    QList<object> cordList;
    //rezultat badania obrazu bez mgły
    QMap<int, int> ClearResultArea;
    //rezultat badania obrazu testowego
    QMap<int, double> detectResult;

    void analysisResults(QMap<int, double> result, int threshold, int& visibility, int& fog);
    void drawObject(cv::Mat& img, QMap<int, double> result,int threshold);
    void drawRectangle(cv::Mat& img, int x, int y, int w, int h, cv::Scalar color);
    void printResult(cv::Mat& img, int visibility, int fog);
    double getPercentage(int x, int y);
public:
    FogDetect();
    void setTestImage(QImage image);
    void setCordObject(QList<object> cordList);
    void detectObject(cv::Mat const& image, QList<object> cordList, QMap<int, int>& reasultCord);
    QImage getFogDetect(const QImage &image, int threshold);



};


#endif // FOGDETECT_H
