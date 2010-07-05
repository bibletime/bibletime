/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CACCELERATORSETTINGS_H
#define CACCELERATORSETTINGS_H

#include "frontend/bookshelfmanager/btconfigdialog.h"
#include "util/cpointers.h"

#include <QPointer>
#include <QWidget>
#include "frontend/settingsdialogs/btshortcutseditor.h"


class BtActionCollection;
class BtShortcutsEditor;
class QComboBox;
class QStackedWidget;

/**
    @author The BibleTime team <info@bibletime.info>
*/
class CAcceleratorSettingsPage : public BtConfigPage {
        Q_OBJECT
    public:
        CAcceleratorSettingsPage(QWidget *parent = 0);
        ~CAcceleratorSettingsPage();
        void save();

        /** Reimplemented from BtConfigPage. */
        virtual const QIcon &icon() const;

        /** Reimplemented from BtConfigPage. */
        virtual QString label() const;

        /** Reimplemented from BtConfigPage. */
        virtual QString header() const;

    protected slots:

        void slotKeyChooserTypeChanged(const QString& title);

        // complete the keyChangeRequest
        void completeKeyChangeRequest(BtShortcutsEditor* shortcutsEditor, const QString& keys);

    private:
        struct WindowType {
            QPointer<BtShortcutsEditor> keyChooser;
            BtActionCollection* actionCollection;
            QString title;

            WindowType() {
                keyChooser = 0;
                actionCollection = 0;
            };
            WindowType(const QString& newTitle) {
                title = newTitle;
                keyChooser = 0;
                actionCollection = 0;
            }
        };

        void clearConflictsWithKeys(const QString& keys, const QList<BtShortcutsEditor*> list);
        QString findConflictsWithKeys(const QString& keys, const QList<BtShortcutsEditor*> list);
        QList<BtShortcutsEditor*> getShortcutsEditorListForGroup(BtShortcutsEditor* currentEditor);
        QString getTitleForEditor(BtShortcutsEditor* editor);

        WindowType m_application;
        WindowType m_general;
        WindowType m_bible;
        WindowType m_commentary;
        WindowType m_lexicon;
        WindowType m_book;

        QComboBox* m_typeChooser;
        QStackedWidget* m_keyChooserStack;

};

#endif
