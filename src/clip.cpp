#include "clip.h"
#include "clipboardmanager.h"
#include "clipsstorage.h"

#include <QApplication>
#include <QClipboard>
#include <QUrl>

static
void copyMimeData(QMimeData& dest, const QMimeData& src)
{
    if (src.hasUrls())
        dest.setUrls(src.urls());
    if (src.hasHtml())
        dest.setHtml(src.html());
    if (src.hasText())
        dest.setText(src.text());
}

static
QString makeClipPreview(const QString& text)
{
    return text.left(60).replace("\n", "\\n").replace("\r", "\\r");
}


Clip::Clip(const QMimeData& mimeData, ClipboardManager& manager) :
    QObject(nullptr),
    manager(manager)
{
    copyMimeData(this->clipboardData, mimeData);
}

QString Clip::getPreview() const
{
    if (this->preview.isEmpty())
        this->preview = makeClipPreview(this->clipboardData.text());
    return this->preview;
}

void Clip::setId(int64_t id)
{
    this->clipId = id;
}

void Clip::setName(const QString& name)
{
    this->name = name;
    emit namedChanged(name);
}

void Clip::setPreview(const QString& text)
{
    this->preview = makeClipPreview(text);
}

void Clip::setClipboard()
{
    // Load data if not already loaded
    if (this->clipboardData.text().isEmpty())
        this->manager.getStorage().getClipData(this->clipId, this->clipboardData);

    QClipboard* clipboard = QApplication::clipboard();
    QMimeData* clipboardData = new QMimeData;
    copyMimeData(*clipboardData, this->clipboardData);
    clipboard->setMimeData(clipboardData);  // Takes ownership
}
