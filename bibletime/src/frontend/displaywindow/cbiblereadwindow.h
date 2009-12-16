/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CBIBLEREADWINDOW_H
#define CBIBLEREADWINDOW_H

#include "frontend/displaywindow/clexiconreadwindow.h"


class BtActionCollection;
class CSwordKey;
class CSwordVerseKey;
class CTransliterationButton;
class QAction;
class QEvent;
class QMenu;
class QObject;

/** The read display window for Bibles.
  *@author The BibleTime team
  */
class CBibleReadWindow : public CLexiconReadWindow  {
        Q_OBJECT
    public:
        CBibleReadWindow(QList<CSwordModuleInfo*> modules, CMDIArea* parent);
        virtual ~CBibleReadWindow();
        /**
        * Store the settings of this window in the given CProfileWindow object.
        */
        virtual void storeProfileSettings( Profile::CProfileWindow* const settings );
        /**
        * Store the settings of this window in the given profile window.
        */
        virtual void applyProfileSettings( Profile::CProfileWindow* const settings );
        /**
        * Reimplementation.
        */
        static void insertKeyboardActions( BtActionCollection* const a );

    protected:
        virtual void initActions();
        virtual void initToolbars();
        virtual void initConnections();
        virtual void initView();
        /**
        * Reimplementation.
        */
        virtual void setupPopupMenu();
        /**
        * Reimplemented.
        */
        virtual void updatePopupMenu();
        /** Event filter.
        * Reimplementation of the event filter to filter out events like focus in.
        */
        virtual bool eventFilter( QObject* o, QEvent* e);

        struct {
            QAction* selectAll;
            QAction* findText;
            QAction* findStrongs;

            QMenu* copyMenu;
            struct {
                QAction* referenceOnly;
                QAction* referenceTextOnly;
                QAction* referenceAndText;
                QAction* chapter;
                QAction* selectedText;
            }
            copy;

            QMenu* saveMenu;
            struct {
                QAction* referenceAndText;
                QAction* chapterAsPlain;
                QAction* chapterAsHTML;
            }
            save;

            QMenu* printMenu;
            struct {
                QAction* reference;
                QAction* chapter;
            }
            print;
        }
        m_actions;


    public slots:
        void nextBook();
        void previousBook();
        void nextChapter();
        void previousChapter();
        void nextVerse();
        void previousVerse();
        /**
        * Refreshes the content of this display window and the content of the keychooser.
        */
        virtual void reload(CSwordBackend::SetupChangedReason reason);

    protected slots: // Protected slots
        /**
        * Copies the current chapter into the clipboard.
        */
        void copyDisplayedText();
        /**
        * Saves the chapter as valid HTML page.
        */
        void saveChapterHTML();
        /**
        * Saves the chapter as valid HTML page.
        */
        void saveChapterPlain();
        virtual void lookupSwordKey( CSwordKey* newKey );
        void syncWindows();

    private:
        friend class CCommentaryReadWindow;
        /**
        * Wrapper around key() to return the right type of key.
        */
        CSwordVerseKey* verseKey();

        //  CTransliterationButton* m_transliterationButton;
};

#endif
