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

    signal sourceChanged(int index)
    signal categoryChanged(int index)
    signal languageChanged(int index)

    Grid {
        id:  grid
        columns: 3
        rows: 2
        spacing: 3
        width: parent.width
//        height: parent.height / 2.5
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
        anchors.bottom: parent.bottom
    }

}
