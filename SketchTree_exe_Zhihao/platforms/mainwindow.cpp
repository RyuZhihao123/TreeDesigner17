#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Tree Model From Freehand sketch! -- Zhihao Liu (刘志浩)");

    this->m_canvasWidget = new CanvasWidget(ui->centralWidget);
    this->m_glWidget     = new GLWidget(ui->centralWidget);

    resetUI();

    // init leaf texture files
    initLeafTextures();
    initBarkTextures();

    connect(ui->btnCreate2DSkelton,SIGNAL(clicked(bool)),this,SLOT(slot_btnCreate2DSkelton()));
    connect(ui->btnCreate3DTree,SIGNAL(clicked(bool)),this,SLOT(slot_btnCreate3DTree()));
    connect(ui->btnIteration,SIGNAL(clicked(bool)),this,SLOT(slot_btnIteration()));
    connect(ui->btnCreate3DLeaves,SIGNAL(clicked(bool)),this,SLOT(slot_btnCreate3DLeaves()));

    connect(ui->ckbIsDisplayLeaves,SIGNAL(clicked(bool)),this,SLOT(slot_ckbIsDisplayLeaves()));
    connect(ui->cbxLeaves,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_cbxLeafTextures()));
    connect(ui->cbxBarks,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_cbxBarkTextures()));
    connect(ui->ckbBranchMode,SIGNAL(clicked(bool)),this,SLOT(slot_branchMode()));
    connect(ui->ckbIsDisplayMode,SIGNAL(clicked(bool)),this,SLOT(slot_cbxDisplayMode()));

    connect(ui->actionimport,SIGNAL(triggered(bool)),this,SLOT(slot_btnLoadSketch()));
    connect(ui->actionexport,SIGNAL(triggered(bool)),this,SLOT(slot_btnSaveSketch()));

    connect(ui->actionhelp,SIGNAL(triggered(bool)),this,SLOT(slot_btnShowHelpDialog()));
    connect(ui->btnHelp,SIGNAL(clicked(bool)),this,SLOT(slot_btnShowHelpDialog()));

    connect(ui->cbxShowAxis,SIGNAL(clicked(bool)),this,SLOT(slot_cbxShowAxis()));
    connect(ui->cbxLeafStyle,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_btnCreate3DLeaves()));

    connect(ui->actionExportMesh,SIGNAL(triggered(bool)),this,SLOT(slot_btnExportMesh()));

    connect(ui->actionexit,SIGNAL(triggered(bool)),this,SLOT(close()));

    connect(ui->btnRefresh,SIGNAL(clicked(bool)),this,SLOT(slot_btnRefreshTextures()));
    connect(ui->ckbCamera,SIGNAL(clicked(bool)),this,SLOT(slot_ckbOpenCamera()));
}

void MainWindow::resetUI()
{
    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect deskRect = desktopWidget->availableGeometry();

    if(deskRect.width()>1920)
        return;

    this->resize(this->width()/1920.0*deskRect.width(),
                      this->height()/1080.0*deskRect.height());
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
    // set postions
    this->m_canvasWidget->move(0,this->ui->lblCanvasTitle->height());

    this->ui->groupBox->move(this->width()-ui->groupBox->width(),0);

    this->m_glWidget->resize(this->width()-(this->m_canvasWidget->width()+ui->groupBox->width())-5,
                             this->height()-ui->menuBar->height()-10);
    this->m_glWidget->move(this->m_canvasWidget->x() + this->m_canvasWidget->width()+2,
                           this->m_canvasWidget->y());

    this->ui->lblBack->resize(this->width(),this->height());

    this->ui->lblGLWidgetTitle->setGeometry(this->m_glWidget->x(),this->ui->lblCanvasTitle->y(),
                                            this->m_glWidget->width(),this->ui->lblGLWidgetTitle->height());

}

void MainWindow::initLeafTextures()
{
    ui->cbxLeaves->clear();
    QDir dir(LEAF_TEXTURES_PATH);

    QFileInfoList list = dir.entryInfoList(QDir::Files);

    for(unsigned int i=0; i<list.size(); i++)
    {
        this->ui->cbxLeaves->addItem(QIcon(list[i].absoluteFilePath()),list[i].fileName());
    }
}

void MainWindow::initBarkTextures()
{
    ui->cbxBarks->clear();
    QDir dir(BARK_TEXTURES_PATH);

    QFileInfoList list = dir.entryInfoList(QDir::Files);

    for(unsigned int i=0; i<list.size(); i++)
    {
        this->ui->cbxBarks->addItem(QIcon(list[i].absoluteFilePath()),list[i].fileName());
    }
}

