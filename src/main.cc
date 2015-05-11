#include <stdint.h>
#include <Application.h>
#include <RenderManager.h>

#include <Scene.h>
#include <Terrain.h>
#include <Ocean.h>
#include <TextureSkyDome.h>
#include <Grass.h>
#include <Tree.h>

class PlanetRenderManager : public RenderManager {
	enum ACTION_BIT {
		_CAMERA_LEFT_BIT = 0,
		_CAMERA_RIGHT_BIT,
		_CAMERA_FORWARD_BIT,
		_CAMERA_BACKWARD_BIT,
		_CAMERA_PITCH_UP_BIT,
		_CAMERA_PITCH_DOWN_BIT,
	};
#define DEFINE_ACTION(A)	A = (1 << _##A##_BIT),

	enum : uint64_t {
		DEFINE_ACTION(CAMERA_LEFT)
			DEFINE_ACTION(CAMERA_RIGHT)
			DEFINE_ACTION(CAMERA_FORWARD)
			DEFINE_ACTION(CAMERA_BACKWARD)
			DEFINE_ACTION(CAMERA_PITCH_UP)
			DEFINE_ACTION(CAMERA_PITCH_DOWN)
	};
#undef DEFINE_ACTION
public:

	/**
	 * Notice: do not put your object instantiation code
	 * in the construction. This is due to the fact that
	 * when the constructor is called, the OpenGL context
	 * has not been created, therefore the instantiation
	 * will fail. Instead, please move those codes into 
	 * prepare method.
	 * */
	PlanetRenderManager() : resolution(1024, 768) {
		drawMode = GL_FILL;
		action_flag_ = 0;
	}

	virtual ~PlanetRenderManager() {

	}

	void keyPressed(int count, int key, int modifiers, QString text) {
		switch (key) {
			case Qt::Key_Left:
			case Qt::Key_A:
				action_flag_|= CAMERA_LEFT;
				break;
			case Qt::Key_Right:
			case Qt::Key_D:
				action_flag_|= CAMERA_RIGHT;
				break;
			case Qt::Key_Up:
			case Qt::Key_W:
				action_flag_|= CAMERA_FORWARD;
				break;
			case Qt::Key_Down:
			case Qt::Key_S:
				action_flag_|= CAMERA_BACKWARD;
				break;
			case Qt::Key_PageUp:
				action_flag_|= CAMERA_PITCH_UP;
				break;
			case Qt::Key_PageDown:
				action_flag_|= CAMERA_PITCH_DOWN;
				break;
			case Qt::Key_Space:
				if (drawMode == GL_FILL) { drawMode = GL_LINE; }
				else { drawMode = GL_FILL; }
				reflection_->setDrawMode(drawMode);
				finalPass_->setDrawMode(drawMode);
				break;
		}
	}

	void keyReleased(int count, int key, int modifiers, QString text)
	{
		switch (key) {
			case Qt::Key_Left:
			case Qt::Key_A:
				action_flag_ &= ~CAMERA_LEFT;
				break;
			case Qt::Key_Right:
			case Qt::Key_D:
				action_flag_ &= ~CAMERA_RIGHT;
				break;
			case Qt::Key_Up:
			case Qt::Key_W:
				action_flag_ &= ~CAMERA_FORWARD;
				break;
			case Qt::Key_Down:
			case Qt::Key_S:
				action_flag_ &= ~CAMERA_BACKWARD;
				break;
			case Qt::Key_PageUp:
				action_flag_ &= ~CAMERA_PITCH_UP;
				break;
			case Qt::Key_PageDown:
				action_flag_ &= ~CAMERA_PITCH_DOWN;
				break;
		}
	}

	void mouseDragStarted(int x, int y) {
		dragStartX = x;
		dragStartY = y;
	}

	void mouseDragging(int x, int y) {
		int diffX = x - dragStartX;
		int diffY = y - dragStartY;

		dragStartX = x;
		dragStartY = y;

		double angleX = 90 * diffX / (resolution.width() / 2.0);
		camera_->turnRight(angleX);

		double angleY = 90 * diffY / (resolution.height() / 2.0);
		camera_->lookUp(angleY);
	}

