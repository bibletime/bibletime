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

#include "btbiblekeywidget.h"

#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QFocusEvent>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPixmap>
#include <QString>
#include <QStringList>
#include <QtGlobal>
#include <QToolButton>
#include "../../../backend/keys/cswordversekey.h"
#include "../../../util/btconnect.h"
#include "../../../util/cresmgr.h"
#include "../../bibletimeapp.h"
#include "../cscrollerwidgetset.h"
#include "btdropdownchooserbutton.h"


class BtLineEdit : public QLineEdit {
    public:
        BtLineEdit(QWidget* parent)
                : QLineEdit(parent) {
        }
    protected:
        void focusInEvent(QFocusEvent* event) override {
            Qt::FocusReason reason = event->reason();
            if (reason == Qt::OtherFocusReason) {
                selectAll();
            }

            QWidget::focusInEvent(event);
        }
};


BtBibleKeyWidget::BtBibleKeyWidget(const CSwordBibleModuleInfo *mod,
                                   CSwordVerseKey *key, QWidget *parent,
                                   const char *name)
   : QWidget(parent), m_key(key), m_dropDownHoverTimer(this)
{
    Q_UNUSED(name)

    updatelock = false;
    m_module = mod;

    setFocusPolicy(Qt::WheelFocus);

    QToolButton* clearRef = new QToolButton(this);
    clearRef->setIcon(CResMgr::icon_clearEdit());
    clearRef->setAutoRaise(true);
    clearRef->setStyleSheet("QToolButton{margin:0px;}");
    BT_CONNECT(clearRef, SIGNAL(clicked()), SLOT(slotClearRef()) );

    m_bookScroller = new CScrollerWidgetSet(this);

    m_textbox = new BtLineEdit( this );
    setFocusProxy(m_textbox);
    m_textbox->setContentsMargins(0, 0, 0, 0);

    m_chapterScroller = new CScrollerWidgetSet(this);
    m_verseScroller = new CScrollerWidgetSet(this);

    QHBoxLayout* m_mainLayout = new QHBoxLayout( this );
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->addWidget(clearRef);
    m_mainLayout->addWidget(m_bookScroller);
    m_mainLayout->addWidget(m_textbox);
    m_mainLayout->addWidget(m_chapterScroller);
    m_mainLayout->addWidget(m_verseScroller);


    setTabOrder(m_textbox, nullptr);

    // Using "this" instead of "nullptr" works around a Qt 5.9 bug, QTBUG-61213
    m_dropDownButtons = new QWidget(this);

    m_dropDownButtons->setWindowFlags(Qt::Popup);
    m_dropDownButtons->setAttribute(Qt::WA_WindowPropagation);
    m_dropDownButtons->setCursor(Qt::ArrowCursor);
    QHBoxLayout *dropDownButtonsLayout(new QHBoxLayout(m_dropDownButtons));
    m_bookDropdownButton = new BtBookDropdownChooserButton(this);
    dropDownButtonsLayout->addWidget(m_bookDropdownButton, 2);
    m_chapterDropdownButton = new BtChapterDropdownChooserButton(this);
    dropDownButtonsLayout->addWidget(m_chapterDropdownButton, 1);
    m_verseDropdownButton = new BtVerseDropdownChooserButton(this);
    dropDownButtonsLayout->addWidget(m_verseDropdownButton, 1);
    dropDownButtonsLayout->setContentsMargins(0, 0, 0, 0);
    dropDownButtonsLayout->setSpacing(0);
    m_dropDownButtons->setLayout(dropDownButtonsLayout);
    m_dropDownButtons->hide();

    m_dropDownButtons->installEventFilter(this);

    m_dropDownHoverTimer.setInterval(500);
    m_dropDownHoverTimer.setSingleShot(true);
    BT_CONNECT(&m_dropDownHoverTimer, SIGNAL(timeout()),
               m_dropDownButtons, SLOT(hide()));

    QString scrollButtonToolTip(tr("Scroll through the entries of the list. Press the button and move the mouse to increase or decrease the item."));
    m_bookScroller->setToolTips(
        tr("Next book"),
        scrollButtonToolTip,
        tr("Previous book")
    );
    m_chapterScroller->setToolTips(
        tr("Next chapter"),
        scrollButtonToolTip,
        tr("Previous chapter")
    );
    m_verseScroller->setToolTips(
        tr("Next verse"),
        scrollButtonToolTip,
        tr("Previous verse")
    );

    // signals and slots connections

    BT_CONNECT(m_bookScroller, SIGNAL(change(int)), SLOT(slotStepBook(int)));
    BT_CONNECT(m_bookScroller, SIGNAL(scroller_pressed()),
               SLOT(slotUpdateLock()));
    BT_CONNECT(m_bookScroller, SIGNAL(scroller_released()),
               SLOT(slotUpdateUnlock()));
    BT_CONNECT(m_textbox, SIGNAL(returnPressed()),
               SLOT(slotReturnPressed()));
    BT_CONNECT(m_chapterScroller, SIGNAL(change(int)),
               SLOT(slotStepChapter(int)));
    BT_CONNECT(m_chapterScroller, SIGNAL(scroller_pressed()),
               SLOT(slotUpdateLock()));
    BT_CONNECT(m_chapterScroller, SIGNAL(scroller_released()),
               SLOT(slotUpdateUnlock()));
    BT_CONNECT(m_verseScroller, SIGNAL(change(int)), SLOT(slotStepVerse(int)));
    BT_CONNECT(m_verseScroller, SIGNAL(scroller_pressed()),
               SLOT(slotUpdateLock()));
    BT_CONNECT(m_verseScroller, SIGNAL(scroller_released()),
               SLOT(slotUpdateUnlock()));
    BT_CONNECT(m_key->afterChangedSignaller(), SIGNAL(signal()),
               this,                           SLOT(updateText()));

    setKey(key);    // The order of these two functions is important.
    setModule();
}

