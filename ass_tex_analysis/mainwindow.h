#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "texturetablemodel.h"
#include "texturetable.h"
#include "assfile.h"

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
    void setPath();
    void closeFile();

    QAction *act_open;
    QAction *act_setpath;
    QAction *act_close;
    TextureTableModel *tex_model;
    TextureTable *table_view;
    AssFile ass_file;
};

#endif // MAINWINDOW_H
