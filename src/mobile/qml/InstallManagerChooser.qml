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
    property int spacing: 8

    objectName: "installManager"
    color: "lightgray"
    border.color: "black"
    border.width: 2

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
        rows: 1
        spacing: installManager.spacing
        width: parent.width - installManager.spacing
        height: installManager.height/3
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: installManager.spacing

        ListTextView {
            id: sourceView
            onItemSelected: {
                sourceChanged(currentIndex)
            }

            title: "Source"
            width: parent.width/3 - grid.spacing
            height: installManager.height/3
        }

        ListTextView {
            id: categoryView

            title: "Category"
            width: parent.width/3 - grid.spacing
            height: installManager.height/3
            onItemSelected: {
                categoryChanged(currentIndex)
            }
        }

        ListTextView {
            id: languageView

            title: "Language"
            width: parent.width/3 - grid.spacing
            height: installManager.height/3
            onItemSelected: {
                languageChanged(currentIndex)
            }
        }
    }

    ListWorksView {
        id: worksView

        title: "Works"
        width: parent.width - 2 * installManager.spacing
        anchors.top: grid.bottom
        anchors.left: parent.left
        anchors.bottom: installRemoveButton.top
        anchors.margins: installManager.spacing
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
