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

#include <QWidget>

#include <QKeySequence>
#include <QObject>
#include <QString>


class BtActionCollection;
class BtShortcutsDialog;
class QGroupBox;
class QLabel;
class QPushButton;
class QRadioButton;
class QTableWidget;

// This class is the table in the center of the Shortcuts page of the config dialog
class BtShortcutsEditor : public QWidget {
        Q_OBJECT
    public:
        BtShortcutsEditor(BtActionCollection * collection, QWidget * parent);

        // saves shortcut keys into the QAction
        void commitChanges();

        // clears any shortcut keys in the table matching the specified keys
        void clearConflictWithKeys(QKeySequence const & keys);

        // finds any shortcut keys in the table matching the specified keys - returns the Action Name for it.
        QString findConflictWithKeys(QKeySequence const & keys) const;

        // used by application to complete the keyChangeRequest signal
        // stores "keys" into the custom shortcuts dialog field
        void changeShortcutInDialog(QKeySequence const & keys);

    Q_SIGNALS:

        void keyChangeRequest(QString const & actionName,
                              QKeySequence const & keys);

    private Q_SLOTS:

        // called when a different action name row is selected
        void slotSelectionChanged();

        // called when the none radio button is clicked
        void noneButtonClicked(bool checked);

        // called when the default radio button is clicked
        void defaultButtonClicked(bool checked);

        // called when the custom radio button is clicked
        void customButtonClicked(bool checked);

    private:

        BtShortcutsDialog* m_dlg;
        QTableWidget* m_table;
        QGroupBox * m_shortcutChooser;
        QRadioButton* m_noneButton;
        QRadioButton* m_defaultButton;
        QRadioButton* m_customButton;
        QPushButton* m_customPushButton;
        QLabel* m_defaultLabelValue;
        int m_currentRow;
};
