#ifndef STATEBOX_H
#define STATEBOX_H


/*
  * Copyright 2010, David W. Drell
  *
  * This is free sample code. Use as you wish, no warranties.
  */

#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <QGraphicsScene>
#include "cornergrabber.h"
#include "ImageScene.h"
#include "data.h"
#include "tree.h"



/**
  * \class StateBox
  * This is short example/demonstration of creating a custom Qt QGraphicsItem.
  *  Example usage of a custom QGraphicsItem in a QGraphicsScene with examples of mouse hover events.
  *
  *  My goal was to create a simpe box, in the shape of a UML State/Class box, with a title
  *  area seprated from a main arear below by line.
  *
  *  This sample class inherits from QGraphicsItem, and must reimplement boundingRect() and paint()
  *  from the base class.
  *
  *  To learn about handling mouse events in a QGraphicsScene, I wanted my box to turn red when the mouse moves inside, and black
  *  when the mouse moves out.
  */

class QGraphicsPixmapItem;

class StateBox : public QGraphicsItem
{
public:
    StateBox(ImageScene *_scene);
    void setGridSpace(int space);

    private:


    virtual QRectF boundingRect() const; ///< must be re-implemented in this class to provide the diminsions of the box to the QGraphicsView
    virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget); ///< must be re-implemented here to pain the box on the paint-event
    virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event ); ///< must be re-implemented to handle mouse hover enter events
    virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event ); ///< must be re-implemented to handle mouse hover leave events

    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );///< allows the main object to be moved in the scene by capturing the mouse move events
    virtual void mousePressEvent (QGraphicsSceneMouseEvent * event );
    virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent * event );

    virtual void mouseMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void mousePressEvent(QGraphicsSceneDragDropEvent *event);
    virtual bool sceneEventFilter ( QGraphicsItem * watched, QEvent * event ) ;

    void setCornerPositions();

    void adjustSize(int x, int y);


    QImage *_img;
    Tree<Data> t;
    QColor _outterborderColor; ///< the hover event handlers will toggle this between red and black
    QPen _outterborderPen; ///< the pen is used to paint the red/black border
    QPointF _location;
    QPointF _dragStart;
    int     _gridSpace;
    qreal   _width;
    qreal   _height;
    Data points;
    QPointF _cornerDragStart;

    int _XcornerGrabBuffer;
    int _YcornerGrabBuffer;
    qreal   _drawingWidth;
    qreal   _drawingHeight;
    qreal   _drawingOrigenX;
    qreal   _drawingOrigenY;

    ImageScene *_mainScene;

    CornerGrabber*  _corners[8];// 0,1,2,3  - starting at x=0,y=0 and moving clockwise around the box


};

#endif // STATEBOX_H
