#ifndef TEXTURETABLE_H
#define TEXTURETABLE_H

#include <QTableView>

class TextureTable : public QTableView
{
    Q_OBJECT
public:
    explicit TextureTable(QWidget *parent = 0);

    QList<int> selectedRows();

signals:

public slots:
};

#endif // TEXTURETABLE_H
