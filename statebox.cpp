#include "statebox.h"

#include <QBrush>
#include <QPainter>
#include <QSize>
#include <QLinearGradient>
#include <QLineF>
#include <QDebug>
#include <QMessageBox>
#include <QProgressDialog>
#include "point.h"
#include <stdio.h>
#include "cair/CAIR_CML.h"
#include "cair/CAIR.h"
#include "tools.h"
#include "math.h"

using namespace std;

/**
  *  This box can be re-sized and it can be moved. For moving, we capture
  *  the mouse move events and move the box location accordingly.
  *
  *  To resize the box, we place small indicator boxes on the four corners that give the user
  *  a visual cue to grab with the mouse. The user then drags the corner to stretch
  *  or shrink the box.  The corner grabbers are implemented with the CornerGrabber class.
  *  The CornerGrabber class captures the mouse when the mouse is over the corner's area,
  *  but the StateBox object (which owns the corners) captures and processes the mouse
  *  events on behalf of the CornerGrabbers (because the owner wants to be
  *  resized, not the CornerGrabbers themselves). This is accomplished by installed a scene event filter
  *  on the CornerGrabber objects:
          _corners[0]->installSceneEventFilter(this);
  *
  *
  *
  */

void drawImagefromData(Data& d, QImage* _img){
    QPainter qPaint;
    qPaint.begin(_img);
    qPaint.setBrush(Qt::NoBrush);
    qPaint.setPen(Qt::white);
    for(int i = 0; i<d.getDataSize(); i++){
        qPaint.drawLine(d[i][0],d[i][1],d[i][2],d[i][3]);
    }
    qPaint.end();
}

void f(Tree<Data>* t, QPainter& qPaint){
    if(!t->getIsLeaf()){
        for(int i = 0; i<t->getChildrenSize(); i++){
            f((*t)[i], qPaint);
        }
    }
    Data* temp = t->getValue();
    for(int i = 0; i<temp->getDataSize(); i++){
        qPaint.drawLine((*temp)[i][0],(*temp)[i][1],(*temp)[i][2],(*temp)[i][3]);
    }
}

void drawImagefromData(Tree<Data>& t, QImage* _img){
    //cerr << "ENTREE DRAW TREE" << endl;
    QPainter qPaint;
    qPaint.begin(_img);
    qPaint.setBrush(Qt::NoBrush);
    qPaint.setPen(Qt::white);
    for(int i = 0; i<t.getChildrenSize(); i++){
        f(t[i], qPaint);
    }
    qPaint.end();
    //cerr << "SORTIE DRAW TREE" << endl;
}

