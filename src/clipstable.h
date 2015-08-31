#ifndef CLIPSTABLE_H
#define CLIPSTABLE_H

#include <QTableWidget>

enum Cols : int
{
    NumberCol       = 0,
    NameCol         = 1,
    TextPreviewCol  = 2,
};


class ClipsTable : public QTableWidget
{
    Q_OBJECT
public:
    explicit ClipsTable(QWidget* parent = 0);

    void resizeEvent(QResizeEvent* event) override;

signals:

public slots:

};

#endif // CLIPSTABLE_H
