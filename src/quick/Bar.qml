import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

Rectangle {
    width: parent.width; height: 50
    color: '#00ffffff'
    anchors.left: renderer.left; anchors.leftMargin: 5
    anchors.right: renderer.right; anchors.rightMargin: 5;
    anchors.bottom: renderer.bottom
}