StateBox::StateBox(ImageScene *_scene):
        _outterborderColor(Qt::black),
        _outterborderPen(),
        _location(0,0),
        _dragStart(0,0),
        _gridSpace(10),
        _cornerDragStart(0,0),
        _XcornerGrabBuffer(0),
        _YcornerGrabBuffer(0),
        _drawingOrigenX(10),
        _drawingOrigenY(10),
        _mainScene(_scene)
{
    cerr << "ENTREE INIT STATEBOX" << endl;

    Data* d1 = new Data();
    Data* d2 = new Data();
    Data* d3 = new Data();
    Data* d4 = new Data();
    Data* d5 = new Data();
    Data* d6 = new Data();

    Point p11(4);
    p11[0] = 0;
    p11[1] = 10;
    p11[2] = 50;
    p11[3] = 90;
    d1->addData(p11);
    Point p12(4);
    p12[0] = 50;
    p12[1] = 90;
    p12[2] = 100;
    p12[3] = 60;
    d1->addData(p12);
    d1->setMean(Tools::averageMulDim(*d1));
    d1->setVar(Tools::varianceMulDim(*d1,d1->getMean()));
    for(int i = 0; i<1000; i++){
        d1->addSample(Tools::generateMulDim(d1->getMean(),d1->getVar()));
    }

    Point p21(4);
    p21[0] = 0;
    p21[1] = 100;
    p21[2] = 60;
    p21[3] = 120;
    d2->addData(p21);
    Point p22(4);
    p22[0] = 60;
    p22[1] = 120;
    p22[2] = 100;
    p22[3] = 60;
    d2->addData(p22);
    d2->setMean(Tools::averageMulDim(*d2));
    d2->setVar(Tools::varianceMulDim(*d2,d2->getMean()));
    for(int i = 0; i<1000; i++){
        d2->addSample(Tools::generateMulDim(d2->getMean(),d2->getVar()));
    }

    Point p31(4);
    p31[0] = 0;
    p31[1] = 200;
    p31[2] = 80;
    p31[3] = 230;
    d3->addData(p31);
    Point p32(4);
    p32[0] = 80;
    p32[1] = 230;
    p32[2] = 100;
    p32[3] = 60;
    d3->addData(p32);
    d3->setMean(Tools::averageMulDim(*d3));
    d3->setVar(Tools::varianceMulDim(*d3,d3->getMean()));
    for(int i = 0; i<1000; i++){
        d3->addSample(Tools::generateMulDim(d3->getMean(),d3->getVar()));
    }

    Point p41(4);
    p41[0] = 100;
    p41[1] = 60;
    p41[2] = 160;
    p41[3] = 60;
    d4->addData(p41);
    Point p42(4);
    p42[0] = 160;
    p42[1] = 60;
    p42[2] = 250;
    p42[3] = 300;
    d4->addData(p42);
    Point p43(4);
    p43[0] = 250;
    p43[1] = 300;
    p43[2] = 340;
    p43[3] = 100;
    d4->addData(p43);
    Point p44(4);
    p44[0] = 340;
    p44[1] = 100;
    p44[2] = 400;
    p44[3] = 60;
    d4->addData(p44);
    d4->setMean(Tools::averageMulDim(*d4));
    d4->setVar(Tools::varianceMulDim(*d4,d4->getMean()));
    for(int i = 0; i<1000; i++){
        d4->addSample(Tools::generateMulDim(d4->getMean(),d4->getVar()));
    }

    Point p51(4);
    p51[0] = 400;
    p51[1] = 60;
    p51[2] = 450;
    p51[3] = 10;
    d5->addData(p51);
    Point p52(4);
    p52[0] = 450;
    p52[1] = 10;
    p52[2] = 500;
    p52[3] = 50;
    d5->addData(p52);
    d5->setMean(Tools::averageMulDim(*d5));
    d5->setVar(Tools::varianceMulDim(*d5,d5->getMean()));
    for(int i = 0; i<1000; i++){
        d5->addSample(Tools::generateMulDim(d5->getMean(),d5->getVar()));
    }

    Point p61(4);
    p61[0] = 400;
    p61[1] = 60;
    p61[2] = 420;
    p61[3] = 230;
    d6->addData(p61);
    Point p62(4);
    p62[0] = 420;
    p62[1] = 230;
    p62[2] = 500;
    p62[3] = 180;
    d6->addData(p62);
    d6->setMean(Tools::averageMulDim(*d6));
    d6->setVar(Tools::varianceMulDim(*d6,d6->getMean()));
    for(int i = 0; i<1000; i++){
        d6->addSample(Tools::generateMulDim(d6->getMean(),d6->getVar()));
    }

    Tree<Data>* x = new Tree<Data>(d1);
    t.addChild(x);
    x = new Tree<Data>(d2);
    t.addChild(x);
    x = new Tree<Data>(d3);
    t.addChild(x);
    x = new Tree<Data>(d4);
    t[0]->addChild(x);
    t[1]->addChild(x);
    t[2]->addChild(x);
    x = new Tree<Data>(d5);
    (*t[0])[0]->addChild(x);
    x = new Tree<Data>(d6);
    (*t[0])[0]->addChild(x);

    _outterborderPen.setWidth(2);
    _outterborderPen.setColor(_outterborderColor);

    _img = new QImage(QSize(500,500),QImage::Format_RGB32);
    drawImagefromData(t,_img);

    _width = _img->width()+32;
    _height = _img->height()+32;

    _drawingWidth = _width -16 -   _XcornerGrabBuffer;
    _drawingHeight = _height -16 -   _XcornerGrabBuffer;

    this->setAcceptHoverEvents(true);
    cerr << "SORTIE INIT STATEBOX" << endl;
}

//assumes dest is already set to have the save size as source
static void QImagetoCML(QImage source, CML_color &dest)
{
  CML_RGBA p;
  for( int j=0; j<source.height(); j++ )
  {
    for( int i=0; i<source.width(); i++ )
    {
      p.red = qRed( source.pixel(i, j) );
      p.green = qGreen( source.pixel(i, j) );
      p.blue = qBlue( source.pixel(i, j) );
      p.alpha = qAlpha( source.pixel(i, j) );
      dest(i,j) = p;
    }
  }
}

static QImage CMLtoQImage(CML_color &source)
{
  CML_RGBA p;
  QImage newImg = QImage(source.Width(), source.Height(), QImage::Format_RGB32);
  for( int j=0; j<source.Height(); j++ )
  {
    for( int i=0; i<source.Width(); i++ )
    {
      p = source(i,j);
      newImg.setPixel(i, j, qRgba( p.red, p.green, p.blue, p.alpha ));
    }
  }
  return newImg;
}

