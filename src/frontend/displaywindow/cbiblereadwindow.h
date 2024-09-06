/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include "cdisplaywindow.h"

#include <QAction>
#include "../cexportmanager.h"
#include "btactioncollection.h"


class CSwordKey;
class CSwordVerseKey;
class QAction;
class QEvent;
class QMenu;
class QObject;

class CBibleReadWindow: public CDisplayWindow  {

    Q_OBJECT

public: // methods:

    CBibleReadWindow(QList<CSwordModuleInfo *> const & modules,
                     CMDIArea * parent)
        : CDisplayWindow(modules, true, parent)
    { init(); }

    CSwordModuleInfo::ModuleType moduleType() const override
    { return CSwordModuleInfo::Bible; }

    void storeProfileSettings(BtConfigCore & windowConf) const override;
    void applyProfileSettings(BtConfigCore const & windowConf) override;
    static void insertKeyboardActions( BtActionCollection* const a );

protected: // methods:

    void initActions() override;
    QMenu * newDisplayWidgetPopupMenu() override;

    struct {
        QAction* findText;
        QAction* findStrongs;

        QMenu* copyMenu;
        struct {
            QAction* referenceOnly;
            QAction* referenceTextOnly;
            QAction* referenceAndText;
            QAction* chapter;
            QAction* selectedText;
            QAction* byReferences;
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

#ifdef BUILD_TEXT_TO_SPEECH
        QAction* speakSelectedText;
#endif
    }
    m_actions;


public Q_SLOTS:

    void nextBook();
    void previousBook();
    void nextChapter();
    void previousChapter();
    void nextVerse();
    void previousVerse();

    void reload() override;

protected Q_SLOTS:

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
    void lookupSwordKey(CSwordKey * newKey) override;
    void syncWindows();

private: // methods:

    /**
        * Wrapper around key() to return the right type of key.
        */
    CSwordVerseKey* verseKey();

    void saveChapter(CExportManager::Format const format);

};
