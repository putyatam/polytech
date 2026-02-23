#include <string>
#include <QString>
#include <vector>
#include <iostream>
#include <fstream>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QQueue>
#pragma once
using namespace std;

struct Node {
    QString word;
    bool is_black;
    Node* left, * right, * parent;

    Node(QString word) : word(word), is_black(false), left(nullptr), right(nullptr), parent(nullptr) {}
};

class dict_rb_tree {
private:
    Node* root;
    Node* nil;

    void rotateLeft(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left != nil)
            y->left->parent = x;
        y->parent = x->parent;
        if (x->parent == nil)
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void rotateRight(Node* x) {
        Node* y = x->left;
        x->left = y->right;
        if (y->right != nil)
            y->right->parent = x;
        y->parent = x->parent;
        if (x->parent == nil)
            root = y;
        else if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
        y->right = x;
        x->parent = y;
    }

    void fixInsert(Node* z) {
        while (z->parent->is_black == false) {
            if (z->parent == z->parent->parent->left) {
                Node* y = z->parent->parent->right;
                if (y->is_black == false) {
                    z->parent->is_black = true;
                    y->is_black = true;
                    z->parent->parent->is_black = false;
                    z = z->parent->parent;
                }
                else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        rotateLeft(z);
                    }
                    z->parent->is_black = true;
                    z->parent->parent->is_black = false;
                    rotateRight(z->parent->parent);
                }
            }
            else {
                Node* y = z->parent->parent->left;
                if (y->is_black == false) {
                    z->parent->is_black = true;
                    y->is_black = true;
                    z->parent->parent->is_black = false;
                    z = z->parent->parent;
                }
                else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rotateRight(z);
                    }
                    z->parent->is_black = true;
                    z->parent->parent->is_black = false;
                    rotateLeft(z->parent->parent);
                }
            }
        }
        root->is_black = true;
    }

    void transplant(Node* u, Node* v) {
        if (u->parent == nil)
            root = v;
        else if (u == u->parent->left)
            u->parent->left = v;
        else
            u->parent->right = v;
        v->parent = u->parent;
    }

    Node* minimum(Node* x) {
        while (x->left != nil)
            x = x->left;
        return x;
    }
    void fixDelete(Node* x) {
        while (x != root && x->is_black == true) {
            if (x == x->parent->left) {
                Node* w = x->parent->right;
                if (w->is_black == false) {
                    w->is_black = true;
                    x->parent->is_black = false;
                    rotateLeft(x->parent);
                    w = x->parent->right;
                }
                if (w->left->is_black == true && w->right->is_black == true) {
                    w->is_black = false;
                    x = x->parent;
                }
                else {
                    if (w->right->is_black == true) {
                        w->left->is_black = true;
                        w->is_black = false;
                        rotateRight(w);
                        w = x->parent->right;
                    }
                    w->is_black = x->parent->is_black;
                    x->parent->is_black = true;
                    w->right->is_black = true;
                    rotateLeft(x->parent);
                    x = root;
                }
            }
            else {
                Node* w = x->parent->left;
                if (w->is_black == false) {
                    w->is_black = true;
                    x->parent->is_black = false;
                    rotateRight(x->parent);
                    w = x->parent->left;
                }
                if (w->right->is_black == true && w->left->is_black == true) {
                    w->is_black = false;
                    x = x->parent;
                }
                else {
                    if (w->left->is_black == true) {
                        w->right->is_black = true;
                        w->is_black = false;
                        rotateLeft(w);
                        w = x->parent->left;
                    }
                    w->is_black = x->parent->is_black;
                    x->parent->is_black = true;
                    w->left->is_black = true;
                    rotateRight(x->parent);
                    x = root;
                }
            }
        }
        x->is_black = true;
    }

    void clearHelper(Node* node) {
        if (node != nil) {
            clearHelper(node->left);
            clearHelper(node->right);
            delete node;
        }
    }

