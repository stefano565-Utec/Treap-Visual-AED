#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <map>
#include "treap.h"
#include "visualnode.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onInsertClicked();
    void onDeleteClicked();
    void onSearchClicked();

    void onSplitClicked();
    void onJoinClicked();
    void onDeleteTreapClicked();
    void onCreateTreapClicked();

    void onNodeVisualClicked(QString ownerName);

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;

    std::map<QString, Treap<int>*> treaps;
    QString selectedTree1;
    QString selectedTree2;

    std::map<TreapNode<int>*, VisualNode*> visualMap;
    QList<QGraphicsItem*> tempItems; // Lista genérica para textos y líneas

    int calculateSubtreeWidth(TreapNode<int>* node);
    void updateVisualization();
    void recursivePosCalculation(TreapNode<int>* node, int x, int y, int width, QString owner, QColor color, std::map<TreapNode<int>*, QPointF>& targetPositions);
    void updateStatus();

    QString generateUniqueName(QString base);
};

#endif // MAINWINDOW_H
