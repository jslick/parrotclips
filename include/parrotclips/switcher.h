#ifndef SWITCHER_H
#define SWITCHER_H

#include <QList>
#include <QSharedPointer>

class Clip;

class Switcher
{
public:
    virtual ~Switcher() {}

    virtual QList<QSharedPointer<Clip>> getClips() = 0;
};

#endif // SWITCHER_H
