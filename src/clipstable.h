#ifndef CLIPSTABLE_H
#define CLIPSTABLE_H

#include <QTableWidget>
#include <functional>

class Clip;

enum TableMenuAction : int
{
    ActivateAction = 1,
};

enum Cols : int
{
    NumberCol       = 0,
    NameCol         = 1,
    TextPreviewCol  = 2,
};

class ClipItem : public QTableWidgetItem
{
public:
    ClipItem(QSharedPointer<Clip>& clip);

    QSharedPointer<Clip> clip;
};


class ClipsTable : public QTableWidget
{
    Q_OBJECT
public:
    explicit ClipsTable(QWidget* parent = 0);

    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    void addContextMenuAction(QAction* action);

    void searchTableItems(std::function<bool(ClipItem*)> handler) const;
    void searchSelectedItems(std::function<bool(ClipItem*)> handler) const;
    void searchSelectedClips(std::function<bool(QSharedPointer<Clip>)> handler) const;
    void removeClipFromTable(QSharedPointer<Clip> clip);

signals:
    void contextActionTriggered(QAction* action);

public slots:
    void showContextMenu(const QPoint& pos);

protected slots:
    void handleContextMenuAction(QAction* action);

protected:
    void focusInEvent(QFocusEvent*);

private:
    static ClipsTable* currentContextTable;

    QMenu* menu = 0;
};

#endif // CLIPSTABLE_H
