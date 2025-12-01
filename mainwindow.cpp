#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <cmath>
#include <QGraphicsTextItem>
#include <limits> // Necesario para min/max

// --- IMPLEMENTACIÓN MAINWINDOW ---

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , scene(new QGraphicsScene(this))
{
    ui->setupUi(this);
    ui->graphicsView->setScene(scene);

    // Mapa infinito
    scene->setSceneRect(-5000, -5000, 10000, 10000);

    treaps["Main"] = new Treap<int>();
    selectedTree1 = "Main";

    // Conexiones
    connect(ui->insertButton, &QPushButton::clicked, this, &MainWindow::onInsertClicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);
    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::onSearchClicked);
    connect(ui->splitButton, &QPushButton::clicked, this, &MainWindow::onSplitClicked);
    connect(ui->joinButton, &QPushButton::clicked, this, &MainWindow::onJoinClicked);

    connect(ui->createTreapButton, &QPushButton::clicked, this, &MainWindow::onCreateTreapClicked);
    connect(ui->deleteTreapButton, &QPushButton::clicked, this, &MainWindow::onDeleteTreapClicked);

    updateStatus();
    updateVisualization();
}

MainWindow::~MainWindow() {
    for(auto const& [name, t] : treaps) delete t;
    delete ui;
}

int MainWindow::calculateSubtreeWidth(TreapNode<int>* node) {
    if (!node) return 0;
    // Esto es "peso" lógico, no píxeles. Sirve para la separación interna.
    return 1 + calculateSubtreeWidth(node->left) + calculateSubtreeWidth(node->right);
}

QString MainWindow::generateUniqueName(QString base) {
    if (treaps.find(base) == treaps.end()) return base;
    int i = 1;
    while (treaps.find(base + "_" + QString::number(i)) != treaps.end()) i++;
    return base + "_" + QString::number(i);
}

// --- VISUALIZACIÓN ---

// Calcula posiciones RELATIVAS al centro (0,0) del árbol
void MainWindow::recursivePosCalculation(TreapNode<int>* node, int x, int y, int width, QString owner, QColor color, std::map<TreapNode<int>*, QPointF>& targetPositions) {
    if (!node) return;
    targetPositions[node] = QPointF(x, y);

    int offset = width * 25;
    if (width < 2) offset = 50;

    int leftW = calculateSubtreeWidth(node->left);
    int rightW = calculateSubtreeWidth(node->right);

    recursivePosCalculation(node->left, x - offset, y + 80, leftW, owner, color, targetPositions);
    recursivePosCalculation(node->right, x + offset, y + 80, rightW, owner, color, targetPositions);
}

