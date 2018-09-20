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
#include "frontend/display/bthtmlreaddisplay.h"
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

    m_htmlPart = new BtHtmlReadDisplay(nullptr, this);
    m_htmlPart->setMouseTracking(false); // We don't want strong/lemma/note mouse infos
    m_htmlPart->view()->setAcceptDrops(false);

    QAction * const selectAllAction = new QAction(QIcon(), tr("Select all"), this);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    BT_CONNECT(selectAllAction, SIGNAL(triggered()),
               this,            SLOT(selectAll()));

    QAction * const copyAction = new QAction(tr("Copy"), this);
    copyAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    BT_CONNECT(copyAction,                     SIGNAL(triggered()),
               m_htmlPart->connectionsProxy(), SLOT(copySelection()));

    QMenu * const menu = new QMenu(this);
    menu->addAction(selectAllAction);
    menu->addAction(copyAction);
    m_htmlPart->installPopup(menu);

    BT_CONNECT(m_htmlPart->connectionsProxy(),
               SIGNAL(referenceClicked(QString const &, QString const &)),
               SLOT(lookupInfo(QString const &, QString const &)));

    layout->addWidget(m_htmlPart->view());

    unsetInfo();
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
    m_htmlPart->setText(text);
}

void CInfoDisplay::lookupInfo(const QString & mod_name,
                              const QString & key_text)
{
    qDebug() << "CInfoDisplay::lookup";
    qDebug() <<  mod_name <<  key_text;
    CSwordModuleInfo * const m = CSwordBackend::instance()->findModuleByName(mod_name);
    BT_ASSERT(m);
    std::unique_ptr<CSwordKey> key(CSwordKey::createInstance(m));
    key->setKey(key_text);

    setInfo(key->renderedText(), m->language()->abbrev());
}

void CInfoDisplay::setInfo(const Rendering::InfoType type, const QString & data) {
    setInfo(Rendering::ListInfoData() << qMakePair(type, data));
}


void CInfoDisplay::setInfo(const Rendering::ListInfoData & list) {
    // If the widget is hidden it would be inefficient to render and display the data
    if (!isVisible())
        return;

    if (list.isEmpty()) {
        m_htmlPart->setText("<html></html>");
        return;
    }

    BtConstModuleList l;
    const CSwordModuleInfo * m(m_mainWindow->getCurrentModule());
    if(m != nullptr)
        l.append(m);
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

void CInfoDisplay::selectAll() {
    m_htmlPart->selectAll();
}

QSize CInfoDisplay::sizeHint() const {
    return QSize(100, 150);
}

} //end of namespace InfoDisplay
