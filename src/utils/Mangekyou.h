#ifndef MANGEKYOU_H
#define MANGEKYOU_H

#include <memory>
#include <QQuickItem>

class RenderThread;

class Mangekyou : public QQuickItem {
	Q_OBJECT;
public:
	Mangekyou();
	~Mangekyou();

protected:
	QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);

private:
	std::unique_ptr<RenderThread> thread_;
};

#endif
