#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "texturetablemodel.h"
#include <QMainWindow>

class QAction;
class QTableView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void open();

    QAction *act_open;
    TextureTableModel *tex_model;
    QTableView *table_view;
};

#endif // MAINWINDOW_H
