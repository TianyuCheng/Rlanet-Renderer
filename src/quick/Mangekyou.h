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

public slots:
	void get_thread_ready();

protected:
	QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);

private:
	RenderThread* thread_;
};

#endif
