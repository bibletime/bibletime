import QtQuick 2.0
import BibleTime 1.0

Rectangle {
    id: gridChooser

    property int columns: 5
    property int buttonWidth: 100
    property int buttonHeight: 30
    property int topMargin: 10
    property int leftMargin: 10
    property int space:5
    property string selected: ""

    signal accepted(string choosenText);
    signal canceled();

    onOpacityChanged: {

        var count = gridChooserModel.length
        if (count < 36)
            count = 36;
        width = root.width
        height = root.height

        var columnsF = Math.sqrt(count*0.85*height/width);
        columns = Math.ceil(columnsF);

        var rows = Math.ceil((count-0.01)/columns);
        buttonWidth = (width-50)/columns;
        buttonHeight = Math.floor((height-(rows*5))/rows);

        topMargin = (height - rows*(buttonHeight+space) + space)/2;
        leftMargin = (width - columns*(buttonWidth+space) + space)/2;

        if (opacity == 0)
            z = -5000
        else
            z = 5000
    }

    function accept(value) {
        z = -5000
        opacity = 0
        gridChooser.accepted(value);
    }

    z: 1000

    BtStyle {
        id: btStyle
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        enabled: gridChooser.opacity
    }

    Rectangle {
        anchors.fill: parent
        color: btStyle.buttonBackground
    }

    Rectangle {
        id: topSpace

        width: leftMargin
        height: topMargin
        color: btStyle.buttonBackground
    }

    Grid {
        id: grid

        anchors.top: topSpace.bottom
        anchors.bottom: bottom.top
        anchors.left: topSpace.right
        anchors.right: parent.right
        columns: gridChooser.columns
        spacing: gridChooser.space

        Repeater {
            id: repeater

            model: gridChooserModel

            GridChooserButton {
                id: buttonX

                text: modelData
                textHeight: gridChooser.width/60
                buttonWidth: gridChooser.buttonWidth
                buttonHeight: gridChooser.buttonHeight
                textColor: {
                    if (text == gridChooser.selected)
                        return btStyle.buttonHighlightedText
                    else
                        return btStyle.buttonText
                }
                buttonColor: btStyle.button
                activeButtonColor: btStyle.buttonText
                onClicked: gridChooser.accept(text)
            }
        }
    }
}
