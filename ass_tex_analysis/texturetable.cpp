#include "texturetable.h"
#include <QAbstractItemView>
#include <QHeaderView>

TextureTable::TextureTable(QWidget *parent) : QTableView(parent)
{
    horizontalHeader()->setStretchLastSection(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
}

QList<int> TextureTable::selectedRows()
{
    auto indexs = selectedIndexes();

    QList<int> rows;

    for(int i = 0; i < indexs.size(); ++i)
    {
        auto index = indexs[i];
        if(index.column() == 0)
        {
            rows.append(index.row());
        }
    }
    return rows;
}