BtBibleKeyWidget::~BtBibleKeyWidget() {
    delete m_dropDownButtons;
}

void BtBibleKeyWidget::setModule(const CSwordBibleModuleInfo *m) {
    if (m) { //can be null
        m_module = m;
        m_key->setModule(m);
    }
}

bool BtBibleKeyWidget::eventFilter(QObject *o, QEvent *e) {
    if (o != m_dropDownButtons) return false;
    switch (e->type()) {
        case QEvent::Enter:
            m_dropDownHoverTimer.stop();
            return true;
        case QEvent::Leave:
            m_dropDownHoverTimer.start();
            return true;
        default:
            return false;
    }
}

void BtBibleKeyWidget::enterEvent(QEvent *) {
    m_dropDownHoverTimer.stop();

    resetDropDownButtons();

    m_dropDownButtons->raise();
    m_dropDownButtons->show();
}

void BtBibleKeyWidget::leaveEvent(QEvent *) {
    m_dropDownHoverTimer.start();
}

void BtBibleKeyWidget::resizeEvent(QResizeEvent *event) {
    if (m_dropDownButtons->isVisible()) {
        resetDropDownButtons();
    }
    QWidget::resizeEvent(event);
}

void BtBibleKeyWidget::resetDropDownButtons() {
    m_dropDownButtons->setParent(window());
    int h(m_dropDownButtons->layout()->minimumSize().height());
    QPoint topLeft(mapTo(window(),
                         QPoint(m_textbox->x(), m_textbox->y() + m_textbox->height())));
    m_dropDownButtons->setGeometry(topLeft.x(), topLeft.y(),
                                   m_textbox->width(), h);
}

void BtBibleKeyWidget::slotClearRef( ) {
    m_textbox->setText("");
    m_textbox->setFocus();
}

void BtBibleKeyWidget::updateText() {
    QString text(m_key->key());
    m_textbox->setText(text);
    QFontMetrics fm(m_textbox->font());
    #if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    int nw(m_textbox->minimumSizeHint().width() + fm.horizontalAdvance(text));
    #else
    int nw(m_textbox->minimumSizeHint().width() + fm.width(text));
    #endif
    if (nw > m_textbox->minimumWidth()) {
        m_textbox->setMinimumWidth(nw);
        m_textbox->updateGeometry();
    }
}

bool BtBibleKeyWidget::setKey(CSwordVerseKey *key) {
    if (!key) return false;

    m_key->setKey(key->key());
    return true;
}

void BtBibleKeyWidget::slotReturnPressed() {
    m_key->setKey(m_textbox->text());
    emit changed(m_key);
}

/* Handlers for the various scroller widgetsets. Do we really want a verse scroller? */
void BtBibleKeyWidget::slotUpdateLock() {
    updatelock = true;
    oldKey = m_key->key();
}

void BtBibleKeyWidget::slotUpdateUnlock() {
    updatelock = false;
    if (oldKey != m_key->key())
        emit changed(m_key);
}

void BtBibleKeyWidget::slotStepBook(int offset) {
    emit beforeChange(m_key);

    if(offset >= 0)
        for(; offset != 0; offset--)
            m_key->next( CSwordVerseKey::UseBook );
    else
        for(; offset != 0; offset++)
            m_key->previous( CSwordVerseKey::UseBook );

    if (!updatelock)
        emit changed(m_key);
}

void BtBibleKeyWidget::slotStepChapter(int offset) {
    emit beforeChange(m_key);

    if(offset >= 0)
        for(; offset != 0; offset--)
            m_key->next( CSwordVerseKey::UseChapter );
    else
        for(; offset != 0; offset++)
            m_key->previous( CSwordVerseKey::UseChapter );

    if (!updatelock)
        emit changed(m_key);
}

void BtBibleKeyWidget::slotStepVerse(int offset) {
    emit beforeChange(m_key);

    if(offset >= 0)
        for(; offset != 0; offset--)
            m_key->next( CSwordVerseKey::UseVerse );
    else
        for(; offset != 0; offset++)
            m_key->previous( CSwordVerseKey::UseVerse );

    if (!updatelock)
        emit changed(m_key);
}


void BtBibleKeyWidget::slotChangeVerse(int n) {
    if (m_key->getVerse() != n) {
        emit beforeChange(m_key);
        m_key->emitBeforeChanged();
        m_key->setVerse(n);
        m_key->emitAfterChanged();
        setKey( m_key );
    }
    if (!updatelock) emit changed(m_key);
}

void BtBibleKeyWidget::slotChangeChapter(int n) {
    if (m_key->getChapter() != n) {
        emit beforeChange(m_key);
        m_key->emitBeforeChanged();
        m_key->setChapter(n);
        m_key->emitAfterChanged();
        setKey( m_key );
    }
    if (!updatelock)
        emit changed(m_key);
}

void BtBibleKeyWidget::slotChangeBook(QString bookname) {
    if (m_key->book() != bookname) {
        emit beforeChange(m_key);
        m_key->emitBeforeChanged();
        m_key->book( bookname );
        m_key->emitAfterChanged();
        setKey( m_key );
    }
    if (!updatelock)
        emit changed(m_key);
}

