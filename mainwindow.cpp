#include "mainwindow.h"
#include <QtWidgets>
#include <QMenu>
#include <QToolBar>
#include <QTranslator>
static inline QString picturesLocation()
{
    return QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).value(0, QDir::currentPath());
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_view(new GraphicsView)
{

      QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
      fileMenu->addAction(tr("&Open..."), this, &MainWindow::openFile);
      fileMenu->addAction( tr("&Export..."), this, &MainWindow::exportImage);
      fileMenu->addAction( tr("&ExportGcode"), this, &MainWindow::exportGcode);
      fileMenu->addAction(tr("Language"), this, &MainWindow::language);
      fileMenu->addAction(tr("E&xit"), qApp, QCoreApplication::quit);

      QToolBar *toolBar = new QToolBar(this);
      addToolBar(Qt::TopToolBarArea, toolBar);

      toolBar->addAction(tr("delete"),m_view,&GraphicsView::deleteItem);
      toolBar->addAction(tr("test"),m_view,&GraphicsView::test);
      setCentralWidget(m_view);
}

MainWindow::~MainWindow()
{

}
void MainWindow::language()
{
    qDebug()<<"language";
    QTranslator translator;           //创建翻译器
    translator.load(":/drawApp_zh.qm");    //加载语言包
    qApp->installTranslator(&translator); //安装翻译器

     qDebug()<<"language end";
}
void MainWindow::openFile()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setMimeTypeFilters(QStringList() << "image/svg+xml" << "image/svg+xml-compressed");
    fileDialog.setWindowTitle(tr("Open SVG File"));
    if (m_currentPath.isEmpty())
        fileDialog.setDirectory(picturesLocation());

    while (fileDialog.exec() == QDialog::Accepted && !loadFile(fileDialog.selectedFiles().constFirst()))
        ;
}

bool MainWindow::loadFile(const QString &fileName)
{
    if (!QFileInfo::exists(fileName) || !m_view->openFile(fileName)) {
        QMessageBox::critical(this, tr("Open SVG File"),
                              tr("Could not open file '%1'.").arg(QDir::toNativeSeparators(fileName)));
        return false;
    }

    if (!fileName.startsWith(":/")) {
        m_currentPath = fileName;
        setWindowFilePath(fileName);
        const QSize size = m_view->svgSize();
        const QString message =
            tr("Opened %1, %2x%3").arg(QFileInfo(fileName).fileName()).arg(size.width()).arg(size.width());
        statusBar()->showMessage(message);
    }


    const QSize availableSize = QApplication::desktop()->availableGeometry(this).size();
    resize(m_view->sizeHint().expandedTo(availableSize / 4) + QSize(80, 80 + menuBar()->height()));

    return true;
}
void MainWindow::exportImage()
{
    QString path;
    QString newPath = QFileDialog::getSaveFileName(this, tr("Save bmp"),
             path, tr("bmp files (*.bmp)"));
    if (newPath.isEmpty())
          return;
    path = newPath;
    QImage image = m_view->outputBmp();
    image.save(path,"BMP");
}
void MainWindow::exportGcode()
{

    QString path;
//    QString newPath = QFileDialog::getOpenFileName(this, tr("open bmp"),
//                                 path, tr("bmp files (*.bmp)"));
    QString newPath = QFileDialog::getSaveFileName(this,tr("save gcode"),
                                                   path,tr("gcode files (*.gcode)"));

    if (newPath.isEmpty())
          return;

    path = newPath;

    int no_suffixes=path.lastIndexOf('.');
    QString bmpPath =path.replace(no_suffixes,path.length()-1,".bmp");
    QString svgPath =path.replace(no_suffixes,path.length()-1,".svg");
    QString gcodePath =path.replace(no_suffixes,path.length()-1,".gcode");

    QImage image = m_view->outputBmp();
    QFile* imageFile= new QFile(bmpPath);
    if(imageFile==nullptr)
        return;
    //binary image
    image.convertToFormat(QImage::Format_Grayscale8);

    int nWidth = image.width();
    int nHeight =image.height();
    QRgb rgbVal = 0;
    int grayVal = 0;
    if(0){
        for (int x = 0; x < nWidth; ++x)
         {
            for (int y = 0; y < nHeight; ++y)
              {
                   rgbVal = image.pixel(x, y);

                   grayVal = qGray(rgbVal);    // 这里调用Qt的函数，使用(R * 11 + G * 16 + B * 5)/32的方法计算

                   image.setPixel(x, y, QColor(grayVal, grayVal, grayVal).rgb());
             }
         }
    }

    image.save(imageFile,"bmp");
    imageFile->waitForReadyRead(1000);
    if(imageFile->exists()){
        QProcess p(0);
        QStringList arg1,arg2;
        // potrace --svg --flat [filename]
        QString applicationPath = QCoreApplication::applicationDirPath();
        qDebug()<<"applicationPath"<<applicationPath;
        //F:/start/QT/build-drawApp-Desktop_Qt_5_13_0_MSVC2015_64bit-Debug/debug

        QString potrace= "F:/start/QT/drawApp/tool/potrace.exe";
        arg1<< "--svg" << "--flat" <<bmpPath;
        //gogcode --file [filename] --output [filename] --scale [float]
        QString gogcode ="F:/start/QT/drawApp/tool/gogcode.exe";
        arg2<< "--file"<<svgPath<<"--output"<<gcodePath<<"--scale"<< QString("0.01");

        p.start(potrace,arg1);
        qDebug()<<"process"<<potrace<<arg1;
        p.waitForStarted();
        p.waitForFinished();

        p.start(gogcode,arg2);
        qDebug()<<"process"<<gogcode<<arg2;
        p.waitForStarted();
        p.waitForFinished();
        QString strTemp=QString::fromLocal8Bit(p.readAllStandardOutput());
        qDebug()<<"process"<<strTemp;
    }else{
        qDebug()<<"imageFile is not exists";
    }

}
