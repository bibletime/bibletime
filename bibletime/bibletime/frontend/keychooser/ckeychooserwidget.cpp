/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/


//BibleTime includes
#include "ckeychooserwidget.h"

//BibleTime frontend includes
#include "frontend/cbtconfig.h"


//Qt includes
#include <qlineedit.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qlistbox.h>
#include <qtoolbutton.h>
#include <qevent.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qtooltip.h>
#include <qrect.h>

CKCComboBox::CKCComboBox(bool rw,QWidget* parent,const char* name)
: QComboBox(rw,parent,name) {
	setFocusPolicy(QWidget::WheelFocus);
	if (lineEdit()) {
		installEventFilter( lineEdit() );
	}
}

/** Reimplementation. */
bool CKCComboBox::eventFilter( QObject *o, QEvent *e ) {
	if (e->type() == QEvent::FocusOut) {
		QFocusEvent* f = static_cast<QFocusEvent*>(e);

		if (o == lineEdit() && f->reason() == QFocusEvent::Tab) {
			int index = listBox()->index( listBox()->findItem(currentText()) );
			if (index == -1) {
				index = 0;// return 0 if not found
			}
			setCurrentItem( index );
			emit focusOut( index );

			return false;
		}
		else if (f->reason() == QFocusEvent::Popup) {
			return false;
		}
		else if (f->reason() == QFocusEvent::ActiveWindow) {
			emit activated(currentText());
			return false;
		}
		else if (f->reason() == QFocusEvent::Mouse) {
			emit activated(currentText());
			return false;
		}
		else if (o == this) {
			emit activated(currentText());
			return false;
		}
	}

	return QComboBox::eventFilter(o,e);
}

/** Scrolls in the list if the wheel of the mouse was used. */
void CKCComboBox::wheelEvent( QWheelEvent* e ) {
	return QComboBox::wheelEvent(e);

	const signed int change = (int)((float)e->delta()/(float)120);
	int current = currentItem();

	if ((current+change >= 0) && (current+change<count()) ) {
		setCurrentItem(current+change);
		e->accept();
		emit activated( currentItem() );
	}
	else {
		e->ignore();
	}
}

//**********************************************************************************/

CKeyChooserWidget::CKeyChooserWidget(int count, const bool useNextPrevSignals,  QWidget *parent, const char *name) : QWidget(parent,name) {
	m_useNextPrevSignals = useNextPrevSignals;

	for (int index=1; index <= count; index++) {
		m_list.append( QString::number(index) );
	}
	init();
	reset(m_list,0,false);
};

CKeyChooserWidget::CKeyChooserWidget(QStringList *list, const bool useNextPrevSignals, QWidget *parent, const char *name ) : QWidget(parent,name) {
	m_useNextPrevSignals = useNextPrevSignals;

	if (list) {
		m_list = *list; //deep copy the items of list
	}
	else {
		m_list.clear();
	}

	init();
	reset(m_list,0,false);
}

void CKeyChooserWidget::reset(const int count, int index, bool do_emit) {
	if (!isUpdatesEnabled())
		return;

	m_list.clear();
	for (int i=1; i <= count; i++) { //TODO: CHECK
		m_list.append( QString::number(i) );
	}

	reset(&m_list,index,do_emit);
}

void CKeyChooserWidget::reset(QStringList& list, int index, bool do_emit) {
	if (!isUpdatesEnabled())
		return;

	m_list = list;
	reset(&m_list,index,do_emit);
}


void CKeyChooserWidget::reset(QStringList *list, int index, bool do_emit) {
	if (isResetting || !isUpdatesEnabled())
		return;

	//  qWarning("starting insert");
	isResetting = true;

	oldKey = QString::null;

	//  m_comboBox->setUpdatesEnabled(false);
	//DON'T REMOVE THE HIDE: Otherwise QComboBox's sizeHint() function won't work properly
	m_comboBox->hide();
	m_comboBox->clear();
	if (list) {
		m_comboBox->insertStringList(*list);
	}

	if (!list || (list && !list->count())) { //nothing in the combobox
		setEnabled(false);
	}
	else if (!isEnabled()) { //was disabled
		setEnabled(true);
	}

	if (list->count()) {
		m_comboBox->setCurrentItem(index);
	}
	if (do_emit) {
		emit changed(m_comboBox->currentItem());
	}

	const QSize dummySize = m_comboBox->sizeHint(); //without this function call the combo box won't be properly sized!
	//DON'T REMOVE OR MOVE THE show()! Otherwise QComboBox's sizeHint() function won't work properly!
	m_comboBox->show();

	//  m_comboBox->setFont( m_comboBox->font() );
	//  m_comboBox->setUpdatesEnabled(true);

	isResetting = false;
	//  qWarning("inserted");
}

