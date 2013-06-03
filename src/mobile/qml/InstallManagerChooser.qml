import QtQuick 2.0

Rectangle {
    id: installManager

    property alias sourceModel: sourceView.model
    property alias categoryModel: categoryView.model
    property alias languageModel: languageView.model
    property alias worksModel: worksView.model
    property alias sourceIndex: sourceView.currentIndex
    property alias categoryIndex: categoryView.currentIndex
    property alias languageIndex: languageView.currentIndex

    objectName: "installManager"
    color: "lightgray"

    signal sourceChanged(int index);
    signal categoryChanged(int index);
    signal languageChanged(int index);
    signal workSelected(int index);
    signal cancel();
    signal installRemove();
    signal refreshLists();

    Grid {
        id:  grid
        columns: 3
        rows: 2
        spacing: 3
        width: parent.width
        anchors.left: parent.left
        anchors.leftMargin: 3

        Rectangle {

            height: 25
            width: parent.width/3 - grid.spacing
            color: "lightgray"

            Text {
                id: sourceText

                text: "Source"
                anchors.centerIn: parent
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignBottom
                style: Text.Sunken
            }
        }

        Rectangle {

            height: 25
            width: parent.width/3 - grid.spacing
            color: "lightgray"

            Text {
                id: categoryText

                text: "Category"
                anchors.centerIn: parent
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignBottom
                style: Text.Sunken
            }
        }

        Rectangle {

            height: 25
            width: parent.width/3 - grid.spacing
            color: "lightgray"

            Text {
                id: languageText

                text: "Language"
                anchors.centerIn: parent
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignBottom
                style: Text.Sunken
            }
        }

        ListTextView {
            id: sourceView
            onItemSelected: {
                sourceChanged(currentIndex)
            }


            width: parent.width/3 - grid.spacing
            height: installManager.height/3
        }

        ListTextView {
            id: categoryView

            width: parent.width/3 - grid.spacing
            height: installManager.height/3
            onItemSelected: {
                categoryChanged(currentIndex)
            }
        }

        ListTextView {
            id: languageView

            width: parent.width/3 - grid.spacing
            height: installManager.height/3
            onItemSelected: {
                languageChanged(currentIndex)
            }
        }
    }

    Rectangle {
        id: spacer

        width: parent.width
        height: 40
        anchors.top: grid.bottom
        color: "lightgray"

        Text {
            text: "Works"
            anchors.centerIn: parent
            style: Text.Sunken
        }
    }

    ListWorksView {
        id: worksView

        width: parent.width
        anchors.top: spacer.bottom
        anchors.left: parent.left
        anchors.bottom: installRemoveButton.top
        anchors.bottomMargin: 10
        onItemSelected: {
            workSelected(index)
        }
    }

    Rectangle {
        id: "refreshButton"
        width:150
        height: 40
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.right: installRemoveButton.left
        anchors.rightMargin: 10
        border.width: 1
        border.color: "black"

        Text {
            text: "Refresh Lists"
            anchors.centerIn: parent
        }

        MouseArea {
            anchors.fill: parent
            onClicked: installManager.refreshLists()
        }
    }

    Rectangle {
        id: "installRemoveButton"
        width:150
        height: 40
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.right: cancelButton.left
        anchors.rightMargin: 10
        border.width: 1
        border.color: "black"

        Text {
            text: "Install / Remove"
            anchors.centerIn: parent
        }

        MouseArea {
            anchors.fill: parent
            onClicked: installManager.installRemove()
        }
    }

    Rectangle {
        id: "cancelButton"
        width: installRemoveButton.width
        height: installRemoveButton.height
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10
        border.width: 1
        border.color: "black"

        Text {
            text: "Cancel"
            anchors.centerIn: parent
        }

        MouseArea {
            anchors.fill: parent
            onClicked: installManager.cancel();
        }
    }

}
