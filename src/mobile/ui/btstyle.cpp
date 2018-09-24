/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btstyle.h"
#include <QGuiApplication>
#include <QList>
#include <QPointer>
#include <QScreen>
#include "backend/config/btconfig.h"
#include "backend/models/btmoduletextmodel.h"

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
static QColor textColor               = QColor();
static QColor linkColor               = QColor();
static QColor textBackgroundColor     = QColor();
static QColor textBackgroundHighlightColor= QColor();
static QColor inputBorderColor        = QColor();

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
static QColor toolbarTextColor        = QColor();
static QColor toolbarButtonText       = QColor();
static double toolbarTextPointSize    = 6;

int currentStyle = BtStyle::darkTheme;

static QList<QPointer<BtStyle> > styles;

static double millimeterPerInch = 25.4;

static int s_width = 0;
static int s_height = 0;

static void emitChanged() {
    for (int i=0; i<styles.count(); ++i) {
        QPointer<BtStyle> style = styles.at(i);
        if (style != nullptr)
            style->changed();
    }
}

int BtStyle::getStyle() {
    return currentStyle;
}

void BtStyle::setStyle(int style) {
    setCurrentStyle(style);
    emitChanged();
}

int BtStyle::getCurrentStyle() {
    return currentStyle;
}

void BtStyle::setCurrentStyle(int style) {
    if (style == BtStyle::darkTheme) {

        currentStyle = style;

        BtModuleTextModel::setLinkColor(QColor(180,180,255));
        BtModuleTextModel::setHighlightColor(QColor(0,0,0));
        BtModuleTextModel::setJesusWordsColor(QColor(255,0,0));

        setTextColor(QColor(255,255,255));
        setLinkColor(QColor(0,0,80));
        setTextBackgroundColor(QColor(0,0,0));
        setTextBackgroundHighlightColor(QColor(184,135,11));
        setInputBorderColor(QColor(218,165,3));

        setButtonColor(QColor(0,0,0));
        setButtonBackground(QColor(35,35,100));
        setButtonTextColor(QColor(255,210,0));
        setButtonHighlightedText(QColor(255,255,0));
        setButtonBorder(QColor(110,110,110));
        setButtonRadius(3);

        setButtonGradient0(QColor(90,90,90));
        setButtonGradient1(QColor(255,255,255));
        setButtonGradient2(QColor(0,0,0));
        setButtonGradient3(QColor(100,100,100));

        setWindowTab(QColor(100,100,100));
        setWindowTabSelected(QColor(218,165,3));
        setWindowTabText(QColor(255,255,255));
        setWindowTabTextSelected(QColor(0,0,0));

        setMenu(QColor(255,255,255));
        setMenuBorder(QColor(60,60,60));
        setMenuText(QColor(0,0,0));
        setMenuHeight(34);

        setToolbarColor(QColor(0,0,0));
        setToolbarTextColor(QColor(218,165,3));
        setToolbarButtonText(QColor(255,255,255));
        setToolbarTextPointSize(10);
    }
    else if (style == BtStyle::lightBlueTheme) {

        currentStyle = style;

        BtModuleTextModel::setLinkColor(QColor(0,0,255));
        BtModuleTextModel::setHighlightColor(QColor(0,0,255));
        BtModuleTextModel::setJesusWordsColor(QColor(255,0,0));

        setTextColor(QColor(0,0,0));
        setLinkColor(QColor(0,0,220));
        setTextBackgroundColor(QColor(255,255,255));
        setTextBackgroundHighlightColor(QColor(255,240,170));
        setInputBorderColor(QColor(0,0,0));

        setButtonColor(QColor(0,0,0));
        setButtonBackground(QColor(190,220,255));
        setButtonTextColor(QColor(0,0,0));
        setButtonHighlightedText(QColor(0,0,255));
        setButtonBorder(QColor(80,80,0));
        setButtonRadius(3);

        setButtonGradient0(QColor(255,255,255));
        setButtonGradient1(QColor(140,140,140));
        setButtonGradient2(QColor(190,190,190));
        setButtonGradient3(QColor(0,0,0));

        setWindowTab(QColor(225,225,225));
        setWindowTabSelected(QColor(55,55,160));
        setWindowTabText(QColor(50,50,50));
        setWindowTabTextSelected(QColor(255,255,255));

        setMenu(QColor(255,255,255));
        setMenuBorder(QColor(220,220,220));
        setMenuText(QColor(0,0,0));
        setMenuHeight(40);

        setToolbarColor(QColor(190,220,255));
        setToolbarTextColor(QColor(0,0,0));
        setToolbarButtonText(QColor(0,0,0));
        setToolbarTextPointSize(10);
    }
    else if (style == BtStyle::crimsonTheme) {

        currentStyle = style;

        BtModuleTextModel::setLinkColor(QColor(0,0,255));
        BtModuleTextModel::setHighlightColor(QColor(0,0,255));
        BtModuleTextModel::setJesusWordsColor(QColor(170,0,0));

        setTextColor(QColor(0,0,0));
        setLinkColor(QColor(0,0,220));
        setTextBackgroundColor(QColor(255,255,255));
        setTextBackgroundHighlightColor(QColor(255,240,170));
        setInputBorderColor(QColor(99,0,0));

        setButtonColor(QColor(0,0,0));
        setButtonBackground(QColor(190,220,255));
        setButtonTextColor(QColor(0,0,0));
        setButtonHighlightedText(QColor(0,0,255));
        setButtonBorder(QColor(80,80,0));
        setButtonRadius(3);

        setButtonGradient0(QColor(255,255,255));
        setButtonGradient1(QColor(140,140,140));
        setButtonGradient2(QColor(190,190,190));
        setButtonGradient3(QColor(0,0,0));

        setWindowTab(QColor(225,225,225));
        setWindowTabSelected(QColor(255,215,0));
        setWindowTabText(QColor(50,50,50));
        setWindowTabTextSelected(QColor(0,0,0));

        setMenu(QColor(255,255,255));
        setMenuBorder(QColor(220,220,220));
        setMenuText(QColor(0,0,0));
        setMenuHeight(40);

        setToolbarColor(QColor(99,0,0));
        setToolbarTextColor(QColor(255,255,255));
        setToolbarButtonText(QColor(0,0,0));
        setToolbarTextPointSize(10);
    }
}

