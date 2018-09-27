/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "frontend/cinfodisplay.h"

#include <memory>
#include <QAction>
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QRegExp>
#include <QSize>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QtAlgorithms>
#include <QMenu>
#include "backend/config/btconfig.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordkey.h"
#include "backend/keys/cswordversekey.h"
#include "backend/managers/referencemanager.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "bibletime.h"
#include "frontend/crossrefrendering.h"
#include "util/btassert.h"
#include "util/btconnect.h"


using namespace Rendering;
using namespace sword;

namespace InfoDisplay {

CInfoDisplay::CInfoDisplay(BibleTime * parent)
        : QWidget(parent)
        , m_mainWindow(parent)
{
    QVBoxLayout * const layout = new QVBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2); // Leave small border
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_textBrowser = new QTextBrowser(this);
    m_textBrowser->setContextMenuPolicy(Qt::CustomContextMenu);
    BT_CONNECT(m_textBrowser, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(slotContextMenu(const QPoint&)));
    BT_CONNECT(m_textBrowser, SIGNAL(anchorClicked(const QUrl&)),
               this, SLOT(lookupInfo(const QUrl&)));
    layout->addWidget(m_textBrowser);
    unsetInfo();
}

void CInfoDisplay::slotContextMenu(const QPoint& /* point */ ) {

    QAction selectAllAction(tr("Select all"));
    selectAllAction.setShortcut(QKeySequence::SelectAll);
    BT_CONNECT(&selectAllAction, SIGNAL(triggered()),
               this,            SLOT(selectAll()));

    QAction copyAction(tr("Copy"));
    copyAction.setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    BT_CONNECT(&copyAction,                     SIGNAL(triggered()),
               m_textBrowser, SLOT(copy()));

    QMenu menu;
    menu.addAction(&selectAllAction);
    menu.addAction(&copyAction);
    menu.exec(QCursor::pos());
}

void CInfoDisplay::unsetInfo() {
    setInfo(tr("<small>This is the Mag viewer area. Hover the mouse over links "
               "or other items which include some data and the contents appear "
               "in the Mag after a short delay. Move the mouse into Mag "
               "rapidly or lock the view by pressing and holding Shift while "
               "moving the mouse.</small>"));
}

void CInfoDisplay::setInfo(const QString & renderedData, const QString & lang) {
    QString text = Rendering::formatInfo(renderedData, lang);
    text.replace("#CHAPTERTITLE#", "");
    m_textBrowser->setText(text);
}

void CInfoDisplay::lookupInfo(const QUrl & url) {


    if (!url.isEmpty() && ReferenceManager::isHyperlink(url.toString())) {
        QString module;
        QString keyName;
        ReferenceManager::Type type;

        ReferenceManager::decodeHyperlink(url.toString(), module, keyName, type);
        if (module.isEmpty()) {
            module = ReferenceManager::preferredModule( type );
        }

        qDebug() << "CInfoDisplay::lookup";
        qDebug() <<  module <<  keyName << type;
        CSwordModuleInfo * const m = CSwordBackend::instance()->findModuleByName(module);
        BT_ASSERT(m);
        std::unique_ptr<CSwordKey> key(CSwordKey::createInstance(m));
        key->setKey(keyName);

        setInfo(key->renderedText(), m->language()->abbrev());
    }
}

void CInfoDisplay::setInfo(const Rendering::InfoType type, const QString & data) {
    setInfo(Rendering::ListInfoData() << qMakePair(type, data));
}

void CInfoDisplay::setInfo(const Rendering::ListInfoData & list) {
    // If the widget is hidden it would be inefficient to render and display the data
    if (!isVisible())
        return;

    if (list.isEmpty()) {
        m_textBrowser->setText("");
        return;
    }

    BtConstModuleList l;
    const CSwordModuleInfo * m(m_mainWindow->getCurrentModule());
    if(m != nullptr)
        l.append(m);
    setBrowserFont(m);
    setInfo(Rendering::formatInfo(list, l));
}

void CInfoDisplay::setInfo(CSwordModuleInfo * const module) {
    if (module) {
        setInfo(tr("<div class=\"moduleinfo\"><h3>%1</h3><p>%2</p><p>Version: %3</p></div>")
                .arg(module->name().toHtmlEscaped())
                .arg(module->config(CSwordModuleInfo::Description).toHtmlEscaped())
                .arg(module->config(CSwordModuleInfo::ModuleVersion).toHtmlEscaped()));
    } else {
        unsetInfo();
    }
}

void CInfoDisplay::setBrowserFont(const CSwordModuleInfo* const module) {
    if (module) {
            const CLanguageMgr::Language* lang = module->language();
            m_textBrowser->setFont(btConfig().getFontForLanguage(*lang).second);
    } else {
        m_textBrowser->setFont(btConfig().getDefaultFont());
    }
}

void CInfoDisplay::selectAll() {
    m_textBrowser->selectAll();
}

QSize CInfoDisplay::sizeHint() const {
    return QSize(100, 150);
}

} //end of namespace InfoDisplay