/** Initializes this widget. We need this function because we have more than one constructor. */
void CKeyChooserWidget::init() {
	oldKey = QString::null;

	setFocusPolicy(QWidget::WheelFocus);

	m_comboBox = new CKCComboBox( true, this );
	m_comboBox->setAutoCompletion( true );
	m_comboBox->setInsertionPolicy(QComboBox::NoInsertion);
	m_comboBox->setFocusPolicy(QWidget::WheelFocus);

	m_mainLayout = new QHBoxLayout( this );
	m_mainLayout->addWidget(m_comboBox,5);

	m_scroller = new CScrollerWidgetSet(this);

	m_mainLayout->addWidget( m_scroller );
	m_mainLayout->addSpacing(2);

	setTabOrder(m_comboBox, 0);

	connect(m_scroller, SIGNAL(scroller_pressed()), SLOT(lock()));
	connect(m_scroller, SIGNAL(scroller_released()), SLOT(unlock()));
	connect(m_scroller, SIGNAL(change(int)), SLOT(changeCombo(int)) );

	connect(m_comboBox, SIGNAL(activated(int)), SLOT(slotComboChanged(int)));
	//  connect(m_comboBox, SIGNAL(activated(const QString&)), SLOT(slotReturnPressed(const QString&)));
	connect(m_comboBox->lineEdit(), SIGNAL(returnPressed()), SLOT(slotReturnPressed()));
	connect(m_comboBox, SIGNAL(focusOut(int)), SIGNAL(focusOut(int)));

	updatelock = false;
	isResetting = false;
}

/** Is called when the return key was presed in the combobox. */
void CKeyChooserWidget::slotReturnPressed( /*const QString& text*/) {
	Q_ASSERT(comboBox()->lineEdit());
	qWarning("return pressed");

	QString text = comboBox()->lineEdit()->text();
	for (int index = 0; index < comboBox()->count(); ++index) {
		if (comboBox()->text(index) == text) {
//			emit changed(index);
			emit focusOut(index); // a workaround because focusOut is not checked, the slot connected to changed to check
			break;
		}
	}
}

/** Is called when the current item of the combo box was changed. */
void CKeyChooserWidget::slotComboChanged(int index) {
	qWarning("CKeyChooserWidget::slotComboChanged(int index)");
	if (!isUpdatesEnabled()) {
		return;
	}

	setUpdatesEnabled(false);

	const QString key = comboBox()->text( index );
	if (oldKey.isNull() || (oldKey != key)) {
		emit changed(index);
	}

	oldKey = key;

	setUpdatesEnabled(true);
}

/** Sets the tooltips for the given entries using the parameters as text. */
void CKeyChooserWidget::setToolTips( const QString comboTip, const QString nextEntryTip, const QString scrollButtonTip, const QString previousEntryTip) {
	QToolTip::add (comboBox(),comboTip);
	m_scroller->setToolTips(nextEntryTip, scrollButtonTip, previousEntryTip);
}

/** Sets the current item to the one with the given text */
bool CKeyChooserWidget::setItem( const QString item ) {
	bool ret = false;
	const int count = comboBox()->count();
	for (int i = 0; i < count; ++i) {
		if (comboBox()->text(i) == item) {
			comboBox()->setCurrentItem(i);
			ret = true;
			break;
		}
	}
	if (!ret)
		comboBox()->setCurrentItem(-1);
	return ret;
}

/* Handlers for the various scroller widgetset. */
void CKeyChooserWidget::lock() {
	updatelock = true;
	comboBox()->setEditable(false);
	oldKey = comboBox()->currentText();
}

void CKeyChooserWidget::unlock() {
	updatelock = false;
	comboBox()->setEditable(true);
	comboBox()->setEditText(comboBox()->text(comboBox()->currentItem()));
	if (comboBox()->currentText() != oldKey) {
		emit changed(comboBox()->currentItem());
	}
}

void CKeyChooserWidget::changeCombo(int n) {
	const int old_item = comboBox()->currentItem();
	int new_item = old_item + n;

	//index of highest Item
	const int max = comboBox()->count()-1;
	if(new_item > max) new_item = max;
	if(new_item < 0) new_item = 0;

	if(new_item != old_item) {
		comboBox()->setCurrentItem(new_item);
		if(!updatelock)
			emit changed(new_item);
	}
}

