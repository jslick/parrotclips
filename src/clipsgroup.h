#ifndef CLIPSGROUP_H
#define CLIPSGROUP_H

#include <QObject>
#include <QSharedPointer>
#include <QList>
#include <cstdint>

class Clip;
class ClipboardManager;

class ClipsGroup : public QObject
{
    Q_OBJECT
public:
    explicit ClipsGroup(int64_t groupId, ClipboardManager& clipboardManager, QObject* parent = 0);

signals:
    void clipInserted(int position,QSharedPointer<Clip>);
    void clipRemoved(QSharedPointer<Clip>);

public slots:
    void addClip(const QSharedPointer<Clip>& clip);
    void removeClip(const QSharedPointer<Clip>& clip);
    void loadClips();

private:
    int64_t groupId;
    ClipboardManager& clipboardManager;

    QList<QSharedPointer<Clip>> clips;
};

#endif // CLIPSGROUP_H
