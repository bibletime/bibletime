/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/cinfodisplay.h"

#include <QSharedPointer>
#include <QAction>
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QRegExp>
#include <QSize>
#include <QVBoxLayout>
#include <QtAlgorithms>
#include <QMenu>

#include "backend/config/cbtconfig.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordkey.h"
#include "backend/keys/cswordversekey.h"
#include "backend/managers/referencemanager.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "frontend/crossrefrendering.h"
#include "frontend/display/cdisplay.h"
#include "frontend/display/creaddisplay.h"

// Sword includes:
#include <listkey.h>


using namespace Rendering;
using namespace sword;

namespace InfoDisplay {

CInfoDisplay::CInfoDisplay(QWidget *parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2); // Leave small border
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    m_htmlPart = CDisplay::createReadInstance(0, this);
    m_htmlPart->setMouseTracking(false); //we don't want strong/lemma/note mouse infos
    m_htmlPart->view()->setAcceptDrops(false);

    QAction* selectAllAction = new QAction(QIcon(), tr("Select all"), this);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    QObject::connect(selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()) );

    QAction* copyAction = new QAction(tr("Copy"), this);
    copyAction->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_C) );
    QObject::connect(copyAction, SIGNAL(triggered()), m_htmlPart->connectionsProxy(), SLOT(copySelection()) );

    QMenu* menu = new QMenu(this);
    menu->addAction(selectAllAction);
    menu->addAction(copyAction);
    m_htmlPart->installPopup(menu);

    connect(
        m_htmlPart->connectionsProxy(),
        SIGNAL(referenceClicked(const QString&, const QString&)),
        SLOT(lookupInfo(const QString&, const QString&))
    );

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

void CInfoDisplay::setInfo(const QString &data, const QString &lang) {
    CDisplayTemplateMgr *mgr = CDisplayTemplateMgr::instance();
    Q_ASSERT(mgr != 0);

    CDisplayTemplateMgr::Settings settings;
    settings.pageCSS_ID = "infodisplay";

    QString div = "<div class=\"infodisplay\"";
    if (!lang.isEmpty()) {
        div.append(" lang=\"");
        div.append(lang);
        div.append("\"");
    }
    div.append(">");

    QString content(mgr->fillTemplate(CBTConfig::get(CBTConfig::displayStyle),
                                      div + data + "</div>",
                                      settings));
    m_htmlPart->setText(content);
}

void CInfoDisplay::lookupInfo(const QString &mod_name, const QString &key_text) {
    qDebug() << "CInfoDisplay::lookup";
    qDebug() <<  mod_name <<  key_text;
    CSwordModuleInfo* m = CSwordBackend::instance()->findModuleByName(mod_name);
    Q_ASSERT(m);
    if (!m)
        return;
    QSharedPointer<CSwordKey> key( CSwordKey::createInstance(m) );
    key->setKey(key_text);

    setInfo(key->renderedText(), m->language()->abbrev());
}

void CInfoDisplay::setInfo(const InfoType type, const QString& data) {
    ListInfoData list;
    list.append( qMakePair(type, data) );

    setInfo(list);
}


void CInfoDisplay::setInfo(const ListInfoData& list) {
    //if the widget is hidden it would be inefficient to render and display the data
    if (!isVisible()) {
        return;
    }

    if (list.isEmpty()) {
        m_htmlPart->setText("<html></html>");
        return;
    }

    QString text;

    ListInfoData::const_iterator end = list.end();
    for (ListInfoData::const_iterator it = list.begin(); it != end; ++it) {
        switch ( (*it).first ) {
            case Lemma:
                text.append( decodeStrongs( (*it).second ) );
                continue;
            case Morph:
                text.append( decodeMorph( (*it).second ) );
                continue;
            case CrossReference:
                text.append( decodeCrossReference( (*it).second ) );
                continue;
            case Footnote:
                text.append( decodeFootnote( (*it).second ) );
                continue;
            case WordTranslation:
                text.append( getWordTranslation( (*it).second ) );
                continue;
            case WordGloss:
                //text.append( getWordTranslation( (*it).second ) );
                continue;
            case Abbreviation:
                text.append( decodeAbbreviation( (*it).second ) );
                continue;
            case Text:
                text.append( (*it).second );
                continue;
            default:
                continue;
        };
    }

    setInfo(text);
}

void CInfoDisplay::setInfo(CSwordModuleInfo *module) {
    if (module) {
        setInfo(tr("<div class=\"moduleinfo\"><h3>%1</h3><p>%2</p><p>Version: %3</p></div>")
                .arg(module->name())
                .arg(module->config(CSwordModuleInfo::Description))
                .arg(module->config(CSwordModuleInfo::ModuleVersion)));
    } else {
        unsetInfo();
    }
}

void CInfoDisplay::selectAll() {
    m_htmlPart->selectAll();
}

