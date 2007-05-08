/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "crangechooser.h"

#include "backend/cswordkey.h"
#include "backend/cswordversekey.h"

#include "frontend/cbtconfig.h"

#include "util/cresmgr.h"
#include "util/ctoolclass.h"

//Qt includes
#include <qhbox.h>
#include <qvbox.h>
#include <qptrlist.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qmap.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qsizepolicy.h>
#include <qpushbutton.h>
#include <qheader.h>
#include <qregexp.h>
#include <qmessagebox.h>

//KDE includes
#include <kapplication.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kiconloader.h>

namespace Search {
	namespace Options {

/****************************/
CRangeChooserDialog::RangeItem::RangeItem(QListView* view, QListViewItem* afterThis, const QString caption, const QString range) : KListViewItem(view, afterThis) {
	setCaption(caption);
	setRange(range);
}

CRangeChooserDialog::RangeItem::~RangeItem() {}

const QString& CRangeChooserDialog::RangeItem::range() {
	//  qWarning("ange is %s", (const char*)m_range.utf8());
	return m_range;
}

void CRangeChooserDialog::RangeItem::setRange(QString newRange) {
	m_range = newRange;
}

const QString CRangeChooserDialog::RangeItem::caption() {
	return text(0);
}

void CRangeChooserDialog::RangeItem::setCaption(const QString newCaption) {
	setText(0, newCaption);
}


/**************************/
CRangeChooserDialog::CRangeChooserDialog( QWidget* parentDialog ) : KDialogBase(Plain, i18n("Search range editor"), Default | Ok | Cancel, Ok, parentDialog, "CRangeChooserDialog", false, true) {
	initView();
	initConnections();

	//add the existing scopes
	CBTConfig::StringMap map = CBTConfig::get
								   (CBTConfig::searchScopes);
	CBTConfig::StringMap::Iterator it;
	for (it = map.begin(); it != map.end(); ++it) {
		new RangeItem(m_rangeList, 0, it.key(), it.data());
	};

	editRange(0);
	if (RangeItem* i = dynamic_cast<RangeItem*>(m_rangeList->currentItem())
	   ) {
		nameChanged(i->caption());
	}
}

CRangeChooserDialog::~CRangeChooserDialog() {}

/** Initializes the view of this object. */
void CRangeChooserDialog::initView() {
	//  setButtonOKText(i18n(""));

	QGridLayout* grid = new QGridLayout(plainPage(),6,5,0,3);

	m_rangeList = new KListView(plainPage());
	m_rangeList->addColumn(i18n("Search range"));
	m_rangeList->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));
	m_rangeList->setFullWidth(true);
	m_rangeList->setSorting(0, true);
	m_rangeList->header()->setClickEnabled(false);
	m_rangeList->header()->setMovingEnabled(false);
	grid->addMultiCellWidget(m_rangeList,0,4,0,1);

	m_newRangeButton = new QPushButton(i18n("Add new range"),plainPage());
	connect(m_newRangeButton, SIGNAL(clicked()), this, SLOT(addNewRange()));
	grid->addWidget(m_newRangeButton,5,0);

	m_deleteRangeButton = new QPushButton(i18n("Delete current range"),plainPage());
	connect(m_deleteRangeButton, SIGNAL(clicked()), this, SLOT(deleteCurrentRange()));
	grid->addWidget(m_deleteRangeButton,5,1);

	grid->addColSpacing(2, 5);

	QLabel* label = new QLabel(i18n("Name:"), plainPage());
	m_nameEdit = new QLineEdit(plainPage());
	grid->addWidget(label,0,3);
	grid->addWidget(m_nameEdit,0,4);

	label = new QLabel(i18n("Edit current search range:"), plainPage());
	label->setFixedSize(label->sizeHint());
	m_rangeEdit = new QTextEdit(plainPage());
	m_rangeEdit->setTextFormat(Qt::PlainText);
	grid->addMultiCellWidget(label,1,1,3,4);
	grid->addMultiCellWidget(m_rangeEdit,2,2,3,4);

	grid->addRowSpacing(3, 10);

	m_resultList = new KListView(plainPage());
	m_resultList->addColumn(i18n("Parsed search range:"));
	m_resultList->setFullWidth(true);
	m_resultList->setSorting(-1);
	m_resultList->setShowSortIndicator(false);
	m_resultList->header()->setClickEnabled(false);
	m_resultList->header()->setMovingEnabled(false);
	m_resultList->setSelectionModeExt(KListView::NoSelection);

	grid->addMultiCellWidget(m_resultList, 4,5,3,4);

	grid->setRowStretch(4,5);
}

/** Initializes the connections of this widget. */
void CRangeChooserDialog::initConnections() {
	connect(m_rangeList, SIGNAL(selectionChanged(QListViewItem*)),
			this, SLOT(editRange(QListViewItem*)));

	connect(m_rangeEdit, SIGNAL(textChanged()),
			this, SLOT(parseRange()));
	connect(m_rangeEdit, SIGNAL(textChanged()),
			this, SLOT(rangeChanged()));

	connect(m_nameEdit, SIGNAL(textChanged(const QString&)),
			this, SLOT(nameChanged(const QString&)));
}

