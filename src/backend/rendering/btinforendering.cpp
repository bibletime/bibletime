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

#include "btinforendering.h"

#include <QStringList>
#include "../../util/btassert.h"
#include "../btglobal.h"
#include "../keys/cswordversekey.h"
#include "../managers/cdisplaytemplatemgr.h"
#include "../managers/referencemanager.h"
#include "chtmlexportrendering.h"
#include "../drivers/cswordlexiconmoduleinfo.h"


using namespace Rendering;

namespace Rendering {

class CrossRefRendering : public CHTMLExportRendering {
public:
    CrossRefRendering(
            const DisplayOptions &displayOptions = btConfig().getDisplayOptions(),
            const FilterOptions &filterOptions = btConfig().getFilterOptions())
     : CHTMLExportRendering(true, displayOptions, filterOptions) {;}

    QString entryLink(const KeyTreeItem &item, const CSwordModuleInfo *module) override {
        BT_ASSERT(module);

        QString linkText;

        const bool isBible = (module->type() == CSwordModuleInfo::Bible);
        CSwordVerseKey vk(module); //only valid for bible modules, i.e. isBible == true
        if (isBible) {
            vk.setKey(item.key());
        }

        switch (item.settings().keyRenderingFace) {
            case KeyTreeItem::Settings::NoKey: {
                linkText = QString::null;
                break; //no key is valid for all modules
            }
            case KeyTreeItem::Settings::CompleteShort: {
                if (isBible) {
                    linkText = QString::fromStdString(vk.getShortText());
                    break;
                }
                //fall through for non-Bible modules
            }
            case KeyTreeItem::Settings::CompleteLong: {
                if (isBible) {
                    linkText = vk.key();
                    break;
                }
                //fall through for non-Bible modules
            }
            case KeyTreeItem::Settings::SimpleKey: {
                if (isBible) {
                    linkText = QString::number(vk.getVerse());
                    break;
                }
                //fall through for non-Bible modules
            }
            default: { //default behaviour to return the passed key
                linkText = item.key();
                break;
            }
        }

        if (!linkText.isEmpty()) { //if we have a valid link text
            //     qWarning("rendering");
            return QString("<a href=\"%1\">%2</a>")
                   .arg(
                       ReferenceManager::encodeHyperlink(
                           module->name(),
                           item.key(),
                           ReferenceManager::typeFromModule(module->type())
                       )
                   )
                   .arg(linkText);
        }

        return QString::null;
    }

    QString finishText(const QString &text, const KeyTree &tree) override {
        Q_UNUSED(tree);
        return text;
    }
};


ListInfoData detectInfo(QString const &data)
{
    ListInfoData list;
    QStringList attrList = data.split("||");

    for (int i = 0; i < attrList.count(); i++) {
        QString attrPair = attrList[i];
        QStringList attr = attrPair.split("=");
        if (attr.count() == 2) {
            QString attrName = attr[0];
            QString attrValue = attr[1];
            if (attrName == "note") {
                list.append( qMakePair(Footnote, attrValue));
            }
            if (attrName == "lemma") {
                list.append( qMakePair(Lemma, attrValue));
            }
            if (attrName == "morph") {
                list.append( qMakePair(Morph, attrValue));
            }
            if (attrName == "expansion") {
                list.append( qMakePair(Abbreviation, attrValue));
            }
            if (attrName == "crossrefs") {
                list.append( qMakePair(CrossReference, attrValue));
            }
            if (attrName == "href") {
                list.append( qMakePair(Reference, attrValue));
            }
            if (attrName == "key") {
                list.append( qMakePair(Key, attrValue));
            }
        }
    }

    return list;
}


QString formatInfo(const ListInfoData & list,  BtConstModuleList const & modules)
{
    BT_ASSERT(!modules.contains(nullptr) && (modules.size() <= 1 && "not implemented"));

    if (list.isEmpty())
        return QString();

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
                text.append( decodeCrossReference( (*it).second, modules ) );
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
            case Reference:
                if((*it).second.contains("strongs:"))
                {
                    QString v = (*it).second.right((*it).second.size() -
                        (*it).second.lastIndexOf('/') - 1);

                    if((*it).second.contains("GREEK"))
                        v.prepend('G');
                    else if((*it).second.contains("HEBREW"))
                        v.prepend('H');
                    else
                        BT_ASSERT(false && "not implemented");

                    text.append(decodeStrongs(v));
                }
                else if ((*it).second.contains("sword:"))
                {
                    //text.append("Not implemented yet.");
                }
                else
                    BT_ASSERT(false); /// \todo Why is this here?
            default:
                continue;
        };
    }

    return formatInfo(text);
}

