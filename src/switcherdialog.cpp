#include "switcherdialog.h"
#include "clipstable.h"
#include "clip.h"
#include <parrotclips/switcher.h>

#include <QVBoxLayout>
#include <QAction>

SwitcherDialog::SwitcherDialog()
    : QDialog(),
      mainLayout(new QVBoxLayout),
      clipsTable(new ClipsTable(this))
{
    Qt::WindowFlags flags = this->windowFlags();
    flags &= ~Qt::WindowMaximizeButtonHint;
    flags &= ~Qt::WindowMinMaxButtonsHint;
    flags |= Qt::WindowStaysOnTopHint;
    this->setWindowFlags(flags);
    this->setAttribute(Qt::WA_QuitOnClose, false);

    this->resize(500, 175);

    this->clipsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    this->mainLayout->addWidget(this->clipsTable);
    this->setLayout(this->mainLayout);

    connect(this->clipsTable, &ClipsTable::contextActionTriggered, this, &SwitcherDialog::handleContextMenuAction);
}

void SwitcherDialog::setSwitcher(QSharedPointer<Switcher> switcher)
{
    this->clipsTable->setRowCount(0);

    QList<QSharedPointer<Clip>> clips = switcher->getClips();
    for (int i = clips.length() - 1; i >= 0; i--)
    {
        auto& clip = clips[i];
        int rownum = this->clipsTable->rowCount();
        this->clipsTable->insertRow(rownum);

        ClipItem* clipItem = new ClipItem(clip);
        this->clipsTable->setItem(rownum, Cols::TextPreviewCol, clipItem);
    }
    this->clipsTable->resizeRowsToContents();

    this->clipsTable->selectRow(0);
}

void SwitcherDialog::selectNext()
{
    if (this->clipsTable->rowCount() <= 1)
        return;

    int select = this->clipsTable->selectionModel()->selectedRows().first().row() + 1;
    if (select >= this->clipsTable->rowCount())
        select = 0;
    this->clipsTable->selectRow(select);
}

void SwitcherDialog::handleContextMenuAction(QAction* action)
{
    if (action->data().value<int>() == TableMenuAction::ActivateAction)
        this->hide();
}
