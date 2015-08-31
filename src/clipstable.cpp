#include "clipstable.h"

#include <QHeaderView>

ClipsTable::ClipsTable(QWidget* parent) :
    QTableWidget(parent)
{
    this->setColumnCount(3);
    this->horizontalHeader()->setStretchLastSection(true);
    this->horizontalHeader()->hide();
    this->verticalHeader()->hide();
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setColumnWidth(Cols::NumberCol, 20);
    this->setWordWrap(false);
    this->setAlternatingRowColors(true);

#ifdef Q_OS_LINUX
    QFont tableFont = this->font();
    tableFont.setPointSize(tableFont.pointSize() * 0.9);
    this->setFont(tableFont);
#endif
}

void ClipsTable::resizeEvent(QResizeEvent* event)
{
    QTableWidget::resizeEvent(event);

    this->setColumnWidth(Cols::NameCol, this->width() * 0.3);
}