QString formatInfo(QString const & info, QString const & lang) {
    CDisplayTemplateMgr *mgr = CDisplayTemplateMgr::instance();
    BT_ASSERT(mgr);

    CDisplayTemplateMgr::Settings settings;
    settings.pageCSS_ID = "infodisplay";

    QString div = "<div class=\"infodisplay\"";
    if (!lang.isEmpty())
        div.append(" lang=\"").append(lang).append("\"");
    div.append(">");

    QString content(mgr->fillTemplate(CDisplayTemplateMgr::activeTemplateName(),
                                      div + info + "</div>",
                                      settings));
    return content;
}


QString decodeAbbreviation(QString const & data) {
    // QStringList strongs = QStringList::split("|", data);
    return QString("<div class=\"abbreviation\"><h3>%1: %2</h3><p>%3</p></div>")
        .arg(QObject::tr("Abbreviation"))
        .arg("text")
        .arg(data);
}

QString decodeCrossReference(QString const & data, BtConstModuleList const & modules) {
    if (data.isEmpty())
        return QString("<div class=\"crossrefinfo\"><h3>%1</h3></div>")
               .arg(QObject::tr("Cross references"));

    // qWarning("setting crossref %s", data.latin1());

    DisplayOptions dispOpts;
    dispOpts.lineBreaks  = false;
    dispOpts.verseNumbers = true;

    FilterOptions filterOpts;
    CrossRefRendering renderer(dispOpts, filterOpts);
    CTextRendering::KeyTree tree;

    // const bool isBible = true;
    const CSwordModuleInfo * module(nullptr);

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
    }

    if (!module)
        module = btConfig().getDefaultSwordModuleByType("standardBible");

    if (!module && modules.size() > 0)
        module = modules.at(0);

    // BT_ASSERT(module); // why? the existense of the module is tested later
    CTextRendering::KeyTreeItem::Settings settings{
        false,
        CTextRendering::KeyTreeItem::Settings::CompleteShort
    };

    if (module && (module->type() == CSwordModuleInfo::Bible)) {
        swordxx::VerseKey vk;
        swordxx::ListKey refs = vk.parseVerseList((const char*) data.mid((pos == -1) ? 0 : pos + 1).toUtf8(), "Gen 1:1", true);

        for (std::size_t i = 0u; i < refs.getCount(); i++) {
            swordxx::SWKey * const key = refs.getElement(i);
            BT_ASSERT(key);
            swordxx::VerseKey * const vk = dynamic_cast<swordxx::VerseKey*>(key);

            if (vk && vk->isBoundSet()) { // render a range of keys
                tree.append(new CTextRendering::KeyTreeItem(
                    QString::fromStdString(vk->getLowerBound().getText()),
                    QString::fromStdString(vk->getUpperBound().getText()),
                    module,
                    settings
                ));
            } else {
                tree.append(new CTextRendering::KeyTreeItem(
                    QString::fromStdString(key->getText()),
                    QString::fromStdString(key->getText()),
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
           .arg(QObject::tr("Cross references"))
           .arg(module ? ((module->textDirection() == CSwordModuleInfo::LeftToRight) ? "ltr" : "rtl") : "")
           .arg(renderer.renderKeyTree(tree));
}

/*!
    \fn CInfoDisplay::decodeFootnote(QString const & data)
    */
QString decodeFootnote(QString const & data) {
    QStringList list = data.split("/");
    BT_ASSERT(list.count() >= 3);
    if (!list.count())
        return QString::null;

    FilterOptions filterOpts;
    filterOpts.footnotes   = true;
    CSwordBackend::instance()->setFilterOptions(filterOpts);

    const QString modulename = list.first();
    const QString swordFootnote = list.last();

    // remove the first and the last and then rejoin it to get a key
    list.pop_back();
    list.pop_front();
    const QString keyname = list.join("/");

    CSwordModuleInfo * const module = CSwordBackend::instance()->findModuleByName(modulename);
    if (!module)
        return QString::null;

    QSharedPointer<CSwordKey> key(CSwordKey::createInstance(module));
    key->setKey(keyname);
    key->renderedText(CSwordKey::ProcessEntryAttributesOnly); // force entryAttributes

    auto & m = module->module();
    const char * const note =
        m.getEntryAttributes()
            ["Footnote"][swordFootnote.toLatin1().data()]["body"].c_str();

    QString text = module->isUnicode() ? QString::fromUtf8(note) : QString(note);
    text = QString::fromStdString(
               m.renderText(module->isUnicode()
                            ? static_cast<const char *>(text.toUtf8())
                            : static_cast<const char *>(text.toLatin1())));

    return QString("<div class=\"footnoteinfo\" lang=\"%1\"><h3>%2</h3><p>%3</p></div>")
           .arg(module->language()->abbrev())
           .arg(QObject::tr("Footnote"))
           .arg(text);
}

QString decodeStrongs(QString const & data) {
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
            QSharedPointer<CSwordKey> key(CSwordKey::createInstance(module));
            auto lexModule = qobject_cast<CSwordLexiconModuleInfo*>(module);
            key->setKey(lexModule->normalizeStrongsKey(*it));
            text = key->renderedText();
        }
        //if the module could not be found just display an empty lemma info

        QString lang = "en";  // default english
        if (module)
            lang = module->language()->abbrev();
        ret.append(
            QString("<div class=\"strongsinfo\" lang=\"%1\"><h3>%2: %3</h3><p>%4</p></div>")
            .arg(lang)
            .arg(QObject::tr("Strongs"))
            .arg(*it)
            .arg(text)
        );
    }

    return ret;
}

QString decodeMorph(QString const & data) {
    QStringList morphs = data.split("|");
    QString ret;

    Q_FOREACH (QString morph, morphs) {
        //qDebug() << "CInfoDisplay::decodeMorph, morph: " << morph;
        CSwordModuleInfo * module = nullptr;
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
        // BT_ASSERT(module);
        if (module) {
            QSharedPointer<CSwordKey> key(CSwordKey::createInstance(module));

            // skip H or G (language sign) if we have to skip it
            const bool isOk = key->setKey(skipFirstChar ? value.mid(1) : value);
            // BT_ASSERT(isOk);
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
                    .arg(QObject::tr("Morphology"))
                    .arg(value)
                    .arg(text)
                  );
    }

    return ret;
}

QString getWordTranslation(QString const & data) {
    CSwordModuleInfo * const module = btConfig().getDefaultSwordModuleByType("standardLexicon");
    if (!module)
        return QString::null;

    QSharedPointer<CSwordKey> key(CSwordKey::createInstance(module));
    key->setKey(data);
    if (key->key().toUpper() != data.toUpper()) //key not present in the lexicon
        return QString::null;

    return QString("<div class=\"translationinfo\" lang=\"%1\"><h3>%2: %3</h3><p>%4</p></div>")
                  .arg(module->language()->abbrev())
                  .arg(QObject::tr("Word lookup"))
                  .arg(data)
                  .arg(key->renderedText());
}

} // namespace Rendering {
