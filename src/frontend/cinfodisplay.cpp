/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/cinfodisplay.h"

#include <QScopedPointer>
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
#include "util/htmlescape.h"

// Sword includes:
#include <listkey.h>


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

    m_htmlPart = new BtHtmlReadDisplay(0, this);
    m_htmlPart->setMouseTracking(false); // We don't want strong/lemma/note mouse infos
    m_htmlPart->view()->setAcceptDrops(false);

    QAction * const selectAllAction = new QAction(QIcon(), tr("Select all"), this);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    QObject::connect(selectAllAction, SIGNAL(triggered()),
                     this,            SLOT(selectAll()));

    QAction * const copyAction = new QAction(tr("Copy"), this);
    copyAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    QObject::connect(copyAction,                     SIGNAL(triggered()),
                     m_htmlPart->connectionsProxy(), SLOT(copySelection()));

    QMenu * const menu = new QMenu(this);
    menu->addAction(selectAllAction);
    menu->addAction(copyAction);
    m_htmlPart->installPopup(menu);

    QObject::connect(m_htmlPart->connectionsProxy(),
                     SIGNAL(referenceClicked(const QString &, const QString &)),
                     SLOT(lookupInfo(const QString &, const QString &)));

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
    CDisplayTemplateMgr * const mgr = CDisplayTemplateMgr::instance();
    Q_ASSERT(mgr != 0);

    CDisplayTemplateMgr::Settings settings;
    settings.pageCSS_ID = "infodisplay";

    QString div = "<div class=\"infodisplay\"";
    if (!lang.isEmpty())
        div.append(" lang=\"").append(lang).append("\"");

    div.append(">");

    QString content(mgr->fillTemplate(CDisplayTemplateMgr::activeTemplateName(),
                                      div + renderedData + "</div>",
                                      settings));
    m_htmlPart->setText(content);
}

void CInfoDisplay::lookupInfo(const QString & mod_name,
                              const QString & key_text)
{
    qDebug() << "CInfoDisplay::lookup";
    qDebug() <<  mod_name <<  key_text;
    CSwordModuleInfo * const m = CSwordBackend::instance()->findModuleByName(mod_name);
    Q_ASSERT(m);
    if (!m)
        return;
    QScopedPointer<CSwordKey> key(CSwordKey::createInstance(m));
    key->setKey(key_text);

    setInfo(key->renderedText(), m->language()->abbrev());
}

void CInfoDisplay::setInfo(const InfoType type, const QString & data) {
    ListInfoData list;
    list.append(qMakePair(type, data));
    setInfo(list);
}


void CInfoDisplay::setInfo(const ListInfoData & list) {
    // If the widget is hidden it would be inefficient to render and display the data
    if (!isVisible())
        return;

    if (list.isEmpty()) {
        m_htmlPart->setText("<html></html>");
        return;
    }

    QString renderedText;

    ListInfoData::const_iterator end = list.end();
    for (ListInfoData::const_iterator it = list.begin(); it != end; ++it) {
        switch ((*it).first) {
            case Lemma:
                renderedText.append(decodeStrongs((*it).second));
                continue;
            case Morph:
                renderedText.append(decodeMorph((*it).second));
                continue;
            case CrossReference:
                renderedText.append(decodeCrossReference((*it).second));
                continue;
            case Footnote:
                renderedText.append(decodeFootnote((*it).second));
                continue;
            case WordTranslation:
                renderedText.append(getWordTranslation((*it).second));
                continue;
            case WordGloss:
                //text.append(getWordTranslation((*it).second));
                continue;
            case Abbreviation:
                renderedText.append(decodeAbbreviation((*it).second));
                continue;
            case Text:
                renderedText.append((*it).second);
                continue;
            default:
                continue;
        };
    }
    setInfo(renderedText);
}

void CInfoDisplay::setInfo(CSwordModuleInfo * const module) {
    using util::htmlEscape;

    if (module) {
        setInfo(tr("<div class=\"moduleinfo\"><h3>%1</h3><p>%2</p><p>Version: %3</p></div>")
                .arg(htmlEscape(module->name()))
                .arg(htmlEscape(module->config(CSwordModuleInfo::Description)))
                .arg(htmlEscape(module->config(CSwordModuleInfo::ModuleVersion))));
    } else {
        unsetInfo();
    }
}

void CInfoDisplay::selectAll() {
    m_htmlPart->selectAll();
}

QString CInfoDisplay::decodeAbbreviation(QString const & data) {
    // QStringList strongs = QStringList::split("|", data);
    return QString("<div class=\"abbreviation\"><h3>%1: %2</h3><p>%3</p></div>")
        .arg(tr("Abbreviation"))
        .arg("text")
        .arg(data);
}

