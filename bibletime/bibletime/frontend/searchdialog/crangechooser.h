/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CRANGECHOOSER_H
#define CRANGECHOOSER_H

//BibleTime includes
#include "backend/cswordmoduleinfo.h"
#include "backend/cswordbackend.h"

#include "util/cpointers.h"

//Sword includes


//Qt includes
#include <qwidget.h>
#include <qstring.h>
#include <qcanvas.h>
#include <qdict.h>
#include <qtooltip.h>

//KDE includes
#include <kdialog.h>
#include <kdialogbase.h>
#include <klistview.h>

//forward declarations
class CSearchAnalysisItem;
class CSearchAnalysisLegendItem;
class CSearchAnalysis;
class CSearchAnalysisView; 

class QTextEdit;

namespace Search {
	namespace Options {
	
class CRangeChooserDialog : public KDialogBase {
	Q_OBJECT
public:
	CRangeChooserDialog(QWidget* parentDialog);
	~CRangeChooserDialog();

protected: // Protected methods
class RangeItem : public KListViewItem {
public:
		RangeItem(QListView*, QListViewItem* afterThis = 0, const QString caption = QString::null, const QString range = QString::null);
		~RangeItem();
		const QString& range();
		const QString caption();
		void setRange(QString range);
		void setCaption(const QString);
private:
		QString m_range;
	};

	/**
	* Initializes the connections of this widget.
	*/
	void initConnections();
	/**
	* Initializes the view of this object.
	*/
	void initView();

protected slots: // Protected slots
	/**
	* Adds a new range to the list.
	*/
	void addNewRange();
	void editRange(QListViewItem*);
	/**
	* Parses the entered text and prints out the result in the list box below the edit area.
	*/
	void parseRange();
	void nameChanged(const QString&);
	void rangeChanged();
	/**
	* Deletes the selected range.
	*/
	void deleteCurrentRange();
	virtual void slotDefault();
	virtual void slotOk();

private:
	KListView* m_rangeList;
	KListView* m_resultList;
	QLineEdit* m_nameEdit;
	QTextEdit* m_rangeEdit;
	QPushButton* m_newRangeButton;
	QPushButton* m_deleteRangeButton;
};

	} //end of namespace Search.Options
} //end of namespace Search

#endif
