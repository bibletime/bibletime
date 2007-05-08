/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CKEYCHOOSERWIDGET_H
#define CKEYCHOOSERWIDGET_H

#include <qwidget.h>
#include <qmap.h>
#include <qstringlist.h>

#include <kcombobox.h>
#include "cscrollerwidgetset.h"


class CLexiconKeyChooser;
//class CMinMaxLayout;

class QIconSet;
//class QComboBox;
class QStringList;
class QPoint;
class QMouseEvent;
class QWheelEvent;
class QHBoxLayout;

/*
* We use this class to conrtol the focus move in the combobox
* This class is used in the key chooser widgets
*/
class CKCComboBox : public QComboBox {
	Q_OBJECT

public:
	CKCComboBox(bool rw, QWidget * parent=0, const char * name=0 );
	/**
	* Returns the size this widget would like to have.
	*/
	//  virtual QSize sizeHint() const;

protected:
	/**
	* Reimplementation.
	*/
	virtual bool eventFilter( QObject *o, QEvent *e );
	/**
	* Scrolls in the list if the wheel of the mouse was used.
	*/
	virtual void wheelEvent( QWheelEvent* e);

signals:
	/**
	* Emitted when the user moves the focus away from the combo by pressing tab
	*/
	void focusOut(int itemIndex);
};

/**
 * This class implements the KeyCooser Widget, which
 * consists of a @ref QComboBox, two normal ref @QToolButton
 * and a enhanced @ref CScrollButton
 *
  * @author The BibleTime team
  */
class CKeyChooserWidget : public QWidget  {
	Q_OBJECT
public:
	/**
	* the constructor
	*/
	CKeyChooserWidget(QStringList *list=0, const bool useNextPrevSignals = false, QWidget *parent=0, const char *name=0);
	/**
	* the constructor
	*/
	CKeyChooserWidget(int count=0, const bool useNextPrevSignals = false, QWidget *parent=0, const char *name=0);
	/**
	* This function does clear the combobox, then fill in
	* the StringList, set the ComboBox' current item to index
	* and if do_emit is true, it will emit @ref #changed
	*
	* @param list the stringlist to be inserted
	* @param index the index that the combobox is to jump to
	* @param do_emit should we emit @ref #changed(int)
	*/
	void reset(const int count, int index, bool do_emit);
	void reset(QStringList& list, int index, bool do_emit);
	void reset(QStringList *list, int index, bool do_emit);
	/**
	* Initializes this widget. We need this function because
	* we have more than one constructor.
	*/
	virtual void init();
	/**
	*
	*/
	//  virtual void adjustSize();
	/**
	* Sets the tooltips for the given entries using the parameters as text.
	*/
	void setToolTips( const QString comboTip, const QString nextEntry, const QString scrollButton, const QString previousEntry);
	/**
	* Sets the current item to the one with the given text
	*/
	bool setItem( const QString item);
	/**
	* Return the combobox of this key chooser widget.
	*/
	QComboBox* comboBox() {return m_comboBox;};

public slots:
	/**
	* is called to lock the combobox
	*/
	void lock()
		;
	/**
	* is called to unlock the combobox
	*/
	void unlock();
	/**
	* is called to move the combobox to a certain index
	* @param index the index to jump to
	*/
	void changeCombo(int index);
	void slotComboChanged(int index);

signals:
	/**
	* Is emitted if the widget changed, but
	* only if it is not locked or being reset
	*
	* @param the current ComboBox index
	*/
	void changed(int index);
	/**
	* Is emitted if the widget was left with a focus out event.
	* @param index The new index of the ComboBox
	*/
	void focusOut(int index);

protected:
	/**
	* indicates wheter we are resetting at the moment
	*/
	bool isResetting;
	/**
	*
	*/
	QString oldKey;

protected slots: // Protected slots
	/**
	* Is called when the return key was presed in the combobox.
	*/
	void slotReturnPressed( /*const QString&*/ );


private:
	friend class CLexiconKeyChooser;
	QStringList m_list;
	bool m_useNextPrevSignals;
	bool updatelock;

	/**
	* Members should never be public!!
	*/
	CKCComboBox* m_comboBox;
	QHBoxLayout *m_mainLayout;
	CScrollerWidgetSet * m_scroller;
};

#endif
