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

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSpacerItem>
#include <QToolButton>
#include <utility>
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"


BtFindWidget::BtFindWidget(QWidget * parent)
    : QWidget(parent)
{
    // Overall layout:
    auto widgetLayout = new QHBoxLayout(this);
    widgetLayout->setMargin(0);
    widgetLayout->setSpacing(8);

    // Buttons and text editor:
    auto const newButton =
            [this, widgetLayout](QIcon const & icon, auto && ... slot) {
                QToolButton * const button = new QToolButton(this);
                button->setIcon(icon);
                button->setIconSize(QSize(16, 16));
                button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
                button->setAutoRaise(true);
                widgetLayout->addWidget(button);
                BT_CONNECT(button, &QToolButton::released,
                           std::forward<decltype(slot)>(slot)...);
                return button;
            };

    // Close button:
    newButton(CResMgr::findWidget::icon_close(), this, &BtFindWidget::hide);

    // Text editor:
    m_textEditor = new QLineEdit(this);
    widgetLayout->addWidget(m_textEditor);
    BT_CONNECT(m_textEditor, &QLineEdit::textChanged,
               [this](QString const & v) { highlightText(v, caseSensitive()); });
    BT_CONNECT(m_textEditor, &QLineEdit::returnPressed,
               [this] { highlightText(text(), caseSensitive()); });

    // Next and Previous buttons:
    m_previousButton = newButton(
                           CResMgr::findWidget::icon_previous(),
                           [this] { findPrevious(text(), caseSensitive()); });
    m_nextButton = newButton(CResMgr::findWidget::icon_next(),
                             [this] { findNext(text(), caseSensitive()); });

    // Case checkbox:
    m_caseCheckBox = new QCheckBox(this);
    BT_CONNECT(m_caseCheckBox, &QCheckBox::stateChanged,
               [this](int const s) { highlightText(text(), s == Qt::Checked); });
    widgetLayout->addWidget(m_caseCheckBox);

    // Spacer:
    widgetLayout->addItem(new QSpacerItem(0,
                                          0,
                                          QSizePolicy::Expanding,
                                          QSizePolicy::Minimum));
    setFocusProxy(m_textEditor);

    retranslateUi();
}

bool BtFindWidget::caseSensitive() const
{ return m_caseCheckBox->checkState() == Qt::Checked; }

QString BtFindWidget::text() const { return m_textEditor->text(); }

void BtFindWidget::showAndSelect() {
    setVisible(true);
    show();
    m_textEditor->selectAll();
    m_textEditor->setFocus(Qt::ShortcutFocusReason);
}

void BtFindWidget::retranslateUi() {
    m_textEditor->setToolTip(tr("The text you want to search for",
                                "findWidget"));
    m_previousButton->setText(tr("Previous"));
    m_nextButton->setText(tr("Next"));
    m_caseCheckBox->setText(tr("Match case"));
}
