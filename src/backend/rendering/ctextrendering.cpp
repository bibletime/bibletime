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

#include "ctextrendering.h"

#include <memory>
#include <QRegExp>
#include <QtAlgorithms>
#include "../../util/btassert.h"
#include "../drivers/cswordmoduleinfo.h"
#include "../keys/cswordkey.h"
#include "../keys/cswordversekey.h"
#include "../managers/cdisplaytemplatemgr.h"
#include "../managers/cswordbackend.h"
#include "../managers/referencemanager.h"

// Sword includes:
#include <swkey.h>


using namespace Rendering;

CTextRendering::KeyTreeItem::KeyTreeItem(const QString &key,
                                         const CSwordModuleInfo *module,
                                         const Settings &settings)
    : m_settings(settings)
    , m_key(key)
{ m_moduleList.append(module); }

CTextRendering::KeyTreeItem::KeyTreeItem(const QString &content,
                                         const Settings &settings)
    : m_settings(settings)
    , m_alternativeContent(content)
{}

CTextRendering::KeyTreeItem::KeyTreeItem(const QString &key,
                                         const BtConstModuleList &mods,
                                         const Settings &settings)
    : m_settings(settings)
    , m_moduleList(mods)
    , m_key(key)
{}

CTextRendering::KeyTreeItem::KeyTreeItem(const QString &startKey,
                                         const QString &stopKey,
                                         const CSwordModuleInfo *module,
                                         const Settings &settings)
    : m_settings(settings)
    , m_key(startKey)
    , m_stopKey(stopKey)
{
    BT_ASSERT(module);
    m_moduleList.append(module);

    //use the start and stop key to ceate our child items

    if (module->type() == CSwordModuleInfo::Bible) {
        CSwordVerseKey start(module);
        start.setKey(startKey);

        CSwordVerseKey stop(module);
        stop.setKey(stopKey);

        if (!m_key.isEmpty() && !m_stopKey.isEmpty()) { //we have a range of keys
            bool ok = true;

            while (ok && ((start < stop) || (start == stop)) ) { //range
                m_childList.emplace_back(start.key(),
                                         module,
                                         KeyTreeItem::Settings{
                                             false,
                                             settings.keyRenderingFace});
                ok = start.next(CSwordVerseKey::UseVerse);
            }
        }
        else if (m_key.isEmpty()) {
            m_childList.emplace_back(startKey,
                                     module,
                                     KeyTreeItem::Settings{
                                         false,
                                         settings.keyRenderingFace});
        }
    }
    else if ((module->type() == CSwordModuleInfo::Lexicon) || (module->type() == CSwordModuleInfo::Commentary) ) {
        m_childList.emplace_back(startKey,
                                 module,
                                 KeyTreeItem::Settings{
                                     false,
                                     KeyTreeItem::Settings::NoKey});
    }
    else if (module->type() == CSwordModuleInfo::GenericBook) {
        m_childList.emplace_back(startKey,
                                 module,
                                 KeyTreeItem::Settings{
                                     false,
                                     KeyTreeItem::Settings::NoKey});
    }

    //make it into "<simple|range> (modulename)"

    if (startKey == stopKey) {
        m_alternativeContent = startKey;
    }
    else {
        sword::VerseKey vk(startKey.toUtf8().constData(), stopKey.toUtf8().constData());

        if (vk.getLowerBound().getBook() != vk.getUpperBound().getBook()) {
            m_alternativeContent = QString::fromUtf8(vk.getRangeText());
        }
        else if (vk.getLowerBound().getChapter() != vk.getUpperBound().getChapter()) {
            m_alternativeContent = QString("%1 - %2:%3")
                                   .arg(QString::fromUtf8(vk.getLowerBound().getText()))
                                   .arg(vk.getUpperBound().getChapter())
                                   .arg(vk.getUpperBound().getVerse());
        }
        else { //only verses differ (same book, same chapter)
            m_alternativeContent = QString("%1 - %2")
                                   .arg(QString::fromUtf8(vk.getLowerBound().getText()))
                                   .arg(vk.getUpperBound().getVerse());
        }
    }

    m_alternativeContent.append(" (").append(module->name()).append(")");
    m_alternativeContent.prepend("<div class=\"rangeheading\" dir=\"ltr\">").append("</div>"); //insert the right tags
}

CTextRendering::CTextRendering(
        bool addText,
        DisplayOptions const & displayOptions,
        FilterOptions const & filterOptions)
    : m_displayOptions(displayOptions)
    , m_filterOptions(filterOptions)
    , m_addText(addText)
{}

BtConstModuleList CTextRendering::collectModules(const KeyTree &tree) const {
    //collect all modules which are available and used by child items
    BtConstModuleList modules;

    for (auto const & item : tree)
        for (auto const * const mod : item.modules())
            if (!modules.contains(mod))
                modules.append(mod);
    return modules;
}

