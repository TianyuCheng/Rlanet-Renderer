import QtQuick 2.0
import QtMultimedia 5.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

Rectangle {
    // style
    radius: 10
    border.width: 2
    border.color: "#aaffffff"
    color: "#55eeeeee"

    Rectangle {
        anchors.margins: 10
        anchors.top: parent.top
        width: parent.width; height: 30
        color: "#ffffffff"
    }

    Rectangle {
        anchors.top: parent.top
        width: parent.width; height: 40
        radius: 10
        color: "#ffffffff"
        Text {
            text: "Planet Renderer"
            width: parent.width
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.family: neuropolFont.name
            font.pointSize: 20
        }
    }

} // end of menu
