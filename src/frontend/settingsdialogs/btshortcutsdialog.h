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

#ifndef BT_SHORTCUTS_DIALOG_H
#define BT_SHORTCUTS_DIALOG_H

#include <QDialog>


class QLabel;
class QRadioButton;

// *************** BtShortcutsDialog ***************************************************************************
// A dialog to allow the user to input a shortcut for a primary and alternate key

class BtShortcutsDialog : public QDialog {
        Q_OBJECT;
    public:
        BtShortcutsDialog(QWidget* parent);


        // get new first keys from dialog
        QString getFirstKeys();

        // set the initial value of the first keys
        void setFirstKeys(const QString& keys);

        // get new second keys from dialog
        QString getSecondKeys();

        // set the initial value of the second keys
        void setSecondKeys(const QString& keys);

        // change the First or Second shortcut in the dialog
        void changeSelectedShortcut(const QString& keys);

    signals:
        // make a keyChangeRequest back to the application
        void keyChangeRequest(const QString& keys);

    protected:
        // get key from users input, put into primary or alternate label for display to user
        void keyReleaseEvent(QKeyEvent* event) override;

    private:
        QLabel* m_primaryLabel;
        QLabel* m_alternateLabel;
        QRadioButton* m_primaryButton;
        QRadioButton* m_alternateButton;
};

#endif
