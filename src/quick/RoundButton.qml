import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

// FontLoader { id: neuropolFont; source: "qrc:/fonts/neuropol x rg.ttf" }

Button {
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