void MainWindow::slot_btnCreate2DSkelton()
{
    if(!this->m_canvasWidget->isReadyFinished())
    {
        QMessageBox::warning(this,"未选择根节点和树冠区域","您未选择“根节点”和“树冠区域”。\n"
                                                "CTRL+鼠标左键：点击某点，选择根节点。\n"
                                                "ALT +鼠标左键：绘制矩形区域，选择树冠区域。\n"
                                                "其他操作，您可查看“使用帮助”进一步了解。");
        return;
    }
    m_tree.getSkeltonFromImage(this->m_canvasWidget->imageBack(),this->m_canvasWidget->rootPoint(),
                               this->m_canvasWidget->crownRect());
    this->m_canvasWidget->update();

    this->m_canvasWidget->setFocus();
}

void MainWindow::slot_btnCreate3DTree()
{
    m_tree.getBranchesFromSkelton(this->m_canvasWidget->rootPoint());
    m_tree.setBaseRadius(this->ui->sbxBranchRadius->value());
    m_tree.setRadiusRadio(this->ui->sbxBranchRadio->value()/100.0);

    this->m_glWidget->update();
    this->m_canvasWidget->setFocus();
}

void MainWindow::slot_btnIteration()
{
    m_tree.setBendDegree(this->ui->sbxBendDegree->value());

    m_tree.getIterationBranchs(this->ui->sbxNIteration->value());

    this->m_glWidget->update();

    this->m_canvasWidget->setFocus();
}

void MainWindow::slot_btnCreate3DLeaves()
{
    this->m_glWidget->setLeafSize(this->ui->sbxLeafSize->value());

    m_tree.setLeafDensity(this->ui->sbxLeafDensity->value()/100.0);

    if(ui->cbxLeafStyle->currentIndex() == 0)
    {
        m_tree.getLeavesFromBranches1();
    }
    else
    {
        m_tree.getLeavesFromBranches2();
    }

    this->m_glWidget->update();
    this->m_canvasWidget->setFocus();
}

void MainWindow::slot_cbxDisplayMode()
{
    if(ui->ckbIsDisplayMode->isChecked())
        this->m_glWidget->setDisplayMode(GLWidget::_LINE);
    else
        this->m_glWidget->setDisplayMode(GLWidget::_FILL);
}

void MainWindow::slot_branchMode()
{
    if(ui->ckbBranchMode->isChecked())
        this->m_glWidget->setBranchMode(GLWidget::_TEXTURE);
    else
        this->m_glWidget->setBranchMode(GLWidget::_COLOR);
}

void MainWindow::slot_btnLoadSketch()
{
    QString filename = QFileDialog::getOpenFileName(this,"加载Sketch",".","Image Files (*.png *.jpg *.bmp)");

    if(filename == "")
        return;

    m_tree.release2DSkelton();
    this->m_canvasWidget->setSketchImage(filename);
    this->m_canvasWidget->update();
}

void MainWindow::slot_btnSaveSketch()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("保存图像"),
                               "./untitled.png",
                               tr("Images (*.png *.xpm *.jpg)"));

    if(filename == "")
        return;

    QImage img = this->m_canvasWidget->imageBack();
    img.save(filename);
}

void MainWindow::slot_cbxLeafTextures()
{
    this->m_glWidget->setLeafTexture(LEAF_TEXTURES_PATH+this->ui->cbxLeaves->currentText());

    this->m_glWidget->update();
}

void MainWindow::slot_cbxBarkTextures()
{
    this->m_glWidget->setBarkTexture(BARK_TEXTURES_PATH+this->ui->cbxBarks->currentText());

    this->m_glWidget->update();
}

void MainWindow::slot_ckbIsDisplayLeaves()
{
    this->m_glWidget->setIsDisplayLeaves(this->ui->ckbIsDisplayLeaves->isChecked());

    this->m_glWidget->update();
}

void MainWindow::slot_btnShowHelpDialog()
{
    DialogHelp dialog(this);
    dialog.exec();
}

void MainWindow::slot_btnExportMesh()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("保存网格"),
                               "./untitled.obj",
                               tr("Mesh (*.obj)"));

    if(filename == "")
        return;

    this->m_glWidget->saveModelToFile(filename,
                                      LEAF_TEXTURES_PATH+ui->cbxLeaves->currentText(),
                                      BARK_TEXTURES_PATH+ui->cbxBarks->currentText());
}

void MainWindow::slot_cbxShowAxis()
{
    this->m_glWidget->setAxisMode(ui->cbxShowAxis->isChecked());
}

void MainWindow::slot_btnRefreshTextures()
{
    this->initBarkTextures();
    this->initLeafTextures();
}

void MainWindow::slot_ckbOpenCamera()
{
    this->m_glWidget->openCameraMode(ui->ckbCamera->isChecked());
}

MainWindow::~MainWindow()
{
    delete ui;
}
