#ifndef CLIPSTABLE_H
#define CLIPSTABLE_H

#include <QTableWidget>
#include <functional>

class Clip;

enum TableMenuAction : int
{
    ActivateAction = 1,
};

class ClipItem;


class ClipsTable : public QTableWidget
{
    Q_OBJECT
public:
    explicit ClipsTable(QWidget* parent = 0);

    void addContextMenuAction(QAction* action);

    void searchTableItems(std::function<bool(ClipItem*)> handler) const;
    void searchSelectedItems(std::function<bool(ClipItem*)> handler) const;
    void searchSelectedClips(std::function<bool(QSharedPointer<Clip>)> handler) const;
    void removeClipFromTable(QSharedPointer<Clip> clip);

signals:
    void contextActionTriggered(QAction* action);

public slots:
    void prependClip(QSharedPointer<Clip> clip);
    void appendClip(QSharedPointer<Clip> clip, bool size);

    void showContextMenu(const QPoint& pos);

protected slots:
    void handleContextMenuAction(QAction* action);

    void rowsInserted(const QModelIndex& parent, int start, int end) override;
    void rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end) override;

protected:
    void focusInEvent(QFocusEvent*) override;
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    void renumberRows(int from);

private:
    static ClipsTable* currentContextTable;

    QMenu* menu = 0;

    int renumberStart = -1;
};

#endif // CLIPSTABLE_H
