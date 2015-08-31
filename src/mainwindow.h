#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>

class ClipboardManager;
class ClipsStorage;
class View;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

protected:
    void showEvent(QShowEvent* event);

private slots:
    void positionWindow();

    void fetchClip();

    void maybeHide();

private:
    QScopedPointer<ClipsStorage> storage;
    ClipboardManager* clipboardManager = 0;
    View* view = 0;
    bool shown = false;
};

#endif // MAINWINDOW_H
