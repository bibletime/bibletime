/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
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
#include <QMenu>
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
    BT_CONNECT(clearRef, &QToolButton::clicked,
               [this]{
                   m_textbox->setText("");
                   m_textbox->setFocus();
               });

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

    auto * const bookChooser =
            new BtDropdownChooserButton(&BtBibleKeyWidget::populateBookMenu,
                                        *this);
    bookChooser->setToolTip(tr("Select book"));
    BT_CONNECT(bookChooser->menu(), &QMenu::triggered,
               [this](QAction * const action)
               { slotChangeBook(action->property("bookname").toString()); });
    BT_CONNECT(bookChooser, &BtDropdownChooserButton::stepItem,
               this, &BtBibleKeyWidget::slotStepBook);
    dropDownButtonsLayout->addWidget(bookChooser, 2);

    auto * const chapterChooser =
            new BtDropdownChooserButton(&BtBibleKeyWidget::populateChapterMenu,
                                        *this);
    chapterChooser->setToolTip(tr("Select chapter"));
    BT_CONNECT(chapterChooser->menu(), &QMenu::triggered,
               [this](QAction * const action)
               { slotChangeChapter(action->property("chapter").toInt()); });
    BT_CONNECT(chapterChooser, &BtDropdownChooserButton::stepItem,
               this, &BtBibleKeyWidget::slotStepChapter);
    dropDownButtonsLayout->addWidget(chapterChooser, 1);

    auto * const verseChooser =
            new BtDropdownChooserButton(&BtBibleKeyWidget::populateVerseMenu,
                                        *this);
    verseChooser->setToolTip(tr("Select verse"));
    BT_CONNECT(verseChooser->menu(), &QMenu::triggered,
               [this](QAction * const action)
               { slotChangeVerse(action->property("verse").toInt()); });
    BT_CONNECT(verseChooser, &BtDropdownChooserButton::stepItem,
               this, &BtBibleKeyWidget::slotStepVerse);
    dropDownButtonsLayout->addWidget(verseChooser, 1);

    dropDownButtonsLayout->setContentsMargins(0, 0, 0, 0);
    dropDownButtonsLayout->setSpacing(0);
    m_dropDownButtons->setLayout(dropDownButtonsLayout);
    m_dropDownButtons->hide();

    m_dropDownButtons->installEventFilter(this);

    m_dropDownHoverTimer.setInterval(500);
    m_dropDownHoverTimer.setSingleShot(true);
    BT_CONNECT(&m_dropDownHoverTimer, &QTimer::timeout,
               m_dropDownButtons, &QWidget::hide);

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
    auto const initScrollerConnections =
            [this](CScrollerWidgetSet & scroller,
                   void (BtBibleKeyWidget::*stepFunction)(int))
            {
                BT_CONNECT(&scroller, &CScrollerWidgetSet::change,
                           this, stepFunction);
                BT_CONNECT(&scroller, &CScrollerWidgetSet::scroller_pressed,
                           [this]{
                               updatelock = true;
                               oldKey = m_key->key();
                           });
                BT_CONNECT(&scroller, &CScrollerWidgetSet::scroller_released,
                           [this]{
                               updatelock = false;
                               if (oldKey != m_key->key())
                                   Q_EMIT changed(m_key);
                           });
            };
    initScrollerConnections(*m_bookScroller, &BtBibleKeyWidget::slotStepBook);
    initScrollerConnections(*m_chapterScroller,
                            &BtBibleKeyWidget::slotStepChapter);
    initScrollerConnections(*m_verseScroller, &BtBibleKeyWidget::slotStepVerse);

    BT_CONNECT(m_textbox, &QLineEdit::returnPressed,
               [this]{
                   m_key->setKey(m_textbox->text());
                   Q_EMIT changed(m_key);
               });

    BT_CONNECT(m_key->afterChangedSignaller(), &BtSignal::signal,
               this,                           &BtBibleKeyWidget::updateText);

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

void BtBibleKeyWidget::slotStepBook(int offset) {
    if(offset >= 0)
        for(; offset != 0; offset--)
            m_key->next( CSwordVerseKey::UseBook );
    else
        for(; offset != 0; offset++)
            m_key->previous( CSwordVerseKey::UseBook );

    if (!updatelock)
        Q_EMIT changed(m_key);
}

void BtBibleKeyWidget::slotStepChapter(int offset) {
    if(offset >= 0)
        for(; offset != 0; offset--)
            m_key->next( CSwordVerseKey::UseChapter );
    else
        for(; offset != 0; offset++)
            m_key->previous( CSwordVerseKey::UseChapter );

    if (!updatelock)
        Q_EMIT changed(m_key);
}

void BtBibleKeyWidget::slotStepVerse(int offset) {
    if(offset >= 0)
        for(; offset != 0; offset--)
            m_key->next( CSwordVerseKey::UseVerse );
    else
        for(; offset != 0; offset++)
            m_key->previous( CSwordVerseKey::UseVerse );

    if (!updatelock)
        Q_EMIT changed(m_key);
}


void BtBibleKeyWidget::slotChangeVerse(int n) {
    if (m_key->verse() != n) {
        m_key->setVerse(n);
        m_key->emitAfterChanged();
        setKey( m_key );
    }
    if (!updatelock)
        Q_EMIT changed(m_key);
}

void BtBibleKeyWidget::slotChangeChapter(int n) {
    if (m_key->chapter() != n) {
        m_key->setChapter(n);
        m_key->emitAfterChanged();
        setKey( m_key );
    }
    if (!updatelock)
        Q_EMIT changed(m_key);
}

void BtBibleKeyWidget::slotChangeBook(QString bookname) {
    if (m_key->bookName() != bookname) {
        m_key->setBookName(bookname);
        m_key->emitAfterChanged();
        setKey( m_key );
    }
    if (!updatelock)
        Q_EMIT changed(m_key);
}


void BtBibleKeyWidget::populateBookMenu(QMenu & menu) {
    for (auto const & bookname : m_module->books())
        menu.addAction(bookname)->setProperty("bookname", bookname);
}

void BtBibleKeyWidget::populateChapterMenu(QMenu & menu) {
    int count = m_module->chapterCount(m_key->book());
    for (int i = 1; i <= count; i++)
        menu.addAction(QString::number(i))->setProperty("chapter", i);
}

void BtBibleKeyWidget::populateVerseMenu(QMenu & menu) {
    int count = m_module->verseCount(m_key->bookName(), m_key->chapter());
    for (int i = 1; i <= count; i++)
        menu.addAction(QString::number(i))->setProperty("verse", i);
}