	void prepare() {
		int width = resolution.width();
		int height = resolution.height();

		// Create camera
		camera_.reset(new Camera("Viewer Camera"));
		camera_->setPerspective(45.0, (float)width/(float)height, 1.0, 10000.0);
		reflectCamera_.reset(new Camera("Reflection Camera"));
		reflectCamera_->setPerspective(75.0, 1.0, 1.0, 10000.0);

		// For terrain navigation
		camera_->lookAt(
				QVector3D(1000.0, 150.0, 1000.0),
				QVector3D(1000.0, 150.0, 1020.0),
				QVector3D(0.0, 1.0, 0.0)
			       );

		// Create two passes
		reflection_.reset(new Scene("reflection", resolution));

		finalPass_.reset(new Scene("final result", resolution));
		finalPass_->setCamera(camera_.get());

		// Instantiate scene objects
		skydome_.reset(new TextureSkyDome(64, finalPass_.get()));
		terrain_.reset(new Terrain(32, 5, finalPass_.get()));
		ocean_.reset(new Ocean(32, 5, finalPass_.get()));

		grassFactory_.reset(new GrassFactory(terrain_.get()));
		grass_.reset(grassFactory_->createGrass(QVector2D(1000, 1000), 1000.0, 40.0, 80.0, 40.0));

		treeFactory_.reset(new TreeFactory(terrain_.get()));
		tree1_.reset(treeFactory_->createTree(TreeType::PALM, QVector2D(2000, 2000), 1000.0, 200.0, 140.0, 200.0));
		tree2_.reset(treeFactory_->createTree(TreeType::TREE1, QVector2D(3100, 3100), 1000.0, 200.0, 140.0, 200.0));

		// Adding objects into reflection pass
		reflection_->addObject(skydome_.get());
		reflection_->addObject(terrain_.get());
		reflection_->addObject(grass_.get());
		reflection_->addObject(tree1_.get());
		reflection_->addObject(tree2_.get());

		// Adding objects into final pass
		finalPass_->addObject(skydome_.get());
		finalPass_->addObject(terrain_.get());
		finalPass_->addObject(ocean_.get());
		finalPass_->addObject(grass_.get());
		finalPass_->addObject(tree1_.get());
		finalPass_->addObject(tree2_.get());

		// setContextProperty("fps", "0");

		// OpenGL settings
		glEnable(GL_DEPTH_TEST);
	}

	void do_camera_move()
	{
		double interval = getInterval();

		if (action_flag_ & CAMERA_LEFT) {
			camera_->turnLeft(1);
		}
		if (action_flag_ & CAMERA_RIGHT) {
			camera_->turnRight(1);
		}
		if (action_flag_ & CAMERA_FORWARD) {
			camera_->moveForward(500 * interval);
		}
		if (action_flag_ & CAMERA_BACKWARD) {
			camera_->moveBackward(500 * interval);
		}
		if (action_flag_ & CAMERA_PITCH_UP) {
			camera_->lookUp(5);
		}
		if (action_flag_ & CAMERA_PITCH_DOWN) {
			camera_->lookDown(5);
		}

	}

	void render(QOpenGLFramebufferObject *fbo) {

		// qDebug() << fps();
		// setContextProperty("fps", QString::number(fps()));

		do_camera_move();

		terrain_->underWaterCulling(QVector4D(0.0, 1.0, 0.0, 0.0));
		if (camera_->getPosition().y() >= 0) {
			// reflection
			camera_->reflectCamera(QVector4D(0.0, 1.0, 0.0, 0.0), reflectCamera_.get());
			reflection_->setCamera(reflectCamera_.get());
		} else {
			// refraction
			reflection_->setCamera(camera_.get());
		}
		reflection_->renderScene();

		terrain_->disableUnderWaterCulling();
		finalPass_->useTexture(reflection_->texture());
		finalPass_->setCamera(camera_.get());
		finalPass_->renderScene(fbo);
	}

	void shutdown() {
		/*
 		 * Release GL resource here since we need GL context to do
		 * this.
		 */
		// Reset scenes
		reflection_.reset();
		finalPass_.reset();

		// Reset scene objects
		skydome_.reset();
		terrain_.reset();
		grassFactory_.reset();
		treeFactory_.reset();

		grass_.reset();
		tree1_.reset();
		tree2_.reset();

#if 0 // No, we don't need to release normal objects.
		camera_.reset();
		reflectCamera_.reset();
#endif
	}

private:
	QSize resolution;
	GLuint drawMode;

	// Render Targets
	std::unique_ptr<Scene> reflection_;
	// std::unique_ptr<Scene> shadow_;
	std::unique_ptr<Scene> finalPass_;

	// Scene Objects
	std::unique_ptr<Terrain> terrain_;
	std::unique_ptr<Ocean> ocean_;
	std::unique_ptr<TextureSkyDome> skydome_;
	std::unique_ptr<GrassFactory> grassFactory_;
	std::unique_ptr<Grass> grass_;
	std::unique_ptr<TreeFactory> treeFactory_;
	std::unique_ptr<Tree> tree1_;
	std::unique_ptr<Tree> tree2_;

	// Camera
	std::unique_ptr<Camera> camera_;
	std::unique_ptr<Camera> reflectCamera_;
	uint64_t action_flag_;

	// Mouse movement
	int dragStartX;
	int dragStartY;
};


int main(int argc, char *argv[])
{
	/**
	 * Sugguested framework:
	 * 1. Create a Application wrapper, where common
	 * stuff are managed, such as shaders containing common
	 * functions, render managers.
	 *
	 * 2. Create a render manager, which is used to render
	 * customized objects with render targets. The render
	 * manager should be a singleton all over the application.
	 * In the render manager, we can customize the rendering
	 * sequence of multiple passes rendering.
	 *
	 * 3. Register the render manager in application so that 
	 * the render thread will use render manager as the rendering
	 * engine.
	 * */

	Application app("PlanetRenderer", argc, argv);

	app.registerRenderManager(new PlanetRenderManager());

	return app.exec();
}
