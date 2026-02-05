/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2026 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include "cdisplaywindow.h"


class BtActionCollection;
class CSwordVerseKey;
class QAction;

/**
  *@author The BibleTime team
  */
class CCommentaryReadWindow : public CDisplayWindow  {
        Q_OBJECT

public: // Types:

    class ActionCollection : public CDisplayWindow::ActionCollection {

    public: // Methods:

        ActionCollection(QObject * parent = nullptr);

    };

    public:

        CCommentaryReadWindow(QList<CSwordModuleInfo *> const & modules,
                              QString const & key,
                              CMDIArea * parent);

        CSwordModuleInfo::ModuleType moduleType() const override
        { return CSwordModuleInfo::Commentary; }

        void storeProfileSettings(BtConfigCore & windowConf) const override;
        void applyProfileSettings(BtConfigCore const & windowConf) override;
        bool syncAllowed() const noexcept override;

    public Q_SLOTS:
        void nextBook();
        void previousBook();
        void nextChapter();
        void previousChapter();
        void nextVerse();
        void previousVerse();
        void reload() override;

    protected:
        void initActions() override;
        void initToolbars() override;
        void setupMainWindowToolBars() override;

    private:
        QAction* m_syncButton;
        CSwordVerseKey* verseKey();
};
