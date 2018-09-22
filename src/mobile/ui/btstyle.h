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

#ifndef BT_STYLE_H
#define BT_STYLE_H

#include <QObject>
#include <QColor>

namespace btm {

class BtStyle : public QObject {
    Q_OBJECT

    Q_PROPERTY(QColor textColor             READ getTextColor NOTIFY changed)
    Q_PROPERTY(QColor linkColor             READ getLinkColor NOTIFY changed)
    Q_PROPERTY(QColor textBackgroundColor   READ getTextBackgroundColor NOTIFY changed)
    Q_PROPERTY(QColor textBackgroundHighlightColor   READ getTextBackgroundHighlightColor NOTIFY changed)
    Q_PROPERTY(QColor inputBorderColor      READ getInputBorderColor WRITE setInputBorderColor NOTIFY changed)

    Q_PROPERTY(QColor buttonColor           READ getButtonColor NOTIFY changed)
    Q_PROPERTY(QColor buttonBackground      READ getButtonBackground NOTIFY changed)
    Q_PROPERTY(QColor buttonTextColor       READ getButtonTextColor NOTIFY changed)
    Q_PROPERTY(QColor buttonHighlightedText READ getButtonHighlightedText NOTIFY changed)
    Q_PROPERTY(QColor buttonBorder          READ getButtonBorder NOTIFY changed)
    Q_PROPERTY(int    buttonRadius          READ getButtonRadius NOTIFY changed)

    Q_PROPERTY(QColor buttonGradient0       READ getButtonGradient0 NOTIFY changed)
    Q_PROPERTY(QColor buttonGradient1       READ getButtonGradient1 NOTIFY changed)
    Q_PROPERTY(QColor buttonGradient2       READ getButtonGradient2 NOTIFY changed)
    Q_PROPERTY(QColor buttonGradient3       READ getButtonGradient3 NOTIFY changed)


    Q_PROPERTY(QColor windowTab             READ getWindowTab NOTIFY changed)
    Q_PROPERTY(QColor windowTabSelected     READ getWindowTabSelected NOTIFY changed)
    Q_PROPERTY(QColor windowTabText         READ getWindowTabText NOTIFY changed)
    Q_PROPERTY(QColor windowTabTextSelected READ getWindowTabTextSelected NOTIFY changed)

    Q_PROPERTY(QColor menu                  READ getMenu NOTIFY changed)
    Q_PROPERTY(QColor menuBorder            READ getMenuBorder NOTIFY changed)
    Q_PROPERTY(QColor menuText              READ getMenuText NOTIFY changed)
    Q_PROPERTY(int    menuHeight            READ getMenuHeight() NOTIFY changed)

    Q_PROPERTY(QColor toolbarColor          READ getToolbarColor NOTIFY changed)
    Q_PROPERTY(QColor toolbarTextColor      READ getToolbarTextColor NOTIFY changed)
    Q_PROPERTY(QColor toolbarButtonText     READ getToolbarButtonText NOTIFY changed)
    Q_PROPERTY(double toolbarTextPointSize  READ getToolbarTextPointSize NOTIFY changed);

    Q_PROPERTY(double uiFontPointSize       READ getUiFontPointSize   WRITE setUiFontPointSize   NOTIFY changed);

    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged);
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged);

    Q_PROPERTY(double pixelsPerMillimeterX   READ pixelsPerMillimeterX NOTIFY changed);
    Q_PROPERTY(double pixelsPerMillimeterY   READ pixelsPerMillimeterY NOTIFY changed);

    Q_PROPERTY(QString appVersion            READ getAppVersion        NOTIFY versionChanged);
    Q_PROPERTY(QString gitVersion            READ getGitVersion        NOTIFY versionChanged);
    Q_PROPERTY(QString qtVersion             READ getQtVersion         NOTIFY versionChanged);
    Q_PROPERTY(QString swordVersion          READ getSwordVersion      NOTIFY versionChanged);

    Q_PROPERTY(QString systemLocale READ getSystemLocale NOTIFY systemLocaleChanged);
    Q_PROPERTY(QString systemLocaleCountry READ getSystemLocaleCountry NOTIFY systemLocaleCountryChanged);
    Q_PROPERTY(QString systemLocaleLanguage READ getSystemLocaleLanguage NOTIFY systemLocaleLanguageChanged);

