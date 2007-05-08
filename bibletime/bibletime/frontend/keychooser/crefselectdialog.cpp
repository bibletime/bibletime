/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "crefselectdialog.h"

// QT
#include "qlayout.h"
#include "qlineedit.h"
#include "qlabel.h"

// backend
#include "backend/cswordkey.h"
#include "backend/cswordversekey.h"

//KDE includes
#include <klocale.h>


//CRefSelectDialog::CRefSelectDialog(QWidget *parent)
//  : KDialogBase(parent, "Refenences",
//  true, i18n("Refenence selector"),
//  KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, true),
//  m_oldPos(0,0)
//
//{
//   m_page = new QWidget( this );
//   m_placeholder = new QWidget( m_page);
//   QVBoxLayout* layout = new QVBoxLayout( m_page, 0, spacingHint());
//   m_lineedit = new QLineEdit( QString::null, m_page, "select" );
//   m_lineedit->setMinimumWidth(fontMetrics().maxWidth()*8);
//
//   QHBoxLayout* hlayout = new QHBoxLayout(m_page, 0, spacingHint());
//   hlayout->addWidget(new QLabel(i18n("Verse key:"), m_page));
//   hlayout->addWidget(m_lineedit);
//   layout->addItem(hlayout);
//   layout->addWidget(m_placeholder);
//
//   m_chooser = NULL;
//   m_oldParent = NULL;
//   setMainWidget(m_page);
//
//   connect(this, SIGNAL(okClicked()), SLOT(slotPressedOk()));
//   connect(this, SIGNAL(cancelClicked()), SLOT(slotPressedCancel()));
//}
//
//CRefSelectDialog::~CRefSelectDialog() {
//}
//
//
//void CRefSelectDialog::setChooser(CKeyChooser* const chooser) {
//  if (chooser && chooser != m_chooser) {
//    if (m_chooser) restoreParent();
//
//    m_chooser = chooser;
//
//    // store old owner and position
//    m_oldParent = chooser->parentWidget(false);
//    m_oldPos = chooser->pos();
//
//    // we temporary borrow chooser widget from the MDI window
//    m_placeholder->setMinimumSize(chooser->size());
//    chooser->reparent(m_placeholder, QPoint(0,0), true);
//  }
//}
//
//void CRefSelectDialog::slotPressedOk() {
//  // go to proper key
//  CSwordKey* key = m_chooser->key();
//  key->key(m_lineedit->text());
//  m_chooser->setKey(key);
//
//  // restore parent of stolen widget
//  restoreParent();
//}
//
//void CRefSelectDialog::slotPressedCancel() {
//  // restore parent of stolen widget
//  restoreParent();
//}
//
//void CRefSelectDialog::restoreParent() {
//  if (m_chooser && m_oldParent) {
//    m_chooser->reparent(m_oldParent, m_oldPos, true);
//  }
//  m_chooser = NULL;
//}