bool updateCallbackStateBox(float percDone)
{
//  qApp->processEvents();
//  gProg->setValue((int)(percDone*100));
//  if(gProg->wasCanceled())
//    return false; //exit out
  return true;
}

/**
 *  To allow the user to grab the corners to re-size, we need to get a hover
 *  indication. But if the mouse pointer points to the left, then when the mouse
 *  tip is to the left but just outsize the box, we will not get the hover.
 *  So the solution is to tell the graphics scene the box is larger than
 *  what the painter actually paints in. This way when the user gets the mouse
 *  within a few pixels of what appears to be the edge of the box, we get
 *  the hover indication.

 *  So the cornerGrabBuffer is a few pixel wide buffer zone around the outside
 *  edge of the box.
 *
 */
void StateBox::adjustSize(int x, int y)
{
    int width = _img->width();
    int height = _img->height();
    _width += x;
    _height += y;
    int newWidth = width+x;
    int newHeight = height+y;

    _drawingWidth += x; // _width - _XcornerGrabBuffer;
    _drawingHeight += y; // _height - _YcornerGrabBuffer;

/************
 * **********
*/


//      int weight_scale = (int)(_resizeWidget.weightScaleLineEdit->text().toInt() * (_resizeWidget.brushWeightSlider->value() / 100.0));
      CAIR_convolution conv = V1;
//      switch( _resizeWidget.edgeDetector->currentIndex() )
//      {
//        case 0 : conv = V1; break;
//        case 1 : conv = V_SQUARE; break;
//        case 2 : conv = PREWITT; break;
//        case 3 : conv = SOBEL; break;
//        case 4 : conv = LAPLACIAN; break;
//      }

      CAIR_energy ener = BACKWARD;
//      if( _resizeWidget.energyCheckBox->isChecked() )
//      {
//        ener = FORWARD;
//      }

      if(newWidth < 1 || newHeight < 1)
      {
//        QMessageBox::information(this, tr("Seam Carving GUI"),
//                                 tr("Invalid dimensions."));
        return;
      }
      if(width == newWidth && height == newHeight)
        return;

      //int widthAdjustment = (width < newWidth ? newWidth - width : width - newWidth);
      //int heightAdjustment = (height < newHeight ? newHeight - height : height - newHeight);
//      QProgressDialog prog("Resizing...", "&Cancel", 0, 100, this);
//      gProg = &prog;
//      qApp->processEvents();

      //Transfer the image over to cair image format.
      CML_color source(width, height);
//      CML_color dest(1, 1);
      CML_int source_weights(width, height);
//      CML_int dest_weights(1,1);
      QImagetoCML(*_img,source);
//      QImage mskImg = _maskPix.toImage();
      for( int j=0; j<height; j++ )
      {
        for( int i=0; i<width; i++ )
        {
//          QRgb m = mskImg.pixel(i,j);
//          if(qGreen(m) > 0)
//            source_weights(i,j) = (int)(qGreen(m) * weight_scale);
//          else if(qRed(m) > 0)
//            source_weights(i,j) = (int)(qRed(m) * -weight_scale);
//          else
            source_weights(i,j) = 0;
        }
      }
      //Call CAIR
      CAIR_Data(&source, &source_weights, newWidth, conv, ener, t);
//      if( !_resizeWidget.hdCheckBox->isChecked() )
//      {
//        CAIR( &source, &source_weights, newWidth, newHeight, conv, ener, &dest_weights, &dest, updateCallbackStateBox );
        //      }
//      else
//      {
//        CAIR_HD( &source, &source_weights, newWidth, newHeight, conv, ener, &dest_weights, &dest, updateCallback );
//      }
//      if(prog.wasCanceled())
//        return;
      //QImage *newImg = new QImage(CMLtoQImage(dest));
//      saveInUndoStack();
      delete _img;
      _img = new QImage(QSize(newWidth,newHeight),QImage::Format_RGB32);
      _img->fill(0);
      drawImagefromData(t,_img);
//      _img = new QImage(CMLtoQImage(dest));
      //_imgItem->setPixmap(QPixmap::fromImage(*_img));
      update();
      //Set the weight mask to the now reduced size version shrunk by CAIR
//      QImage maskImg(_img.width(), _img.height(), QImage::Format_ARGB32);
//      maskImg.fill(qRgba(0,0,0,0));
//      for( int j=0; j<_img.height(); j++ )
//      {
//        for( int i=0; i<_img.width(); i++ )
//        {
//          if(dest_weights(i,j) > 0)
//            maskImg.setPixel(i, j, qRgba( 0, dest_weights(i,j) / weight_scale, 0, dest_weights(i,j) / weight_scale));
//          else if(dest_weights(i,j) < 0)
//            maskImg.setPixel(i, j, qRgba( dest_weights(i,j) / -weight_scale, 0, 0, dest_weights(i,j) / -weight_scale));
//        }
//      }
//      _maskPix = QPixmap::fromImage(maskImg);
//      _maskItem->setPixmap(_maskPix);
//      _scaleFactor = 1.0;
//      addToUndoStack(); //new image

/*********
 * *******
*/
}

