#ifndef CLIPSWIDGET_H
#define CLIPSWIDGET_H

#include "view.h"

#include <functional>

class Clip;
class ClipboardManager;
class ClipsTable;
class QVBoxLayout;
class QStackedLayout;
class QLineEdit;
class QMenu;

class ClipItem;

class ClipsWidget : public View
{
    Q_OBJECT
public:
    explicit ClipsWidget(ClipboardManager& clipboardManager, QWidget* parent = 0);

    bool escapeRequested() override;

signals:

private slots:
    void pushMru(QSharedPointer<Clip> clip);
    void addSavedClip(QSharedPointer<Clip> clip);
    void updateResults(const QString& text);

private:
    void assignTableMenu(ClipsTable* table, QMenu* menu);

    void handleContextMenuAction(QAction* action);

    static void searchTableItems(ClipsTable* table, std::function<bool(ClipItem*)> handler);
    static void searchSelectedItems(ClipsTable* table, std::function<bool(ClipItem*)> handler);
    static void searchSelectedClips(ClipsTable* table, std::function<bool(QSharedPointer<Clip>)> handler);
    static void removeClipFromTable(ClipsTable* table, QSharedPointer<Clip> clip);

    ClipboardManager& clipboardManager;

    QVBoxLayout*    mainLayout = 0;
    QLineEdit*      searchEdit = 0;
    QStackedLayout* stacked = 0;
    ClipsTable*     mruTable = 0;
    QMenu*          mruMenu = 0;
    ClipsTable*     savedTable = 0;
    QMenu*          savedMenu = 0;
    QPoint          contextMenuPoint;
    ClipsTable*     currentContextTable = 0;
    ClipsTable*     resultsTable = 0;
    QMenu*          resultsMenu = 0;
};

#endif // CLIPSWIDGET_H
