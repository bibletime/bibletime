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

#ifndef BTEDITBOOKMARKDIALOG_H
#define BTEDITBOOKMARKDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>

class QDialogButtonBox;
class QFormLayout;
class QLabel;
class QWidget;

/**
  \brief A dialog box for editing bookmarks.
*/
class BtEditBookmarkDialog : public QDialog  {
        Q_OBJECT

    public: /* Methods: */
        BtEditBookmarkDialog(const QString &key,
                             const QString &title,
                             const QString &description,
                             QWidget *parent = nullptr,
                             Qt::WindowFlags wflags = Qt::Dialog);

        /**
        * Returns the description written in the description box.
        */
        inline const QString descriptionText() {
            return m_descriptionEdit->toPlainText();
        }

        /**
        * Returns the title written in the title box.
        */
        inline const QString titleText() { return m_titleEdit->text(); }

    protected: /* Methods: */
        void retranslateUi();

    private: /* Fields: */
        QFormLayout *m_layout;
            QLabel *m_keyLabel;
                QLabel *m_keyTextLabel;
            QLabel *m_titleLabel;
                QLineEdit *m_titleEdit;
            QLabel *m_descriptionLabel;
                QTextEdit *m_descriptionEdit;
        QDialogButtonBox *m_buttonBox;

};

#endif
