/*********
*
* This file is part of BibleTime's BtActionCollection code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime BtActionCollection code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CBOOKREADWINDOW_H
#define CBOOKREADWINDOW_H

#include "frontend/displaywindow/clexiconreadwindow.h"


class BtActionCollection;
class CBookTreeChooser;
class QAction;

/**
  * @author The BibleTime team
  */
class CBookReadWindow : public CLexiconReadWindow  {
    Q_OBJECT
    public:
        static void insertKeyboardActions( BtActionCollection* const a );

        CBookReadWindow(QList<CSwordModuleInfo*> modules, CMDIArea* parent);

        virtual ~CBookReadWindow();
        /**
        * Store the settings of this window in the given CProfileWindow object.
        */
        virtual void storeProfileSettings( Profile::CProfileWindow* profileWindow );
        /**
        * Store the settings of this window in the given profile window.
        */
        virtual void applyProfileSettings( Profile::CProfileWindow* profileWindow );

    public slots:
        /**
        * Refreshes the content of this display window and the content of the keychooser.
        */
        virtual void reload(CSwordBackend::SetupChangedReason reason);

    protected:
        virtual void initActions();
        virtual void initToolbars();
        virtual void initConnections();
        virtual void initView();

        virtual void setupPopupMenu();

    protected slots: // Protected slots
        /**
         * Reimplementation to take care of the tree chooser.
         */
        virtual void modulesChanged();

    private:
        QAction* m_treeAction;
        CBookTreeChooser* m_treeChooser;

    private slots: // Private slots
        /**
        * Is called when the action was executed to toggle the tree view.
        */
        void treeToggled();
};

#endif
