#ifndef CLIPSWIDGET_H
#define CLIPSWIDGET_H

#include <parrotclips/view.h>

class Clip;
class ClipboardManager;
class ClipsTable;
class QVBoxLayout;
class QStackedLayout;
class QLineEdit;

class ClipsWidget : public View
{
    Q_OBJECT
public:
    explicit ClipsWidget(ClipboardManager& clipboardManager, QWidget* parent = 0);

    // View interface
    bool escapeRequested() override;
    void windowShown() override;

signals:

private slots:
    void pushMru(QSharedPointer<Clip> clip);
    void addSavedClip(QSharedPointer<Clip> clip);
    void updateResults(const QString& text);
    void handleContextMenuAction(QAction* action);

private:
    void saveClip(ClipsTable* table);
    void removeClips(ClipsTable* table);

    ClipboardManager& clipboardManager;

    QVBoxLayout*    mainLayout = 0;
    QLineEdit*      searchEdit = 0;
    QStackedLayout* stacked = 0;
    ClipsTable*     mruTable = 0;
    ClipsTable*     savedTable = 0;
    QAction*        saveAction = 0;
    QAction*        removeAction = 0;
    ClipsTable*     resultsTable = 0;
};

#endif // CLIPSWIDGET_H
