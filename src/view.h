#ifndef VIEW_H
#define VIEW_H

#include <QWidget>

class View : public QWidget
{
public:
    explicit View(QWidget* parent = 0)
        : QWidget(parent)
    {}

    virtual ~View() {}

    virtual bool escapeRequested() { return true; }
};

#endif // VIEW_H
