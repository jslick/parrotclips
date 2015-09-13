#ifndef MRUSWITCHER_H
#define MRUSWITCHER_H

#include <parrotclips/switcher.h>

class ClipboardManager;

class MruSwitcher : public Switcher
{
public:
    MruSwitcher(ClipboardManager& clipboardManager);

    virtual QList<QSharedPointer<Clip>> getClips() override;

private:
    ClipboardManager& clipboardManager;
};

#endif // MRUSWITCHER_H
