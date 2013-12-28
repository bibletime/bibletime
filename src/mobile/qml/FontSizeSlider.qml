import QtQuick 2.1
import BibleTime 1.0

Rectangle {
    id: fontPointSize

    property string title: ""
    property int min: 16
    property int max: 40
    property int current: 20
    property int previous: 20

    signal accepted(int pointSize);

    color: "#f8f8f8"
    border.color: "black"
    border.width: 1
    anchors.centerIn: parent
    width: parent.width * 0.85
    height: btStyle.pixelsPerMillimeterY * 24

    Text {
        text: title
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 10
        height: btStyle.pixelsPerMillimeterY * 7
        font.pointSize: btStyle.uiFontPointSize
    }

    Rectangle {
        id: bar

        color: "blue"
        width: parent.width *.80
        height: 3
        anchors.centerIn: parent
    }

    Rectangle {
        id: indicator

        width: 18
        height: 18
        color: "red"
        y: bar.y - height / 2
        x: {
            var range = fontPointSize.max - fontPointSize.min;
            var xpos = bar.width *
                    (fontPointSize.current - fontPointSize.min) / range;
            xpos = xpos + bar.x
            return xpos
        }
    }

    MouseArea {
        property bool active: false
        width: bar.width
        anchors.left: bar.left
        height: btStyle.pixelsPerMillimeterY * 7
        anchors.verticalCenter: bar.verticalCenter

        onPressed: {
            active = true;
        }

        onReleased: {
            active = false;
        }

        onMouseXChanged: {
            if ( ! active)
                return;
            var range = fontPointSize.max - fontPointSize.min;
            var currentF = mouse.x / bar.width * range  + fontPointSize.min;
            var value = Math.round(currentF);
            if (value < fontPointSize.min)
                value = min;
            if (value > fontPointSize.max)
                value = max;
            fontPointSize.current = value;
            accepted(value);
        }

    }

    Grid {
        id: buttons

        spacing: btStyle.pixelsPerMillimeterY * 4
        columns: 2
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 10
        anchors.bottomMargin: 10

        Rectangle {
            height: btStyle.pixelsPerMillimeterY * 7
            width: btStyle.pixelsPerMillimeterY * 18
            border.color: "black"
            border.width: 1

            Text {
                text: "Ok"
                anchors.centerIn: parent
                font.pointSize: btStyle.uiFontPointSize
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    fontPointSize.visible = false;
                }
            }
        }

        Rectangle {
            height: btStyle.pixelsPerMillimeterY * 7
            width: btStyle.pixelsPerMillimeterY * 18
            border.color: "black"
            border.width: 1

            Text {
                text: "Cancel"
                anchors.centerIn: parent
                font.pointSize: btStyle.uiFontPointSize
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    accepted(previous);
                    fontPointSize.visible = false;
                }
            }
        }
    }

}
