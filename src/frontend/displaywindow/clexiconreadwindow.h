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
    ~CLexiconReadWindow() override;

    CSwordModuleInfo::ModuleType moduleType() const override
    { return CSwordModuleInfo::Lexicon; }

    /** Insert the keyboard accelerators of this window into the given actioncollection.*/
    static void insertKeyboardActions( BtActionCollection* const a );

public slots:
    void reload(CSwordBackend::SetupChangedReason reason) override;

protected:

    template <typename ... Args>
    QAction & initAction(QString actionName, Args && ... args) {
        QAction & action =
                actionCollection()->action(std::move(actionName));
        BT_CONNECT(&action,
                   &QAction::triggered,
                   std::forward<Args>(args)...);
        addAction(&action);
        return action;
    }

    void initActions() override;
    void initToolbars() override;
    void initConnections() override;
    void initView() override;
    void updatePopupMenu() override;
    void setupPopupMenu() override;

    void setupMainWindowToolBars() override;

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
