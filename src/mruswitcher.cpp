#include "mruswitcher.h"
#include "clipboardmanager.h"

MruSwitcher::MruSwitcher(ClipboardManager& clipboardManager)
    : clipboardManager(clipboardManager)
{
}

QList<QSharedPointer<Clip>> MruSwitcher::getClips()
{
    return this->clipboardManager.getMruClips();
}
