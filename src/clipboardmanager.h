#ifndef CLIPBOARDMANAGER_H
#define CLIPBOARDMANAGER_H

#include "clip.h"
#include "clipsgroup.h"

#include <QObject>
#include <QSharedPointer>
#include <QQueue>

class ClipsStorage;

class ClipboardManager : public QObject
{
    Q_OBJECT
public:
    explicit ClipboardManager(ClipsStorage& storage, QObject* parent = 0);

    ClipsStorage& getStorage() const;

    ClipsGroup& getRootGroup();

    const QQueue<QSharedPointer<Clip>>& getMruClips() const;

    QList<QSharedPointer<Clip>> searchClips(const QString& text) const;

signals:
    void mruPushed(QSharedPointer<Clip> clip);
    void mruRemoved(QSharedPointer<Clip> clip);
    void clipInserted(ClipsGroup*,int position,QSharedPointer<Clip>);
    void clipRemoved(ClipsGroup*,QSharedPointer<Clip>);

public slots:
    void pushMru(const QSharedPointer<Clip>& clip);
    void removeMruClip(const QSharedPointer<Clip>& clip);

private:
    ClipsStorage& storage;

    QQueue<QSharedPointer<Clip>> mruClips;
    ClipsGroup rootGroup;
};

#endif // CLIPBOARDMANAGER_H
