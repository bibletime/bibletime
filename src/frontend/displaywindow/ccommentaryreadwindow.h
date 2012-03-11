/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CCOMMENTARYREADWINDOW_H
#define CCOMMENTARYREADWINDOW_H

#include "frontend/displaywindow/cbiblereadwindow.h"


class BtActionCollection;
class CSwordVerseKey;
class QAction;

/**
  *@author The BibleTime team
  */
class CCommentaryReadWindow : public CLexiconReadWindow  {
        Q_OBJECT
    public:
        /**
        * Reimplementation.
        */
        static void insertKeyboardActions( BtActionCollection* const a );

        inline CCommentaryReadWindow(const QList<CSwordModuleInfo *> & modules, CMDIArea * parent)
            : CLexiconReadWindow(modules, parent) {}

        virtual void storeProfileSettings(const QString & windowGroup);
        virtual void applyProfileSettings(const QString & windowGroup);
        virtual bool syncAllowed() const;

    public slots: // Public slots
        void nextBook();
        void previousBook();
        void nextChapter();
        void previousChapter();
        void nextVerse();
        void previousVerse();
        /**
        * Reimplementation to handle the keychooser refresh.
        */
        virtual void reload(CSwordBackend::SetupChangedReason);

    protected:
        virtual void initActions();
        virtual void initToolbars();
        /** Called to add actions to mainWindow toolbars */
        virtual void setupMainWindowToolBars();

    private:
        QAction* m_syncButton;
        CSwordVerseKey* verseKey();
    protected:
        virtual void setupPopupMenu();
};

#endif
