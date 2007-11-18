/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CINPUTDIALOG_H
#define CINPUTDIALOG_H


#include <QDialog>

class QTextEdit;
class QWidget;

/** This is a small input dialog with
 * a multiline edit for the text input.
  * @author The BibleTime team
  */
class CInputDialog : public QDialog  {
	Q_OBJECT
public:
	CInputDialog(const QString& caption, const QString& description, const QString& text, QWidget *parent=0, Qt::WindowFlags wflags = Qt::Dialog);
	/**
	* A static function to get some using CInputDialog.
	*/
	static const QString getText( const QString& caption, const QString& description, const QString& text = QString::null, bool* ok = 0, QWidget* parent = 0, Qt::WindowFlags wflags = Qt::Dialog);
	/**
	* Returns the text entered at the moment.
	*/
	const QString text();
	// ~CInputDialog();

private:
	QTextEdit* m_textEdit;
};

#endif
