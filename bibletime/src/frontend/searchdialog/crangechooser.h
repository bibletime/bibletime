/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CRANGECHOOSER_H
#define CRANGECHOOSER_H

//BibleTime includes
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/cswordbackend.h"

#include "util/cpointers.h"

//Sword includes


//Qt includes
#include <qwidget.h>
#include <QString>
#include <q3canvas.h>
#include <q3dict.h>
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

class Q3TextEdit;

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
		RangeItem(Q3ListView*, Q3ListViewItem* afterThis = 0, const QString caption = QString::null, const QString range = QString::null);
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
	void editRange(Q3ListViewItem*);
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
	Q3TextEdit* m_rangeEdit;
	QPushButton* m_newRangeButton;
	QPushButton* m_deleteRangeButton;
};

	} //end of namespace Search.Options
} //end of namespace Search

#endif