public:
    dict_rb_tree() {
        nil = new Node("");
        nil->is_black = true;
        root = nil;
    }

    ~dict_rb_tree() {
        clear();
        delete nil;
    }
    Node* get_nil() {
        return nil;
    }
    Node* get_root() {
        return root;
    }

    void add(const QString& word) {
        Node* z = new Node(word);
        Node* y = nil;
        Node* x = root;

        while (x != nil) {
            y = x;
            if (z->word < x->word)
                x = x->left;
            else if (z->word > x->word)
                x = x->right;
            else {
                delete z;
                return;
            }
        }

        z->parent = y;
        if (y == nil)
            root = z;
        else if (z->word < y->word)
            y->left = z;
        else
            y->right = z;

        z->left = nil;
        z->right = nil;
        fixInsert(z);
    }

    bool remove(const QString& word) {
        Node* z = root;
        while (z != nil) {
            if (word == z->word) break;
            z = (word < z->word) ? z->left : z->right;
        }
        if (z == nil) return false;

        Node* y = z;
        Node* x;
        bool y_original_color = y->is_black;

        if (z->left == nil) {
            x = z->right;
            transplant(z, z->right);
        }
        else if (z->right == nil) {
            x = z->left;
            transplant(z, z->left);
        }
        else {
            y = minimum(z->right);
            y_original_color = y->is_black;
            x = y->right;
            if (y->parent == z)
                x->parent = y;
            else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->is_black = z->is_black;
        }

        delete z;
        if (y_original_color == true)
            fixDelete(x);
        return true;
    }
    bool find(const QString& word) const {
        Node* current = root;
        while (current != nil) {
            if (word == current->word)
                return true;
            current = (word < current->word) ? current->left : current->right;
        }
        return false;
    }

    void clear() {
        clearHelper(root);
        root = nil;
    }

    void loadFromFile(const string& filename) {
        clear();
        ifstream file(filename);
        string word;
        while (getline(file, word))
            add(QString::fromStdString(word));
    }

    void from_string(QString qtext) {
        string text = qtext.toStdString();
        vector<string> list_text;
        string str;
        for (int i = 0; i < text.size(); i++) {
            char c = text[i];
            if (c != ' ') {
                str += c;
                if (i == text.size() - 1) {
                    add(QString::fromStdString(str));
                }
            }
            else {
                if (!str.empty()) {
                    add(QString::fromStdString(str));
                    str.clear();
                }

            }
        }
    }



    void calculatePositions(Node* node, qreal x, qreal y,
        qreal hStep, int level, QMap<Node*, QPointF>& positions) {
        if (node == nil) return;
        positions[node] = QPointF(x, y);
        qreal childHStep = hStep * 0.5;
        calculatePositions(node->left, x - childHStep, y + 80,
            childHStep, level + 1, positions);
        calculatePositions(node->right, x + childHStep, y + 80,
            childHStep, level + 1, positions);
    }

    void draw(QGraphicsScene* scene) {
        scene->clear();
        if (root == nil) return;

        QMap<Node*, QPointF> positions;
        QMap<Node*, QVector<Node*>> children;

        buildChildrenStructure(root, children);

        int xCounter = 0;
        calculateNodePositions(root, children, positions, 0, xCounter);

        normalizePositions(positions);

        drawNodesAndEdges(scene, positions);
    }

