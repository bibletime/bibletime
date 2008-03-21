/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTCONFIGDIALOG_H
#define BTCONFIGDIALOG_H


#include <QDialog>
#include <QWidget>

#include <QDebug>

class BtConfigPage;

class QListWidgetItem;
class QListWidget;
class QStackedWidget;
class QDialogButtonBox;
class QVBoxLayout;

/**
* Base class for configuration dialogs. A dialog which has a page chooser (icons
* + text) at the left, widget pages and a buttonbox.
*
* Usage: add BtConfigPage pages with addPage(), add a button box with addButtonBox().
* Connect the button box signals. Use setAttribute(Qt::WA_DeleteOnClose) if you want
* an auto-destroying window.
*/
class BtConfigDialog : public QDialog
{
	Q_OBJECT
public:
	BtConfigDialog(QWidget* parent);
	virtual ~BtConfigDialog();
	
	/** Add a BtConfigPage to the paged widget stack. */
	void addPage(BtConfigPage* pageWidget);
	/** Adds a button box to the lower edge of the dialog. */
	void addButtonBox(QDialogButtonBox* buttonBox);
	
	/** Return the currently selected page. */
	BtConfigPage* currentPage();

private slots:
	/** Change the page. */
	void slotChangePage(QListWidgetItem *current, QListWidgetItem *previous);

private:
	QListWidget* m_contentsList;
	QStackedWidget* m_pageWidget;
	QVBoxLayout* m_pageLayout;
	int m_maxItemWidth;
};



/**
* Base class for configuration dialog pages.
*/
class BtConfigPage : public QWidget
{
	Q_OBJECT
public:
	BtConfigPage();
	virtual ~BtConfigPage();
	
	/** Implement these to return the correct values.
	* For example: header(){return tr("General");}
	*/
	virtual QString iconName() = 0;
	virtual QString label() = 0;
	virtual QString header() = 0;
	BtConfigDialog* parentDialog() {return m_parentDialog;}

private:
	friend class BtConfigDialog;
	BtConfigDialog* m_parentDialog;

};


#endif

