/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTEDITBOOKMARKDIALOG_H
#define BTEDITBOOKMARKDIALOG_H

#include <QDialog>


class QLineEdit;
class QTextEdit;
class QWidget;

/** This dialog box is design for editing bookmark
 * titles and descriptions
  * @author The BibleTime team
  */
class BtEditBookmarkDialog : public QDialog  {
        Q_OBJECT
    public:
        BtEditBookmarkDialog(const QString& caption, const QString& keyText,
			     const QString& title, const QString& titleText, 
			     const QString& description, const QString& descriptionText, 
			     QWidget *parent = 0, Qt::WindowFlags wflags = Qt::Dialog);
        /**
        * A static function to get some using BtEditBookmarkDialog.
        */
        static void getText( const QString& caption, const QString& keyText,
			     const QString& title, QString* titleText, 
			     const QString& description, QString* descriptionText, 
				      bool* ok = 0, QWidget* parent = 0, Qt::WindowFlags wflags = Qt::Dialog);
        /**
        * Returns the description.
        */
        const QString descriptionText();

        /**
        * Returns the title.
        */
        const QString titleText();
	
	
    private:
        QLineEdit* m_titleEdit;
        QTextEdit* m_descriptionEdit;
};

#endif
