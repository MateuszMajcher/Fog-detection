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

void detectObject(cv::Mat const& image, QList<object> cordList);

class FogDetect {
private:
    cv::Mat testImage;
    QList<object> cordList;
    QMap<int, int> ClearResultArea;

public:
    FogDetect();
    void setTestImage(QImage image);
    void setCordObject(QList<object> cordList);
    void detectObject(cv::Mat const& image, QList<object> cordList, QMap<int, int>& reasultCord);
    QImage getFogDetect(const QImage &image);

};


#endif // FOGDETECT_H
