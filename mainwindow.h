#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include "graphicsview.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool loadFile(const QString &path);
public slots:
       void openFile();
       void exportImage();
       void exportGcode();
       void language();

private:
    QAction *m_nativeAction;
    QAction *m_glAction;
    QAction *m_imageAction;
    QAction *m_highQualityAntialiasingAction;
    QAction *m_backgroundAction;
    QAction *m_outlineAction;

    GraphicsView *m_view;

    QString m_currentPath;
};

#endif // MAINWINDOW_H
