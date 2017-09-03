#ifndef BALL_H
#define BALL_H

#include "entities.h"

#include <QGraphicsEllipseItem>
#include <qdebug.h>

using namespace constants;

class throwableBall : public QGraphicsEllipseItem
{
public:
    throwableBall()
    {
        this->setTransformOriginPoint(QPointF(BALL_DIAMETER/2., BALL_DIAMETER/2.));
    }

    virtual void advance(int phase) override
    {
        this->setPos(this->pos().x() , this->pos().y() - 1);
    }

private:
    QPointF newPos;
};

#endif // BALL_H
