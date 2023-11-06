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

#include "btinforendering.h"

#include <map>
#include <memory>
#include <QByteArray>
#include <QChar>
#include <QObject>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSharedPointer>
#include <QStringList>
#include <Qt>
#include <utility>
#include "../../util/btassert.h"
#include "../btglobal.h"
#include "../config/btconfig.h"
#include "../drivers/btmodulelist.h"
#include "../drivers/cswordlexiconmoduleinfo.h"
#include "../drivers/cswordmoduleinfo.h"
#include "../keys/cswordkey.h"
#include "../language.h"
#include "../managers/cdisplaytemplatemgr.h"
#include "../managers/cswordbackend.h"
#include "crossrefrendering.h"
#include "ctextrendering.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#endif
#include <listkey.h>
#include <swbuf.h>
#include <swkey.h>
#include <swmodule.h>
#include <versekey.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#pragma GCC diagnostic pop


namespace {

QString decodeAbbreviation(QString const & data) {
    /// \todo Is "text" correct?
    /* before:
    return QString("<div class=\"abbreviation\"><h3>%1: %2</h3><p>%3</p></div>")
        .arg(QObject::tr("Abbreviation"))
        .arg("text")
        .arg(data);
    */
    return QStringLiteral(
                "<div class=\"abbreviation\"><h3>%1: text</h3><p>%2</p></div>")
            .arg(QObject::tr("Abbreviation"), data);
}

QString decodeCrossReference(QString const & data,
                             BtConstModuleList const & modules)
{
    if (data.isEmpty())
        return QStringLiteral("<div class=\"crossrefinfo\"><h3>%1</h3></div>")
               .arg(QObject::tr("Cross references"));

    // qWarning("setting crossref %s", data.latin1());

    DisplayOptions dispOpts;
    dispOpts.lineBreaks  = false;
    dispOpts.verseNumbers = true;

    FilterOptions filterOpts;
    Rendering::CrossRefRendering renderer(dispOpts, filterOpts);
    Rendering::CTextRendering::KeyTree tree;

    // const bool isBible = true;
    const CSwordModuleInfo * module(nullptr);

    // a prefixed module gives the module to look into
    static QRegularExpression const re(QStringLiteral(R"PCRE(^[^ ]+:)PCRE"));
    // re.setMinimal(true);
    int pos = re.match(data).capturedEnd();
    if (pos >= 0)
        --pos;

    if (pos > 0) {
        auto moduleName = data.left(pos);
        // qWarning("found module %s", moduleName.latin1());
        module = CSwordBackend::instance().findModuleByName(
                     std::move(moduleName));
    }

    if (!module)
        module = btConfig().getDefaultSwordModuleByType(
                     QStringLiteral("standardBible"));

    if (!module && modules.size() > 0)
        module = modules.at(0);

    // BT_ASSERT(module); // why? the existense of the module is tested later
    Rendering::CTextRendering::KeyTreeItem::Settings settings{
        false,
        Rendering::CTextRendering::KeyTreeItem::Settings::CompleteShort
    };

    if (module && (module->type() == CSwordModuleInfo::Bible)) {
        sword::VerseKey vk;
        sword::ListKey refs =
                vk.parseVerseList(
                    (const char*) data.mid((pos == -1) ? 0 : pos + 1).toUtf8(),
                    "Gen 1:1",
                    true);

        for (int i = 0; i < refs.getCount(); i++) {
            sword::SWKey * const key = refs.getElement(i);
            BT_ASSERT(key);
            sword::VerseKey * const vk = dynamic_cast<sword::VerseKey*>(key);

            if (vk && vk->isBoundSet()) { // render a range of keys
                tree.emplace_back(
                            QString::fromUtf8(vk->getLowerBound().getText()),
                            QString::fromUtf8(vk->getUpperBound().getText()),
                            module,
                            settings);
            } else {
                tree.emplace_back(QString::fromUtf8(key->getText()),
                                  QString::fromUtf8(key->getText()),
                                  module,
                                  settings);
            }
        }
    } else if (module) {
        tree.emplace_back(data.mid((pos == -1) ? 0 : pos + 1),
                          module,
                          settings);
    }

    // qWarning("rendered the tree: %s", renderer.renderKeyTree(tree).latin1());
    /* spans containing rtl text need dir=rtl on their parent tag to be aligned
       properly */
    return QStringLiteral("<div class=\"crossrefinfo\" lang=\"%1\"><h3>%2</h3>"
                          "<div class=\"para\" dir=\"%3\">%4</div></div>")
           .arg(module ? module->language()->abbrev() : QStringLiteral("en"),
                QObject::tr("Cross references"),
                module ? module->textDirectionAsHtml() : QString())
           .arg(renderer.renderKeyTree(tree));
}

QString decodeFootnote(QString const & data) {
    QStringList list = data.split('/');
    BT_ASSERT(list.count() >= 3);
    if (!list.count())
        return QString();

    FilterOptions filterOpts;
    filterOpts.footnotes   = true;
    CSwordBackend::instance().setFilterOptions(filterOpts);

    const QString modulename = list.first();
    const QString swordFootnote = list.last();

    // remove the first and the last and then rejoin it to get a key
    list.pop_back();
    list.pop_front();
    const QString keyname = list.join('/');

    auto * const module =
            CSwordBackend::instance().findModuleByName(modulename);
    if (!module)
        return QString();

    QSharedPointer<CSwordKey> key(module->createKey());
    key->setKey(keyname);

    // force entryAttributes:
    key->renderedText(CSwordKey::ProcessEntryAttributesOnly);

    auto & m = module->swordModule();
    const char * const note =
        m.getEntryAttributes()
            ["Footnote"][swordFootnote.toLatin1().data()]["body"].c_str();
    return QStringLiteral("<div class=\"footnoteinfo\" lang=\"%1\"><h3>%2</h3>"
                          "<p>%3</p></div>")
           .arg(module->language()->abbrev(),
                QObject::tr("Footnote"),
                QString::fromUtf8(m.renderText(note).c_str()));
}

CSwordModuleInfo * getFirstAvailableStrongsModule(bool wantHebrew) {
    for (auto * const m : CSwordBackend::instance().moduleList()) {
        if (m->type() == CSwordLexiconModuleInfo::Lexicon) {
            auto lexModule = qobject_cast<CSwordLexiconModuleInfo *>(m);
            if (wantHebrew
                && m->has(CSwordModuleInfo::FeatureHebrewDef)
                && lexModule->hasStrongsKeys())
                return m;
            if (!wantHebrew
                && m->has(CSwordModuleInfo::FeatureGreekDef)
                && lexModule->hasStrongsKeys())
                return m;
        }
    }
    return nullptr;
}

CSwordModuleInfo * getStrongsModule(bool const wantHebrew) {
    auto * const m =
            btConfig().getDefaultSwordModuleByType(
                wantHebrew
                ? QStringLiteral("standardHebrewStrongsLexicon")
                : QStringLiteral("standardGreekStrongsLexicon"));
    return m ? m : getFirstAvailableStrongsModule(wantHebrew);
}

QString decodeStrongs(QString const & data) {
    QString ret;
    for (auto const & strongs : data.split('|')) {
        bool const wantHebrew = strongs.left(1) == 'H';
        CSwordModuleInfo * module = getStrongsModule(wantHebrew);
        QString text;
        if (module) {
            QSharedPointer<CSwordKey> key(module->createKey());
            auto lexModule = qobject_cast<CSwordLexiconModuleInfo *>(module);
            key->setKey(lexModule->normalizeStrongsKey(strongs));
            text = key->renderedText();
        }
        //if the module could not be found just display an empty lemma info

        ret.append(
            QStringLiteral("<div class=\"strongsinfo\" lang=\"%1\"><h3>%2: %3"
                           "</h3><p>%4</p></div>")
            .arg(module ? module->language()->abbrev() : QStringLiteral("en"),
                 QObject::tr("Strongs"),
                 strongs,
                 text)
        );
    }
    return ret;
}

QString decodeMorph(QString const & data) {
    QStringList morphs = data.split('|');
    QString ret;

    for (auto const & morph : morphs) {
        //qDebug() << "CInfoDisplay::decodeMorph, morph: " << morph;
        CSwordModuleInfo * module = nullptr;
        bool skipFirstChar = false;
        QString value;
        QString valueClass;

        int valStart = morph.indexOf(':');
        if (valStart > -1) {
            valueClass = morph.mid(0, valStart);
            // qDebug() << "valueClass: " << valueClass;
            module = CSwordBackend::instance().findModuleByName(valueClass);
        }
        value = morph.mid(valStart + 1); /* works for prepended module and
                                            without (-1 +1 == 0). */

        // if we don't have a class assigned or desired one isn't installed...
        if (!module) {
            // Morphs usually don't have [GH] prepended, but some old OLB
            // codes do.  We should check if we're digit after first char
            // to better guess this.
            if (value.size() > 1 && value.at(1).isDigit()) {
                switch (value.at(0).toLatin1()) {
                    case 'G':
                        module = btConfig().getDefaultSwordModuleByType(
                                     QStringLiteral(
                                         "standardGreekMorphLexicon"));
                        skipFirstChar = true;
                        break;
                    case 'H':
                        module = btConfig().getDefaultSwordModuleByType(
                                     QStringLiteral(
                                         "standardHebrewMorphLexicon"));
                        skipFirstChar = true;
                        break;
                    default:
                        skipFirstChar = false;
                        /** \todo we can't tell here if it's a greek or hebrew
                                  moprh code, that's a problem we have to solve
                        */
                        /* module = getBtConfig().getDefaultSwordModuleByType(
                                        "standardGreekMorphLexicon"); */
                        break;
                }
            }
            //if it is still not set use the default
            if (!module)
                module = btConfig().getDefaultSwordModuleByType(
                             QStringLiteral("standardGreekMorphLexicon"));
        }

        QString text;
        // BT_ASSERT(module);
        if (module) {
            QSharedPointer<CSwordKey> key(module->createKey());

            // skip H or G (language sign) if we have to skip it
            const bool isOk = key->setKey(skipFirstChar ? value.mid(1) : value);
            // BT_ASSERT(isOk);
            /* try to use the other morph lexicon, because this one failed with
               the current morph code. */
            if (!isOk) {
                /// \todo: what if the module doesn't exist?
                key->setModule(
                            btConfig().getDefaultSwordModuleByType(
                                QStringLiteral("standardHebrewMorphLexicon")));
                key->setKey(skipFirstChar ? value.mid(1) : value);
            }

            text = key->renderedText();
        }

        // if the module wasn't found just display an empty morph info
        ret.append(QStringLiteral("<div class=\"morphinfo\" lang=\"%1\">"
                                  "<h3>%2: %3</h3><p>%4</p></div>")
                   .arg(module
                        ? module->language()->abbrev()
                        : QStringLiteral("en"),
                        QObject::tr("Morphology"),
                        value,
                        text));
    }

    return ret;
}

QString decodeSwordReference(QString const & data) {
    static QRegularExpression const rx(
        QStringLiteral(R"PCRE(sword://(bible|lexicon)/(.*?)/(.*?))PCRE"),
        QRegularExpression::CaseInsensitiveOption);
    if (auto const match = rx.match(data); match.hasMatch()) {
        if (auto * const module =
                    CSwordBackend::instance().findModuleByName(
                        match.captured(2)))
        {
            std::unique_ptr<CSwordKey> key(module->createKey());
            auto reference = match.captured(3);
            key->setKey(reference);
            return QStringLiteral("<div class=\"crossrefinfo\" lang=\"%1\">"
                                  "<h3>%2</h3><p>%3</p></div>")
                    .arg(module->language()->abbrev(),
                         std::move(reference),
                         key->renderedText());
        }
    }
    return {};
}

} // anonymous namespace

