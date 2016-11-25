
#include "mainwindow.h"
#include "texturetablemodel.h"

#include <QAction>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QTableView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Ass Texture Analysis"));

    act_open = new QAction(tr("Open Ass"), this);
    act_open->setShortcuts(QKeySequence::Open);
    act_open->setStatusTip(tr("Open an ass file"));
    connect(act_open, &QAction::triggered, this, &MainWindow::open);

    QMenu *file_menu = menuBar()->addMenu(tr("&File"));
    file_menu->addAction(act_open);

    auto toolbar = addToolBar(tr("&File"));
    toolbar->setFloatable(false);
    toolbar->setMovable(false);
    toolbar->addAction(act_open);

    tex_model = new TextureTableModel(this);
    table_view = new QTableView(this);
    table_view->setModel(tex_model);

    setCentralWidget(table_view);

    statusBar();
}

MainWindow::~MainWindow()
{

}

void MainWindow::open()
{
    QMessageBox::information(this, tr("Information"), tr("Open"));
}