/**
  * This scene event filter has been registered with all four corner grabber items.
  * When called, a pointer to the sending item is provided along with a generic
  * event.  A dynamic_cast is used to determine if the event type is one of the events
  * we are interrested in.
  */
bool StateBox::sceneEventFilter ( QGraphicsItem * watched, QEvent * event )
{
    //qDebug() << " QEvent == " + QString::number(event->type());

    CornerGrabber * corner = dynamic_cast<CornerGrabber *>(watched);
    if ( corner == NULL) return false; // not expected to get here

    QGraphicsSceneMouseEvent * mevent = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
    if ( mevent == NULL)
    {
        // this is not one of the mouse events we are interrested in
        return false;
    }


    switch (event->type() )
    {
            // if the mouse went down, record the x,y coords of the press, record it inside the corner object
        case QEvent::GraphicsSceneMousePress:
            {
                corner->setMouseState(CornerGrabber::kMouseDown);
                corner->mouseDownX = mevent->pos().x();
                corner->mouseDownY = mevent->pos().y();
            }
            break;

        case QEvent::GraphicsSceneMouseRelease:
            {
                corner->setMouseState(CornerGrabber::kMouseReleased);

            }
            break;

        case QEvent::GraphicsSceneMouseMove:
            {
                corner->setMouseState(CornerGrabber::kMouseMoving );
            }
            break;

        default:
            // we dont care about the rest of the events
            return false;
            break;
    }


    if ( corner->getMouseState() == CornerGrabber::kMouseMoving )
    {

        qreal x = mevent->pos().x(), y = mevent->pos().y();

        // depending on which corner has been grabbed, we want to move the position
        // of the item as it grows/shrinks accordingly. so we need to eitehr add
        // or subtract the offsets based on which corner this is.

        int XaxisSign = 0;
        int YaxisSign = 0;
        switch( corner->getCorner() )
        {
        case 0:
            {
                XaxisSign = +1;
                YaxisSign = +1;
            }
            break;

        case 1:
            {
                XaxisSign = -1;
                YaxisSign = +1;
            }
            break;

        case 2:
            {
                XaxisSign = -1;
                YaxisSign = -1;
            }
            break;

        case 3:
            {
                XaxisSign = +1;
                YaxisSign = -1;
            }
            break;

        case 4:
            {
                XaxisSign = +1;
                YaxisSign = 0;
            }
            break;

        case 5:
            {
                XaxisSign = -1;
                YaxisSign = 0;
            }
            break;

        case 6:
            {
                XaxisSign = 0;
                YaxisSign = +1;
            }
            break;

        case 7:
            {
                XaxisSign = 0;
                YaxisSign = -1;
            }
            break;

        }

        // if the mouse is being dragged, calculate a new size and also re-position
        // the box to give the appearance of dragging the corner out/in to resize the box

        int xMoved = corner->mouseDownX - x;
        int yMoved = corner->mouseDownY - y;

        int newWidth = _width + ( XaxisSign * xMoved);
        if ( newWidth < 40 ) newWidth  = 40;

        int newHeight = _height + (YaxisSign * yMoved) ;
        if ( newHeight < 40 ) newHeight = 40;

        int deltaWidth  =   newWidth - _width;
        int deltaHeight =   newHeight - _height;

        adjustSize(  deltaWidth ,   deltaHeight);

        deltaWidth *= (-1);
        deltaHeight *= (-1);

        if ( corner->getCorner() == 0 )
        {
            int newXpos = this->pos().x() + deltaWidth;
            int newYpos = this->pos().y() + deltaHeight;
            this->setPos(newXpos, newYpos);
        }
        else   if ( corner->getCorner() == 1 || corner->getCorner() == 6)
        {
            int newYpos = this->pos().y() + deltaHeight;
            this->setPos(this->pos().x(), newYpos);
        }
        else   if ( corner->getCorner() == 3 || corner->getCorner() == 4)
        {
            int newXpos = this->pos().x() + deltaWidth;
            this->setPos(newXpos,this->pos().y());
        }
        else   if ( corner->getCorner() == 4 )
        {
            int newXpos = this->pos().x() + deltaWidth;
            this->setPos(newXpos,this->pos().y());
        }
        setCornerPositions();

        this->update();
    }

    return true;// true => do not send event to watched - we are finished with this event
}



