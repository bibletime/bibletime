import QtQuick 2.1
import BibleTime 1.0

Rectangle {
    id: button

    property int buttonWidth
    property int buttonHeight
    property int textHeight
    property color textColor
    property color buttonColor
    property color activeButtonColor
    property alias text: buttonText.text

    signal clicked

    width: buttonWidth
    height: buttonHeight
    gradient: Gradient {
        GradientStop { position: 0.0;  color: btStyle.buttonGradient0 }
        GradientStop { position: 0.15; color: btStyle.buttonGradient1 }
        GradientStop { position: 0.85; color: btStyle.buttonGradient2 }
        GradientStop { position: 1.0;  color: btStyle.buttonGradient3 }
    }
    smooth: true

    border {
        width: 1
        color: btStyle.buttonBorder
    }

    BtStyle {
        id: btStyle
    }

    Text {
        id: buttonText

        width: buttonWidth-8
        anchors.centerIn: parent
        color: button.textColor
        font.pointSize: parent.textHeight
        elide: Text.ElideRight
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        onClicked: button.clicked()
    }
}
