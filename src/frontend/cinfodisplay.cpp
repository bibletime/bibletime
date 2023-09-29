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

#include "cinfodisplay.h"

#include <memory>
#include <QAction>
#include <QLabel>
#include <QLayout>
#include <QSize>
#include <QVBoxLayout>
#include <QtAlgorithms>
#include <QMenu>
#include "../backend/config/btconfig.h"
#include "../backend/drivers/cswordmoduleinfo.h"
#include "../backend/keys/cswordkey.h"
#include "../backend/managers/colormanager.h"
#include "../backend/managers/referencemanager.h"
#include "../util/btassert.h"
#include "../util/btconnect.h"
#include "bibletime.h"
#include "bttextbrowser.h"

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
    m_textBrowser = new BtTextBrowser(this);
    m_textBrowser->setContextMenuPolicy(Qt::CustomContextMenu);
    BT_CONNECT(m_textBrowser, &QTextBrowser::customContextMenuRequested,
               [this]{
                   QAction selectAllAction(tr("Select all"));
                   selectAllAction.setShortcut(QKeySequence::SelectAll);
                   BT_CONNECT(&selectAllAction, &QAction::triggered,
                              m_textBrowser, &QTextBrowser::selectAll);

                   QAction copyAction(tr("Copy"));
                   copyAction.setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));
                   BT_CONNECT(&copyAction, &QAction::triggered,
                              m_textBrowser, &QTextBrowser::copy);

                   QMenu menu;
                   menu.addAction(&selectAllAction);
                   menu.addAction(&copyAction);
                   menu.exec(QCursor::pos());
               });
    BT_CONNECT(m_textBrowser, &QTextBrowser::anchorClicked,
               [this](QUrl const & url) {
                   auto const decodedLink(
                               ReferenceManager::decodeHyperlink(
                                   url.toString()));
                   if (!decodedLink && !decodedLink->module)
                       return;

                   auto const * const m = decodedLink->module;
                   std::unique_ptr<CSwordKey> key(m->createKey());
                   key->setKey(decodedLink->key);

                   setInfo(key->renderedText(), m->language()->abbrev());
               });
    layout->addWidget(m_textBrowser);
    unsetInfo();
}

void CInfoDisplay::unsetInfo() {
    setInfo(tr("<small>This is the Mag viewer area. Hover the mouse over links "
               "or other items which include some data and the contents appear "
               "in the Mag after a short delay. Move the mouse into Mag "
               "rapidly or lock the view by pressing and holding Shift while "
               "moving the mouse.</small>"));
}

void CInfoDisplay::updateColors() {
    QPalette p = m_textBrowser->palette();
    p.setColor(QPalette::Base, ColorManager::getBackgroundColor());
    p.setColor(QPalette::Text, ColorManager::getForegroundColor());
    m_textBrowser->setPalette(p);
}

void CInfoDisplay::setInfo(const QString & renderedData, const QString & lang) {
    QString text = Rendering::formatInfo(renderedData, lang);
    text.replace(QStringLiteral("#CHAPTERTITLE#"), QString());
    text.replace(QStringLiteral("#TEXT_ALIGN#"), QStringLiteral("left"));
    text = ColorManager::replaceColors(text);
    m_textBrowser->setText(text);
}

void CInfoDisplay::setInfo(const Rendering::InfoType type, const QString & data) {
    setInfo(Rendering::ListInfoData() << qMakePair(type, data));
}

void CInfoDisplay::setInfo(const Rendering::ListInfoData & list) {
    // If the widget is hidden it would be inefficient to render and display the data
    if (!isVisible())
        return;

    if (list.isEmpty()) {
        m_textBrowser->setText(QString());
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
            auto const lang = module->language();
            m_textBrowser->setFont(btConfig().getFontForLanguage(*lang).second);
    } else {
        m_textBrowser->setFont(btConfig().getDefaultFont());
    }
}

QSize CInfoDisplay::sizeHint() const {
    return QSize(100, 150);
}

} //end of namespace InfoDisplay
