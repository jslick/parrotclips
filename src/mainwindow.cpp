#include "mainwindow.h"
#include "clipswidget.h"
#include "clip.h"
#include "clipboardmanager.h"
#include "clipsstorage.h"

#include <QApplication>
#include <QAction>
#include <QDesktopWidget>
#include <QStandardPaths>
#include <QDir>
#include <QStyle>
#include <QClipboard>
#include <QTimer>
#include <QMessageBox>
#include <QDebug>

#include <UGlobalHotkey/uglobalhotkeys.h>

#define APP_NAME "Parrot Clips"

static
QString getDbPath()
{
    const QString storagePath = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation).first();
    QDir storageDir(storagePath);
    if (storageDir.exists() == false)
        storageDir.mkpath(".");
    const QString dbPath = storageDir.absoluteFilePath("parrotclips.db");
    return dbPath;
}


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    {
        Qt::WindowFlags flags = this->windowFlags();
        flags &= ~Qt::WindowMaximizeButtonHint;
//        flags |= Qt::WindowStaysOnTopHint;
        this->setWindowFlags(flags);
    }

    this->setWindowTitle(APP_NAME);
    this->setWindowIcon(QIcon(":/icons/app.png"));

    UGlobalHotkeys* hotkeyManager = new UGlobalHotkeys(this);
    try
    {
        hotkeyManager->registerHotkey("Alt+Shift+C");

        connect(hotkeyManager, SIGNAL(activated(size_t)), SLOT(bringToFront()));
    }
    catch (UException& e)
    {
        qDebug() << "Unable to register hotkey: " << e.what();
    }

    try
    {
        this->storage.reset(new ClipsStorage(getDbPath()));
    }
    catch (OpenException& e)
    {
        QMessageBox::warning(this,
                             tr("Error opening clips file"),
                             tr("Parrot Clips could not open the clips database:\n%1").arg(e.getMessage())
                             );
    }
    this->clipboardManager = new ClipboardManager(*this->storage, this);
    this->view = new ClipsWidget(*this->clipboardManager, this);

    this->setCentralWidget(this->view);
    this->resize(560, 450);

    this->clipboardManager->getRootGroup().loadClips();

    QClipboard* clipboard = QApplication::clipboard();
    connect(clipboard, &QClipboard::dataChanged, this, &MainWindow::fetchClip);
    QTimer::singleShot(0, this, SLOT(fetchClip()));

    QAction* hideAction = new QAction(tr("Hide"), this);
    hideAction->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(hideAction, SIGNAL(triggered()), SLOT(maybeHide()));
    this->addAction(hideAction);
}

MainWindow::~MainWindow()
{
}

void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);

    if (!this->shown)
        this->positionWindow();

    this->shown = true;
}

void MainWindow::positionWindow()
{
    const QRect avail = QApplication::desktop()->availableGeometry(this);
    int frameSize = QApplication::style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    Q_UNUSED(frameSize);
    struct {
        int x;
        int y;
    } OS_DIFF =
#ifdef Q_OS_WIN
    { frameSize*8, frameSize*8 };
#else
    { 0, 0 };
#endif
    int w = this->width();
    int h = this->height();
    this->setGeometry(avail.width() - w - OS_DIFF.x, avail.height() - h - OS_DIFF.y, w, h);
}

void MainWindow::bringToFront()
{
    this->show();
    this->raise();
    this->activateWindow();

    this->view->windowShown();
}

void MainWindow::fetchClip()
{
    QClipboard* clipboard = QApplication::clipboard();
    const QMimeData* data = clipboard->mimeData();

    if (data && data->hasText())  // NOTE:  Only supporting text-able clips for now
    {
        QSharedPointer<Clip> clip(new Clip(*data, *this->clipboardManager));
        this->clipboardManager->pushMru(clip);
    }
}

void MainWindow::maybeHide()
{
    if (this->view->escapeRequested())
        this->hide();
}
