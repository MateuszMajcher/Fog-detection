#include "mainwindow.h"
#include <QtWidgets>


MainWindow::MainWindow()
    :imageLabel(new QLabel)
    ,scrollArea(new QScrollArea)
    ,scaleFactor(1)
    ,fog(new FogDetect)
{
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setVisible(false);
    setCentralWidget(scrollArea);
    statusBar()->showMessage("threshold: " + QString::number(threshold) + "%");
    createActions();

    resize(640,480);
}

MainWindow::~MainWindow()
{

}


/*
 *  Ustawienie obrazu dla label
 */
void MainWindow::setImage(const QImage& newImage) {
    image = fog->getFogDetect(newImage, threshold);

    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->setScaledContents(true);
    scaleFactor = 1.0;

    scrollArea->setVisible(true);
    fitToWindowAct->setEnabled(true);
    updateActions();
    fitToWindowAct->setChecked(true);
    if (!fitToWindowAct->isChecked())
           imageLabel->adjustSize();
    fitToWindow();



}

bool MainWindow::loadImage(const QString& path, QImage& image) {
    QImageReader reader(path);
    reader.setAutoTransform(true);
    image = reader.read();
    if (image.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(path), reader.errorString()));
        return false;
    }
    return true;
}

/*
 *  Ładowanie obrazu
 */
bool MainWindow::loadTestImage(const QString& path) {
    QImage newImage;
    if (loadImage(path, newImage)) {
        setImage(newImage);
        setWindowFilePath(path);
        const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
            .arg(QDir::toNativeSeparators(path)).arg(image.width()).arg(image.height()).arg(image.depth());
        statusBar()->showMessage(message);
        return true;
    } else {
        return false;
    }
}

/*
 *  Ładowanie obrazu
 */
bool MainWindow::loadClearImage(const QString& path) {
    QImage newImage;
    if (loadImage(path, newImage)) {
        fog->setTestImage(newImage);
        const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
            .arg(QDir::toNativeSeparators(path)).arg(image.width()).arg(image.height()).arg(image.depth());
        statusBar()->showMessage(message);
        return true;
    } else {
        return false;
    }
}



/*
 * Załadowanie pliku z objektami
 */
bool MainWindow::loadObjectsFile(const QString& path) {
    if (!QFile::exists(path))
        return false;
    QFile file(path);
    if (!file.open(QFile::ReadOnly))
        return false;


    QTextStream stream(&file);
    QString line;
    while (!stream.atEnd()){
               line = stream.readLine();
               QStringList l = split_line(line, ',');
               objectList.append({l.at(0).toInt()
                                  ,l.at(1).toInt()
                                  ,l.at(2).toInt()
                                  ,l.at(3).toInt()
                                  ,l.at(4).toInt()});
           }
    fog->setCordObject(objectList);

    return true;
}

/*
 *  Init dla okna wyboru
 */
static void initializeImageFileDialog(QFileDialog& dialog, QFileDialog::AcceptMode acceptMode) {
    static bool firstDialog = true;

        if (firstDialog) {
            firstDialog = false;
            const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
            dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
        }

        QStringList mimeTypeFilters;
        const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
            ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
        foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
            mimeTypeFilters.append(mimeTypeName);
        mimeTypeFilters.sort();
        dialog.setMimeTypeFilters(mimeTypeFilters);
        dialog.selectMimeTypeFilter("image/jpeg");
        if (acceptMode == QFileDialog::AcceptSave)
            dialog.setDefaultSuffix("jpg");
    }

/*
 *  Otwarcie okna wyboru obrazów
 */