public:
    Q_INVOKABLE void setStyle(int style);
    Q_INVOKABLE int getStyle();

    enum Style {
        darkTheme = 1,
        lightBlueTheme = 2,
        crimsonTheme = 3
    };

    static int getCurrentStyle();
    static void setCurrentStyle(int style);

    BtStyle(QObject *parent = nullptr);

    static QColor getTextColor();
    static void setTextColor(const QColor& color);

    static QColor getLinkColor();
    static void setLinkColor(const QColor& color);

    static QColor getTextBackgroundColor();
    static void setTextBackgroundColor(const QColor& color);

    static QColor getTextBackgroundHighlightColor();
    static void setTextBackgroundHighlightColor(const QColor& color);

    static QColor getInputBorderColor();
    static void setInputBorderColor(const QColor& color);

    static QColor getButtonColor();
    static void setButtonColor(const QColor& color);

    static QColor getButtonBackground();
    static void setButtonBackground(const QColor& color);

    static QColor getButtonTextColor();
    static void setButtonTextColor(const QColor& color);

    static QColor getButtonHighlightedText();
    static void setButtonHighlightedText(const QColor& color);

    static QColor getButtonBorder();
    static void setButtonBorder(const QColor& color);

    static int getButtonRadius();
    static void setButtonRadius(int radius);

    static QColor getButtonGradient0();
    static void setButtonGradient0(const QColor& color);

    static QColor getButtonGradient1();
    static void setButtonGradient1(const QColor& color);

    static QColor getButtonGradient2();
    static void setButtonGradient2(const QColor& color);

    static QColor getButtonGradient3();
    static void setButtonGradient3(const QColor& color);

    static QColor getWindowTab();
    static void setWindowTab(const QColor& color);

    static QColor getWindowTabSelected();
    static void setWindowTabSelected(const QColor& color);

    static QColor getWindowTabText();
    static void setWindowTabText(const QColor& color);

    static QColor getWindowTabTextSelected();
    static void setWindowTabTextSelected(const QColor& color);

    static QColor getMenu();
    static void setMenu(const QColor& color);

    static QColor getMenuBorder();
    static void setMenuBorder(const QColor& color);

    static QColor getMenuText();
    static void setMenuText(const QColor& color);

    static int getMenuHeight();
    static void setMenuHeight(int height);

    static QColor getToolbarColor();
    static void setToolbarColor(const QColor& color);

    static QColor getToolbarTextColor();
    static void setToolbarTextColor(const QColor& color);

    static QColor getToolbarButtonText();
    static void setToolbarButtonText(const QColor& color);

    static double getToolbarTextPointSize();
    static void setToolbarTextPointSize(double pointSize);

    static double getUiFontPointSize();
    static void setUiFontPointSize(double pointSize);

    int width() const;
    int height() const;

    void setWidth(int value);
    void setHeight(int value);

    static int pixelsPerMillimeterX();
    static int pixelsPerMillimeterY();

    static QString getAppVersion();
    QString getGitVersion() const;
    QString getQtVersion() const;
    QString getSwordVersion() const;

    QString getSystemLocale() const;
    QString getSystemLocaleCountry() const;
    QString getSystemLocaleLanguage() const;

signals:
    void changed();
    void versionChanged();
    void widthChanged();
    void heightChanged();
    void systemLocaleChanged();
    void systemLocaleCountryChanged();
    void systemLocaleLanguageChanged();
};

} // end namespace

#endif