void MainWindow::updateVisualization() {
    // 1. Limpiar elementos temporales
    for (auto item : tempItems) {
        scene->removeItem(item);
        delete item;
    }
    tempItems.clear();

    std::map<TreapNode<int>*, QPointF> allNodesPositions;
    int currentX = 100; // Donde empieza a dibujarse el primer árbol

    // Recorremos cada árbol
    for (auto const& [name, tree] : treaps) {

        bool isSel = (name == selectedTree1 || name == selectedTree2);

        // CASO 1: ÁRBOL VACÍO
        if (tree->empty()) {
            ClickableTreeLabel* placeholder = new ClickableTreeLabel(name, isSel, true);
            placeholder->setPos(currentX, 50);
            connect(placeholder, &ClickableTreeLabel::labelClicked, this, &MainWindow::onNodeVisualClicked);

            scene->addItem(placeholder);
            tempItems.push_back(placeholder);

            currentX += 200; // Espacio fijo para el placeholder
            continue;
        }

        // CASO 2: ÁRBOL LLENO - CALCULO INTELIGENTE DE ANCHO

        // A. Calculamos posiciones "locales" asumiendo que la raíz está en X=0
        std::map<TreapNode<int>*, QPointF> localPositions;
        int w = calculateSubtreeWidth(tree->getRoot());
        QColor c = QColor(255, 255, 160);
        if (isSel) c = Qt::cyan;

        recursivePosCalculation(tree->getRoot(), 0, 60, w, name, c, localPositions);

        // B. Encontramos los límites reales (Bounding Box) de este árbol
        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();

        for (auto const& [node, pos] : localPositions) {
            if (pos.x() < minX) minX = pos.x();
            if (pos.x() > maxX) maxX = pos.x();
        }

        // C. Calculamos cuánto hay que mover para que no se solape
        // Queremos que el borde izquierdo (minX) empiece en currentX
        // Shift = currentX - minX
        float shiftX = currentX - minX;

        // D. Guardamos las posiciones finales ajustadas
        for (auto const& [node, pos] : localPositions) {
            QPointF finalPos(pos.x() + shiftX, pos.y());
            allNodesPositions[node] = finalPos;
        }

        // E. Dibujar el nombre del árbol centrado en su espacio real
        float treeRealWidth = maxX - minX;
        float centerOfTree = currentX + (treeRealWidth / 2.0);

        ClickableTreeLabel* lbl = new ClickableTreeLabel(name, isSel, false);
        qreal lblWidth = lbl->boundingRect().width();
        lbl->setPos(centerOfTree - lblWidth/2, 0);
        connect(lbl, &ClickableTreeLabel::labelClicked, this, &MainWindow::onNodeVisualClicked);
        scene->addItem(lbl);
        tempItems.push_back(lbl);

        // F. Actualizar currentX para el siguiente árbol (+ margen de 100px)
        currentX += treeRealWidth + 150;
    }

    // --- AHORA DIBUJAMOS LOS NODOS USANDO LAS POSICIONES CALCULADAS ---
    for (auto const& [logicNode, pos] : allNodesPositions) {
        QString owner = "";
        for(auto const& [n, t] : treaps) {
            if(t->search(logicNode->key)) { owner = n; break; }
        }

        QColor c = QColor(255, 255, 160);
        bool isSel = false;
        if (owner == selectedTree1 || owner == selectedTree2) {
            c = Qt::cyan;
            isSel = true;
        }

        if (visualMap.find(logicNode) == visualMap.end()) {
            // NUEVO
            VisualNode* v = new VisualNode(logicNode->key, logicNode->priority, owner, c);
            connect(v, &VisualNode::nodeClicked, this, &MainWindow::onNodeVisualClicked);
            scene->addItem(v);
            v->setPos(pos.x(), -100);
            v->animateTo(pos);
            v->setSelected(isSel);
            visualMap[logicNode] = v;
        } else {
            // EXISTENTE
            VisualNode* v = visualMap[logicNode];
            // Verificar integridad por si hubo reutilización de memoria
            if (v->key != logicNode->key || v->priority != logicNode->priority) {
                scene->removeItem(v); delete v;
                VisualNode* nv = new VisualNode(logicNode->key, logicNode->priority, owner, c);
                connect(nv, &VisualNode::nodeClicked, this, &MainWindow::onNodeVisualClicked);
                scene->addItem(nv);
                nv->setPos(pos);
                nv->setSelected(isSel);
                visualMap[logicNode] = nv;
            } else {
                v->ownerName = owner;
                v->setColor(c);
                v->setSelected(isSel);
                v->animateTo(pos);
            }
        }
    }

    // Limpieza de nodos que ya no existen
    auto it = visualMap.begin();
    while (it != visualMap.end()) {
        if (allNodesPositions.find(it->first) == allNodesPositions.end()) {
            scene->removeItem(it->second); delete it->second;
            it = visualMap.erase(it);
        } else ++it;
    }

    // Dibujar Flechas
    for (auto const& [logicNode, pos] : allNodesPositions) {
        if (logicNode->left && allNodesPositions.count(logicNode->left)) {
            QPointF p2 = allNodesPositions[logicNode->left];
            QGraphicsLineItem* li = scene->addLine(pos.x(), pos.y()+20, p2.x(), p2.y()-20, QPen(Qt::black, 2));
            li->setZValue(0); tempItems.push_back(li);
        }
        if (logicNode->right && allNodesPositions.count(logicNode->right)) {
            QPointF p2 = allNodesPositions[logicNode->right];
            QGraphicsLineItem* li = scene->addLine(pos.x(), pos.y()+20, p2.x(), p2.y()-20, QPen(Qt::black, 2));
            li->setZValue(0); tempItems.push_back(li);
        }
    }
}

// --- INTERACCIÓN ---

void MainWindow::onNodeVisualClicked(QString ownerName) {
    if (ownerName.isEmpty()) return;

    if (selectedTree1 == ownerName) {
        selectedTree1 = "";
        if (!selectedTree2.isEmpty()) { selectedTree1 = selectedTree2; selectedTree2 = ""; }
    }
    else if (selectedTree2 == ownerName) {
        selectedTree2 = "";
    }
    else {
        if (!selectedTree1.isEmpty()) selectedTree2 = selectedTree1;
        selectedTree1 = ownerName;
    }
    updateStatus();
    updateVisualization();
}

