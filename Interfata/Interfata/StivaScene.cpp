#include <QtWidgets>

class StivaScene : public QGraphicsScene {
public:
    StivaScene(QObject* parent = nullptr) : QGraphicsScene(parent) {
    }

    void push(int value) {
        QGraphicsRectItem* rectItem = new QGraphicsRectItem(0, 0, 50, 30);
        rectItem->setPos(10, 30 * stack.size());
        addItem(rectItem);

        QGraphicsTextItem* textItem = new QGraphicsTextItem(QString::number(value));
        textItem->setPos(25, 30 * stack.size());
        addItem(textItem);

        stack.push(value);
    }

    void pop() {
        if (!stack.isEmpty()) {
            QGraphicsItem* item = items().last();
            removeItem(item);
            delete item;

            stack.pop();
        }
    }

private:
    QStack<int> stack;
};



