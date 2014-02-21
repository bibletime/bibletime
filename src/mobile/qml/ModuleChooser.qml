import QtQuick 2.1
import BibleTime 1.0

Rectangle {
    id: moduleChooser

    property alias categoryModel: categoryView.model
    property alias languageModel: languageView.model
    property alias worksModel: worksView.model
    property alias categoryIndex: categoryView.currentIndex
    property alias languageIndex: languageView.currentIndex
    property alias moduleIndex: worksView.currentIndex
    property int lastCategoryIndex: 0
    property int lastLanguageIndex: 0
    property int spacing: 5
    property string selectedModule: ""
    property string selectedCategory: ""

    objectName: "moduleChooser"
    color: "lightgray"
    border.color: "black"
    border.width: 2

    Keys.onReleased: {
        if (event.key == Qt.Key_Back) {
            event.accepted = true;
            moduleChooser.visible = false;
        }
    }

    onVisibleChanged: {
        if (visible == true) {
            moduleInterface.updateCategoryAndLanguageModels();
            categoryIndex = lastCategoryIndex;
            languageIndex = lastLanguageIndex;
        }
    }

    onCategoryIndexChanged: {
        if (visible == true) {
            moduleInterface.updateWorksModel();
        }
    }

    onLanguageIndexChanged: {
        if (visible == true) {
            moduleInterface.updateWorksModel();
        }
    }

    signal categoryChanged(int index);
    signal languageChanged(int index);
    signal moduleSelected();

    ModuleInterface {
        id: moduleInterface
    }

    Grid {
        id:  grid
        columns: 2
        rows: 1
        spacing: parent.spacing
        width: parent.width - moduleChooser.spacing
        height: parent.height/2.5
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: parent.spacing

        ListTextView {
            id: categoryView

            title: "Category"
            width: grid.width/2 - grid.spacing
            height: grid.height
            onItemSelected: {
                categoryChanged(currentIndex)
            }
        }

        ListTextView {
            id: languageView

            title: "Language"
            width: grid.width/2 - grid.spacing
            height: grid.height
            onItemSelected: {
                languageChanged(currentIndex);
            }
        }
    }

    ListTextView {
        id: worksView

        title: "Work"
        width: parent.width - 2 * parent.spacing
        anchors.top: grid.bottom
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: moduleChooser.spacing
        highlight: false
        onItemSelected: {
            selectedModule = moduleInterface.module(index);
            selectedCategory = moduleInterface.category(index);
            moduleSelected();
            moduleChooser.visible = false;
        }
    }

//    MouseArea {
//        anchors.fill: parent
//        onClicked: moduleChooser.cancel();
//    }
}