BtStyle::BtStyle(QObject* parent)
    : QObject(parent) {
    styles.append(this);
}

QColor BtStyle::getTextColor() {
    return textColor;
}
void BtStyle::setTextColor(const QColor& color) {
    textColor = color;
    emitChanged();
}


QColor BtStyle::getLinkColor() {
    return linkColor;
}
void BtStyle::setLinkColor(const QColor& color) {
    linkColor = color;
    emitChanged();
}


QColor BtStyle::getTextBackgroundColor() {
    return textBackgroundColor;
}
void BtStyle::setTextBackgroundColor(const QColor& color) {
    textBackgroundColor = color;
    emitChanged();
}


QColor BtStyle::getTextBackgroundHighlightColor() {
    return textBackgroundHighlightColor;
}
void BtStyle::setTextBackgroundHighlightColor(const QColor& color) {
    textBackgroundHighlightColor = color;
    emitChanged();
}


QColor BtStyle::getInputBorderColor() {
    return inputBorderColor;
}
void BtStyle::setInputBorderColor(const QColor& color) {
    inputBorderColor = color;
    emitChanged();
}


QColor BtStyle::getButtonColor() {
    return buttonColor;
}
void BtStyle::setButtonColor(const QColor& color) {
    buttonColor = color;
    emitChanged();
}


QColor BtStyle::getButtonBackground() {
    return buttonBackground;
}
void BtStyle::setButtonBackground(const QColor& color) {
    buttonBackground = color;
    emitChanged();
}


QColor BtStyle::getButtonTextColor() {
    return buttonTextColor;
}
void BtStyle::setButtonTextColor(const QColor& color) {
    buttonTextColor = color;
    emitChanged();
}


QColor BtStyle::getButtonHighlightedText() {
    return buttonHighlightedText;
}
void BtStyle::setButtonHighlightedText(const QColor& color) {
    buttonHighlightedText = color;
    emitChanged();
}


QColor BtStyle::getButtonBorder() {
    return buttonBorder;
}
void BtStyle::setButtonBorder(const QColor& color) {
    buttonBorder = color;
    emitChanged();
}

int BtStyle::getButtonRadius() {
    return buttonRadius;
}
void BtStyle::setButtonRadius(int radius) {
    buttonRadius = radius;
    emitChanged();
}

QColor BtStyle::getButtonGradient0() {
    return buttonGradient0;
}
void BtStyle::setButtonGradient0(const QColor& color) {
    buttonGradient0 = color;
    emitChanged();
}

QColor BtStyle::getButtonGradient1() {
    return buttonGradient1;
}
void BtStyle::setButtonGradient1(const QColor& color) {
    buttonGradient1 = color;
    emitChanged();
}

QColor BtStyle::getButtonGradient2() {
    return buttonGradient2;
}
void BtStyle::setButtonGradient2(const QColor& color) {
    buttonGradient2 = color;
    emitChanged();
}

