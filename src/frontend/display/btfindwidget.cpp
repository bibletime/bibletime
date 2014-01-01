/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/display/btfindwidget.h"

#include "QApplication"
#include "QCheckBox"
#include "QHBoxLayout"
#include "QLineEdit"
#include "QSpacerItem"
#include "QToolButton"
#include "bibletimeapp.h"
#include "util/cresmgr.h"
#include "util/geticon.h"


BtFindWidget::BtFindWidget(QWidget* parent)
        : QWidget(parent) {
    createLayout();
    createToolButton(CResMgr::findWidget::close_icon, "", SLOT(hide()));
    createTextEditor();
    createToolButton(CResMgr::findWidget::previous_icon, tr("Previous"), SLOT(findPrevious()));
    createToolButton(CResMgr::findWidget::next_icon, tr("Next"), SLOT(findNext()));
    createCaseCheckBox();
    createSpacer();
    setFocusProxy(m_textEditor);
}

BtFindWidget::~BtFindWidget() {
}

void BtFindWidget::createLayout() {
    m_layout = new QHBoxLayout(this);
    m_layout->setMargin(0);
    m_layout->setSpacing(8);
}

void BtFindWidget::createToolButton(const QString& iconName, const QString& text, const char* slot) {
    QToolButton* button = new QToolButton(this);
    button->setIcon(util::getIcon(iconName));
    button->setIconSize(QSize(16,16));
    button->setText(text);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setAutoRaise(true);
    m_layout->addWidget(button);
    bool ok = connect(button, SIGNAL(released()), this, slot);
    Q_ASSERT(ok);
}

void BtFindWidget::createTextEditor() {
    m_textEditor = new QLineEdit(this);
#if QT_VERSION < 0x050000
    m_textEditor->setToolTip(QApplication::translate("findWidget",
        "The text you want to search for", 0, QApplication::UnicodeUTF8));
#else
    m_textEditor->setToolTip(QApplication::translate("findWidget",
        "The text you want to search for", 0));
#endif
    m_layout->addWidget(m_textEditor);
    bool ok = connect(m_textEditor, SIGNAL(textChanged(const QString&)),
        this, SLOT(textChanged(const QString&)));
    Q_ASSERT(ok);
    ok = connect(m_textEditor,SIGNAL(returnPressed()), this, SLOT(returnPressed()));
    Q_ASSERT(ok);
}

void BtFindWidget::createCaseCheckBox() {
    m_caseCheckBox = new QCheckBox(tr("Match case"), this);
    m_layout->addWidget(m_caseCheckBox);
}

void BtFindWidget::createSpacer() {
    QSpacerItem* spacer = new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_layout->addItem(spacer);
}

void BtFindWidget::highlightText(const QString& text) {
    bool caseSensitive = m_caseCheckBox->checkState() == Qt::Checked;
    emit highlightText(text, caseSensitive);
}

void BtFindWidget::returnPressed() {
    bool caseSensitive = m_caseCheckBox->checkState() == Qt::Checked;
    QString text = m_textEditor->text();
    emit highlightText(text, caseSensitive);
    emit findNext(text, caseSensitive);
}

void BtFindWidget::textChanged(const QString& text) {
    bool caseSensitive = m_caseCheckBox->checkState() == Qt::Checked;
    emit highlightText(text, caseSensitive);
    emit findNext(text, caseSensitive);
}

void BtFindWidget::findNext() {
    bool caseSensitive = m_caseCheckBox->checkState() == Qt::Checked;
    QString text = m_textEditor->text();
    emit findNext(text, caseSensitive);
}

void BtFindWidget::findPrevious() {
    bool caseSensitive = m_caseCheckBox->checkState() == Qt::Checked;
    QString text = m_textEditor->text();
    emit findPrevious(text, caseSensitive);
}

void BtFindWidget::showAndSelect(){
    setVisible(true);
    QWidget::show();
    m_textEditor->selectAll();
    m_textEditor->setFocus(Qt::ShortcutFocusReason);
}
