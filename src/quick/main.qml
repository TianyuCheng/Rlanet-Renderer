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

        /* key events */
        signal keypressed(int count, int key, int modifiers, string text)
        signal keyreleased(int count, int key, int modifiers, string text)

        /* mouse events */
        // signal

		/*
		 * Step 2: accept keyboard signals, and translate them to UI
		 * signals
		 *
		 * This allows us translate multiple events to single UI
		 * signals, say we can support WSAD and 5213 simultaneously in
		 * Qt Quick part, without even touching C++ part.
		 */
		focus : true;
        Keys.onPressed : {
            renderer.keypressed(event.count, event.key, event.modifiers, event.text);
            event.accepted = true;
        }
        Keys.onReleased : {
            renderer.keyreleased(event.count, event.key, event.modifiers, event.text);
            event.accepted = true;
        }
	}
}
