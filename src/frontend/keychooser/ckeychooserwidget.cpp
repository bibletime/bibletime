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

#include "ckeychooserwidget.h"

#include <QComboBox>
#include <QFocusEvent>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QString>
#include <QWheelEvent>
#include "../../util/btconnect.h"
#include "cscrollerwidgetset.h"


class BtKeyLineEdit : public QLineEdit {

public: /* Methods: */

    BtKeyLineEdit(QWidget * parent)
        : QLineEdit(parent) {}

protected: /* Methods: */

    void focusInEvent(QFocusEvent * event) override {
        const Qt::FocusReason reason = event->reason();
        if (reason == Qt::OtherFocusReason)
            selectAll();
        QWidget::focusInEvent(event);
    }

};


CKCComboBox::CKCComboBox(QWidget * parent)
    : QComboBox(parent)
{
    setFocusPolicy(Qt::WheelFocus);
    setLineEdit(new BtKeyLineEdit(this));
    if (lineEdit())
        installEventFilter(lineEdit());
}

bool CKCComboBox::eventFilter(QObject * o, QEvent * e) {
    if (e->type() == QEvent::FocusOut) {
        QFocusEvent * const f = static_cast<QFocusEvent *>(e);

        if (o == lineEdit() && f->reason() == Qt::TabFocusReason) {
            int index = findText(currentText());
            if (index == -1)
                index = 0; // return 0 if not found
            setCurrentIndex(index);
            emit focusOut(index);
            return false;
        }

        if (f->reason() == Qt::PopupFocusReason)
            return false;

        if (f->reason() == Qt::ActiveWindowFocusReason) {
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
            emit activated(currentText());
#else
            emit textActivated(currentText());
#endif
            return false;
        }

        if (f->reason() == Qt::MouseFocusReason) {
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
            emit activated(currentText());
#else
            emit textActivated(currentText());
#endif
            return false;
        }

        if (o == this) {
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
            emit activated(currentText());
#else
            emit textActivated(currentText());
#endif
            return false;
        }
    }

    return QComboBox::eventFilter(o, e);
}


//**********************************************************************************/

CKeyChooserWidget::CKeyChooserWidget(int count, QWidget * parent)
    : QWidget(parent)
{
    for (int index = 1; index <= count; index++) {
        m_list.append( QString::number(index) );
    }
    init();
    reset(m_list, 0, false);
}

CKeyChooserWidget::CKeyChooserWidget(QStringList * list, QWidget * parent )
    : QWidget(parent)
{
    if (list)
        m_list = *list;

    init();
    reset(m_list, 0, false);
}

void CKeyChooserWidget::reset(const int count, int index, bool do_emit) {
    //This prevents the widget from resetting during application load, which
    //produces undesirable behavior.
    //if (!updatesEnabled())
    //    return;

    m_list.clear();
    for (int i = 1; i <= count; i++) /// \todo CHECK
        m_list.append(QString::number(i));

    reset(&m_list, index, do_emit);
}

void CKeyChooserWidget::reset(const QStringList & list, int index, bool do_emit) {
    //This prevents the widget from resetting during application load, which
    //produces undesirable behavior.
    //if (!updatesEnabled())
    //    return;

    m_list = list;
    reset(&m_list, index, do_emit);
}


void CKeyChooserWidget::reset(const QStringList * list, int index, bool do_emit) {
    //if (isResetting || !updatesEnabled())
    if (m_isResetting)
        return;

    //  qWarning("starting insert");
    m_isResetting = true;

    m_oldKey = QString();

    //  m_comboBox->setUpdatesEnabled(false);
    //DON'T REMOVE THE HIDE: Otherwise QComboBox's sizeHint() function won't work properly
    m_comboBox->hide();
    m_comboBox->clear();
    if (list && !list->empty()) {
        m_comboBox->insertItems(0, *list); // Prepend items
        setEnabled(true);
        m_comboBox->setCurrentIndex(index);
    } else {
        setEnabled(false);
    }

    if (do_emit)
        emit changed(m_comboBox->currentIndex());

    m_comboBox->sizeHint(); //without this function call the combo box won't be properly sized!
    //DON'T REMOVE OR MOVE THE show()! Otherwise QComboBox's sizeHint() function won't work properly!
    m_comboBox->show();

    //  m_comboBox->setFont( m_comboBox->font() );
    //  m_comboBox->setUpdatesEnabled(true);

    m_isResetting = false;
    //  qWarning("inserted");
}

