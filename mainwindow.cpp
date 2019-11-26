#include "mainwindow.h"
#include <QtWidgets>
#include <QMenu>
#include <QToolBar>
#include <QTranslator>
#include "util.h"
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
//    fileDialog.setMimeTypeFilters(QStringList() << "image/svg+xml" << "image/svg+xml-compressed");
    fileDialog.setWindowTitle(tr("Open A File"));
    if (m_currentPath.isEmpty())
        fileDialog.setDirectory(picturesLocation());

    while (fileDialog.exec() == QDialog::Accepted && !loadFile(fileDialog.selectedFiles().constFirst()))
        ;
}

bool MainWindow::loadFile(const QString &fileName)
{
    if (!QFileInfo::exists(fileName) || !m_view->openFile(fileName)) {
        QMessageBox::critical(this, tr("Open A File"),
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
    QImage image = m_view->outPutImage();
    image.save(path,"BMP");
}
void MainWindow::exportGcode()
{
    if(!m_view){
        return;
    }
    QString path;
    QString newPath = QFileDialog::getSaveFileName(this, tr("Save gcode"),
             path, tr("gcode files (*.gcode)"));
    if (newPath.isEmpty())
          return;
    path = newPath;
    statusBar()->showMessage(path);
    if(!m_view->exportGcode(path)){
        const QString message =tr("save gcode failed");
        statusBar()->showMessage(message);
    }else{
        return;
    }
}

