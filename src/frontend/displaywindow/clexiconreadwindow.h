/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2025 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include "cdisplaywindow.h"

#include <QList>
#include <QObject>


class BtActionCollection;
class CMDIArea;
class CSwordLDKey;
class CSwordModuleInfo;

/** \brief The class used to display lexicons. */
class CLexiconReadWindow: public CDisplayWindow {

    Q_OBJECT

public: // Types:

    class ActionCollection : public CDisplayWindow::ActionCollection {

    public: // Methods:

        ActionCollection(QObject * parent = nullptr);

    };

public: // methods:

    CLexiconReadWindow(QList<CSwordModuleInfo *> const & modules,
                       QString const & key,
                       CMDIArea * parent);

public Q_SLOTS:

    void reload() override;

protected: // methods:

    void initActions() override;

protected Q_SLOTS:

    void previousEntry();
    void nextEntry();

private: // methods:

    CSwordLDKey * ldKey();

};
