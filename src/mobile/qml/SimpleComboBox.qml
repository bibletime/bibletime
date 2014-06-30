import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0


Rectangle {
    id: comboBox

    property alias model: popupItems.model
    property alias currentIndex: popup.__selectedIndex
    readonly property alias currentText: popup.currentText
    property bool editable: false
    property alias editText: input.text
    property bool activeFocusOnPress: false
    readonly property bool pressed: mouseArea.pressed && mouseArea.containsMouse || popup.__popupVisible
    readonly property alias hovered: mouseArea.containsMouse
    readonly property alias count: popupItems.count
    
    function textAt (index) {
        if (index >= count || index < 0)
            return null;
        return popupItems.objectAt(index).text;
    }

    function find (text) {
        return input.find(text, Qt.MatchExactly)
    }

    property alias validator: input.validator
    readonly property alias acceptableInput: input.acceptableInput

    signal accepted
    signal activated(int index)

    function selectAll() {
        input.selectAll()
    }

    function __selectPrevItem() {
        input.blockUpdate = true
        if (currentIndex > 0) {
            currentIndex--;
            input.text = popup.currentText;
            activated(currentIndex);
        }
        input.blockUpdate = false;
    }

    function __selectNextItem() {
        input.blockUpdate = true;
        if (currentIndex < popupItems.count - 1) {
            currentIndex++;
            input.text = popup.currentText;
            activated(currentIndex);
        }
        input.blockUpdate = false;
    }

    property var __popup: popup

//    style: Qt.createComponent(Settings.style + "/ComboBoxStyle.qml", comboBox)

    activeFocusOnTab: true

    Accessible.role: Accessible.ComboBox

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onPressed: {
            if (comboBox.activeFocusOnPress)
                forceActiveFocus()
            popup.show()
        }
        onWheel: {
            if (wheel.angleDelta.y > 0) {
                __selectPrevItem();
            } else if (wheel.angleDelta.y < 0){
                __selectNextItem();
            }
        }
    }

    Component.onCompleted: {
        if (currentIndex === -1)
            currentIndex = 0

        popup.ready = true
//        popup.resolveTextValue(textRole)
    }

    Keys.onPressed: {
        // Perform one-character based lookup for non-editable combo box
        if (!editable && event.text.length > 0) {
            var index = input.find(event.text, Qt.MatchStartsWith);
            if (index >= 0 && index !== currentIndex) {
                currentIndex = index;
                activated(currentIndex);
            }
        }
    }

    TextInput {
        id: input

        visible: editable
        enabled: editable
        focus: true
        clip: contentWidth > width

        anchors.fill: parent
        anchors.leftMargin: 8

        verticalAlignment: Text.AlignVCenter

        renderType: Text.NativeRendering
        selectByMouse: true
        color: "red"
        onAccepted: {
            var idx = input.find(editText, Qt.MatchFixedString)
            if (idx > -1) {
                editTextMatches = true;
                currentIndex = idx;
                editText = textAt(idx);
            } else {
                editTextMatches = false;
                currentIndex = -1;
                popup.currentText = editText;
            }
            comboBox.accepted();
        }

        property bool blockUpdate: false
        property string prevText
        property bool editTextMatches: true

        function find (text, searchType) {
            for (var i = 0 ; i < popupItems.count ; ++i) {
                var currentString = popupItems.objectAt(i).text
                if (searchType === Qt.MatchExactly) {
                    if (text === currentString)
                        return i;
                } else if (searchType === Qt.CaseSensitive) {
                    if (currentString.indexOf(text) === 0)
                        return i;
                } else if (searchType === Qt.MatchFixedString) {
                    if (currentString.toLowerCase().indexOf(text.toLowerCase()) === 0
                            && currentString.length === text.length)
                        return i;
                } else if (currentString.toLowerCase().indexOf(text.toLowerCase()) === 0) {
                    return i
                }
            }
            return -1;
        }

        // Finds first entry and shortest entry. Used by editable combo
        function tryComplete (inputText) {
            var candidate = "";
            var shortestString = "";
            for (var i = 0 ; i < popupItems.count ; ++i) {
                var currentString = popupItems.objectAt(i).text;

                if (currentString.toLowerCase().indexOf(inputText.toLowerCase()) === 0) {
                    if (candidate.length) { // Find smallest possible match
                        var cmp = 0;

                        // We try to complete the shortest string that matches our search
                        if (currentString.length < candidate.length)
                            candidate = currentString

                        while (cmp < Math.min(currentString.length, shortestString.length)
                               && shortestString[cmp].toLowerCase() === currentString[cmp].toLowerCase())
                            cmp++;
                        shortestString = shortestString.substring(0, cmp);
                    } else { // First match, select as current index and find other matches
                        candidate = currentString;
                        shortestString = currentString;
                    }
                }
            }

            if (candidate.length)
                return inputText + candidate.substring(inputText.length, candidate.length);
            return inputText;
        }

        property bool allowComplete: false
        Keys.onPressed: allowComplete = (event.key !== Qt.Key_Backspace && event.key !== Qt.Key_Delete);

        onTextChanged: {
            if (editable && !blockUpdate && allowComplete && text.length > 0) {
                var completed = input.tryComplete(text)
                if (completed.length > text.length) {
                    var oldtext = input.text;
                    input.text = completed;
                    input.select(text.length, oldtext.length);
                }
            }
            prevText = text
        }
    }

    Binding {
        target: input
        property: "text"
        value: popup.currentText
        when: input.editTextMatches
    }

