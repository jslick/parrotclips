#include "clipswidget.h"
#include "clip.h"
#include "clipboardmanager.h"
#include "clipstable.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QSplitter>
#include <QStackedLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>

enum ContextMenuAction : int
{
    ActivateAction = 1,
    SaveAction = 2,
    RemoveAction = 3,
};

class ClipItem : public QTableWidgetItem
{
public:
    ClipItem(QSharedPointer<Clip>& clip)
        : QTableWidgetItem(clip->getPreview(), Cols::TextPreviewCol),
          clip(clip)
    {}

    QSharedPointer<Clip> clip;
};

ClipsWidget::ClipsWidget(ClipboardManager& clipboardManager, QWidget* parent) :
    View(parent),
    clipboardManager(clipboardManager),
    mainLayout(new QVBoxLayout),
    searchEdit(new QLineEdit(this)),
    stacked(new QStackedLayout),
    mruTable(new ClipsTable(this)),
    savedTable(new ClipsTable(this)),
    resultsTable(new ClipsTable(this))
{
    // Actions
    QAction* activateAction = new QAction(tr("Make &active clip"), this);
    activateAction->setData(QVariant(ContextMenuAction::ActivateAction));

    QAction* saveAction = new QAction(tr("&Save as..."), this);
    saveAction->setData(QVariant(ContextMenuAction::SaveAction));

    QAction* removeAction = new QAction(tr("&Remove"), this);
    removeAction->setData(QVariant(ContextMenuAction::RemoveAction));

    // Layout
    this->setLayout(this->mainLayout);

    QFormLayout* searchForm = new QFormLayout;
    searchForm->addRow(tr("&Search:"), this->searchEdit);
    this->mainLayout->addLayout(searchForm);

    QSplitter* tableSplitter = new QSplitter(Qt::Vertical, this);

    tableSplitter->addWidget(this->mruTable);

    QWidget* savedContainer = new QWidget(this);
    QVBoxLayout* savedLayout = new QVBoxLayout;
    savedLayout->setContentsMargins(0, 0, 0, 0);
    savedContainer->setLayout(savedLayout);
    QLabel* savedLabel = new QLabel(tr("Saved clips:"), this);
    savedLabel->setBuddy(this->savedTable);
    savedLayout->addWidget(savedLabel);
    savedLayout->addWidget(this->savedTable);
    tableSplitter->addWidget(savedContainer);

    this->stacked->addWidget(tableSplitter);

    QWidget* resultsContainer = new QWidget(this);
    resultsContainer->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout* resultsLayout = new QVBoxLayout;
    resultsLayout->setContentsMargins(0, 0, 0, 0);
    resultsLayout->addWidget(new QLabel(tr("Results:"), this));
    resultsLayout->addWidget(this->resultsTable);
    resultsContainer->setLayout(resultsLayout);

    this->stacked->addWidget(resultsContainer);

    this->mainLayout->addLayout(this->stacked);

    // Context menus
    this->mruMenu = new QMenu(this);
    this->mruMenu->addAction(activateAction);
    this->mruMenu->addAction(saveAction);
    this->mruMenu->addAction(removeAction);
    this->assignTableMenu(this->mruTable, this->mruMenu);

    this->savedMenu = new QMenu(this);
    this->savedMenu->addAction(activateAction);
    this->savedMenu->addAction(removeAction);
    this->assignTableMenu(this->savedTable, this->savedMenu);

    this->resultsMenu = new QMenu(this);
    this->resultsMenu->addAction(activateAction);
    this->assignTableMenu(this->resultsTable, this->resultsMenu);

    // Data binding
    connect(&this->clipboardManager, &ClipboardManager::mruPushed, this, &ClipsWidget::pushMru);
    connect(&this->clipboardManager, &ClipboardManager::mruRemoved, [this](QSharedPointer<Clip> clip)
    {
        this->removeClipFromTable(this->mruTable, clip);
    });
    connect(&this->clipboardManager, &ClipboardManager::clipInserted, [this](ClipsGroup*,int,QSharedPointer<Clip> clip)
    {
        this->addSavedClip(clip);
    });
    connect(&this->clipboardManager, &ClipboardManager::clipRemoved, [this](ClipsGroup*,QSharedPointer<Clip> clip)
    {
        this->removeClipFromTable(this->savedTable, clip);
    });

    connect(this->searchEdit, &QLineEdit::textChanged, this, &ClipsWidget::updateResults);
}

bool ClipsWidget::escapeRequested()
{
    if (this->searchEdit->text().isEmpty())
    {
        return true;
    }
    else
    {
        this->searchEdit->clear();
        return false;
    }
}

void ClipsWidget::pushMru(QSharedPointer<Clip> clip)
{
    this->mruTable->insertRow(0);
    this->mruTable->setItem(0, Cols::TextPreviewCol, new ClipItem(clip));
    this->mruTable->resizeRowToContents(0);

    for (int i = 0; i < this->mruTable->rowCount(); i++)
    {

        if (i < 10)
        {
            int num = i == 9 ? 0 : i+1;
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(num));
            item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            this->mruTable->setItem(i, Cols::NumberCol, item);
        }
        else
        {
            QTableWidgetItem* item = this->mruTable->item(i, Cols::NumberCol);
            if (item)
                item->setText("");
        }
    }
}

