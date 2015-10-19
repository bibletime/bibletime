/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/display/btfindwidget.h"

#include "QCheckBox"
#include "QHBoxLayout"
#include "QLineEdit"
#include "QSpacerItem"
#include "QToolButton"
#include "util/cresmgr.h"


namespace {
inline QToolButton * newToolButton(QIcon const & icon,
                                   char const * const slot,
                                   QWidget * const parent,
                                   QHBoxLayout * const layout)
{
    QToolButton * const button = new QToolButton(parent);
    button->setIcon(icon);
    button->setIconSize(QSize(16, 16));
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setAutoRaise(true);
    layout->addWidget(button);
    #ifndef QT_NO_DEBUG
    bool const ok =
    #endif
        QObject::connect(button, SIGNAL(released()), parent, slot);
    #ifndef QT_NO_DEBUG
    Q_ASSERT(ok);
    #endif
    return button;
}
} // anonymous namespace

BtFindWidget::BtFindWidget(QWidget * parent)
    : QWidget(parent)
{
    // Overall layout:
    m_layout = new QHBoxLayout(this);
    m_layout->setMargin(0);
    m_layout->setSpacing(8);

    // Buttons and text editor:
    #define newButton(...) newToolButton(__VA_ARGS__, this, m_layout)

    // Close button:
    newButton(CResMgr::findWidget::icon_close(), SLOT(hide()));

    // Text editor:
    m_textEditor = new QLineEdit(this);
    m_layout->addWidget(m_textEditor);
    bool ok = connect(m_textEditor, SIGNAL(textChanged(QString const &)),
                      this,         SLOT(textChanged(QString const &)));
    Q_ASSERT(ok);
    ok = connect(m_textEditor, SIGNAL(returnPressed()),
                 this,         SLOT(returnPressed()));
    Q_ASSERT(ok);

    // Next and Previous buttons:
    m_previousButton = newButton(CResMgr::findWidget::icon_previous(),
                                 SLOT(findPrevious()));
    m_nextButton = newButton(CResMgr::findWidget::icon_next(),
                             SLOT(findNext()));

    // Case checkbox:
    m_caseCheckBox = new QCheckBox(this);
    ok = connect(m_caseCheckBox, SIGNAL(stateChanged(int)),
                 this,           SLOT(caseStateChanged(int)));
    Q_ASSERT(ok);
    m_layout->addWidget(m_caseCheckBox);

    // Spacer:
    m_layout->addItem(new QSpacerItem(0,
                                      0,
                                      QSizePolicy::Expanding,
                                      QSizePolicy::Minimum));
    setFocusProxy(m_textEditor);

    retranslateUi();
}

void BtFindWidget::retranslateUi() {
    m_textEditor->setToolTip(tr("The text you want to search for",
                                "findWidget"));
    m_previousButton->setText(tr("Previous"));
    m_nextButton->setText(tr("Next"));
    m_caseCheckBox->setText(tr("Match case"));
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
