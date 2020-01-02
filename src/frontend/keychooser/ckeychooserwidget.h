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

#ifndef CKEYCHOOSERWIDGET_H
#define CKEYCHOOSERWIDGET_H

#include <QComboBox>

#include <QStringList>
#include "../../util/btassert.h"


class CLexiconKeyChooser;
class CScrollerWidgetSet;
class QWheelEvent;
class QHBoxLayout;
class QWidget;
class QObject;
class QEvent;

/**
* We use this class to control the focus move in the combobox
* This class is used in the key chooser widgets
*/
class CKCComboBox: public QComboBox {

    Q_OBJECT

public: /* Methods: */

    CKCComboBox(QWidget * parent = nullptr);

protected: /* Methods: */

    bool eventFilter(QObject * o, QEvent * e) override;

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
class CKeyChooserWidget: public QWidget  {

    Q_OBJECT

public: /* Methods: */

    CKeyChooserWidget(QStringList * list = nullptr,
                      QWidget * parent = nullptr);

    CKeyChooserWidget(int count = 0,
                      QWidget * parent = nullptr);

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

    void reset(const QStringList & list, int index, bool do_emit);

    void reset(const QStringList * list, int index, bool do_emit);

    /**
    * Initializes this widget. We need this function because
    * we have more than one constructor.
    */
    void init();

    /**
    * Sets the tooltips for the given entries using the parameters as text.
    */
    void setToolTips(const QString & comboTip,
                     const QString & nextEntry,
                     const QString & scrollButton,
                     const QString & previousEntry);

    /**
    * Sets the current item to the one with the given text
    */
    bool setItem(const QString & item);

    /**
    * Return the combobox of this key chooser widget.
    */
    QComboBox & comboBox() const {
        BT_ASSERT(m_comboBox);
        return *m_comboBox;
    }

public slots:

    /**
    * is called to lock the combobox
    */
    void lock();

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

protected slots:

    /**
    * Is called when the return key was presed in the combobox.
    */
    void slotReturnPressed();

private: /* Fields: */

    QStringList m_list;
    bool updatelock;

    /**
    * Members should never be public!!
    */
    CKCComboBox * m_comboBox;
    QHBoxLayout * m_mainLayout;
    CScrollerWidgetSet * m_scroller;

    /**
    * indicates wheter we are resetting at the moment
    */
    bool m_isResetting;

    QString m_oldKey;

};

#endif