/** Adds a new range to the list. */
void CRangeChooserDialog::addNewRange() {
	RangeItem* i = new RangeItem(m_rangeList, m_rangeList->lastItem(), i18n("New range"));
	m_rangeList->setSelected(i, true);
	m_rangeList->setCurrentItem(i);
	editRange(i);

	m_nameEdit->setFocus();
}

/** No descriptions */
void CRangeChooserDialog::editRange(QListViewItem* item) {
	RangeItem* const range = dynamic_cast<RangeItem*>(item);

	m_nameEdit->setEnabled( range ); //only if an item is selected enable the edit part
	m_rangeEdit->setEnabled( range );
	m_resultList->setEnabled( range );
	m_deleteRangeButton->setEnabled( range );

	if (range) {
		m_nameEdit->setText(range->caption());
		m_rangeEdit->setText(range->range());
	}
}

/** Parses the entered text and prints out the result in the list box below the edit area. */
void CRangeChooserDialog::parseRange() {
	m_resultList->clear();

	//hack: repair range to  work with Sword 1.5.6
	QString range( m_rangeEdit->text() );
	range.replace(QRegExp("\\s{0,}-\\s{0,}"), "-" );

	sword::VerseKey key;
	sword::ListKey verses = key.ParseVerseList((const char*)range.utf8(), "Genesis 1:1", true);
	for (int i = 0; i < verses.Count(); ++i) {
		new KListViewItem(m_resultList, QString::fromUtf8(verses.GetElement(i)->getRangeText()));
		//    qWarning("range=%s, text=%s",verses.GetElement(i)->getRangeText(), verses.GetElement(i)->getText() );
	}

}

/** No descriptions */
void CRangeChooserDialog::rangeChanged() {
	if (RangeItem* i = dynamic_cast<RangeItem*>(m_rangeList->currentItem())
	   ) {
		QString range( m_rangeEdit->text() );
		//hack: repair range to work with Sword 1.5.6
		range.replace(QRegExp("\\s{0,}-\\s{0,}"), "-" );
		i->setRange(range);
	};
}

/** No descriptions */
void CRangeChooserDialog::nameChanged(const QString& newCaption) {
	m_rangeEdit->setEnabled(!newCaption.isEmpty());
	m_resultList->setEnabled(!newCaption.isEmpty());
	m_resultList->header()->setEnabled(!newCaption.isEmpty());

	if (RangeItem* i = dynamic_cast<RangeItem*>(m_rangeList->currentItem())
	   ) {
		if (!newCaption.isEmpty()) {
			m_newRangeButton->setEnabled(true);
			i->setCaption(newCaption);
			m_rangeList->sort();
		}
		else { //invalid name
			i->setCaption(i18n("<invalid name of search range>"));
			m_newRangeButton->setEnabled(false);
		};
	};
}

/** Deletes the selected range. */
void CRangeChooserDialog::deleteCurrentRange() {
	if (RangeItem* i = dynamic_cast<RangeItem*>(m_rangeList->currentItem())
	   ) {
		if (QListViewItem* selection = i->itemBelow() ? i->itemBelow() : i->itemAbove()) {
			m_rangeList->setSelected(selection, true);
			m_rangeList->setCurrentItem(selection);
		}
		else {
			m_rangeList->setSelected(m_rangeList->firstChild(), true);
			m_rangeList->setCurrentItem(m_rangeList->firstChild());
		}
		delete i;
	}
	editRange(m_rangeList->currentItem());
}

void CRangeChooserDialog::slotOk() {
	//save the new map of search scopes
	CBTConfig::StringMap map;
	QListViewItemIterator it( m_rangeList );
	for (;it.current(); ++it) {
		if ( RangeItem* i = dynamic_cast<RangeItem*>(it.current()) ) {
			map[i->caption()] = i->range();
		};
	};
	CBTConfig::set
		(CBTConfig::searchScopes, map);

	KDialogBase::slotOk();
}

void CRangeChooserDialog::slotDefault() {
	m_rangeList->clear();
	CBTConfig::StringMap map = CBTConfig::getDefault(CBTConfig::searchScopes);
	CBTConfig::StringMap::Iterator it;
	for (it = map.begin(); it != map.end(); ++it) {
		new RangeItem(m_rangeList, 0, it.key(), it.data());
	};
	m_rangeList->setSelected(m_rangeList->selectedItem(), false);
	m_rangeList->setCurrentItem(0);

	editRange(0);
	if (RangeItem* i = dynamic_cast<RangeItem*>(m_rangeList->currentItem())
	   ) {
		nameChanged(i->caption());
	}

	KDialogBase::slotDefault();
}
 
	} //end of namespace Search::Options
} //end of namespace Search
