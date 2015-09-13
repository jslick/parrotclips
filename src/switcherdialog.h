#ifndef SWITCHERDIALOG_H
#define SWITCHERDIALOG_H

#include <QDialog>
#include <QSharedPointer>

class QVBoxLayout;
class ClipsTable;
class Switcher;

class SwitcherDialog : public QDialog
{
public:
    SwitcherDialog();

    void setSwitcher(QSharedPointer<Switcher> switcher);

signals:

public slots:
    void selectNext();

private slots:
    void handleContextMenuAction(QAction* action);

private:
    QVBoxLayout* mainLayout = 0;
    ClipsTable* clipsTable = 0;
};

#endif // SWITCHERDIALOG_H
