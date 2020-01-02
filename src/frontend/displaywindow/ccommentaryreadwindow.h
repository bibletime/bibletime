/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef CCOMMENTARYREADWINDOW_H
#define CCOMMENTARYREADWINDOW_H

#include "clexiconreadwindow.h"


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

        CSwordModuleInfo::ModuleType moduleType() const override
        { return CSwordModuleInfo::Commentary; }

        void storeProfileSettings(QString const & windowGroup) const override;
        void applyProfileSettings(const QString & windowGroup) override;
        bool syncAllowed() const override;

    public slots:
        void nextBook();
        void previousBook();
        void nextChapter();
        void previousChapter();
        void nextVerse();
        void previousVerse();
        void reload(CSwordBackend::SetupChangedReason) override;

    protected:
        void initActions() override;
        void initToolbars() override;
        void setupMainWindowToolBars() override;

    private:
        QAction* m_syncButton;
        CSwordVerseKey* verseKey();
    protected:
        void setupPopupMenu() override;
};

#endif