void MainWindow::updateStatus() {
    QString s = "Treaps: " + QString::number(treaps.size());
    if (!selectedTree1.isEmpty()) s += " | Sel 1: " + selectedTree1;
    if (!selectedTree2.isEmpty()) s += " | Sel 2: " + selectedTree2;
    ui->statusLabel->setText(s);
}

// --- OPERACIONES ---

void MainWindow::onInsertClicked() {
    if (selectedTree1.isEmpty()) { ui->statusLabel->setText("Selecciona un Treap."); return; }
    QString txt = ui->keyLineEdit->text();
    if (txt.isEmpty()) return;
    int val = txt.toInt();

    if (treaps[selectedTree1]->search(val)) {
        ui->statusLabel->setText("Clave ya existe en " + selectedTree1);
        return;
    }

    treaps[selectedTree1]->insert(val);

    updateVisualization();
    ui->keyLineEdit->clear();
    ui->keyLineEdit->setFocus();
}

void MainWindow::onDeleteClicked() {
    if (selectedTree1.isEmpty()) return;
    QString txt = ui->keyLineEdit->text();
    if (txt.isEmpty()) return;
    int val = txt.toInt();
    treaps[selectedTree1]->remove(val);
    updateVisualization();
    ui->keyLineEdit->clear(); ui->keyLineEdit->setFocus();
}

void MainWindow::onSearchClicked() {
    int val = ui->keyLineEdit->text().toInt();
    for (auto const& [name, t] : treaps) {
        if (t->search(val)) {
            selectedTree1 = name; selectedTree2 = "";
            updateStatus(); updateVisualization();
            ui->statusLabel->setText("Encontrado en: " + name);
            return;
        }
    }
    ui->statusLabel->setText("No encontrado.");
}

void MainWindow::onSplitClicked() {
    if (selectedTree1.isEmpty()) { ui->statusLabel->setText("Selecciona árbol."); return; }

    int key;
    QString splitInput = ui->splitKeyLineEdit->text();
    QString mainInput = ui->keyLineEdit->text();

    if (!splitInput.isEmpty()) key = splitInput.toInt();
    else if (!mainInput.isEmpty()) key = mainInput.toInt();
    else {
        ui->statusLabel->setText("ERROR: Ingresa un valor para el Split.");
        return;
    }

    QString nameL = generateUniqueName(selectedTree1 + "_L");
    QString nameR = generateUniqueName(selectedTree1 + "_R");

    Treap<int>* TL = new Treap<int>();
    Treap<int>* TR = new Treap<int>();

    try {
        treaps[selectedTree1]->split(key, *TL, *TR);
        delete treaps[selectedTree1]; treaps.erase(selectedTree1);

        treaps[nameL] = TL; treaps[nameR] = TR;
        selectedTree1 = nameL; selectedTree2 = nameR;

        updateStatus(); updateVisualization();
        ui->statusLabel->setText("Split OK en: " + QString::number(key));

    } catch (std::exception& e) {
        delete TL; delete TR;
        ui->statusLabel->setText("Error: " + QString(e.what()));
    }
}

void MainWindow::onJoinClicked() {
    if (selectedTree1.isEmpty() || selectedTree2.isEmpty()) {
        ui->statusLabel->setText("Selecciona 2 treaps."); return;
    }

    Treap<int>* T1 = treaps[selectedTree1];
    Treap<int>* T2 = treaps[selectedTree2];

    if (!T1->empty() && !T2->empty()) {
        if (T1->maxKey() > T2->minKey()) {
            std::swap(T1, T2);
            std::swap(selectedTree1, selectedTree2);
        }
    }

    QString newName = generateUniqueName("JoinResult");
    Treap<int>* TM = new Treap<int>();

    try {
        TM->join(*T1, *T2);

        delete treaps[selectedTree1]; treaps.erase(selectedTree1);
        delete treaps[selectedTree2]; treaps.erase(selectedTree2);

        treaps[newName] = TM;
        selectedTree1 = newName; selectedTree2 = "";

        updateStatus(); updateVisualization();
        ui->statusLabel->setText("Join OK.");

    } catch (std::exception& e) {
        delete TM;
        ui->statusLabel->setText("Error Join: " + QString(e.what()));
    }
}

void MainWindow::onCreateTreapClicked() {
    QString name = generateUniqueName("NewTree");
    treaps[name] = new Treap<int>();
    selectedTree1 = name; selectedTree2 = "";
    updateVisualization();
}

void MainWindow::onDeleteTreapClicked() {
    if (selectedTree1.isEmpty()) return;
    delete treaps[selectedTree1];
    treaps.erase(selectedTree1);
    selectedTree1 = "";
    updateStatus(); updateVisualization();
}
