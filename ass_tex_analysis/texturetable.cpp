#include "texturetable.h"
#include <QAbstractItemView>
#include <QHeaderView>

TextureTable::TextureTable(QWidget *parent) : QTableView(parent)
{
    horizontalHeader()->setStretchLastSection(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
}
