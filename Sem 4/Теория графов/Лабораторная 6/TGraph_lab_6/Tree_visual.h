#include <QGraphicsView>
#include <QWheelEvent>
#pragma once


class Tree_view : public QGraphicsView {
public:
	Tree_view(QWidget* parent = nullptr): QGraphicsView(parent){}
	void wheelEvent(QWheelEvent* event) {
		// Масштабирование при прокрутке колеса мыши
		if (event->modifiers() & Qt::ControlModifier) {  // Удерживайте Ctrl для масштабирования
			double scaleFactor = 1.15;
			if (event->angleDelta().y() < 0) {
				scaleFactor = 1.0 / scaleFactor;  // Уменьшение масштаба
			}
			scale(scaleFactor, scaleFactor);
		}
		else {
			QGraphicsView::wheelEvent(event);  // Стандартная прокрутка
		}
	}
};