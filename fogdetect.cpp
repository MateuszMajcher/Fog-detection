#include "fogdetect.h"


void drawRectangle(cv::Mat& img, int x, int y, int w, int h) {
    // our rectangle...
    cv::Rect rect(x, y, w, h);
    // and its top left corner...
    cv::Point pt1(x, y);
    // and its bottom right corner.
    cv::Point pt2(x + w, y + h);
    // These two calls...
    cv::rectangle(img, pt1, pt2, cv::Scalar(0, 255, 0), 10, 8, 0);
}

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

QImage FogDetect::getFogDetect(const QImage &image) {
    QMap<int, int> tmp;
    cv::Mat cvMat = ASM::QImageToCvMat( image, true );
    detectObject(cvMat, cordList, tmp);
    for (auto& p: cordList) {
        //obilczenie procentów zamglenia
        double percent = ((double)tmp[p.distanse]/(double)ClearResultArea[p.distanse])*100;
        //tekst do wyswietlenia
        std::string dist_text = "d: " + std::to_string(p.distanse)+"m";
        std::stringstream str;
        str<<"w: "<<std::setprecision( 2 ) << percent<<"%";
        std::string percent_text = str.str();
        //rysowanie analizowanych obszarów
        drawRectangle(cvMat, p.x, p.y, p.w, p.h);
        putText(cvMat, dist_text.c_str(), cv::Point(p.x,p.y - 20), CV_FONT_NORMAL, 1.2, cv::Scalar(0,0,255),2,1);
        putText(cvMat, percent_text .c_str(), cv::Point(p.x,p.y + p.h + 40), CV_FONT_NORMAL, 1.2, cv::Scalar(0,0,255),2,1);

        qDebug()<<"detect "<<p.distanse<<" "<<tmp[p.distanse]<<" "<<ClearResultArea[p.distanse];
        qDebug()<<percent;
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
