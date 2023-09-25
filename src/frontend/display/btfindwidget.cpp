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

#include "btfindwidget.h"

#include <QIcon>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSize>
#include <QSizePolicy>
#include <QSpacerItem>
#include <Qt>
#include <QTimerEvent>
#include <QToolButton>
#include <utility>
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"


BtFindWidget::BtFindWidget(QWidget * parent)
    : QWidget(parent)
{
    // Overall layout:
    auto widgetLayout = new QHBoxLayout(this);
    widgetLayout->setContentsMargins(0, 0, 0, 0);
    widgetLayout->setSpacing(8);

    // Buttons and text editor:
    auto const newButton =
            [this, widgetLayout](QIcon const & icon) {
                QToolButton * const button = new QToolButton(this);
                button->setIcon(icon);
                button->setIconSize(QSize(16, 16));
                button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
                button->setAutoRaise(true);
                widgetLayout->addWidget(button);
                return button;
            };

    // Close button:
    auto * const closeButton = newButton(CResMgr::findWidget::icon_close());
    BT_CONNECT(closeButton, &QToolButton::released, this, &BtFindWidget::hide);

    // Text editor:
    m_textEditor = new QLineEdit(this);
    widgetLayout->addWidget(m_textEditor);
    BT_CONNECT(m_textEditor, &QLineEdit::textChanged,
               this, &BtFindWidget::queueHighlight);
    BT_CONNECT(m_textEditor, &QLineEdit::returnPressed,
               this, &BtFindWidget::highlightImmediately);

    // Next and Previous buttons:
    m_previousButton = newButton(CResMgr::findWidget::icon_previous());
    BT_CONNECT(m_previousButton, &QToolButton::released,
               this, &BtFindWidget::highlightImmediately);
    BT_CONNECT(m_previousButton, &QToolButton::released,
               this, &BtFindWidget::findPrevious);
    m_nextButton = newButton(CResMgr::findWidget::icon_next());
    BT_CONNECT(m_nextButton, &QToolButton::released,
               this, &BtFindWidget::highlightImmediately);
    BT_CONNECT(m_nextButton, &QToolButton::released,
               this, &BtFindWidget::findNext);

    // Case checkbox:
    m_caseCheckBox = new QCheckBox(this);
    BT_CONNECT(m_caseCheckBox, &QCheckBox::stateChanged,
               this, &BtFindWidget::queueHighlight);
    widgetLayout->addWidget(m_caseCheckBox);

    // Spacer:
    widgetLayout->addItem(new QSpacerItem(0,
                                          0,
                                          QSizePolicy::Expanding,
                                          QSizePolicy::Minimum));
    setFocusProxy(m_textEditor);

    retranslateUi();

    BT_ASSERT(m_lastHighlightState.text == m_textEditor->text());
    BT_ASSERT(m_lastHighlightState.caseSensitive
              == m_caseCheckBox->isChecked());
}

void BtFindWidget::showAndSelect() {
    setVisible(true);
    m_textEditor->selectAll();
    m_textEditor->setFocus(Qt::ShortcutFocusReason);
}

void BtFindWidget::timerEvent(QTimerEvent * const event) {
    auto const timerId = event->timerId();
    BT_ASSERT(timerId);
    if (timerId != m_throttleTimerId)
        return QObject::timerEvent(event);

    event->accept();
    highlightImmediately();
}

void BtFindWidget::queueHighlight() {
    killTimer(m_throttleTimerId);
    m_throttleTimerId = startTimer(900);
}

void BtFindWidget::highlightImmediately() {
    killTimer(m_throttleTimerId);
    m_throttleTimerId = 0;

    HighlightState newState{m_textEditor->text(), m_caseCheckBox->isChecked()};
    if (m_lastHighlightState != newState) {
        m_lastHighlightState = std::move(newState);
        Q_EMIT highlightText(m_textEditor->text(), m_caseCheckBox->isChecked());
    }
}

void BtFindWidget::retranslateUi() {
    m_textEditor->setToolTip(tr("The text you want to search for",
                                "findWidget"));
    m_previousButton->setText(tr("Previous"));
    m_nextButton->setText(tr("Next"));
    m_caseCheckBox->setText(tr("Match case"));
}
