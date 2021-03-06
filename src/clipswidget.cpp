#include "clipswidget.h"
#include "clip.h"
#include "clipboardmanager.h"
#include "clipstable.h"

#include <QAction>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QSplitter>
#include <QStackedLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QInputDialog>
#include <QMessageBox>

ClipsWidget::ClipsWidget(ClipboardManager& clipboardManager, QWidget* parent) :
    View(parent),
    clipboardManager(clipboardManager),
    mainLayout(new QVBoxLayout),
    searchEdit(new QLineEdit(this)),
    stacked(new QStackedLayout),
    mruTable(new ClipsTable(this)),
    savedTable(new ClipsTable(this)),
    saveAction(new QAction(tr("&Save as..."), this)),
    removeAction(new QAction(tr("&Remove"), this)),
    resultsTable(new ClipsTable(this))
{
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
    QLabel* savedLabel = new QLabel(tr("Sa&ved clips:"), this);
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

    // Context menu actions
    this->mruTable->addContextMenuAction(this->saveAction);
    this->mruTable->addContextMenuAction(this->removeAction);
    this->savedTable->addContextMenuAction(this->removeAction);
    connect(this->mruTable, &ClipsTable::contextActionTriggered, this, &ClipsWidget::handleContextMenuAction);
    connect(this->savedTable, &ClipsTable::contextActionTriggered, this, &ClipsWidget::handleContextMenuAction);

    // Data binding
    connect(&this->clipboardManager, &ClipboardManager::mruPushed, this->mruTable, &ClipsTable::prependClip);
    connect(&this->clipboardManager, &ClipboardManager::mruRemoved, this->mruTable, &ClipsTable::removeClipFromTable);
    connect(&this->clipboardManager, &ClipboardManager::clipInserted, [this](ClipsGroup*,int,QSharedPointer<Clip> clip)
    {
        this->savedTable->prependClip(clip);
    });
    connect(&this->clipboardManager, &ClipboardManager::clipRemoved, [this](ClipsGroup*,QSharedPointer<Clip> clip)
    {
        this->savedTable->removeClipFromTable(clip);
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

void ClipsWidget::windowShown()
{
    this->searchEdit->selectAll();
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
            this->resultsTable->appendClip(clip, false);
        }
        this->resultsTable->resizeRowsToContents();

        this->stacked->setCurrentIndex(1);
    }
}

void ClipsWidget::handleContextMenuAction(QAction* action)
{
    ClipsTable* table = dynamic_cast<ClipsTable*>(QObject::sender());
    Q_ASSERT(table);
    if (!table)
        return;

    if (action == this->saveAction)
        this->saveClip(table);
    else if (action == this->removeAction)
        this->removeClips(table);
}

void ClipsWidget::saveClip(ClipsTable* table)
{
    bool ok = false;
    QString name = QInputDialog::getText(this,
                                         tr("Save As"), tr("Name"),
                                         QLineEdit::Normal, QString(), &ok);
    if (ok)
    {
        table->searchSelectedClips([this,&name](QSharedPointer<Clip> clip)
        {
            clip->setName(name);
            this->clipboardManager.getRootGroup().addClip(clip);
            return false;
        });
    }
}

void ClipsWidget::removeClips(ClipsTable* table)
{
    int rv = QMessageBox::question(this,
                          tr("Remove Confirmation"),
                          tr("Are you should you want to remove the selected clip(s)?")
                          );
    if (rv == QMessageBox::Yes)
    {
        if (table == this->mruTable)
        {
            table->searchSelectedClips([this](QSharedPointer<Clip> clip)
            {
                this->clipboardManager.removeMruClip(clip);
                return true;
            });
        }
        else
        {
            table->searchSelectedClips([this](QSharedPointer<Clip> clip)
            {
                this->clipboardManager.getRootGroup().removeClip(clip);
                return true;
            });
        }
    }
}
