#include "clipsstorage.h"

#include "clip.h"

#include <QFile>
#include <QRegularExpression>
#include <QDebug>
#include <sqlite3.h>

static
QByteArray readResourceFile(const QString& name)
{
    QFile resourceFile(name);
    resourceFile.open(QIODevice::ReadOnly | QIODevice::Text);
    return resourceFile.readAll();
}


ClipsStorage::ClipsStorage(const QString& dbPath, QObject* parent)
    : QObject(parent)
{
    int rv = sqlite3_open(dbPath.toStdString().c_str(), &this->db);
    if (rv != SQLITE_OK)
        throw OpenException(tr("Could not open the clips database"));

    this->ensureSchema();
}

ClipsStorage::~ClipsStorage()
{
    if (this->db)
        sqlite3_close(this->db);
}

QList<QSharedPointer<Clip>> ClipsStorage::getClips(int64_t groupId, ClipboardManager& clipboardManager) const
{
    QList<QSharedPointer<Clip>> clips;

    static const char SELECT_CLIPS[] =
            "SELECT clips.id, clips.time, clips.name, clips.sequence, cd.id AS clip_data_id, cd.mimetype, cd.text_contents "
            "FROM clips "
            "INNER JOIN clip_data cd ON cd.clip_id=clips.id "
            "WHERE clips.owner_group_id=? "
            "  AND  cd.mimetype = 'text/plain'";
    sqlite3_stmt* stmt = 0;
    sqlite3_prepare_v2(this->db, SELECT_CLIPS, -1, &stmt, nullptr);
    sqlite3_bind_int64(stmt, 1, groupId);
    clips = unpackStatement(stmt, clipboardManager);
    sqlite3_finalize(stmt);

    return clips;
}

QList<QSharedPointer<Clip>> ClipsStorage::searchClips(const QString& text, ClipboardManager& clipboardManager) const
{
    static const QString SEARCH_CLIPS =
            "SELECT clips.id, clips.time, clips.name, clips.sequence, cd.id AS clip_data_id, cd.mimetype, cd.text_contents "
            "FROM clips "
            "INNER JOIN clip_data cd ON cd.clip_id=clips.id "
            "WHERE ( %1 ) "
            "  AND  cd.mimetype = 'text/plain'";

    // Split on words, as conjunctions
    const QStringList words = text.split(QRegularExpression("\\s+"));
    if (words.isEmpty())
        return QList<QSharedPointer<Clip>>();

    QStringList clauses;
    for (const QString& word : words)
    {   Q_UNUSED(word)
        clauses.append("( name LIKE ? OR cd.text_contents LIKE ? )");
    }
    const QString clause = clauses.join(" AND ");

    sqlite3_stmt* stmt = 0;
    sqlite3_prepare_v2(this->db, SEARCH_CLIPS.arg(clause).toStdString().c_str(), -1, &stmt, nullptr);
    for (int i = 0; i < words.length(); i++)
    {
        const QString word = words.at(i);
        const QString search = "%" + word + "%";
        sqlite3_bind_text(stmt, i*2+1, search.toStdString().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, i*2+2, search.toStdString().c_str(), -1, SQLITE_TRANSIENT);
    }
    QList<QSharedPointer<Clip>> results = unpackStatement(stmt, clipboardManager);
    sqlite3_finalize(stmt);

    return results;
}

void ClipsStorage::getClipData(int64_t clipId, QMimeData& mimeData) const
{
    static const char SELECT_DATA[] =
            "SELECT mimetype, text_contents, binary_contents "
            "FROM clip_data "
            "WHERE clip_id=?";
    sqlite3_stmt* stmt = 0;
    sqlite3_prepare_v2(this->db, SELECT_DATA, -1, &stmt, nullptr);
    sqlite3_bind_int64(stmt, 1, clipId);
    // TODO:  Use consistent encodings in save & get
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const QString mimetype = QString::fromUtf8((char*)sqlite3_column_text(stmt, 0));
        const int binary_size = sqlite3_column_bytes(stmt, 2);
        if (binary_size)
        {
            QByteArray binaryBytes((const char*)sqlite3_column_blob(stmt, 2), binary_size);
            mimeData.setData(mimetype, binaryBytes);
        }
        else
        {
            const QString textContents = QString::fromUtf8((char*)sqlite3_column_text(stmt, 1));
            if (textContents.length())
                mimeData.setData(mimetype, textContents.toUtf8());
        }
    }
}