const QString CTextRendering::renderKeyTree(const KeyTree &tree) {
    //CSwordBackend::instance()()->setDisplayOptions( m_displayOptions );
    CSwordBackend::instance()->setFilterOptions(m_filterOptions);

    const BtConstModuleList modules = collectModules(tree);
    QString t;

    //optimization for entries with the same key

    if (modules.count() == 1) { //this optimizes the rendering, only one key created for all items
        std::unique_ptr<CSwordKey> key(
                CSwordKey::createInstance(modules.first()));
        for (auto const & item : tree) {
            key->setKey(item.key());
            t.append(renderEntry(item, key.get()));
        }
    }
    else {
        for (auto const & item : tree)
            t.append(renderEntry(item));
    }

    return finishText(t, tree);
}

const QString CTextRendering::renderKeyRange(
        CSwordVerseKey const & lowerBound,
        CSwordVerseKey const & upperBound,
        const BtConstModuleList &modules,
        const QString &highlightKey,
        const KeyTreeItem::Settings &keySettings)
{

    if (lowerBound == upperBound) { //same key, render single key
        return renderSingleKey(lowerBound.key(), modules, keySettings);
    } else { // Render range
        BT_ASSERT(lowerBound < upperBound);
        KeyTree tree;
        KeyTreeItem::Settings settings = keySettings;

        auto curKey = lowerBound;
        do {
            //make sure the key given by highlightKey gets marked as current key
            settings.highlight = (!highlightKey.isEmpty() ? (curKey.key() == highlightKey) : false);

            /**
                \todo We need to take care of linked verses if we render one or
                      (esp) more modules. If the verses 2,3,4,5 are linked to 1,
                      it should be displayed as one entry with the caption 1-5.
            */

            if (curKey.chapter() == 0) { // range was 0:0-1:x, render 0:0 first and jump to 1:0
                curKey.setVerse(0);
                tree.emplace_back(curKey.key(), modules, settings);
                curKey.setChapter(1);
                curKey.setVerse(0);
            }
            tree.emplace_back(curKey.key(), modules, settings);
            if (!curKey.next(CSwordVerseKey::UseVerse)) {
                /// \todo Notify the user about this failure.
                break;
            }
        } while (curKey < upperBound);
        return renderKeyTree(tree);
    }

    return QString();
}

const QString CTextRendering::renderSingleKey(
        const QString &key,
        const BtConstModuleList &modules,
        const KeyTreeItem::Settings &settings)
{
    KeyTree tree;
    tree.emplace_back(key, modules, settings);
    return renderKeyTree(tree);
}

