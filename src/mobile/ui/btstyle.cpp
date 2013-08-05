
#include "btstyle.h"
#include <QList>
#include <QPointer>

// BtStyle is a class that is registered at a QML item. It can be placed into
// QML files and its properties are available to be used in QML. It contains
// colors, sizes, etc. that affect the look of the UI. You can have multipe
// instances of the item. Changing a property on one of them changes all of them
/*
   import BibleTime 1.0

    BtStyle {
       id: btStyle
    }

    Rectangle {
        color: btStyle.button
    }
*/

namespace btm {

// Only one copy of properties so they are the same everywhere used.
static QColor buttonColor             = QColor();
static QColor buttonBackground        = QColor();
static QColor buttonTextColor         = QColor();
static QColor buttonHighlightedText   = QColor();
static QColor buttonBorder            = QColor();
static int buttonRadius               = 0;
static QColor buttonGradient0;
static QColor buttonGradient1;
static QColor buttonGradient2;
static QColor buttonGradient3;

static QColor windowTab               = QColor();
static QColor windowTabSelected       = QColor();
static QColor windowTabText           = QColor();
static QColor windowTabTextSelected   = QColor();

static QColor menu                    = QColor();
static QColor menuBorder              = QColor();
static QColor menuText                = QColor();
static int menuHeight                 = 0;

static QColor toolbarColor            = QColor();
static QColor toolbarButton           = QColor();
static QColor toolbarButtonText       = QColor();
static double toolbarTextPointSize    = 6;


static QList<QPointer<BtStyle> > styles;

void BtStyle::setStyle(int style) {
    if (style == BtStyle::gnome) {
        setButtonColor(QColor(0,0,0));
        setButtonBackground(QColor(237, 237, 237));
        setButtonTextColor(QColor(0,0,0));
        setButtonHighlightedText(QColor(0,0,255));
        setButtonBorder(QColor(80,80,0));
        setButtonRadius(3);
        setButtonGradient0(QColor(200,200,200));
        setButtonGradient1(QColor(246,246,246));
        setButtonGradient2(QColor(246,246,246));
        setButtonGradient3(QColor(200,200,200));

        setWindowTab(QColor(206,206,206));
        setWindowTabSelected(QColor(255,255,255));
        setWindowTabText(QColor(0,0,0));
        setWindowTabTextSelected(QColor(0,0,0));

        setMenu(QColor(255,255,255));
        setMenuBorder(QColor(255,255,255));
        setMenuText(QColor(0,0,0));
        setMenuHeight(40);

        setToolbarColor(QColor(237,237,237));
        setToolbarButton(QColor(237,237,237));
        setToolbarButtonText(QColor(0,0,0));
        setToolbarTextPointSize(10);
    }
    else if (style == BtStyle::android) {
        setButtonColor(QColor(0,0,0));
        setButtonBackground(QColor(35,35,100));
        setButtonTextColor(QColor(255,210,0));
        setButtonHighlightedText(QColor(255,255,0));
        setButtonBorder(QColor(80,80,0));
        setButtonRadius(3);
        setButtonGradient0(QColor(125,125,125));
        setButtonGradient1(QColor(60,60,60));
        setButtonGradient2(QColor(50,50,50));
        setButtonGradient3(QColor(20,20,20));

        setWindowTab(QColor(100,100,100));
        setWindowTabSelected(QColor(255,255,255));
        setWindowTabText(QColor(255,255,255));
        setWindowTabTextSelected(QColor(0,0,0));

        setMenu(QColor(255,255,255));
        setMenuBorder(QColor(224,224,224));
        setMenuText(QColor(0,0,0));
        setMenuHeight(34);

        setToolbarColor(QColor(120,120,120));
        setToolbarButton(QColor(255,210,0));
        setToolbarButtonText(QColor(0,0,0));
        setToolbarTextPointSize(10);
    }

    for (int i=0; i<styles.count(); ++i) {
        QPointer<BtStyle> style = styles.at(i);
        if (style != 0)
            style->changed();
    }
}

BtStyle::BtStyle(QObject* parent)
    : QObject(parent) {
    styles.append(this);
}

QColor BtStyle::getButtonColor() const {
    return buttonColor;
}
void BtStyle::setButtonColor(const QColor& color) {
    buttonColor = color;
    emit changed();
}


QColor BtStyle::getButtonBackground() const {
    return buttonBackground;
}
void BtStyle::setButtonBackground(const QColor& color) {
    buttonBackground = color;
    emit changed();
}


QColor BtStyle::getButtonTextColor() const {
    return buttonTextColor;
}
void BtStyle::setButtonTextColor(const QColor& color) {
    buttonTextColor = color;
    emit changed();
}


QColor BtStyle::getButtonHighlightedText() const {
    return buttonHighlightedText;
}
void BtStyle::setButtonHighlightedText(const QColor& color) {
    buttonHighlightedText = color;
    emit changed();
}


QColor BtStyle::getButtonBorder() const {
    return buttonBorder;
}
void BtStyle::setButtonBorder(const QColor& color) {
    buttonBorder = color;
    emit changed();
}

int BtStyle::getButtonRadius() const {
    return buttonRadius;
}
void BtStyle::setButtonRadius(int radius) {
    buttonRadius = radius;
    emit changed();
}

QColor BtStyle::getButtonGradient0() const {
    return buttonGradient0;
}
void BtStyle::setButtonGradient0(const QColor& color) {
    buttonGradient0 = color;
    emit changed();
}

QColor BtStyle::getButtonGradient1() const {
    return buttonGradient1;
}
void BtStyle::setButtonGradient1(const QColor& color) {
    buttonGradient1 = color;
    emit changed();
}

QColor BtStyle::getButtonGradient2() const {
    return buttonGradient2;
}
void BtStyle::setButtonGradient2(const QColor& color) {
    buttonGradient2 = color;
    emit changed();
}

QColor BtStyle::getButtonGradient3() const {
    return buttonGradient3;
}
void BtStyle::setButtonGradient3(const QColor& color) {
    buttonGradient3 = color;
    emit changed();
}

QColor BtStyle::getWindowTab() const {
    return windowTab;
}

void BtStyle::setWindowTab(const QColor& color) {
    windowTab = color;
    emit changed();
}

QColor BtStyle::getWindowTabSelected() const {
    return windowTabSelected;
}

void BtStyle::setWindowTabSelected(const QColor& color) {
    windowTabSelected = color;
    emit changed();
}

QColor BtStyle::getWindowTabText() const {
    return windowTabText;
}

void BtStyle::setWindowTabText(const QColor& color) {
    windowTabText = color;
    emit changed();
}

QColor BtStyle::getWindowTabTextSelected() const {
    return windowTabTextSelected;
}

void BtStyle::setWindowTabTextSelected(const QColor& color) {
    windowTabTextSelected = color;
    emit changed();
}

QColor BtStyle::getMenu() const {
    return menu;
}

void BtStyle::setMenu(const QColor& color) {
    menu = color;
    emit changed();
}

QColor BtStyle::getMenuBorder() const {
    return menuBorder;
}

void BtStyle::setMenuBorder(const QColor& color) {
    menuBorder = color;
    emit changed();
}

QColor BtStyle::getMenuText() const {
    return menuText;
}

void BtStyle::setMenuText(const QColor& color) {
    menuText = color;
    emit changed();
}

int BtStyle::getMenuHeight() const {
    return menuHeight;
}
void BtStyle::setMenuHeight(int height) {
    menuHeight = height;
    emit changed();
}

QColor BtStyle::getToolbarColor() const {
    return toolbarColor;
}
void BtStyle::setToolbarColor(const QColor& color) {
    toolbarColor = color;
    emit changed();
}

QColor BtStyle::getToolbarButton() const {
    return toolbarButton;
}
void BtStyle::setToolbarButton(const QColor& color) {
    toolbarButton = color;
    emit changed();
}

QColor BtStyle::getToolbarButtonText() const {
    return toolbarButtonText;
}
void BtStyle::setToolbarButtonText(const QColor& color) {
    toolbarButtonText = color;
    emit changed();
}

double BtStyle::getToolbarTextPointSize() const {
    return toolbarTextPointSize;
}

void BtStyle::setToolbarTextPointSize(double pointSize) {
    toolbarTextPointSize = pointSize;
}

} // end namespace