void ClipsStorage::saveClip(ClipsGroup* group, int position, QSharedPointer<Clip> clip)
{
    if (clip->clipId)
        return;

    static const char INSERT_CLIP[] =
            "INSERT INTO clips(name,owner_group_id,sequence) "
            "VALUES(?,?,?)";

    sqlite3_stmt* stmt = 0;
    sqlite3_prepare_v2(this->db, INSERT_CLIP, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, clip->name.toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, 1);   // TODO:  Get the actual owner group id
    sqlite3_bind_int(stmt, 3, 0);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    const sqlite3_int64 clip_id = sqlite3_last_insert_rowid(this->db);
    clip->setId(clip_id);

    static const char INSERT_CLIP_DATA[] =
            "INSERT INTO clip_data(clip_id,mimetype,text_contents,binary_contents) "
            "VALUES(?,?,?,?)";

    stmt = 0;
    sqlite3_prepare_v2(this->db, INSERT_CLIP_DATA, -1, &stmt, nullptr);

    sqlite3_bind_int64(stmt, 1, clip_id);
    sqlite3_bind_text(stmt, 2, "text/plain", -1, SQLITE_STATIC);
    if (clip->clipboardData.text().length() <= 512)
    {
        sqlite3_bind_text(stmt, 3, clip->clipboardData.text().toStdString().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_null(stmt, 4);
    }
    else
    {
        sqlite3_bind_text(stmt, 3, clip->clipboardData.text().left(512).toStdString().c_str(), -1, SQLITE_TRANSIENT);
        const std::string contents = clip->clipboardData.text().toStdString();
        sqlite3_bind_blob(stmt, 4, contents.data(), contents.size(), SQLITE_TRANSIENT);
    }
    sqlite3_step(stmt);

    if (clip->clipboardData.hasHtml())
    {
        sqlite3_reset(stmt);

        sqlite3_bind_int64(stmt, 1, clip_id);
        sqlite3_bind_text(stmt, 2, "text/html", -1, SQLITE_STATIC);
        sqlite3_bind_null(stmt, 3);
        const std::string contents = clip->clipboardData.html().toStdString();
        sqlite3_bind_blob(stmt, 4, contents.data(), contents.size(), SQLITE_TRANSIENT);
        sqlite3_step(stmt);
    }

    sqlite3_finalize(stmt);

    Q_UNUSED(group);
    Q_UNUSED(position);
}

void ClipsStorage::removeClip(const QSharedPointer<Clip>& clip)
{
    if (clip->clipId < 0)
    {
        qDebug() << "Attempted to remove unsaved clip";
        return;
    }

    static const char REMOVE_CLIP_DATA[] =
            "DELETE FROM clip_data "
            "WHERE clip_id=?";
    sqlite3_stmt* stmt = 0;
    sqlite3_prepare_v2(this->db, REMOVE_CLIP_DATA, -1, &stmt, nullptr);
    sqlite3_bind_int64(stmt, 1, clip->clipId);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    static const char REMOVE_CLIP[] =
            "DELETE from clips "
            "WHERE id=?";
    stmt = 0;
    sqlite3_prepare_v2(this->db, REMOVE_CLIP, -1, &stmt, nullptr);
    sqlite3_bind_int64(stmt, 1, clip->clipId);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void ClipsStorage::ensureSchema() const
{
    if (!hasSchema())
    {
        qDebug() << "Creating schema";

        QByteArray schemaBytes = readResourceFile(":/sql/schema.sql");
        Q_ASSERT(schemaBytes.length());
        char* errmsg = nullptr;
        sqlite3_exec(this->db, schemaBytes, nullptr, nullptr, &errmsg);
        if (errmsg)
        {
            QString msg = errmsg;
            sqlite3_free(errmsg);
            throw OpenException(tr("Could not initialize clips database:  %1").arg(msg));
        }

        qDebug() << "Created schema";

        seedData();
    }
}

static
int countRows(void* data, int, char**, char**)
{
    int& numRows = *static_cast<int*>(data);
    numRows++;
    return 0;
}

bool ClipsStorage::hasSchema() const
{
    static const char SQL[] = "SELECT name FROM sqlite_master WHERE type='table'";

    int numRows = 0;
    sqlite3_exec(this->db, SQL, countRows, &numRows, nullptr);
    return numRows > 0;
}

void ClipsStorage::seedData() const
{
    QByteArray seedBytes = readResourceFile(":/sql/seed.sql");
    char* errmsg = 0;
    sqlite3_exec(this->db, seedBytes.toStdString().c_str(), nullptr, nullptr, &errmsg);
    if (errmsg)
    {
        qDebug() << "Could not seed database: " << errmsg;
        sqlite3_free(errmsg);
        Q_ASSERT(false);
    }
}

QList<QSharedPointer<Clip>> ClipsStorage::unpackStatement(sqlite3_stmt* stmt, ClipboardManager& clipboardManager) const
{
    QList<QSharedPointer<Clip>> clips;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        sqlite3_int64 id = sqlite3_column_int64(stmt, 0);
        QMimeData mimeData;
        QSharedPointer<Clip> clip(new Clip(mimeData, clipboardManager));
        clip->setId(id);
        clip->setName(QString::fromUtf8((char*)sqlite3_column_text(stmt, 2)));
        clip->setPreview(QString::fromUtf8((char*)sqlite3_column_text(stmt, 6)));

        clips.append(clip);
    }

    return clips;
}