const QString CInfoDisplay::decodeAbbreviation( const QString& data ) {
    //  QStringList strongs = QStringList::split("|", data);
    QString ret;
    QString text = data;

    ret.append(
        QString("<div class=\"abbreviation\"><h3>%1: %2</h3><p>%3</p></div>")
        .arg(tr("Abbreviation"))
        .arg("text")
        .arg(text));

    return ret;
}

const QString CInfoDisplay::decodeCrossReference( const QString& data ) {
    Q_ASSERT(!data.isEmpty());
    if (data.isEmpty()) {
        return QString("<div class=\"crossrefinfo\"><h3>%1</h3></div>")
               .arg(tr("Cross references"));
    }

    //  qWarning("setting crossref %s", data.latin1());

    DisplayOptions dispOpts;
    dispOpts.lineBreaks  = false;
    dispOpts.verseNumbers = true;

    FilterOptions filterOpts;
    filterOpts.headings    = false;
    filterOpts.strongNumbers  = false;
    filterOpts.morphTags    = false;
    filterOpts.lemmas     = false;
    filterOpts.footnotes   = false;
    filterOpts.scriptureReferences = false;

    CrossRefRendering renderer(dispOpts, filterOpts);
    CTextRendering::KeyTree tree;

    //  const bool isBible = true;
    CSwordModuleInfo* module = CBTConfig::get(CBTConfig::standardBible);

    //a prefixed module gives the module to look into
    QRegExp re("^[^ ]+:");
    //  re.setMinimal(true);
    int pos = re.indexIn(data);
    if (pos != -1) {
        pos += re.matchedLength() - 1;
    }

    if (pos > 0) {
        const QString moduleName = data.left(pos);
        //     qWarning("found module %s", moduleName.latin1());
        module = CSwordBackend::instance()->findModuleByName(moduleName);
        if (!module) {
            module = CBTConfig::get(CBTConfig::standardBible);
        }
        //   Q_ASSERT(module);
    }

    //Q_ASSERT(module); //why? the existense of the module is tested later
    CTextRendering::KeyTreeItem::Settings settings (
        false,
        CTextRendering::KeyTreeItem::Settings::CompleteShort
    );

    if (module && (module->type() == CSwordModuleInfo::Bible)) {
        VerseKey vk;
        sword::ListKey refs = vk.ParseVerseList((const char*)data.mid((pos == -1) ? 0 : pos + 1).toUtf8(), "Gen 1:1", true);

        for (int i = 0; i < refs.Count(); ++i) {
            SWKey* key = refs.getElement(i);
            Q_ASSERT(key);
            VerseKey* vk = dynamic_cast<VerseKey*>(key);

            CTextRendering::KeyTreeItem* itm = (CTextRendering::KeyTreeItem*)0; //explicit conversion for MS VS
            if (vk && vk->isBoundSet()) { //render a range of keys
                itm = new CTextRendering::KeyTreeItem(
                    QString::fromUtf8(vk->LowerBound().getText()),
                    QString::fromUtf8(vk->UpperBound().getText()),
                    module,
                    settings
                );
            }
            else {
                itm = new CTextRendering::KeyTreeItem(
                    QString::fromUtf8(key->getText()),
                    QString::fromUtf8(key->getText()),
                    module,
                    settings
                );
            }

            Q_ASSERT(itm);

            tree.append( itm );
        }
    }
    else if (module) {
        CTextRendering::KeyTreeItem* itm = new CTextRendering::KeyTreeItem(
            data.mid((pos == -1) ? 0 : pos + 1),
            module,
            settings
        );
        tree.append( itm );
    }

    //  qWarning("rendered the tree: %s", renderer.renderKeyTree(tree).latin1());
    //spanns containing rtl text need dir=rtl on their parent tag to be aligned properly
    QString lang = "en";  // default english
    if (module) {
        lang = module->language()->abbrev();
    }

    const QString RenderedText = renderer.renderKeyTree(tree);

    return QString("<div class=\"crossrefinfo\" lang=\"%1\"><h3>%2</h3><div class=\"para\" dir=\"%3\">%4</div></div>")
           .arg(lang)
           .arg(tr("Cross references"))
           .arg(module ? ((module->textDirection() == CSwordModuleInfo::LeftToRight) ? "ltr" : "rtl") : "")
                   .arg(RenderedText);
}

/*!
    \fn CInfoDisplay::decodeFootnote( const QString& data )
    */