void ClipsWidget::addSavedClip(QSharedPointer<Clip> clip)
{
    this->savedTable->insertRow(0);

    this->savedTable->setItem(0, Cols::NameCol, new QTableWidgetItem(clip->name));
    this->savedTable->setItem(0, Cols::TextPreviewCol, new ClipItem(clip));
    this->savedTable->resizeRowToContents(0);
}

void ClipsWidget::updateResults(const QString& text)
{
    // TODO:  Debounce

    this->resultsTable->setRowCount(0);
    if (text.isEmpty())
    {
        this->stacked->setCurrentIndex(0);
    }
    else
    {
        QList<QSharedPointer<Clip>> results = this->clipboardManager.searchClips(text);
        for (auto& clip : results)
        {
            int rownum = this->resultsTable->rowCount();
            this->resultsTable->insertRow(rownum);

            this->resultsTable->setItem(rownum, Cols::NameCol, new QTableWidgetItem(clip->name));
            this->resultsTable->setItem(rownum, Cols::TextPreviewCol, new ClipItem(clip));
        }
        this->resultsTable->resizeRowsToContents();

        this->stacked->setCurrentIndex(1);
    }
}

void ClipsWidget::assignTableMenu(ClipsTable* table, QMenu* menu)
{
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(table, &QTableWidget::customContextMenuRequested, [this,table,menu](QPoint pos)
    {
        if (menu)
        {
            this->contextMenuPoint = pos;
            this->currentContextTable = table;
            menu->popup(table->viewport()->mapToGlobal(pos));
        }
    });
    connect(menu, &QMenu::triggered, [this,table](QAction* action)
    {
        if (this->currentContextTable == table)
            this->handleContextMenuAction(action);
    });
}

void ClipsWidget::handleContextMenuAction(QAction* action)
{
    if (!this->currentContextTable)
        return;

    // TODO:  Use click point instead

    switch (action->data().value<int>())
    {
    case ActivateAction:
    {
        searchSelectedItems(this->currentContextTable, [](ClipItem* clipItem)
        {
            Clip* clip = clipItem->clip.data();
            clip->setClipboard();
            return false;
        });

        break;
    }

    case SaveAction:
    {
        bool ok = false;
        QString name = QInputDialog::getText(this,
                                             tr("Save As"), tr("Name"),
                                             QLineEdit::Normal, QString(), &ok);
        if (ok)
        {
            searchSelectedClips(this->currentContextTable, [this,&name](QSharedPointer<Clip> clip)
            {
                clip->setName(name);
                this->clipboardManager.getRootGroup().addClip(clip);
                return false;
            });
        }
        break;
    }

    case RemoveAction:
    {
        int rv = QMessageBox::question(this,
                              tr("Remove Confirmation"),
                              tr("Are you should you want to remove the selected clip(s)?")
                              );
        if (rv == QMessageBox::Yes)
        {
            if (this->currentContextTable == this->mruTable)
            {
                searchSelectedClips(this->mruTable, [this](QSharedPointer<Clip> clip)
                {
                    this->clipboardManager.removeMruClip(clip);
                    return true;
                });
            }
            else
            {
                searchSelectedClips(this->savedTable, [this](QSharedPointer<Clip> clip)
                {
                    this->clipboardManager.getRootGroup().removeClip(clip);
                    return true;
                });
            }
        }

        break;
    }
    }
}

void ClipsWidget::searchTableItems(ClipsTable* table, std::function<bool(ClipItem*)> handler)
{
    for (int row = 0; row < table->rowCount(); row++)
    {
        QTableWidgetItem* item = table->item(row, Cols::TextPreviewCol);
        Q_ASSERT(item->type() == Cols::TextPreviewCol);
        ClipItem* clipItem = static_cast<ClipItem*>(item);
        bool kontinue = handler(clipItem);
        if (!kontinue)
            break;
    }
}

void ClipsWidget::searchSelectedItems(ClipsTable* table, std::function<bool(ClipItem*)> handler)
{
    QList<QTableWidgetItem*> items = table->selectedItems();
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

void ClipsWidget::searchSelectedClips(ClipsTable* table, std::function<bool(QSharedPointer<Clip>)> handler)
{
    return searchSelectedItems(table, [&handler](ClipItem* clipItem)
    {
        return handler(clipItem->clip);
    });
}

void ClipsWidget::removeClipFromTable(ClipsTable* table, QSharedPointer<Clip> clip)
{
    searchTableItems(table, [table,clip](ClipItem* clipItem)
    {
        if (clip == clipItem->clip)
        {
            table->removeRow(table->row(clipItem));
            return false;
        }
        else
            return true;
    });
}
