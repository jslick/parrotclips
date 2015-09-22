#include "clipboardmanager.h"
#include "clipsstorage.h"

#include <QClipboard>
#include <QApplication>

static const int MRU_LIMIT = 100;

ClipboardManager::ClipboardManager(ClipsStorage& storage, QObject* parent)
    : QObject(parent),
      storage(storage),
      rootGroup(1, *this)
{
    connect(&this->rootGroup, &ClipsGroup::clipInserted, [this](int position,QSharedPointer<Clip> clip)
    { emit clipInserted(&this->rootGroup, position, clip); });
    connect(&this->rootGroup, &ClipsGroup::clipRemoved, [this](QSharedPointer<Clip> clip)
    { emit clipRemoved(&this->rootGroup, clip); });

    connect(this, &ClipboardManager::clipInserted,
            &this->storage, &ClipsStorage::saveClip);
    connect(this, &ClipboardManager::clipRemoved, [this](ClipsGroup*, QSharedPointer<Clip> clip)
    { this->storage.removeClip(clip); });
}

ClipsStorage& ClipboardManager::getStorage() const
{
    return this->storage;
}

ClipsGroup& ClipboardManager::getRootGroup()
{
    return this->rootGroup;
}

const QQueue<QSharedPointer<Clip>>& ClipboardManager::getMruClips() const
{
    return this->mruClips;
}

QList<QSharedPointer<Clip>> ClipboardManager::searchClips(const QString& text) const
{
    return this->storage.searchClips(text, const_cast<ClipboardManager&>(*this));
}

void ClipboardManager::setClipboard(QSharedPointer<Clip>& clip)
{
    clip->sync();
    this->mruHintClip = clip;

    QClipboard* clipboard = QApplication::clipboard();
    QMimeData* clipboardData = new QMimeData;
    copyMimeData(*clipboardData, clip->getData());
    clipboard->setMimeData(clipboardData);  // Takes ownership
}

void ClipboardManager::pushMru(QSharedPointer<Clip> clip)
{
    if (this->mruHintClip && isClipDataSame(clip->clipboardData, this->mruHintClip->clipboardData))
        clip = this->mruHintClip;
    this->mruHintClip.clear();

    if (this->mruClips.length())
    {
        const QSharedPointer<Clip>& recentClip = this->mruClips.back();
        recentClip->sync();
        if (isClipDataSame(clip->getData(), recentClip->getData()))
            return;
    }

    this->mruClips.enqueue(clip);
    emit mruPushed(clip);

    if (this->mruClips.length() > MRU_LIMIT)
    {
        QSharedPointer<Clip> removedClip = this->mruClips.dequeue();
        emit mruRemoved(removedClip);
    }
}

void ClipboardManager::removeMruClip(const QSharedPointer<Clip>& clip)
{
    bool removed = this->mruClips.removeOne(clip);
    if (removed)
        emit mruRemoved(clip);
}
