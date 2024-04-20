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


BtBibleKeyWidget::BtBibleKeyWidget(
        CSwordBibleModuleInfo const * mod,
        CSwordVerseKey * key,
        QWidget * parent)
   : QWidget(parent)
   , m_key(key)
   , m_dropDownHoverTimer(this)
   , m_module(mod)
{
    auto const slotStep =
            [this](int offset, CSwordVerseKey::JumpType const jumpType) {
                if (!offset)
                    return;
                if (offset > 0) {
                    do {
                        m_key->next(jumpType);
                    } while (--offset);
                } else {
                    do {
                        m_key->previous(jumpType);
                    } while (++offset);
                }
                if (!updatelock)
                    Q_EMIT changed(m_key);
            };
    auto const slotStepBook =
            [slotStep](int offset)
            { slotStep(offset, CSwordVerseKey::UseBook); };
    auto const slotStepChapter =
            [slotStep](int offset)
            { slotStep(offset, CSwordVerseKey::UseChapter); };
    auto const slotStepVerse =
            [slotStep](int offset)
            { slotStep(offset, CSwordVerseKey::UseVerse); };

    updatelock = false;

    setFocusPolicy(Qt::WheelFocus);

    QToolButton* clearRef = new QToolButton(this);
    clearRef->setIcon(CResMgr::icon_clearEdit());
    clearRef->setAutoRaise(true);
    clearRef->setStyleSheet(QStringLiteral("QToolButton{margin:0px;}"));
    BT_CONNECT(clearRef, &QToolButton::clicked,
               [this]{
                   m_textbox->setText(QString());
                   m_textbox->setFocus();
               });

    auto * const bookScroller = new CScrollerWidgetSet(this);

    m_textbox = new BtLineEdit( this );
    setFocusProxy(m_textbox);
    m_textbox->setContentsMargins(0, 0, 0, 0);

    auto * const chapterScroller = new CScrollerWidgetSet(this);
    auto * const verseScroller = new CScrollerWidgetSet(this);

    QHBoxLayout* m_mainLayout = new QHBoxLayout( this );
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->addWidget(clearRef);
    m_mainLayout->addWidget(bookScroller);
    m_mainLayout->addWidget(m_textbox);
    m_mainLayout->addWidget(chapterScroller);
    m_mainLayout->addWidget(verseScroller);


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
               [this](QAction * const action) {
                    auto bookname = action->property("bookname").toString();
                    if (m_key->bookName() != bookname) {
                        m_key->setBookName(std::move(bookname));
                        updateText();
                    }
                    if (!updatelock)
                        Q_EMIT changed(m_key);
               });
    BT_CONNECT(bookChooser, &BtDropdownChooserButton::stepItem, slotStepBook);
    dropDownButtonsLayout->addWidget(bookChooser, 2);

    auto * const chapterChooser =
            new BtDropdownChooserButton(&BtBibleKeyWidget::populateChapterMenu,
                                        *this);
    chapterChooser->setToolTip(tr("Select chapter"));
    BT_CONNECT(chapterChooser->menu(), &QMenu::triggered,
               [this](QAction * const action) {
                   int const n = action->property("chapter").toInt();
                   if (m_key->chapter() != n) {
                       m_key->setChapter(n);
                       updateText();
                   }
                   if (!updatelock)
                       Q_EMIT changed(m_key);
               });
    BT_CONNECT(chapterChooser,
               &BtDropdownChooserButton::stepItem,
               slotStepChapter);
    dropDownButtonsLayout->addWidget(chapterChooser, 1);

    auto * const verseChooser =
            new BtDropdownChooserButton(&BtBibleKeyWidget::populateVerseMenu,
                                        *this);
    verseChooser->setToolTip(tr("Select verse"));
    BT_CONNECT(verseChooser->menu(), &QMenu::triggered,
               [this](QAction * const action) {
                   int const n = action->property("verse").toInt();
                   if (m_key->verse() != n) {
                       m_key->setVerse(n);
                       updateText();
                   }
                   if (!updatelock)
                       Q_EMIT changed(m_key);
               });
    BT_CONNECT(verseChooser, &BtDropdownChooserButton::stepItem, slotStepVerse);
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
    bookScroller->setToolTips(
        tr("Next book"),
        scrollButtonToolTip,
        tr("Previous book")
    );
    chapterScroller->setToolTips(
        tr("Next chapter"),
        scrollButtonToolTip,
        tr("Previous chapter")
    );
    verseScroller->setToolTips(
        tr("Next verse"),
        scrollButtonToolTip,
        tr("Previous verse")
    );

    // signals and slots connections
    auto const initScrollerConnections =
            [this](CScrollerWidgetSet & scroller, auto stepFunction)
            {
                BT_CONNECT(&scroller,
                           &CScrollerWidgetSet::change,
                           stepFunction);
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
    initScrollerConnections(*bookScroller, slotStepBook);
    initScrollerConnections(*chapterScroller, slotStepChapter);
    initScrollerConnections(*verseScroller, slotStepVerse);

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

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
void BtBibleKeyWidget::enterEvent(QEvent *) {
#else
void BtBibleKeyWidget::enterEvent(QEnterEvent *) {
#endif
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
    int nw(m_textbox->minimumSizeHint().width() + fm.horizontalAdvance(text));
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

void BtBibleKeyWidget::populateBookMenu(QMenu & menu) {
    for (auto const & bookname : m_module->books())
        menu.addAction(bookname)->setProperty("bookname", bookname);
}

void BtBibleKeyWidget::populateChapterMenu(QMenu & menu) {
    int count = m_module->chapterCount(m_key->bibleBook());
    for (int i = 1; i <= count; i++)
        menu.addAction(QString::number(i))->setProperty("chapter", i);
}

void BtBibleKeyWidget::populateVerseMenu(QMenu & menu) {
    int count = m_module->verseCount(m_key->bookName(), m_key->chapter());
    for (int i = 1; i <= count; i++)
        menu.addAction(QString::number(i))->setProperty("verse", i);
}
