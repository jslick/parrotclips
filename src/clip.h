#ifndef CLIP_H
#define CLIP_H

#include <QObject>
#include <QMimeData>
#include <cstdint>

class ClipboardManager;

class Clip : public QObject
{
    Q_OBJECT
public:
    explicit Clip(const QMimeData& mimeData, ClipboardManager& manager);

    Q_DISABLE_COPY(Clip)

    QString getPreview() const;

signals:
    void namedChanged(QString newName);

public slots:
    void setId(int64_t id);

    void setName(const QString& name);

    void setPreview(const QString& text);

    void setClipboard();

public:
    ClipboardManager& manager;

    int64_t clipId = 0;
    QString name;
    mutable QString preview;
    QMimeData clipboardData;
};

#endif // CLIP_H
