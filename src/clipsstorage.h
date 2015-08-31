#ifndef CLIPSSTORAGE_H
#define CLIPSSTORAGE_H

#include <QObject>
#include <QSharedPointer>
#include <QList>
#include <QMimeData>
#include <QException>
#include <cstdint>

class Clip;
class ClipsGroup;
class ClipboardManager;

typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;

class OpenException : public QException
{
public:
    OpenException(const QString& message) : msg(message) {}

    void raise() const { throw *this; }
    OpenException* clone() const { return new OpenException(*this); }

    const QString& getMessage() const { return this->msg; }

    QString msg;
};


class ClipsStorage final : public QObject
{
    Q_OBJECT
public:
    explicit ClipsStorage(const QString& dbPath, QObject* parent = 0);

    Q_DISABLE_COPY(ClipsStorage)

    ~ClipsStorage();

    QList<QSharedPointer<Clip>> getClips(int64_t groupId, ClipboardManager& clipboardManager) const;

    QList<QSharedPointer<Clip>> searchClips(const QString& text, ClipboardManager& clipboardManager) const;

    void getClipData(int64_t clipId, QMimeData& mimeData) const;

signals:

public slots:
    void saveClip(ClipsGroup* group, int position, QSharedPointer<Clip> clip);

    void removeClip(const QSharedPointer<Clip>& clip);

private:
    void ensureSchema() const;

    bool hasSchema() const;

    void seedData() const;

    QList<QSharedPointer<Clip>> unpackStatement(sqlite3_stmt* stmt, ClipboardManager& clipboardManager) const;

    sqlite3* db = 0;
};

#endif // CLIPSSTORAGE_H
