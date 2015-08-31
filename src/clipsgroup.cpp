#include "clipsgroup.h"
#include "clipboardmanager.h"
#include "clipsstorage.h"

ClipsGroup::ClipsGroup(int64_t groupId, ClipboardManager& clipboardManager, QObject* parent)
    : QObject(parent),
      groupId(groupId),
      clipboardManager(clipboardManager)
{

}

void ClipsGroup::addClip(const QSharedPointer<Clip>& clip)
{
    // TODO:  Should prolly copy the clip so that the user can save the same MRU to different groups?

    this->clips.append(clip);
    emit clipInserted(this->clips.length()-1, clip);
}

void ClipsGroup::removeClip(const QSharedPointer<Clip>& clip)
{
    bool removed = this->clips.removeOne(clip);
    if (removed)
        emit clipRemoved(clip);
}

void ClipsGroup::loadClips()
{
    QList<QSharedPointer<Clip>> clips = this->clipboardManager.getStorage().getClips(this->groupId, this->clipboardManager);
    for (const QSharedPointer<Clip> clip : clips)
    {
        this->addClip(clip);
    }
}
