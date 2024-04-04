#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <dialoghelp.h>
#include "canvaswidget.h"
#include "glwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    CanvasWidget* m_canvasWidget;
    GLWidget*     m_glWidget;

    void initLeafTextures();
    void initBarkTextures();

    void resetUI();

protected:
    void resizeEvent(QResizeEvent* event);

protected slots:

    void slot_btnLoadSketch();
    void slot_btnSaveSketch();

    void slot_btnCreate2DSkelton();
    void slot_btnCreate3DTree();
    void slot_btnIteration();
    void slot_btnCreate3DLeaves();

    void slot_ckbIsDisplayLeaves();
    void slot_cbxLeafTextures();
    void slot_cbxBarkTextures();
    void slot_cbxDisplayMode();  // 填充/线框
    void slot_branchMode();      // 树干显示模式:颜色/纹理

    void slot_cbxShowAxis();

    void slot_btnShowHelpDialog();

    void slot_btnExportMesh();

    void slot_btnRefreshTextures();

    void slot_ckbOpenCamera();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