// for supporting moving the box across the scene
void StateBox::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);
    _location.setX( ( static_cast<int>(_location.x()) / _gridSpace) * _gridSpace );
    _location.setY( ( static_cast<int>(_location.y()) / _gridSpace) * _gridSpace );
    this->setPos(_location);
}


// for supporting moving the box across the scene
void StateBox::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);
    _dragStart = event->pos();
}


// for supporting moving the box across the scene
void StateBox::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
    QPointF newPos = event->pos() ;
    _location += (newPos - _dragStart);
    this->setPos(_location);
}

// remove the corner grabbers

void StateBox::hoverLeaveEvent ( QGraphicsSceneHoverEvent * )
{
    _outterborderColor = Qt::black;

    _corners[0]->setParentItem(NULL);
    _corners[1]->setParentItem(NULL);
    _corners[2]->setParentItem(NULL);
    _corners[3]->setParentItem(NULL);
    _corners[4]->setParentItem(NULL);
    _corners[5]->setParentItem(NULL);
    _corners[6]->setParentItem(NULL);
    _corners[7]->setParentItem(NULL);

    delete _corners[0];
    delete _corners[1];
    delete _corners[2];
    delete _corners[3];
    delete _corners[4];
    delete _corners[5];
    delete _corners[6];
    delete _corners[7];
}


// create the corner grabbers

void StateBox::hoverEnterEvent ( QGraphicsSceneHoverEvent * )
{
    _outterborderColor = Qt::red;

    _corners[0] = new CornerGrabber(this,0);
    _corners[1] = new CornerGrabber(this,1);
    _corners[2] = new CornerGrabber(this,2);
    _corners[3] = new CornerGrabber(this,3);
    _corners[4] = new CornerGrabber(this,4);
    _corners[5] = new CornerGrabber(this,5);
    _corners[6] = new CornerGrabber(this,6);
    _corners[7] = new CornerGrabber(this,7);


    _corners[0]->installSceneEventFilter(this);
    _corners[1]->installSceneEventFilter(this);
    _corners[2]->installSceneEventFilter(this);
    _corners[3]->installSceneEventFilter(this);
    _corners[4]->installSceneEventFilter(this);
    _corners[5]->installSceneEventFilter(this);
    _corners[6]->installSceneEventFilter(this);
    _corners[7]->installSceneEventFilter(this);


    setCornerPositions();

}

void StateBox::setCornerPositions()
{
    _corners[0]->setPos(_drawingOrigenX, _drawingOrigenY);
    _corners[1]->setPos(_drawingWidth,  _drawingOrigenY);
    _corners[2]->setPos(_drawingWidth , _drawingHeight);
    _corners[3]->setPos(_drawingOrigenX,_drawingHeight);
    _corners[4]->setPos(_drawingOrigenX, _drawingOrigenY+((_drawingOrigenY+_drawingHeight)/2-3));
    _corners[5]->setPos(_drawingWidth, _drawingOrigenY+((_drawingOrigenY+_drawingHeight)/2)-3);
    _corners[6]->setPos(_drawingOrigenX+((_drawingOrigenX+_drawingWidth)/2)-3 , _drawingOrigenY);
    _corners[7]->setPos(_drawingOrigenX+((_drawingOrigenX+_drawingWidth)/2)-3,_drawingHeight);
}

QRectF StateBox::boundingRect() const
{
    return QRectF(0,0,_width,_height);
}


// example of a drop shadow effect on a box, using QLinearGradient and two boxes

void StateBox::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    /*
    if(_img != 0){
    */
        QRectF target(16.0, 16.0, _img->width(), _img->height());
        QRectF source(0.0, 0.0, _img->width(), _img->height());


        painter->drawImage(target, *_img, source);
      /*
    }
    */
//    for(int i = 0; i<points.getDataSize(); i++){
//        painter->drawLine(points[i][0],points[i][1],points[i][2],points[i][3]);
//        painter->drawLine(test,test,test,test);
//    }
    _mainScene->update();
}


void StateBox::mouseMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}

void StateBox::mousePressEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}

