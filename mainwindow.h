#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include "fogdetect.h"

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();
    bool loadImage(const QString& , QImage& );
    bool loadTestImage(const QString&);
    bool loadClearImage(const QString&);
    bool loadObjectsFile(const QString&);

private slots:
    void openImage();
    void openClearImage();
    void openObjects();
    void saveAs();
    void copy();
    void setThreshold();
    void zoomIn();
    void zoomOut();
    void fitToWindow();
    void fitToWindowWithRatio();
    void normalSize();
    void about();

private:
    void createActions();
    void setImage(const QImage& newImage);
    bool saveFile(const QString& filename);
    void updateActions();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    QStringList split_line(const QString&, char );

    FogDetect *fog;
    QLabel *imageLabel;
    QImage image;
    QScrollArea *scrollArea;
    double scaleFactor;
    QList<object> objectList;
    int threshold = 5;


    QAction *saveAsAct;
    QAction *copyAct;
    QAction* setThresholdsAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;

};

#endif // MAINWINDOW_H
