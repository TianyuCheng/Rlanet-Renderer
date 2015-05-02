import QtQuick 2.0

import Mangekyou 1.0

Item {
	width: 1024
	height: 768

	Renderer {
		id: renderer
		anchors.fill: parent
		anchors.margins: 0
		transform: Scale { origin.x: 0; origin.y: parent.height/2; yScale: -1}

		/*
		 * Step 1: add UI signals to send.
		 *
		 * These should have clear semantics about how to modify
		 * attributes provided by the backend, like camera positions.
		 */
		signal cameramove(real dx, real dy, real dz)

		/*
		 * Step 2: accept keyboard signals, and translate them to UI
		 * signals
		 *
		 * This allows us translate multiple events to single UI
		 * signals, say we can support WSAD and 5213 simultaneously in
		 * Qt Quick part, without even touching C++ part.
		 */
		property real camera_speed : 10.0
		focus : true;
		Keys.onPressed : {
			if (event.key == Qt.Key_W) {
				renderer.cameramove(0.0, 0.0, -camera_speed);
				event.accepted = true;
			} else if (event.key == Qt.Key_S) {
				renderer.cameramove(0.0, 0.0, camera_speed);
				event.accepted = true;
			} else if (event.key == Qt.Key_A) {
				renderer.cameramove(0.0, -camera_speed, 0.0);
				event.accepted = true;
			} else if (event.key == Qt.Key_D) {
				renderer.cameramove(0.0, camera_speed, 0.0);
				event.accepted = true;
			}
			/*
			if (event.accepted) {
				console.log("Accepted Keyboard event");
			}
			*/
		}
	}
}
