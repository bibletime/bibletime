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

#ifndef CBIBLEREADWINDOW_H
#define CBIBLEREADWINDOW_H

#include "frontend/displaywindow/clexiconreadwindow.h"

#include <QAction>
#include "frontend/displaywindow/btactioncollection.h"
#include "util/btconnect.h"


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

    CSwordModuleInfo::ModuleType moduleType() const override
    { return CSwordModuleInfo::Bible; }

    void storeProfileSettings(QString const & windowGroup) const override;
    void applyProfileSettings(const QString & windowGroup) override;
    static void insertKeyboardActions( BtActionCollection* const a );

protected: /* Methods: */

    template <typename ... Args>
    QAction & initAction(QString actionName, Args && ... args) {
        QAction & action = m_actionCollection->action(std::move(actionName));
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
    void setupMainWindowToolBars() override;
    void setupPopupMenu() override;
    void updatePopupMenu() override;
    bool eventFilter( QObject* o, QEvent* e) override;

    struct {
        QAction* findText;
        QAction* findStrongs;

        QMenu* copyMenu;
        struct {
            QAction* referenceOnly;
            QAction* referenceTextOnly;
            QAction* referenceAndText;
            QAction* chapter;
            QAction* referencedText;
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

    void reload(CSwordBackend::SetupChangedReason reason) override;

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
    void lookupSwordKey(CSwordKey * newKey) override;
    void syncWindows();

private: /* Methods: */

    /**
        * Wrapper around key() to return the right type of key.
        */
    CSwordVerseKey* verseKey();

};

#endif /* CBIBLEREADWINDOW_H */
