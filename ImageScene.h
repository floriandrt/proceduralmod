#ifndef IMAGESCENE_H
#define IMAGESCENE_H

#include <QGraphicsScene>


class ImageScene : public QGraphicsScene
{
  Q_OBJECT;
public:
  ImageScene(QObject *parent=0) : QGraphicsScene(parent){}
  void mouseMoveEvent(QGraphicsSceneMouseEvent * e );
  void mousePressEvent(QGraphicsSceneMouseEvent * e );
signals:
  void mouseMoved(QPointF oldPos, QPointF newPos);
};

#endif // IMAGESCENE_H