QString CInfoDisplay::decodeCrossReference(QString const & data) {
    Q_ASSERT(!data.isEmpty());
    if (data.isEmpty())
        return QString("<div class=\"crossrefinfo\"><h3>%1</h3></div>")
               .arg(tr("Cross references"));

    // qWarning("setting crossref %s", data.latin1());

    DisplayOptions dispOpts;
    dispOpts.lineBreaks  = false;
    dispOpts.verseNumbers = true;

    FilterOptions filterOpts;
    CrossRefRendering renderer(dispOpts, filterOpts);
    CTextRendering::KeyTree tree;

    // const bool isBible = true;
    const CSwordModuleInfo * module = btConfig().getDefaultSwordModuleByType("standardBible");
    if (!module)
        module = m_mainWindow->getCurrentModule();

    // a prefixed module gives the module to look into
    QRegExp re("^[^ ]+:");
    // re.setMinimal(true);
    int pos = re.indexIn(data);
    if (pos != -1)
        pos += re.matchedLength() - 1;

    if (pos > 0) {
        const QString moduleName = data.left(pos);
        // qWarning("found module %s", moduleName.latin1());
        module = CSwordBackend::instance()->findModuleByName(moduleName);
        if (!module)
            module = btConfig().getDefaultSwordModuleByType("standardBible");
        // Q_ASSERT(module);
    }

    // Q_ASSERT(module); // why? the existense of the module is tested later
    CTextRendering::KeyTreeItem::Settings settings(
        false,
        CTextRendering::KeyTreeItem::Settings::CompleteShort
    );

    if (module && (module->type() == CSwordModuleInfo::Bible)) {
        VerseKey vk;
        sword::ListKey const refs =
                vk.parseVerseList(data.mid((pos == -1)
                                           ? 0
                                           : pos + 1).toUtf8().constData(),
                                  "Gen 1:1",
                                  true);

        for (int i = 0; i < refs.getCount(); i++) {
            SWKey const * const key = refs.getElement(i);
            Q_ASSERT(key);
            VerseKey const * const vk = dynamic_cast<VerseKey const *>(key);

            if (vk && vk->isBoundSet()) { // render a range of keys
                tree.append(new CTextRendering::KeyTreeItem(
                    QString::fromUtf8(vk->getLowerBound().getText()),
                    QString::fromUtf8(vk->getUpperBound().getText()),
                    module,
                    settings
                ));
            } else {
                tree.append(new CTextRendering::KeyTreeItem(
                    QString::fromUtf8(key->getText()),
                    QString::fromUtf8(key->getText()),
                    module,
                    settings
                ));
            }
        }
    } else if (module) {
        tree.append(new CTextRendering::KeyTreeItem(data.mid((pos == -1)
                                                             ? 0
                                                             : pos + 1),
                                                    module,
                                                    settings));
    }

    // qWarning("rendered the tree: %s", renderer.renderKeyTree(tree).latin1());
    // spanns containing rtl text need dir=rtl on their parent tag to be aligned properly
    QString lang = "en";  // default english
    if (module)
        lang = module->language()->abbrev();

    return QString("<div class=\"crossrefinfo\" lang=\"%1\"><h3>%2</h3><div class=\"para\" dir=\"%3\">%4</div></div>")
           .arg(lang)
           .arg(tr("Cross references"))
           .arg(module ? ((module->textDirection() == CSwordModuleInfo::LeftToRight) ? "ltr" : "rtl") : "")
           .arg(renderer.renderKeyTree(tree));
}

QString CInfoDisplay::decodeFootnote(QString const & data) {
    QStringList const list = data.split("/");
    Q_ASSERT(list.count() >= 3);

    FilterOptions filterOpts;
    filterOpts.footnotes   = true;
    CSwordBackend::instance()->setFilterOptions(filterOpts);

    const QString modulename = list.first();
    const QString swordFootnote = list.last();

    CSwordModuleInfo * const module = CSwordBackend::instance()->findModuleByName(modulename);
    if (!module)
        return QString::null;

    const char * const note =
        module->module()->getEntryAttributes()
        ["Footnote"][swordFootnote.toLatin1().data()]["body"].c_str();

    QString text = module->isUnicode() ? QString::fromUtf8(note) : QString(note);
    text = QString::fromUtf8(module->module()->renderText(
                                 module->isUnicode()
                                 ? static_cast<const char *>(text.toUtf8())
                                 : static_cast<const char *>(text.toLatin1())));

    return QString("<div class=\"footnoteinfo\" lang=\"%1\"><h3>%2</h3><p>%3</p></div>")
           .arg(module->language()->abbrev())
           .arg(tr("Footnote"))
           .arg(text);
}