QString CTextRendering::renderEntry(KeyTreeItem const & i, CSwordKey * k)
{
    if (i.hasAlternativeContent()) {
        QString ret = i.settings().highlight
                      ? "<div class=\"currententry\">"
                      : "<div class=\"entry\">";
        ret.append(i.getAlternativeContent());

        if (!i.childList().empty()) {
            KeyTree const & tree = i.childList();

            BtConstModuleList const modules(collectModules(tree));

            if (modules.count() == 1)
                // insert the direction into the surrounding div:
                ret.insert(5,
                           QString("dir=\"%1\" ")
                               .arg(modules.first()->textDirectionAsHtml()));

            for (auto const & item : tree)
                ret.append(renderEntry(item));
        }

        ret.append("</div>");
        return ret; // WARNING: Return already here!
    }


    BtConstModuleList const & modules(i.modules());
    if (modules.isEmpty())
        return ""; // no module present for rendering

    std::unique_ptr<CSwordKey> scoped_key(
            !k ? CSwordKey::createInstance(modules.first()) : nullptr);
    CSwordKey * const key = k ? k : scoped_key.get();
    BT_ASSERT(key);

    CSwordVerseKey * const myVK = dynamic_cast<CSwordVerseKey *>(key);
    if (myVK)
        myVK->setIntros(true);

    QString renderedText((modules.count() > 1) ? "\n\t\t<tr>\n" : "\n");
    // Only insert the table stuff if we are displaying parallel.

    //declarations out of the loop for optimization
    QString entry;
    bool isRTL;
    QString preverseHeading;
    QString langAttr;
    QString key_renderedText;

    for (auto const & modulePtr : modules) {
        BT_ASSERT(modulePtr);
        if (myVK) {
            key->setModule(*modules.begin());
            key->setKey(i.key());

            // this would change key position due to v11n translation
            key->setModule(modulePtr);
        } else {
            key->setModule(modulePtr);
            key->setKey(i.key());
        }

        // indicate that key was changed
        i.setMappedKey(key->key() != i.key() ? key : nullptr);


        isRTL = (modulePtr->textDirection() == CSwordModuleInfo::RightToLeft);
        entry = QString();

        auto & swModule = modulePtr->module();
        if (modulePtr->language()->isValid()) {
            langAttr = QString(" xml:lang=\"")
                       .append(modulePtr->language()->abbrev())
                       .append("\" lang=\"")
                       .append(modulePtr->language()->abbrev())
                       .append("\"");
        } else {
            langAttr = QString(" xml:lang=\"")
                       .append(swModule.getLanguage())
                       .append("\" lang=\"")
                       .append(swModule.getLanguage())
                       .append("\"");
        }

        if (key->isValid() && i.key() == key->key()) {
            key_renderedText = key->renderedText();

            // if key was expanded
            if (CSwordVerseKey const * const vk =
                    dynamic_cast<CSwordVerseKey *>(key))
            {
                if (vk->isBoundSet()) {
                    CSwordVerseKey pk(*vk);
                    auto const lowerBoundIndex = vk->lowerBound().index();
                    auto const upperBoundIndex = vk->upperBound().index();
                    for (auto i = lowerBoundIndex; i < upperBoundIndex; ++i) {
                        key_renderedText += " ";
                        pk.setIndex(i + 1);
                        key_renderedText += pk.renderedText();
                    }
                }
            }
        } else {
            key_renderedText = "<span class=\"inactive\">&#8212;</span>";
        }

        if (m_filterOptions.headings && key->isValid() && i.key() == key->key()) {

            // only process EntryAttributes, do not render, this might destroy the EntryAttributes again
            swModule.renderText(nullptr, -1, 0);

            for (auto const & vp
                 : swModule.getEntryAttributes()["Heading"]["Preverse"])
            {
                QString unfiltered(QString::fromUtf8(vp.second.c_str()));

                /// \todo This is only a preliminary workaround to strip the tags:
                {
                    static QRegExp const staticFilter(
                            "(.*)<title[^>]*>(.*)</title>(.*)");
                    QRegExp filter(staticFilter);
                    while (filter.indexIn(unfiltered) >= 0)
                        unfiltered = filter.cap(1) + filter.cap(2) + filter.cap(3);
                }

                // Filter out offending self-closing div tags, which are bad HTML
                {
                    static QRegExp const staticFilter("(.*)<div[^>]*/>(.*)");
                    QRegExp filter(staticFilter);
                    while (filter.indexIn(unfiltered) >= 0)
                        unfiltered = filter.cap(1) + filter.cap(2);
                }

                preverseHeading = unfiltered;

                /// \todo Take care of the heading type!
                if (!preverseHeading.isEmpty()) {
                    entry.append("<div ")
                         .append(langAttr)
                         .append(" class=\"sectiontitle\">")
                         .append(preverseHeading)
                         .append("</div>");
                }
            }
        }

        entry.append(m_displayOptions.lineBreaks  ? "<div class=\""  : "<div class=\"inline ");

        if (modules.count() == 1) //insert only the class if we're not in a td
            entry.append( i.settings().highlight  ? "currententry " : "entry " );
        entry.append("\"");
        entry.append(langAttr).append(isRTL ? " dir=\"rtl\">" : " dir=\"ltr\">");

        //keys should normally be left-to-right, but this doesn't apply in all cases
        if(key->isValid() && i.key() == key->key())
            entry.append("<span class=\"entryname\" dir=\"ltr\">").append(entryLink(i, *modulePtr)).append("</span>");

        if (m_addText)
            entry.append(key_renderedText);

        if (!i.childList().empty())
            for (auto const & item : i.childList())
                entry.append(renderEntry(item));

        entry.append("</div>");

        if (modules.count() == 1) {
            renderedText.append("\t\t").append(entry).append("\n");
        } else {
            renderedText.append("\t\t<td class=\"")
                .append(i.settings().highlight ? "currententry" : "entry")
                .append("\" ")
                .append(langAttr)
                .append(" dir=\"")
                .append(isRTL ? "rtl" : "ltr")
                .append("\">\n")
                .append( "\t\t\t" ).append( entry ).append("\n")
                .append("\t\t</td>\n");
        }
    }

    if (modules.count() > 1)
        renderedText.append("\t\t</tr>\n");

    //  qDebug("CTextRendering: %s", renderedText.latin1());
    return renderedText;
}

QString CTextRendering::finishText(QString const & text, KeyTree const & tree) {
    CDisplayTemplateMgr::Settings settings;
    settings.modules = collectModules(tree);
    if (settings.modules.count() == 1) {
        CSwordModuleInfo const * const firstModule = settings.modules.first();
        CLanguageMgr::Language const * const lang = firstModule->language();
        settings.langAbbrev = lang->isValid() ? lang->abbrev() : "unknown";
        settings.textDirection = firstModule->textDirection();
    } else {
        settings.langAbbrev = "unknown";
    }

    return CDisplayTemplateMgr::instance()->fillTemplate(
                CDisplayTemplateMgr::activeTemplateName(),
                text,
                settings);
}

/*!
    \fn CTextRendering::entryLink( KeyTreeItem& item )
 */
QString CTextRendering::entryLink(KeyTreeItem const & item,
                                  CSwordModuleInfo const &)
{ return item.key(); }