/** Initializes this widget. We need this function because we have more than one constructor. */
void CKeyChooserWidget::init() {
    m_oldKey = QString();

    setFocusPolicy(Qt::WheelFocus);

    m_comboBox = new CKCComboBox(this);
    setFocusProxy(m_comboBox);
    m_comboBox->setEditable(true); // Also sets completer
    BT_ASSERT(m_comboBox->completer());
    m_comboBox->setInsertPolicy(QComboBox::NoInsert);
    m_comboBox->setFocusPolicy(Qt::WheelFocus);

    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->addWidget(m_comboBox);

    m_scroller = new CScrollerWidgetSet(this);

    m_mainLayout->addWidget(m_scroller);
    m_mainLayout->addSpacing(0);

    setTabOrder(m_comboBox, nullptr);
    setFocusProxy(m_comboBox);

    BT_CONNECT(m_scroller, SIGNAL(scroller_pressed()),  SLOT(lock()));
    BT_CONNECT(m_scroller, SIGNAL(scroller_released()), SLOT(unlock()));
    BT_CONNECT(m_scroller, SIGNAL(change(int)),         SLOT(changeCombo(int)));
    BT_CONNECT(m_comboBox, SIGNAL(activated(int)), SLOT(slotComboChanged(int)));
    BT_CONNECT(m_comboBox->lineEdit(), SIGNAL(returnPressed()),
               SLOT(slotReturnPressed()));
    BT_CONNECT(m_comboBox, SIGNAL(focusOut(int)), SIGNAL(focusOut(int)));

    updatelock = false;
    m_isResetting = false;
}

/** Is called when the return key was presed in the combobox. */
void CKeyChooserWidget::slotReturnPressed() {
    BT_ASSERT(m_comboBox->lineEdit());

    const QString text(m_comboBox->lineEdit()->text());
    for (int index = 0; index < m_comboBox->count(); ++index) {
        if (m_comboBox->itemText(index) == text) {
//            emit changed(index);
            emit focusOut(index); // a workaround because focusOut is not checked, the slot connected to changed to check
            break;
        }
    }
}

/** Is called when the current item of the combo box was changed. */
void CKeyChooserWidget::slotComboChanged(int index) {
    if (!updatesEnabled())
        return;

    setUpdatesEnabled(false);

    const QString key(m_comboBox->itemText(index));
    if (m_oldKey.isNull() || (m_oldKey != key))
        emit changed(index);

    m_oldKey = key;

    setUpdatesEnabled(true);
}

/** Sets the tooltips for the given entries using the parameters as text. */
void CKeyChooserWidget::setToolTips(const QString & comboTip,
                                    const QString & nextEntryTip,
                                    const QString & scrollButtonTip,
                                    const QString & previousEntryTip)
{
    m_comboBox->setToolTip(comboTip);
    m_scroller->setToolTips(nextEntryTip, scrollButtonTip, previousEntryTip);
}

/** Sets the current item to the one with the given text */
bool CKeyChooserWidget::setItem(const QString & item) {
    bool ret = false;
    const int count = m_comboBox->count();
    for (int i = 0; i < count; ++i) {
        if (m_comboBox->itemText(i) == item) {
            m_comboBox->setCurrentIndex(i);
            ret = true;
            break;
        }
    }
    if (!ret)
        m_comboBox->setCurrentIndex(-1);
    return ret;
}

/* Handlers for the various scroller widgetset. */
void CKeyChooserWidget::lock() {
    updatelock = true;
    m_comboBox->setEditable(false);
    m_oldKey = m_comboBox->currentText();
}

void CKeyChooserWidget::unlock() {
    updatelock = false;
    m_comboBox->setEditable(true);
    m_comboBox->setEditText(m_comboBox->itemText(m_comboBox->currentIndex()));
    if (m_comboBox->currentText() != m_oldKey)
        emit changed(m_comboBox->currentIndex());
}

void CKeyChooserWidget::changeCombo(int n) {
    const int old_index = m_comboBox->currentIndex();
    int new_index = old_index + n;

    //index of highest Item
    const int max = m_comboBox->count() - 1;
    if (new_index > max)
        new_index = max;
    if (new_index < 0)
        new_index = 0;

    if (new_index != old_index) {
        m_comboBox->setCurrentIndex(new_index);
        if (!updatelock)
            emit changed(new_index);
    }
}