namespace Rendering {

ListInfoData detectInfo(QString const & data) {
    ListInfoData list;
    auto const attrList(data.split(QStringLiteral("||")));

    for (auto const & attrPair : attrList) {
        auto const attr(attrPair.split('='));
        if (attr.size() == 2) {
            auto const & attrName = attr[0];
            auto const & attrValue = attr[1];
            if (attrName == QStringLiteral("note")) {
                list.append(qMakePair(Footnote, attrValue));
            } else if (attrName == QStringLiteral("lemma")) {
                list.append(qMakePair(Lemma, attrValue));
            } else if (attrName == QStringLiteral("morph")) {
                list.append(qMakePair(Morph, attrValue));
            } else if (attrName == QStringLiteral("expansion")) {
                list.append(qMakePair(Abbreviation, attrValue));
            } else if (attrName == QStringLiteral("crossrefs")) {
                list.append(qMakePair(CrossReference, attrValue));
            } else if (attrName == QStringLiteral("href")) {
                list.append(qMakePair(Reference, attrValue));
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

    for (auto const & infoData : list) {
        auto const & value = infoData.second;
        switch (infoData.first) {
            case Lemma:
                text.append(decodeStrongs(value));
                continue;
            case Morph:
                text.append(decodeMorph(value));
                continue;
            case CrossReference:
                text.append(decodeCrossReference(value, modules));
                continue;
            case Footnote:
                text.append(decodeFootnote(value));
                continue;
            case Abbreviation:
                text.append(decodeAbbreviation(value));
                continue;
            case Text:
                text.append(value);
                continue;
            case Reference:
                if (value.contains(QStringLiteral("strongs:"))) {
                    auto v(value.right(value.size() - value.lastIndexOf('/')
                                       - 1));
                    if (value.contains(QStringLiteral("GREEK"))) {
                        v.prepend('G');
                    } else if (value.contains(QStringLiteral("HEBREW"))) {
                        v.prepend('H');
                    } else {
                        BT_ASSERT(false && "not implemented");
                    }
                    text.append(decodeStrongs(v));
                } else if (value.contains(QStringLiteral("sword:"))) {
                    text.append(decodeSwordReference(value));
                    continue;
                } else {
                    BT_ASSERT(false); /// \todo Why is this here?
                }
                [[fallthrough]];
            default:
                continue;
        }
    }

    return formatInfo(text);
}

QString formatInfo(QString const & info, QString const & lang) {
    CDisplayTemplateMgr *mgr = CDisplayTemplateMgr::instance();
    BT_ASSERT(mgr);

    CDisplayTemplateMgr::Settings settings;
    settings.pageCSS_ID = QStringLiteral("infodisplay");

    return mgr->fillTemplate(
                CDisplayTemplateMgr::activeTemplateName(),
                QStringLiteral("<div class=\"infodisplay\"%1>%2</div>")
                    .arg(lang.isEmpty()
                         ? QString()
                         : QStringLiteral(" lang=\"%1\"").arg(lang),
                         info),
                settings);
}

} // namespace Rendering {
