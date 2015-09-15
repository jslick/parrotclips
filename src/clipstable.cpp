#include "clipstable.h"
#include "clip.h"

#include <QHeaderView>
#include <QMenu>
#include <QKeyEvent>

ClipItem::ClipItem(QSharedPointer<Clip>& clip)
        : QTableWidgetItem(clip->getPreview(), Cols::TextPreviewCol),
          clip(clip)
{}


ClipsTable* ClipsTable::currentContextTable = nullptr;

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

    // Actions
    QAction* activateAction = new QAction(tr("Make &active clip"), this);
    activateAction->setData(QVariant(TableMenuAction::ActivateAction));

    // Context menus
    this->menu = new QMenu(this);
    this->menu->addAction(activateAction);
    this->menu->setDefaultAction(activateAction);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTableWidget::customContextMenuRequested, this, &ClipsTable::showContextMenu);
    connect(this->menu, &QMenu::triggered, this, &ClipsTable::handleContextMenuAction);
}

void ClipsTable::resizeEvent(QResizeEvent* event)
{
    QTableWidget::resizeEvent(event);

    this->setColumnWidth(Cols::NameCol, this->width() * 0.3);
}

void ClipsTable::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return)
    {
        this->searchSelectedItems([this](ClipItem* clipItem)
        {
            int y = this->rowViewportPosition(this->row(clipItem));
            this->showContextMenu(QPoint(this->width(), y + 12));
            this->menu->setActiveAction(this->menu->defaultAction());
            return false;
        });
    }

    return QTableWidget::keyPressEvent(event);
}

void ClipsTable::addContextMenuAction(QAction* action)
{
    this->menu->addAction(action);
}

void ClipsTable::searchTableItems(std::function<bool(ClipItem*)> handler) const
{
    for (int row = 0; row < this->rowCount(); row++)
    {
        QTableWidgetItem* item = this->item(row, Cols::TextPreviewCol);
        Q_ASSERT(item->type() == Cols::TextPreviewCol);
        ClipItem* clipItem = static_cast<ClipItem*>(item);
        bool kontinue = handler(clipItem);
        if (!kontinue)
            break;
    }
}

void ClipsTable::searchSelectedItems(std::function<bool(ClipItem*)> handler) const
{
    QList<QTableWidgetItem*> items = this->selectedItems();
    for (QTableWidgetItem* item : items)
    {
        if (item->type() == Cols::TextPreviewCol)
        {
            ClipItem* clipItem = static_cast<ClipItem*>(item);
            bool kontinue = handler(clipItem);
            if (!kontinue)
                break;
        }
    }
}

void ClipsTable::searchSelectedClips(std::function<bool(QSharedPointer<Clip>)> handler) const
{
    return this->searchSelectedItems([&handler](ClipItem* clipItem)
    {
        return handler(clipItem->clip);
    });
}

void ClipsTable::removeClipFromTable(QSharedPointer<Clip> clip)
{
    this->searchTableItems([this,clip](ClipItem* clipItem)
    {
        if (clip == clipItem->clip)
        {
            this->removeRow(this->row(clipItem));
            return false;
        }
        else
            return true;
    });
}

void ClipsTable::prependClip(QSharedPointer<Clip> clip)
{
    this->insertRow(0);
    this->setItem(0, Cols::NameCol, new QTableWidgetItem(clip->name));
    this->setItem(0, Cols::TextPreviewCol, new ClipItem(clip));
    this->resizeRowToContents(0);
}

void ClipsTable::appendClip(QSharedPointer<Clip> clip, bool size)
{
    int rownum = this->rowCount();
    this->insertRow(rownum);
    this->setItem(rownum, Cols::NameCol, new QTableWidgetItem(clip->name));
    this->setItem(rownum, Cols::TextPreviewCol, new ClipItem(clip));
    if (size)
        this->resizeRowToContents(rownum);
}

void ClipsTable::showContextMenu(const QPoint& pos)
{
    currentContextTable = this;
    this->menu->popup(this->viewport()->mapToGlobal(pos));
}

void ClipsTable::handleContextMenuAction(QAction* action)
{
    if (currentContextTable != this)
        return;

    switch (action->data().value<int>())
    {
    case ActivateAction:
        searchSelectedItems([](ClipItem* clipItem)
        {
            Clip* clip = clipItem->clip.data();
            clip->setClipboard();
            return false;
        });

        break;
    }

    emit contextActionTriggered(action);
}

void ClipsTable::focusInEvent(QFocusEvent*)
{
    if (this->selectedIndexes().length() == 0 && this->rowCount())
        this->selectRow(0);
}
