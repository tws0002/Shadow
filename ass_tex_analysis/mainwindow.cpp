
#include "mainwindow.h"
#include "texturetablemodel.h"

#include <QAction>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QTableView>
#include <QFileDialog>

#include "texture.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Ass Texture Analysis"));

    act_open = new QAction(tr("Open Ass"), this);
    act_open->setShortcuts(QKeySequence::Open);
    act_open->setStatusTip(tr("Open an ass file"));
    connect(act_open, &QAction::triggered, this, &MainWindow::open);

    act_close = new QAction(tr("Close"), this);
    act_close->setStatusTip(tr("Close"));
    connect(act_close, &QAction::triggered, this, &MainWindow::closeFile);

    QMenu *file_menu = menuBar()->addMenu(tr("&File"));
    file_menu->addAction(act_open);
    file_menu->addAction(act_close);

    auto toolbar = addToolBar(tr("&File"));
    toolbar->setFloatable(false);
    toolbar->setMovable(false);
    toolbar->addAction(act_open);
    toolbar->addAction(act_close);

    tex_model = new TextureTableModel(this);
    table_view = new TextureTable(this);
    table_view->setModel(tex_model);

    setCentralWidget(table_view);

    statusBar();
}

MainWindow::~MainWindow()
{

}

void MainWindow::open()
{
//    QMessageBox::information(this, tr("Information"), tr("Open"));
//    for(auto i = 0; i < 1000; i++)
//    {
//        Texture *tex = new Texture();
//        tex->setAssFile("sdfasd");
//        tex->setNodeName("image");
//        tex->setNodeType("iiii");
//        tex->setFileName("dfjskdjfksdjfksd");
//        tex_model->addTexture(tex);
//    }

    QString file = QFileDialog::getOpenFileName(this);
    if(!file.isEmpty())
    {
        ass_file.setFile(file);
        tex_model->setTextures(ass_file.allTextures());
    }
}

void MainWindow::closeFile()
{
//    QMessageBox::information(this, tr("Information"), tr("Open"));
    tex_model->clearAllTexture();
}