QColor BtStyle::getButtonGradient3() {
    return buttonGradient3;
}
void BtStyle::setButtonGradient3(const QColor& color) {
    buttonGradient3 = color;
    emitChanged();
}

QColor BtStyle::getWindowTab() {
    return windowTab;
}

void BtStyle::setWindowTab(const QColor& color) {
    windowTab = color;
    emitChanged();
}

QColor BtStyle::getWindowTabSelected() {
    return windowTabSelected;
}

void BtStyle::setWindowTabSelected(const QColor& color) {
    windowTabSelected = color;
    emitChanged();
}

QColor BtStyle::getWindowTabText() {
    return windowTabText;
}

void BtStyle::setWindowTabText(const QColor& color) {
    windowTabText = color;
    emitChanged();
}

QColor BtStyle::getWindowTabTextSelected() {
    return windowTabTextSelected;
}

void BtStyle::setWindowTabTextSelected(const QColor& color) {
    windowTabTextSelected = color;
    emitChanged();
}

QColor BtStyle::getMenu() {
    return menu;
}

void BtStyle::setMenu(const QColor& color) {
    menu = color;
    emitChanged();
}

QColor BtStyle::getMenuBorder() {
    return menuBorder;
}

void BtStyle::setMenuBorder(const QColor& color) {
    menuBorder = color;
    emitChanged();
}

QColor BtStyle::getMenuText() {
    return menuText;
}

void BtStyle::setMenuText(const QColor& color) {
    menuText = color;
    emitChanged();
}

int BtStyle::getMenuHeight() {
    return menuHeight;
}
void BtStyle::setMenuHeight(int height) {
    menuHeight = height;
    emitChanged();
}

QColor BtStyle::getToolbarColor() {
    return toolbarColor;
}
void BtStyle::setToolbarColor(const QColor& color) {
    toolbarColor = color;
    emitChanged();
}

QColor BtStyle::getToolbarTextColor() {
    return toolbarTextColor;
}
void BtStyle::setToolbarTextColor(const QColor& color) {
    toolbarTextColor = color;
    emitChanged();
}

QColor BtStyle::getToolbarButtonText() {
    return toolbarButtonText;
}
void BtStyle::setToolbarButtonText(const QColor& color) {
    toolbarButtonText = color;
    emitChanged();
}

double BtStyle::getToolbarTextPointSize() {
    return toolbarTextPointSize;
}

void BtStyle::setToolbarTextPointSize(double pointSize) {
    toolbarTextPointSize = pointSize;
    emitChanged();
}

double BtStyle::getUiFontPointSize() {
    return btConfig().value<int>("ui/uiFontSize",16);
}

void BtStyle::setUiFontPointSize(double pointSize) {
    btConfig().setValue<int>("ui/uiFontSize", pointSize);
    emitChanged();
}

int BtStyle::pixelsPerMillimeterX() {
    QScreen* screen = QGuiApplication::screens().at(0);
    return screen->physicalDotsPerInchX() / millimeterPerInch;
}

int BtStyle::pixelsPerMillimeterY() {
    QScreen* screen = QGuiApplication::screens().at(0);
    int dpm = screen->physicalDotsPerInchY() / millimeterPerInch;
    return dpm;
}

int BtStyle::width() const {
    if (s_width == 0)
        s_width = btConfig().value<int>("ui/width",600);
    return s_width;
}
int BtStyle::height() const {
    if (s_height == 0)
        s_height = btConfig().value<int>("ui/height",500);
    return s_height;
}

void BtStyle::setWidth(int value) {
    btConfig().setValue<int>("ui/width", value);
    s_width = value;
}

void BtStyle::setHeight(int value) {
    btConfig().setValue<int>("ui/height", value);
    s_height = value;
}

QString BtStyle::getAppVersion() {
    return  "3.00";
}

QString BtStyle::getGitVersion() const {
#ifdef BT_GIT_VERSION
    return  BT_GIT_VERSION;
#else
    return "";
#endif
}

QString BtStyle::getQtVersion() const {
    QString version =  qVersion();
        return  version;
}

QString BtStyle::getSwordVersion() const {
    QString version(swordxx::SWVersion::currentVersion.getText());
    return  version;
}

QString BtStyle::getSystemLocale() const{
    return QLocale::system().name();
}

QString BtStyle::getSystemLocaleCountry() const {
    auto country = QLocale::system().country();
    return QLocale::countryToString(country);
}

QString BtStyle::getSystemLocaleLanguage() const {
    auto language = QLocale::system().language();
    return QLocale::languageToString(language);
}

} // end namespace

