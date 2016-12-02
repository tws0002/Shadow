
#include "mainwindow.h"
#include "texturetablemodel.h"

#include <QApplication>
#include <QAction>
#include <QIcon>
#include <QStyle>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QTableView>
#include <QFileDialog>
//#include <QDebug>

#include "texture.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Ass Texture Analysis");

    QStyle *style = QApplication::style();

    act_open = new QAction(style->standardIcon(QStyle::SP_DialogOpenButton), u8"打开", this);
    act_open->setShortcuts(QKeySequence::Open);
    act_open->setStatusTip("Open an ass file");
    connect(act_open, &QAction::triggered, this, &MainWindow::open);

    act_setpath = new QAction(style->standardIcon(QStyle::SP_DirIcon), u8"设置路径", this);
    act_setpath->setStatusTip("Set new path to selected.");
    connect(act_setpath, &QAction::triggered, this, &MainWindow::setPath);

    act_setfile = new QAction(style->standardIcon(QStyle::SP_FileIcon), u8"设置贴图", this);
    act_setfile->setStatusTip("Set new file to selected.");
    connect(act_setfile, &QAction::triggered, this, &MainWindow::setFile);

    act_save = new QAction(style->standardIcon(QStyle::SP_DialogSaveButton), u8"保存", this);
    act_save->setStatusTip("Save current ass file.");
    connect(act_save, &QAction::triggered, this, &MainWindow::save);

    act_saveas = new QAction(style->standardIcon(QStyle::SP_DialogSaveButton), u8"另存为", this);
    act_saveas->setStatusTip("Save current ass file as new file.");
    connect(act_saveas, &QAction::triggered, this, &MainWindow::saveAs);

    act_close = new QAction(style->standardIcon(QStyle::SP_DialogCloseButton), u8"关闭", this);
    act_close->setStatusTip("Close");
    connect(act_close, &QAction::triggered, this, &MainWindow::closeFile);

//    QMenu *file_menu = menuBar()->addMenu("&File");
//    file_menu->addAction(act_open);
//    file_menu->addAction(act_setpath);
//    file_menu->addAction(act_close);

    auto toolbar = addToolBar("&File");
    toolbar->setFloatable(false);
    toolbar->setMovable(false);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    toolbar->addAction(act_open);
    toolbar->addAction(act_setpath);
    toolbar->addAction(act_setfile);
    toolbar->addAction(act_save);
    toolbar->addAction(act_saveas);
    toolbar->addAction(act_close);

    tex_model = new TextureTableModel(this);
    tex_model->setAssFile(&ass_file);
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
    QString file = QFileDialog::getOpenFileName(this);
    if(!file.isEmpty())
    {
        closeFile();
        ass_file.setFile(file);
        ass_file.load();
        tex_model->setTextures(ass_file.allTextures());
    }
}

void MainWindow::setPath()
{
    QString path = QFileDialog::getExistingDirectory(this);
    if(!path.isEmpty())
    {
        tex_model->setPath(path, table_view->selectedRows());
    }
}

void MainWindow::setFile()
{
    QString file = QFileDialog::getOpenFileName(this);
    if(!file.isEmpty())
    {
        tex_model->setFile(file, table_view->selectedRows());
    }
}

void MainWindow::save()
{
    ass_file.save();
    QMessageBox::information(this, tr("OK"), tr("Save successful."));
}

void MainWindow::saveAs()
{
    QString file = QFileDialog::getSaveFileName(this);
    if(!file.isEmpty())
    {
        ass_file.saveAs(file);
        QMessageBox::information(this, tr("OK"), tr("Save successful."));
    }
}

void MainWindow::closeFile()
{
//    QMessageBox::information(this, tr("Information"), tr("Open"));
    tex_model->clearAllTexture();
    ass_file.close();
}
