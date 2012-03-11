/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CLEXICONREADWINDOW_H
#define CLEXICONREADWINDOW_H

#include "frontend/displaywindow/creadwindow.h"

#include "frontend/keychooser/ckeychooser.h"


class BtActionCollection;
class BtToolBarPopupAction;
class CSwordLDKey;
class CSwordKey;
class QAction;
class QMenu;

/** The class used to display lexicons. It is also used as the class that other display window types are derived from.
  *
  * Inherits CReadWindow.
  *
  * Inherited by CBibleReadWindow, CBookReadWindow, and CCommentaryReadWindow.
  *
  * @author The BibleTime team
  */
class CLexiconReadWindow : public CReadWindow  {
        Q_OBJECT
    public:
        CLexiconReadWindow(const QList<CSwordModuleInfo *> & modules, CMDIArea * parent);
        virtual ~CLexiconReadWindow();

        /** Insert the keyboard accelerators of this window into the given actioncollection.*/
        static void insertKeyboardActions( BtActionCollection* const a );

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
        virtual void updatePopupMenu();
        virtual void setupPopupMenu();

        /** Called to add actions to mainWindow toolbars */
        virtual void setupMainWindowToolBars();

        struct ActionsStruct {
            BtToolBarPopupAction* backInHistory;
            BtToolBarPopupAction* forwardInHistory;

            QAction* selectAll;
            QAction* findText;
            QAction* findStrongs;

            QMenu* copyMenu;
            struct {
                QAction* reference;
                QAction* entry;
                QAction* selectedText;
            }
            copy;

            QMenu* saveMenu;
            struct {
                QAction* reference;
                QAction* entryAsPlain;
                QAction* entryAsHTML;
            }
            save;

            QMenu* printMenu;
            struct {
                QAction* reference;
                QAction* entry;
            }
            print;
        }
        m_actions;

    private:
        /**
        * Reimplementation to return the right key.
        */
        CSwordLDKey* ldKey();

    protected slots: // Protected slots
        void previousEntry();
        void nextEntry();
        /**
        * This function saves the entry as html using the CExportMgr class.
        */
        void saveAsHTML();
        /**
        * This function saves the entry as plain text using the CExportMgr class.
        */
        void saveAsPlain();
        void saveRawHTML();

        void slotFillBackHistory();
        void slotFillForwardHistory();

        void slotUpdateHistoryButtons(bool backEnabled, bool fwEnabled);
};

#endif
