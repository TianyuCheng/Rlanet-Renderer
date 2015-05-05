import QtQuick 2.0
import QtMultimedia 5.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

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

	Audio {
		id: playMusic
		source: appPath+"/e922d4ffdee2345071e146039b970aea1d4c3d5c.aac"
		autoPlay: true
		loops: Audio.Infinite
	}

    Rectangle {
        id: toolBar
        width: parent.width; height: 30
        color: '#00ffffff'
        anchors.bottom: renderer.bottom

        Button {
            anchors { left: parent.left; verticalCenter: parent.verticalCenter }
            text: "Quit"
            onClicked: Qt.quit()
            style: ButtonStyle {
                background: Rectangle {
                    implicitWidth: 100
                    implicitHeight: 25
                    border.width: control.activeFocus ? 2 : 1
                    border.color: "#ffffffff"
                    radius: 10
                    gradient: Gradient {
                        GradientStop { position: 0 ; color: control.pressed ? "#55cccccc" : "#55eeeeee" }
                        GradientStop { position: 1 ; color: control.pressed ? "#55aaaaaa" : "#55cccccc" }
                    }
                }
            }
        }

    }

}
