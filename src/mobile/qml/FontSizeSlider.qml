import QtQuick 2.1
import BibleTime 1.0

Rectangle {
    id: fontPointSize

    property string title: ""
    property int min: 10
    property int max: 22
    property int current: 12
    property int previous: 12

    signal accepted(int pointSize);

    color: "#f8f8f8"
    border.color: "black"
    border.width: 1
    anchors.centerIn: parent
    width: parent.width * 0.85
    height: 140

    Text {
        text: title
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 10
        height: 40
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
        height: 40
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

        spacing: 10
        columns: 2
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 10
        anchors.bottomMargin: 10

        Rectangle {
            height: 40
            width: 120
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
            height: 40
            width: 120
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