//    onTextRoleChanged: popup.resolveTextValue(textRole)

    Menu {
        id: popup
        objectName: "popup"

//        style: isPopup ? __style.__popupStyle : __style.__dropDownStyle

        property string currentText: selectedText
        onSelectedTextChanged: if (selectedText) popup.currentText = selectedText

        property string selectedText
        on__SelectedIndexChanged: {
            if (__selectedIndex === -1)
                popup.currentText = ""
            else
                updateSelectedText()
        }
        property string textRole: ""

        property bool ready: false
        property bool isPopup: !editable && !!__panel && __panel.popup

//        property int y: isPopup ? (comboBox.__panel.height - comboBox.__panel.implicitHeight) / 2.0 : comboBox.__panel.height
        __minimumWidth: comboBox.width
        __visualItem: comboBox

        property ExclusiveGroup eg: ExclusiveGroup { id: eg }

        property bool modelIsArray: false

        Instantiator {
            id: popupItems
            active: false

            property bool updatingModel: false
            onModelChanged: {
                popup.modelIsArray = !!model ? model.constructor === Array : false
                if (active) {
                    if (updatingModel && popup.__selectedIndex === 0) {
                        // We still want to update the currentText
                        popup.updateSelectedText()
                    } else {
                        updatingModel = true
                        popup.__selectedIndex = 0
                    }
                }
                popup.resolveTextValue(comboBox.textRole)
            }

            MenuItem {
                text: popup.textRole === '' ?
                        modelData :
                          ((popup.modelIsArray ? modelData[popup.textRole] : model[popup.textRole]) || '')
                onTriggered: {
                    if (index !== currentIndex)
                        activated(index)
                    comboBox.editText = text
                }
                checkable: true
                exclusiveGroup: eg
            }
            onObjectAdded: {
                popup.insertItem(index, object)
                if (!updatingModel && index === popup.__selectedIndex)
                    popup.selectedText = object["text"]
            }
            onObjectRemoved: popup.removeItem(object)

        }

        function resolveTextValue(initialTextRole) {
            if (!ready || !model) {
                popupItems.active = false
                return;
            }

            var get = model['get'];
            if (!get && popup.modelIsArray && !!model[0]) {
                if (model[0].constructor !== String && model[0].constructor !== Number)
                    get = function(i) { return model[i]; }
            }

            var modelMayHaveRoles = get !== undefined
            textRole = initialTextRole
            if (textRole === "" && modelMayHaveRoles && get(0)) {
                // No text role set, check whether model has a suitable role
                // If 'text' is found, or there's only one role, pick that.
                var listElement = get(0)
                var roleName = ""
                var roleCount = 0
                for (var role in listElement) {
                    if (listElement[role].constructor === Function)
                        continue;
                    if (role === "text") {
                        roleName = role
                        break
                    } else if (!roleName) {
                        roleName = role
                    }
                    ++roleCount
                }
                if (roleCount > 1 && roleName !== "text") {
                    console.warn("No suitable 'textRole' found for ComboBox.")
                } else {
                    textRole = roleName
                }
            }

            if (!popupItems.active)
                popupItems.active = true
            else
                updateSelectedText()
        }

        function show() {
            if (items[__selectedIndex])
                items[__selectedIndex].checked = true
            __currentIndex = comboBox.currentIndex
            if (Qt.application.layoutDirection === Qt.RightToLeft)
                __popup(comboBox.width, y, isPopup ? __selectedIndex : 0)
            else
                __popup(0, y, isPopup ? __selectedIndex : 0)
        }

        function updateSelectedText() {
            var selectedItem;
            if (__selectedIndex !== -1 && (selectedItem = items[__selectedIndex])) {
                input.editTextMatches = true
                selectedText = selectedItem.text
            }
        }
    }

    // The key bindings below will only be in use when popup is
    // not visible. Otherwise, native popup key handling will take place:
    Keys.onSpacePressed: {
        if (!popup.popupVisible)
            popup.show()
    }

    Keys.onUpPressed: __selectPrevItem()
    Keys.onDownPressed: __selectNextItem()
}
