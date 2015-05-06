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
		signal mouseclicked(int buttons, int modifiers, bool wasHeld, int x, int y);
        signal mousedragstarted(int x, int y);
        signal mousedragging(int x, int y);
        signal mousedragfinished(int x, int y);

		/*
		 * Step 2: accept keyboard signals, and translate them to UI
		 * signals
		 *
		 * This allows us translate multiple events to single UI
		 * signals, say we can support WSAD and 5213 simultaneously in
		 * Qt Quick part, without even touching C++ part.
		 */
		focus : true;
        Keys.onEscapePressed: {
            if (menu.visible == true) menu.visible = false;
            else menu.visible = true;
        }
		Keys.onPressed : {
			renderer.keypressed(event.count, event.key, event.modifiers, event.text);
			event.accepted = true;
		}
		Keys.onReleased : {
			renderer.keyreleased(event.count, event.key, event.modifiers, event.text);
			event.accepted = true;
		}
        MouseArea {
            id: mouseArea
            anchors.fill: parent
            drag.target: renderer
            drag.axis: Drag.XandYAxis
            onClicked: {
                renderer.mouseclicked(mouse.buttons, mouse.modifiers, mouse.wasHeld, mouseX, mouseY);
                mouse.accepted = true;
            }
            onPressed: {
                renderer.mousedragstarted(mouseX, mouseY);
                mouse.accepted = true;
            }
            onReleased: {
                renderer.mousedragfinished(mouseX, mouseY);
                mouse.accepted = true;
            }
            onPositionChanged: {
                if (drag.active) {
                    renderer.mousedragging(mouseX, mouseY);
                }
            }
        }
	}

	Audio {
		id: playMusic
		source: appPath+"/e922d4ffdee2345071e146039b970aea1d4c3d5c.aac"
		autoPlay: true
		loops: Audio.Infinite
	}

    FontLoader { id: neuropolFont; source: "qrc:/fonts/neuropol x rg.ttf" }

    Rectangle {
        id: toolBar
        width: parent.width; height: 50
        color: '#00ffffff'
        anchors.left: renderer.left; anchors.leftMargin: 5
        anchors.right: renderer.right; anchors.rightMargin: 5;
        anchors.bottom: renderer.bottom

        Button {
            anchors { left: parent.left; verticalCenter: parent.verticalCenter }
            text: "Quit"
            onClicked: Qt.quit()
            style: ButtonStyle {
                label: Text {
                    renderType: Text.NativeRendering
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.family: neuropolFont.name
                    font.pointSize: 12
                    color: "#ffffffff"
                    text: control.text
                }
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
    } // end of rectangle

    Rectangle {
        id: menu
        visible: false
        width: 400; height: 200
        anchors { horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter }

        // style
        radius: 10
        border.width: 2
        border.color: "#aaffffff"
        color: "#55eeeeee"

        Rectangle {
            id: menu_caption
            width: parent.width; height: 40
            radius: 10
            Text {
                text: "Planet Renderer"
                width: parent.width
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.family: neuropolFont.name
                font.pointSize: 20
            }
        }

        Rectangle {
            id: menu_list
            width: parent.width
            height: 200
            anchors.top: menu_caption.bottom; anchors.topMargin: 5

            // style
            radius: 10
            border.width: 0
            border.color: "#00ffffff"
            color: "#00ffffff"

            ListModel {
                id: menuModel
                ListElement {
                    name: "LOD Terrain"
                }
                ListElement {
                    name: "Macro Tile"
                }
                ListElement {
                    name: "Ocean Shader"
                }
                ListElement {
                    name: "Biomes"
                }
            }

            ListView {
                model: menuModel
                width: parent.width; height: 200
                anchors.top: parent.top
                anchors.fill: parent; anchors.margins: 5
                spacing: 10
                delegate: Button {
                    text: name
                    width: parent.width
                    style: ButtonStyle {
                        label: Text {
                            renderType: Text.NativeRendering
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            font.family: neuropolFont.name
                            font.pointSize: 12
                            color: "#ffffffff"
                            text: control.text
                        }
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
            } // end of list view
        }
    } // end of menu

}
