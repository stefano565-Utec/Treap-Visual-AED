#ifndef VISUALNODE_H
#define VISUALNODE_H

#include <QObject>
#include <QGraphicsObject>
#include <QGraphicsTextItem>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QPainter>
#include <QPropertyAnimation>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>

// --- NODO VISUAL ---
class VisualNode : public QGraphicsObject {
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
    int key;
    int priority;
    QString ownerName;
    QColor mainColor;
    bool isSelected;

    VisualNode(int k, int p, QString owner, QColor c, QGraphicsItem* parent = nullptr)
        : QGraphicsObject(parent), key(k), priority(p), ownerName(owner), mainColor(c), isSelected(false) {
        setZValue(10);
        // Cambiar el cursor para indicar que es clickeable
        setCursor(Qt::PointingHandCursor);
    }

    QRectF boundingRect() const override {
        return QRectF(-30, -45, 60, 70);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(option); Q_UNUSED(widget);
        painter->setRenderHint(QPainter::Antialiasing);

        if (isSelected) {
            painter->setBrush(Qt::NoBrush);
            painter->setPen(QPen(Qt::red, 3));
            painter->drawEllipse(-23, -23, 46, 46);
        }

        painter->setBrush(mainColor);
        painter->setPen(QPen(Qt::black, 2));
        painter->drawEllipse(-20, -20, 40, 40);

        QFont font = painter->font();
        font.setBold(true);
        font.setPointSize(10);
        painter->setFont(font);
        painter->setPen(QPen(Qt::black));
        painter->drawText(QRectF(-20, -20, 40, 40), Qt::AlignCenter, QString::number(key));

        font.setBold(false);
        font.setPointSize(8);
        painter->setFont(font);
        painter->setPen(QPen(Qt::darkGray));
        painter->drawText(QRectF(-30, -45, 60, 20), Qt::AlignCenter, QString("p:%1").arg(priority));
    }

    void setColor(QColor c) {
        if (mainColor != c) { mainColor = c; update(); }
    }

    void setSelected(bool sel) {
        if (isSelected != sel) { isSelected = sel; update(); }
    }

    void animateTo(QPointF endPos) {
        if (pos() == endPos) return;
        QPropertyAnimation* anim = new QPropertyAnimation(this, "pos");
        anim->setDuration(1000);
        anim->setStartValue(pos());
        anim->setEndValue(endPos);
        anim->setEasingCurve(QEasingCurve::OutQuad);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }

signals:
    void nodeClicked(QString ownerName);

protected:
    // --- CORRECCIÓN AQUÍ ---
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override {
        // 1. Aceptamos el evento PRIMERO.
        event->accept();

        // 2. Guardamos el nombre en una variable local por seguridad
        QString safeName = ownerName;

        // 3. Emitimos la señal AL FINAL.
        // Esto provocará que MainWindow actualice la vista y posiblemente elimine este objeto 'this'.
        // Por eso, no debemos escribir nada después de esta línea que use 'this'.
        emit nodeClicked(safeName);

        // ELIMINADO: QGraphicsObject::mousePressEvent(event);
        // Esa línea causaba el crash al intentar acceder a la clase base de un objeto borrado.
    }
};

// --- ETIQUETA CLICKEABLE ---
class ClickableTreeLabel : public QGraphicsTextItem {
    Q_OBJECT
public:
    QString treeName;
    ClickableTreeLabel(QString name, bool isSelected, bool isEmpty, QGraphicsItem* parent = nullptr)
        : QGraphicsTextItem(parent), treeName(name) {

        if (isEmpty) setPlainText("[" + name + ": Vacio]");
        else setPlainText(name);

        setDefaultTextColor(isSelected ? Qt::blue : Qt::black);
        QFont f = font();
        f.setBold(true);
        f.setPointSize(12);
        setFont(f);
        setCursor(Qt::PointingHandCursor);
        setZValue(5);
    }
signals:
    void labelClicked(QString name);
protected:
    // --- CORRECCIÓN AQUÍ TAMBIÉN ---
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override {
        // 1. Aceptar evento primero
        event->accept();

        // 2. Copia de seguridad
        QString safeName = treeName;

        // 3. Emitir señal (Autodestrucción inminente)
        emit labelClicked(safeName);

        // No hacer nada más aquí
    }
};

#endif // VISUALNODE_H
