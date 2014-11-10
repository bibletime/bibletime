/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
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

    Q_PROPERTY(double pixelsPerMillimeterX   READ pixelsPerMillimeterX NOTIFY changed);
    Q_PROPERTY(double pixelsPerMillimeterY   READ pixelsPerMillimeterY NOTIFY changed);

    Q_PROPERTY(QString appVersion            READ getAppVersion        NOTIFY versionChanged);
    Q_PROPERTY(QString gitVersion            READ getGitVersion        NOTIFY versionChanged);
    Q_PROPERTY(QString qtVersion             READ getQtVersion         NOTIFY versionChanged);
    Q_PROPERTY(QString swordVersion          READ getSwordVersion      NOTIFY versionChanged);


public:
    Q_INVOKABLE void setStyle(int style);

    enum Style {
        nightTheme = 1,
        dayTheme = 2
    };


    BtStyle(QObject *parent = 0);

    QColor getTextColor() const;
    void setTextColor(const QColor& color);

    QColor getLinkColor() const;
    void setLinkColor(const QColor& color);

    QColor getTextBackgroundColor() const;
    void setTextBackgroundColor(const QColor& color);

    QColor getTextBackgroundHighlightColor() const;
    void setTextBackgroundHighlightColor(const QColor& color);

    QColor getButtonColor() const;
    void setButtonColor(const QColor& color);

    QColor getButtonBackground() const;
    void setButtonBackground(const QColor& color);

    QColor getButtonTextColor() const;
    void setButtonTextColor(const QColor& color);

    QColor getButtonHighlightedText() const;
    void setButtonHighlightedText(const QColor& color);

    QColor getButtonBorder() const;
    void setButtonBorder(const QColor& color);

    int getButtonRadius() const;
    void setButtonRadius(int radius);

    QColor getButtonGradient0() const;
    void setButtonGradient0(const QColor& color);

    QColor getButtonGradient1() const;
    void setButtonGradient1(const QColor& color);

    QColor getButtonGradient2() const;
    void setButtonGradient2(const QColor& color);

    QColor getButtonGradient3() const;
    void setButtonGradient3(const QColor& color);

    QColor getWindowTab() const;
    void setWindowTab(const QColor& color);

    QColor getWindowTabSelected() const;
    void setWindowTabSelected(const QColor& color);

    QColor getWindowTabText() const;
    void setWindowTabText(const QColor& color);

    QColor getWindowTabTextSelected() const;
    void setWindowTabTextSelected(const QColor& color);

    QColor getMenu() const;
    void setMenu(const QColor& color);

    QColor getMenuBorder() const;
    void setMenuBorder(const QColor& color);

    QColor getMenuText() const;
    void setMenuText(const QColor& color);

    int getMenuHeight() const;
    void setMenuHeight(int height);

    QColor getToolbarColor() const;
    void setToolbarColor(const QColor& color);

    QColor getToolbarTextColor() const;
    void setToolbarTextColor(const QColor& color);

    QColor getToolbarButtonText() const;
    void setToolbarButtonText(const QColor& color);

    double getToolbarTextPointSize() const;
    void setToolbarTextPointSize(double pointSize);

    double getUiFontPointSize() const;
    void setUiFontPointSize(double pointSize);

    int pixelsPerMillimeterX() const;
    int pixelsPerMillimeterY() const;

    QString getAppVersion() const;
    QString getGitVersion() const;
    QString getQtVersion() const;
    QString getSwordVersion() const;

signals:
    void changed();
    void versionChanged();
};

} // end namespace

#endif
