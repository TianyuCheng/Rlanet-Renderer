#include "RenderManager.h"

void RenderManager::beforeRender() {
	int t = time.elapsed();
	timeStep = t - lastTime;
	lastTime = t;

	_fps++;
	_fpsCounter += timeStep;
	if (_fpsCounter > 1e3) {
		_fpsVal = _fps;
		_fpsCounter = 0;
		_fps = 0;
	}
}

// Time control functions for the rendermanager
void RenderManager::start() { 
	time.start();
	elapsedTime = 0;
}

void RenderManager::pause() { 
	elapsedTime = time.elapsed(); 
}

void RenderManager::resume() {
	// subtracting the time just before we pause it
	// go back in time to fake pause/resume
	int diff = time.elapsed() - elapsedTime;
	time.addMSecs(-diff);
}

void RenderManager::restart() {
	time.restart();
}

double RenderManager::timeElapsed() const
{ 
	return time.elapsed() / 1000.0; 
}

double RenderManager::getInterval() const {
	return timeStep / 1000.0;
}
