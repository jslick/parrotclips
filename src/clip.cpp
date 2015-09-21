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

bool isClipDataSame(const QMimeData& d1, const QMimeData& d2)
{
    if (d1.hasText() != d2.hasText())
        return false;
    if (d1.hasHtml() != d2.hasHtml())
        return false;
    if (d1.hasUrls() != d2.hasUrls())
        return false;

    if (d1.text() != d2.text())
        return false;
    if (d1.html() != d2.html())
        return false;
    QList<QUrl> d1Urls = d1.urls();
    QList<QUrl> d2Urls = d2.urls();
    if (d1Urls.length() == d2Urls.length())
    {
        for (int i = 0; i < d1Urls.length(); i++)
        {
            if (d1Urls.at(i) != d2Urls.at(i))
                return false;
        }
    }
    else
        return false;

    return true;
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

void Clip::sync()
{
    // Load data if not already loaded
    if (this->clipboardData.text().isEmpty())
        this->manager.getStorage().getClipData(this->clipId, this->clipboardData);
}

const QMimeData& Clip::getData() const
{
    return this->clipboardData;
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
    this->sync();

    QClipboard* clipboard = QApplication::clipboard();
    QMimeData* clipboardData = new QMimeData;
    copyMimeData(*clipboardData, this->clipboardData);
    clipboard->setMimeData(clipboardData);  // Takes ownership
}
