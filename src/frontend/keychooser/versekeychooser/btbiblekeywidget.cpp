/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/keychooser/versekeychooser/btbiblekeywidget.h"

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
#include <QToolButton>
#include "backend/config/cbtconfig.h"
#include "backend/keys/cswordversekey.h"
#include "frontend/keychooser/cscrollerwidgetset.h"
#include "frontend/keychooser/versekeychooser/btdropdownchooserbutton.h"
#include "util/btsignal.h"
#include "util/cresmgr.h"
#include "util/directory.h"


class BtLineEdit : public QLineEdit {
    public:
        BtLineEdit(QWidget* parent)
                : QLineEdit(parent) {
        }
    protected:
        void focusInEvent(QFocusEvent* event) {
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
    Q_UNUSED(name);

    namespace DU = util::directory;


    updatelock = false;
    m_module = mod;

    setFocusPolicy(Qt::WheelFocus);

    QToolButton* clearRef = new QToolButton(this);
    clearRef->setIcon(DU::getIcon("edit_clear_locationbar"));
    clearRef->setAutoRaise(true);
    clearRef->setStyleSheet("QToolButton{margin:0px;}");
    connect(clearRef, SIGNAL(clicked()), SLOT(slotClearRef()) );

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


    setTabOrder(m_textbox, 0);
    m_dropDownButtons = new QWidget(0);
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
    connect(&m_dropDownHoverTimer, SIGNAL(timeout()),
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

    connect(m_bookScroller, SIGNAL(change(int)), SLOT(slotStepBook(int)));
    connect(m_bookScroller, SIGNAL(scroller_pressed()), SLOT(slotUpdateLock()));
    connect(m_bookScroller, SIGNAL(scroller_released()), SLOT(slotUpdateUnlock()));
    connect(m_textbox, SIGNAL(returnPressed()), SLOT(slotReturnPressed()));
    connect(m_chapterScroller, SIGNAL(change(int)), SLOT(slotStepChapter(int)));
    connect(m_chapterScroller, SIGNAL(scroller_pressed()), SLOT(slotUpdateLock()));
    connect(m_chapterScroller, SIGNAL(scroller_released()), SLOT(slotUpdateUnlock()));
    connect(m_verseScroller, SIGNAL(change(int)), SLOT(slotStepVerse(int)));
    connect(m_verseScroller, SIGNAL(scroller_pressed()), SLOT(slotUpdateLock()));
    connect(m_verseScroller, SIGNAL(scroller_released()), SLOT(slotUpdateUnlock()));
    bool ok = connect(m_key->signaler(), SIGNAL(changed()), this, SLOT(updateText()));
    Q_ASSERT(ok);

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
    int nw(m_textbox->minimumSizeHint().width() + fm.width(text));
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

void BtBibleKeyWidget::slotStepBook(int n) {
    emit beforeChange(m_key);
    n > 0 ? m_key->next( CSwordVerseKey::UseBook ) : m_key->previous( CSwordVerseKey::UseBook );
    if (!updatelock)
        emit changed(m_key);
}

void BtBibleKeyWidget::slotStepChapter(int n) {
    emit beforeChange(m_key);
    n > 0 ? m_key->next( CSwordVerseKey::UseChapter ) : m_key->previous( CSwordVerseKey::UseChapter );
    if (!updatelock)
        emit changed(m_key);
}

void BtBibleKeyWidget::slotStepVerse(int n) {
    emit beforeChange(m_key);
    n > 0 ? m_key->next( CSwordVerseKey::UseVerse ) : m_key->previous( CSwordVerseKey::UseVerse );
    if (!updatelock)
        emit changed(m_key);
}


void BtBibleKeyWidget::slotChangeVerse(int n) {
    if (m_key->Verse() != n) {
        emit beforeChange(m_key);
        m_key->Verse( n );
        setKey( m_key );
    }
    if (!updatelock) emit changed(m_key);
}

void BtBibleKeyWidget::slotChangeChapter(int n) {
    if (m_key->Chapter() != n) {
        emit beforeChange(m_key);
        m_key->Chapter( n );
        setKey( m_key );
    }
    if (!updatelock)
        emit changed(m_key);
}

void BtBibleKeyWidget::slotChangeBook(QString bookname) {
    if (m_key->book() != bookname) {
        emit beforeChange(m_key);
        m_key->book( bookname );
        setKey( m_key );
    }
    if (!updatelock)
        emit changed(m_key);
}

