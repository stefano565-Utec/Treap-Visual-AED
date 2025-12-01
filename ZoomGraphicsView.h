#ifndef ZOOMGRAPHICSVIEW_H
#define ZOOMGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QScrollBar>

class ZoomGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ZoomGraphicsView(QWidget* parent = nullptr) : QGraphicsView(parent) {
        setRenderHint(QPainter::Antialiasing);
        setRenderHint(QPainter::SmoothPixmapTransform);

        // Configuración para mapa infinito
        setDragMode(QGraphicsView::ScrollHandDrag);
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        setResizeAnchor(QGraphicsView::AnchorUnderMouse);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

protected:
    void wheelEvent(QWheelEvent* event) override {
        double scaleFactor = 1.1;
        if (event->angleDelta().y() > 0)
            scale(scaleFactor, scaleFactor);
        else
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
};

#endif
