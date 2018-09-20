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

#ifndef CBOOKREADWINDOW_H
#define CBOOKREADWINDOW_H

#include "frontend/displaywindow/clexiconreadwindow.h"


class BtActionCollection;
class CBookTreeChooser;
class QAction;

class CBookReadWindow: public CLexiconReadWindow {

    Q_OBJECT

public: /* Methods: */

    inline CBookReadWindow(const QList<CSwordModuleInfo *> & modules, CMDIArea * parent)
        : CLexiconReadWindow(modules, parent)
        , m_treeAction(nullptr)
        , m_treeChooser(nullptr) {}

    CSwordModuleInfo::ModuleType moduleType() const override
    { return CSwordModuleInfo::GenericBook; }

    void storeProfileSettings(QString const & windowGroup) const override;
    void applyProfileSettings(const QString & windowGroup) override;
    static void insertKeyboardActions(BtActionCollection * const a);

public slots:

    void reload(CSwordBackend::SetupChangedReason reason) override;

protected: /* Methods: */

    void initActions() override;
    void initToolbars() override;
    void initConnections() override;
    void initView() override;
    void setupMainWindowToolBars() override;

    void setupPopupMenu() override;

protected slots:

    void modulesChanged() override;

private slots:

    /**
        * Is called when the action was executed to toggle the tree view.
        */
    void treeToggled();

private: /* Fields: */

    QAction * m_treeAction;
    CBookTreeChooser * m_treeChooser;
};

#endif