const QString CInfoDisplay::decodeFootnote( const QString& data ) {
    QStringList list = data.split("/");
    Q_ASSERT(list.count() >= 3);
    if (!list.count()) {
        return QString::null;
    }

    FilterOptions filterOpts;
    filterOpts.headings    = false;
    filterOpts.strongNumbers  = false;
    filterOpts.morphTags    = false;
    filterOpts.lemmas     = false;
    filterOpts.footnotes   = true;
    // turn scripRefs off, so that they do not show up as footnotes in the OSIS filter's EntryAttributes
    filterOpts.scriptureReferences = false;

    CSwordBackend::instance()->setFilterOptions(filterOpts);

    const QString modulename = list.first();
    const QString swordFootnote = list.last();

    // remove the first and the last and then rejoin it to get a key
    list.pop_back();
    list.pop_front();
    const QString keyname = list.join("/");

    CSwordModuleInfo* module = CSwordBackend::instance()->findModuleByName(modulename);
    if (!module) {
        return QString::null;
    }

    QSharedPointer<CSwordKey> key( CSwordKey::createInstance(module) );
    key->setKey(keyname);
    key->renderedText(CSwordKey::ProcessEntryAttributesOnly); //force entryAttributes

    const char* note =
        module->module()->getEntryAttributes()
        ["Footnote"][swordFootnote.toLatin1().data()]["body"].c_str();

    QString text = module->isUnicode() ? QString::fromUtf8(note) : QString(note);
    text = QString::fromUtf8(module->module()->RenderText(
                                 module->isUnicode()
                                 ? (const char*)text.toUtf8()
                                 : (const char*)text.toLatin1()
                             ));

    return QString("<div class=\"footnoteinfo\" lang=\"%1\"><h3>%2</h3><p>%3</p></div>")
           .arg(module->language()->abbrev())
           .arg(tr("Footnote"))
           .arg(text);
}

const QString CInfoDisplay::decodeStrongs( const QString& data ) {
    QStringList strongs = data.split("|");
    QString ret;

    QStringList::const_iterator end = strongs.end();
    for (QStringList::const_iterator it = strongs.begin(); it != end; ++it) {
        CSwordModuleInfo* const module = CBTConfig::get
                                         (
                                             ((*it).left(1) == QString("H")) ?
                                             CBTConfig::standardHebrewStrongsLexicon :
                                             CBTConfig::standardGreekStrongsLexicon
                                         );

        QString text;
        if (module) {
            QSharedPointer<CSwordKey> key( CSwordKey::createInstance(module) );
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

const QString CInfoDisplay::decodeMorph( const QString& data ) {
    QStringList morphs = data.split("|");
    QString ret;

    foreach(QString morph, morphs) {
        //qDebug() << "CInfoDisplay::decodeMorph, morph: " << morph;
        CSwordModuleInfo* module = 0;
        bool skipFirstChar = false;
        QString value = "";
        QString valueClass = "";

        int valStart = morph.indexOf(':');
        if (valStart > -1) {
            valueClass = morph.mid(0, valStart);
            //qDebug() << "valueClass: " << valueClass;
            module = CSwordBackend::instance()->findModuleByName( valueClass );
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
                        module = CBTConfig::get
                                 (CBTConfig::standardGreekMorphLexicon);
                        skipFirstChar = true;
                        break;
                    case 'H':
                        module = CBTConfig::get
                                 (CBTConfig::standardHebrewMorphLexicon);
                        skipFirstChar = true;
                        break;
                    default:
                        skipFirstChar = false;
                        /// \todo we can't tell here if it's a greek or hebrew moprh code, that's a problem we have to solve
                        //       module = CBTConfig::get(CBTConfig::standardGreekMorphLexicon);
                        break;
                }
            }
            //if it is still not set use the default
            if (!module) {
                module = CBTConfig::get
                         (CBTConfig::standardGreekMorphLexicon);
            }
        }

        QString text;
        //Q_ASSERT(module);
        if (module) {
            QSharedPointer<CSwordKey> key( CSwordKey::createInstance(module) );

            //skip H or G (language sign) if we have to skip it
            const bool isOk = key->setKey(skipFirstChar ? value.mid(1) : value);
            //Q_ASSERT(isOk);
            if (!isOk) { //try to use the other morph lexicon, because this one failed with the current morph code
                key->setModule(CBTConfig::get
                               (CBTConfig::standardHebrewMorphLexicon));
                key->setKey(skipFirstChar ? value.mid(1) : value);
            }

            text = key->renderedText();
        }

        //if the module wasn't found just display an empty morph info
        QString lang = "en";  // default to english
        if (module)
            lang = module->language()->abbrev();
        ret.append( QString("<div class=\"morphinfo\" lang=\"%1\"><h3>%2: %3</h3><p>%4</p></div>")
                    .arg(lang)
                    .arg(tr("Morphology"))
                    .arg(value)
                    .arg(text)
                  );
    }

    return ret;
}

const QString CInfoDisplay::getWordTranslation( const QString& data ) {
    CSwordModuleInfo* const module = CBTConfig::get
                                     (CBTConfig::standardLexicon);
    if (!module) {
        return QString::null;
    }

    QSharedPointer<CSwordKey> key( CSwordKey::createInstance(module) );
    key->setKey(data);
    if (key->key().toUpper() != data.toUpper()) { //key not present in the lexicon
        return QString::null;
    }

    QString ret = QString("<div class=\"translationinfo\" lang=\"%1\"><h3>%2: %3</h3><p>%4</p></div>")
                  .arg(module->language()->abbrev())
                  .arg(tr("Word lookup"))
                  .arg(data)
                  .arg(key->renderedText());

    return ret;
}

QSize CInfoDisplay::sizeHint() const {
    return QSize(100, 150);
}

} //end of namespace InfoDisplay
