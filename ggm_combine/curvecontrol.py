from __future__ import division, print_function

import itertools
import qt
import numpy as N

class MarkerItem(qt.QGraphicsEllipseItem):
    def __init__(self, parent, x, y):
        qt.QGraphicsEllipseItem.__init__(self, qt.QRectF(-5, -5, 10, 10))
        self.setPos(x, y)
        self.setFlag(qt.QGraphicsItem.ItemIsMovable)
        self.parent = parent

    def mouseReleaseEvent(self, event):
        qt.QGraphicsEllipseItem.mouseReleaseEvent(self, event)
        self.parent.updatePosns()

    def posn(self):
        return self.pos().x(), self.pos().y()

height = 80
width = 400

class CurveScene(qt.QGraphicsScene):

    changed = qt.pyqtSignal(object)

    def __init__(self, vals, *args):
        qt.QGraphicsScene.__init__(self)

        self.pathitem = qt.QGraphicsPathItem()
        self.addItem(self.pathitem)
        self.pathitem.setZValue(-1)

        self.ellipses = []

        #for i in xrange(10):
        #    ell = MarkerItem(self, i*40, 40)
        #    self.addItem(ell)
        #    self.ellipses.append(ell)

        maxx = N.max(vals[0])
        maxy = N.max(vals[1])
        self.maxx = maxx

        for x, y in zip(*vals):
            e = MarkerItem(self, N.sqrt(x/maxx)*width, height-y/maxy*height)
            self.addItem(e)
            self.ellipses.append(e)

        self.addLine( qt.QLineF(qt.QPointF(0,0), qt.QPointF(width, 0)) )
        self.addLine( qt.QLineF(qt.QPointF(0,height), qt.QPointF(width,height)) )

        self.updatePosns()

    def updatePosns(self):
        xvals = []
        yvals = []

        for e in self.ellipses:
            x, y = e.posn()
            xvals.append( (x/width)**2*self.maxx )
            yvals.append( (height-y)/height )

        xvals = N.array(xvals)
        yvals = N.array(yvals)

        # draw line
        xpts = N.arange(0, width)
        xv = (xpts/width)**2*self.maxx
        yv = N.interp(xv, xvals, yvals)
        ypts = height-yv*height

        path = qt.QPainterPath()
        path.moveTo(qt.QPointF(xpts[0], ypts[0]))
        for x, y in zip(xpts[1:], ypts[1:]):
            path.lineTo(qt.QPointF(x,y))
        self.pathitem.setPath(path)

        # send results to main program
        self.changed.emit((xvals, yvals))

class CurveView(qt.QGraphicsView):

    changed = qt.pyqtSignal(object)

    def __init__(self, vals, *args):
        qt.QGraphicsView.__init__(self, *args)
        self.scene = CurveScene(vals, self)
        self.setScene(self.scene)

        self.scene.changed.connect(self.changed)