QString CInfoDisplay::decodeStrongs(QString const & data) {
    QStringList strongs = data.split("|");
    QString ret;

    QStringList::const_iterator end = strongs.end();
    for (QStringList::const_iterator it = strongs.begin(); it != end; ++it) {
        CSwordModuleInfo * const module = btConfig().getDefaultSwordModuleByType
                                         (
                                             ((*it).left(1) == QString("H")) ?
                                             "standardHebrewStrongsLexicon" :
                                             "standardGreekStrongsLexicon"
                                         );

        QString text;
        if (module) {
            QScopedPointer<CSwordKey> key(CSwordKey::createInstance(module));
            key->setKey((*it).mid(1)); // skip H or G (language sign), will have to change later if we have better modules
            text = key->renderedText();
        }
        //if the module could not be found just display an empty lemma info

        QString lang = "en";  // default english
        if (module)
            lang = module->language()->abbrev();
        ret.append(
            QString("<div class=\"strongsinfo\" lang=\"%1\"><h3>%2: %3</h3><p>%4</p></div>")
            .arg(lang)
            .arg(tr("Strongs"))
            .arg(*it)
            .arg(text)
        );
    }

    return ret;
}

QString CInfoDisplay::decodeMorph(QString const & data) {
    QStringList morphs = data.split("|");
    QString ret;

    Q_FOREACH(QString const & morph, morphs) {
        //qDebug() << "CInfoDisplay::decodeMorph, morph: " << morph;
        CSwordModuleInfo * module = 0;
        bool skipFirstChar = false;
        QString value = "";
        QString valueClass = "";

        int valStart = morph.indexOf(':');
        if (valStart > -1) {
            valueClass = morph.mid(0, valStart);
            // qDebug() << "valueClass: " << valueClass;
            module = CSwordBackend::instance()->findModuleByName(valueClass);
        }
        value = morph.mid(valStart + 1); //works for prepended module and without (-1 +1 == 0).

        // if we don't have a class assigned or desired one isn't installed...
        if (!module) {
            // Morphs usually don't have [GH] prepended, but some old OLB
            // codes do.  We should check if we're digit after first char
            // to better guess this.
            if (value.size() > 1 && value.at(1).isDigit()) {
                switch (value.at(0).toLatin1()) {
                    case 'G':
                        module = btConfig().getDefaultSwordModuleByType("standardGreekMorphLexicon");
                        skipFirstChar = true;
                        break;
                    case 'H':
                        module = btConfig().getDefaultSwordModuleByType("standardHebrewMorphLexicon");
                        skipFirstChar = true;
                        break;
                    default:
                        skipFirstChar = false;
                        /// \todo we can't tell here if it's a greek or hebrew moprh code, that's a problem we have to solve
                        //       module = getBtConfig().getDefaultSwordModuleByType("standardGreekMorphLexicon");
                        break;
                }
            }
            //if it is still not set use the default
            if (!module)
                module = btConfig().getDefaultSwordModuleByType("standardGreekMorphLexicon");
        }

        QString text;
        // Q_ASSERT(module);
        if (module) {
            QScopedPointer<CSwordKey> key(CSwordKey::createInstance(module));

            // skip H or G (language sign) if we have to skip it
            const bool isOk = key->setKey(skipFirstChar ? value.mid(1) : value);
            // Q_ASSERT(isOk);
            if (!isOk) { // try to use the other morph lexicon, because this one failed with the current morph code
                key->setModule(btConfig().getDefaultSwordModuleByType("standardHebrewMorphLexicon")); /// \todo: what if the module doesn't exist?
                key->setKey(skipFirstChar ? value.mid(1) : value);
            }

            text = key->renderedText();
        }

        // if the module wasn't found just display an empty morph info
        QString lang = "en";  // default to english
        if (module)
            lang = module->language()->abbrev();
        ret.append(QString("<div class=\"morphinfo\" lang=\"%1\"><h3>%2: %3</h3><p>%4</p></div>")
                    .arg(lang)
                    .arg(tr("Morphology"))
                    .arg(value)
                    .arg(text)
                  );
    }

    return ret;
}

QString CInfoDisplay::getWordTranslation(QString const & data) {
    CSwordModuleInfo * const module = btConfig().getDefaultSwordModuleByType("standardLexicon");
    if (!module)
        return QString::null;

    QScopedPointer<CSwordKey> key(CSwordKey::createInstance(module));
    key->setKey(data);
    if (key->key().toUpper() != data.toUpper()) //key not present in the lexicon
        return QString::null;

    return QString("<div class=\"translationinfo\" lang=\"%1\"><h3>%2: %3</h3><p>%4</p></div>")
                  .arg(module->language()->abbrev())
                  .arg(tr("Word lookup"))
                  .arg(data)
                  .arg(key->renderedText());
}

QSize CInfoDisplay::sizeHint() const {
    return QSize(100, 150);
}

} //end of namespace InfoDisplay
