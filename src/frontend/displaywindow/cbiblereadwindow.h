/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
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

class CBibleReadWindow: public CLexiconReadWindow  {

        Q_OBJECT

    public: /* Methods: */

        inline CBibleReadWindow(const QList<CSwordModuleInfo*> & modules, CMDIArea* parent)
            : CLexiconReadWindow(modules, parent) {}

        virtual CSwordModuleInfo::ModuleType moduleType() const override
        { return CSwordModuleInfo::Bible; }

        virtual void storeProfileSettings(QString const & windowGroup) const override;
        virtual void applyProfileSettings(const QString & windowGroup) override;
        static void insertKeyboardActions( BtActionCollection* const a );

    protected: /* Methods: */

        virtual void initActions() override;
        virtual void initToolbars() override;
        virtual void initConnections() override;
        virtual void initView() override;
        /** Called to add actions to mainWindow toolbars */
        virtual void setupMainWindowToolBars() override;
        /**
        * Reimplementation.
        */
        virtual void setupPopupMenu() override;
        /**
        * Reimplemented.
        */
        virtual void updatePopupMenu() override;
        /** Event filter.
        * Reimplementation of the event filter to filter out events like focus in.
        */
        virtual bool eventFilter( QObject* o, QEvent* e) override;

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
        virtual void reload(CSwordBackend::SetupChangedReason reason) override;

    protected slots:

        /**
        * Copies the current chapter into the clipboard.
        */
        void copyDisplayedText() override;
        /**
        * Saves the chapter as valid HTML page.
        */
        void saveChapterHTML();
        /**
        * Saves the chapter as valid HTML page.
        */
        void saveChapterPlain();
        virtual void lookupSwordKey( CSwordKey* newKey ) override;
        void syncWindows();

    private: /* Methods: */

        /**
        * Wrapper around key() to return the right type of key.
        */
        CSwordVerseKey* verseKey();

};

#endif /* CBIBLEREADWINDOW_H */