void MainWindow::openImage() {
    QFileDialog dialog(this, tr("Open file"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);
    while (dialog.exec() == QDialog::Accepted && !loadTestImage(dialog.selectedFiles().first())){}
}

/*
 *  Otwarcie okna wyboru obrazów
 */
void MainWindow::openClearImage() {
    QFileDialog dialog(this, tr("Open file"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);
    while (dialog.exec() == QDialog::Accepted && !loadClearImage(dialog.selectedFiles().first())){}
}



void MainWindow::openObjects() {
    QFileDialog fileDialog(this, tr("Open file with objects"));
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setMimeTypeFilters(QStringList()<<"text/plain");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    const QString fn = fileDialog.selectedFiles().first();
    if (loadObjectsFile(fn)) {
        statusBar()->showMessage(tr("Opened \"%1\" Read %2 objects").arg(QDir::toNativeSeparators(fn)).arg(objectList.size()));
    } else {
        statusBar()->showMessage(tr("Could not open \"%1\"").arg(QDir::toNativeSeparators(fn)));
    }
}

/*
 *  kopiowanie obrazu
 */
void MainWindow::copy() {
#ifndef QT_NO_CLIPBOARD
    QGuiApplication::clipboard()->setImage(image);
#endif
}

/*
 *  Ustawienie progu widocznosci
 */
void MainWindow::setThreshold() {
    bool ok;
    threshold = QInputDialog::getInt(this, tr("Set threshold"), tr("Percentage:"), threshold, 0,100,1,&ok);
    if (ok)
        statusBar()->showMessage("set new threshold: " + QString::number(threshold) + "%");
}

/*
 *  Utworzenie akcji menu
 */
void MainWindow::createActions() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *openObjectAct = fileMenu->addAction(tr("Open object file"), this, &MainWindow::openObjects);
    QAction *openClearImage = fileMenu->addAction(tr("Open clear image"), this, &MainWindow::openClearImage);
    QAction *openAct = fileMenu->addAction(tr("&Open image"), this, &MainWindow::openImage);
    fileMenu->addSeparator();
    QAction *exitAct = fileMenu->addAction(tr("&Exit"), this, &QWidget::close);

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    setThresholdsAct = editMenu->addAction(tr("set thresholds"), this, &MainWindow::setThreshold);
    copyAct = editMenu->addAction(tr("&Copy"), this, &MainWindow::copy);
    copyAct->setShortcut(QKeySequence::Copy);
    copyAct->setEnabled(false);

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &MainWindow::zoomIn);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    zoomInAct->setEnabled(false);

    zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &MainWindow::zoomOut);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    zoomOutAct->setEnabled(false);

    normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &MainWindow::normalSize);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);

    viewMenu->addSeparator();

    fitToWindowAct = viewMenu->addAction(tr("&Fit to Window"), this, &MainWindow::fitToWindow);
    fitToWindowAct->setChecked(true);
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));
}

/*
 *  Slot zoomu
 */
void MainWindow::zoomIn() {
    scaleImage(1.25);
}

/*
 *  Slot zoomu
 */
void MainWindow::zoomOut() {
    scaleImage(0.8);
}

/*
 *  ustawienie defaultowego rozmiaru obrazu
 */
void MainWindow::normalSize() {
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}

/*
 *  Skalowanie obrazu
 */
void MainWindow::scaleImage(double factor) {
    qDebug()<<"resize";
    Q_ASSERT(imageLabel->pixmap());
       scaleFactor *= factor;
       imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

       adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
       adjustScrollBar(scrollArea->verticalScrollBar(), factor);

       zoomInAct->setEnabled(scaleFactor < 3.0);
       zoomOutAct->setEnabled(scaleFactor > 0.333);

}

/*
 *  Dopasowanie obrazu do okna
 */
void MainWindow::fitToWindow() {
    bool fitToWindow = fitToWindowAct->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow)
       normalSize();
    updateActions();
}

/*
 *  Dopasowanie obrazu do okna z zachowaniem proporcji
 */
void MainWindow::fitToWindowWithRatio() {
     normalSize();
     int w = imageLabel->width();
     int h = imageLabel->height();
     imageLabel->resize(image.scaled(w, h, Qt::KeepAspectRatio).size());
}

/*
 *  Dopasowanie pasków do zoomu obrazu
 */
void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor) {
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

/*
 *  Uaktualnienie akcji
 */
void MainWindow::updateActions() {
    copyAct->setEnabled(!image.isNull());
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
    normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

/*
 *
 */
QStringList MainWindow::split_line(const QString &s, char delim) {
    return s.split(delim);
}
