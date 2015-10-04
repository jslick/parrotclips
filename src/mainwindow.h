#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>
#include <QSystemTrayIcon>

class ClipboardManager;
class ClipsStorage;
class View;
class SwitcherDialog;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

protected:
    void showEvent(QShowEvent* event) override;

    void closeEvent(QCloseEvent* event) override;

private slots:
    void positionWindow();

    void bringToFront();

    void showSwitcher();

    void fetchClip();

    void maybeHide();

    void handleTray(QSystemTrayIcon::ActivationReason reason);

private:
    QScopedPointer<ClipsStorage> storage;
    ClipboardManager* clipboardManager = 0;
    View* view = 0;
    bool shown = false;
    QScopedPointer<SwitcherDialog> switcherDialog;
};

#endif // MAINWINDOW_H