private:
    void buildChildrenStructure(Node* node, QMap<Node*, QVector<Node*>>& children) {
        if (node == nil) return;

        QVector<Node*> nodeChildren;
        if (node->left != nil) nodeChildren.append(node->left);
        if (node->right != nil) nodeChildren.append(node->right);

        children[node] = nodeChildren;

        buildChildrenStructure(node->left, children);
        buildChildrenStructure(node->right, children);
    }

    void calculateNodePositions(Node* node,
        QMap<Node*, QVector<Node*>>& children,
        QMap<Node*, QPointF>& positions,
        int depth,
        int& xCounter) {
        if (node == nil) return;

        for (Node* child : children[node]) {
            calculateNodePositions(child, children, positions, depth + 1, xCounter);
        }

        if (children[node].isEmpty()) {
            positions[node] = QPointF(xCounter++, depth);
        }
        else {
            double minX = positions[children[node].first()].x();
            double maxX = positions[children[node].last()].x();
            positions[node] = QPointF((minX + maxX) / 2.0, depth);
        }
    }

    void normalizePositions(QMap<Node*, QPointF>& positions) {
        double minX = std::numeric_limits<double>::max();
        double minY = std::numeric_limits<double>::max();
        for (const QPointF& pos : positions) {
            minX = std::min(minX, pos.x());
            minY = std::min(minY, pos.y());
        }

        const double X_SPACING = 150;
        const double Y_SPACING = 150;

        for (Node* node : positions.keys()) {
            QPointF pos = positions[node];
            pos.setX((pos.x() - minX) * X_SPACING);
            pos.setY((pos.y() - minY) * Y_SPACING);
            positions[node] = pos;
        }
    }

    void drawNodesAndEdges(QGraphicsScene* scene, QMap<Node*, QPointF>& positions) {
        const double NODE_WIDTH = 100;
        const double NODE_HEIGHT = 60;

        for (Node* node : positions.keys()) {
            if (node->left != nil) {
                QPointF start = positions[node];
                QPointF end = positions[node->left];
                scene->addLine(start.x(), start.y() + NODE_HEIGHT / 2,
                    end.x(), end.y() - NODE_HEIGHT / 2, QPen(Qt::black));
            }
            if (node->right != nil) {
                QPointF start = positions[node];
                QPointF end = positions[node->right];
                scene->addLine(start.x(), start.y() + NODE_HEIGHT / 2,
                    end.x(), end.y() - NODE_HEIGHT / 2, QPen(Qt::black));
            }
        }

        for (Node* node : positions.keys()) {
            QPointF pos = positions[node];

            QGraphicsEllipseItem* ellipse = scene->addEllipse(
                pos.x() - NODE_WIDTH / 2,
                pos.y() - NODE_HEIGHT / 2,
                NODE_WIDTH,
                NODE_HEIGHT,
                QPen(Qt::black),
                node->is_black ? QBrush(Qt::black) : QBrush(Qt::red)
            );
            ellipse->setZValue(2);
            if (node->left == nil) {
                QGraphicsRectItem* rect_left = scene->addRect(
                    pos.x() - NODE_WIDTH/3 - NODE_WIDTH / 6,
                    pos.y() + 100 - NODE_HEIGHT / 2,
                    NODE_WIDTH / 3,
                    NODE_HEIGHT / 3,
                    QPen(Qt::black),
                    QBrush(Qt::black)
                );
                rect_left->setZValue(2);

                QGraphicsTextItem* text_nil_left = scene->addText("NIL");
                text_nil_left->setPos(
                    pos.x() - NODE_WIDTH / 3 - NODE_WIDTH / 6,
                    pos.y() + 100 - NODE_HEIGHT / 2
                );
                text_nil_left->setDefaultTextColor(Qt::white);
                text_nil_left->setZValue(3);
                scene->addLine(pos.x(), pos.y() + NODE_HEIGHT / 2, pos.x() - NODE_WIDTH / 3, pos.y() + 100 - NODE_HEIGHT / 2);
            }
            if (node->right == nil) {
                QGraphicsRectItem* rect_left = scene->addRect(
                    pos.x() + NODE_WIDTH / 3 - NODE_WIDTH / 6,
                    pos.y() + 100 - NODE_HEIGHT / 2,
                    NODE_WIDTH / 3,
                    NODE_HEIGHT / 3,
                    QPen(Qt::black),
                    QBrush(Qt::black)
                );
                rect_left->setZValue(2);

                QGraphicsTextItem* text_nil_left = scene->addText("NIL");
                text_nil_left->setPos(
                    pos.x() + NODE_WIDTH / 3 - NODE_WIDTH / 6,
                    pos.y() + 100 - NODE_HEIGHT / 2
                );
                text_nil_left->setDefaultTextColor(Qt::white);
                text_nil_left->setZValue(3);
                scene->addLine(pos.x(), pos.y() + NODE_HEIGHT / 2, pos.x() + NODE_WIDTH / 3, pos.y() + 100 - NODE_HEIGHT / 2);
            }

            QGraphicsTextItem* text = scene->addText(node->word.left(10));
            text->setPos(
                pos.x() - text->boundingRect().width() / 2,
                pos.y() - text->boundingRect().height() / 2
            );
            text->setDefaultTextColor(node->is_black ? Qt::white : Qt::black);
            text->setZValue(3);
        }
    }
};