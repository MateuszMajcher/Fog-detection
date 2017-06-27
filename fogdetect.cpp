#include "fogdetect.h"


int countPixel(const cv::Mat& src) {
    int count_black = 0;
    int count_white = 0;
    for( int y = 0; y < src.rows; y++ ) {
    for( int x = 0; x < src.cols; x++ ) {
            // change this to to 'src.atuchar>(y,x) == 255'
            // if your img has only 1 channel
            if ( src.at<uchar>(y,x) == 255 ) {
                count_white++;
            }
            else if ( src.at<uchar>(y,x) == 0 ) {
                count_black++;
            }
        }
    }
   // cout<<count_black+count_white<<endl;
    return count_white;
}

cv::Mat detected_edges_candy(const cv::Mat& image, int threshold1, int threshold2)
{
    cv::Mat endgeImage;
     // Canny edge
    cv::Canny(image, endgeImage, threshold1, threshold2);
    return endgeImage;
}





FogDetect::FogDetect() {

}


void FogDetect::setTestImage(QImage image) {
    this->testImage = ASM::QImageToCvMat( image, true );
    detectObject(testImage, cordList, ClearResultArea);
    for (auto p: ClearResultArea.toStdMap())
        qDebug()<<"ustawiono c dla d: "<<p.first<<" = "<<p.second;
}

void FogDetect::setCordObject(QList<object> cordList) {
    this->cordList = cordList;
}

QImage FogDetect::getFogDetect(const QImage &image, int threshold) {
    QMap<int, int> tmp;
    cv::Mat cvMat = ASM::QImageToCvMat( image, true );
    detectObject(cvMat, cordList, tmp);
    for (auto& p: cordList) {
        //obilczenie procentów zamglenia
        double percent = getPercentage(tmp[p.distanse], ClearResultArea[p.distanse]);
        //tekst do wyswietlenia
        std::string dist_text = "d: " + std::to_string(p.distanse)+"m";
        std::stringstream str;
        str<<"w: "<< std::fixed << std::setprecision(0) << percent<<"%";
        std::string percent_text = str.str();
        qDebug()<< percent_text.c_str() ;
        //rysowanie analizowanych obszarów
        cv::Scalar color;
        if (percent > threshold)
             color  = cv::Scalar(0,255,0);
        else
            color  = cv::Scalar(0,0,255);

        drawRectangle(cvMat, p.x, p.y, p.w, p.h, color);
        putText(cvMat, dist_text.c_str(), cv::Point(p.x,p.y - 20), CV_FONT_NORMAL, 1.2, cv::Scalar(255,0,0),1.5,1);
        putText(cvMat, percent_text.c_str(), cv::Point(p.x,p.y + p.h + 40), CV_FONT_NORMAL, 1.2, cv::Scalar(255,0,0),1.5,1);
        printResult(cvMat);
        qDebug()<<"detect "<<p.distanse<<" "<<tmp[p.distanse]<<" "<<ClearResultArea[p.distanse];

    }
    QImage o = ASM::cvMatToQImage(cvMat);
    return o;
}


void FogDetect::detectObject(cv::Mat const& image, QList<object> cordList, QMap<int, int>& reasultCord) {
    for (auto& p: cordList) {
        cv::Rect myROI(p.x, p.y, p.w, p.h);
        cv::Mat cropp = image(myROI);
        cv::Mat edges = detected_edges_candy(cropp, 80,120);
        int whitePixel = countPixel(edges);
        reasultCord[p.distanse] = whitePixel;
        qDebug()<<reasultCord[p.distanse];
    }

}

double FogDetect::getPercentage(int x, int y) {
    double percent = ((double)x/(double)y)*100;
    if (percent >= 100.0) {
        return 100.0;
    } else {
        return percent;
    }
}


void FogDetect::drawRectangle(cv::Mat& img, int x, int y, int w, int h, cv::Scalar color) {
    cv::Rect rect(x, y, w, h);
    cv::Point pt1(x, y);
    cv::Point pt2(x + w, y + h);
    cv::rectangle(img, pt1, pt2, color, 10, 8, 0);
}

void FogDetect::printResult(cv::Mat& img) {
    cv::Scalar color(0,0,255);
    std::string visibility = "visibility: " + std::to_string(100)+"m";
    std::string fog = "fog: " + std::to_string(100)+"%";
    putText(img, visibility.c_str(), cv::Point(img.cols - 350,50), CV_FONT_NORMAL, 1.4, color,2,2);
    putText(img, fog.c_str(), cv::Point(img.cols - 350,100), CV_FONT_NORMAL, 1.4, color,2,2);
}
