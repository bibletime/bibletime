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
#include <QtAlgorithms>
#include "../../util/btassert.h"
#include "../config/btconfig.h"
#include "../drivers/cswordmoduleinfo.h"
#include "../keys/cswordkey.h"
#include "../keys/cswordversekey.h"
#include "../managers/cdisplaytemplatemgr.h"
#include "../managers/cswordbackend.h"

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
{
    BT_ASSERT(module);
    m_moduleList.append(module);

    //use the start and stop key to ceate our child items

    if (module->type() == CSwordModuleInfo::Bible) {
        CSwordVerseKey start(module);
        start.setKey(startKey);

        CSwordVerseKey stop(module);
        stop.setKey(stopKey);

        if (!m_key.isEmpty() && !stopKey.isEmpty()) { //we have a range of keys
            bool ok = true;

            while (ok && ((start < stop) || (start == stop)) ) { //range
                m_childList.emplace_back(start.key(),
                                         module,
                                         KeyTreeItem::Settings{
                                             false,
                                             settings.keyRenderingFace});
                ok = start.next();
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
            m_alternativeContent = QStringLiteral("%1 - %2:%3")
                                   .arg(QString::fromUtf8(vk.getLowerBound().getText()))
                                   .arg(vk.getUpperBound().getChapter())
                                   .arg(vk.getUpperBound().getVerse());
        }
        else { //only verses differ (same book, same chapter)
            m_alternativeContent = QStringLiteral("%1 - %2")
                                   .arg(QString::fromUtf8(vk.getLowerBound().getText()))
                                   .arg(vk.getUpperBound().getVerse());
        }
    }
    m_alternativeContent =
            QStringLiteral("<div class=\"rangeheading\" dir=\"ltr\">%1 (%2)"
                           "</div>")
                .arg(m_alternativeContent, module->name());
}

CTextRendering::CTextRendering(bool const addText)
    : CTextRendering(addText,
                     btConfig().getDisplayOptions(),
                     btConfig().getFilterOptions())
{}

CTextRendering::CTextRendering(
        bool const addText,
        DisplayOptions const & displayOptions,
        FilterOptions const & filterOptions)
    : m_displayOptions(displayOptions)
    , m_filterOptions(filterOptions)
    , m_addText(addText)
{}

CTextRendering::~CTextRendering() = default;

BtConstModuleList CTextRendering::collectModules(KeyTree const & tree) {
    //collect all modules which are available and used by child items
    BtConstModuleList modules;

    for (auto const & item : tree)
        for (auto const * const mod : item.modules())
            if (!modules.contains(mod))
                modules.append(mod);
    return modules;
}

QString CTextRendering::renderKeyTree(KeyTree const & tree) const {
    //CSwordBackend::instance()()->setDisplayOptions( m_displayOptions );
    CSwordBackend::instance().setFilterOptions(m_filterOptions);

    const BtConstModuleList modules = collectModules(tree);
    QString t;

    //optimization for entries with the same key

    if (modules.count() == 1) { //this optimizes the rendering, only one key created for all items
        std::unique_ptr<CSwordKey> key(modules.first()->createKey());
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

QString CTextRendering::renderKeyRange(
        CSwordVerseKey const & lowerBound,
        CSwordVerseKey const & upperBound,
        const BtConstModuleList &modules,
        const QString &highlightKey,
        const KeyTreeItem::Settings &keySettings)
{

    if (lowerBound == upperBound) // same key, render single key:
        return renderSingleKey(lowerBound.key(), modules, keySettings);

    // Render range:
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
        if (!curKey.next()) {
            /// \todo Notify the user about this failure.
            break;
        }
    } while (curKey < upperBound);
    return renderKeyTree(tree);
}

QString CTextRendering::renderSingleKey(
        const QString &key,
        const BtConstModuleList &modules,
        const KeyTreeItem::Settings &settings)
{
    KeyTree tree;
    tree.emplace_back(key, modules, settings);
    return renderKeyTree(tree);
}

QString CTextRendering::renderEntry(KeyTreeItem const & i, CSwordKey * k) const
{
    if (i.hasAlternativeContent()) {
        auto ret = i.settings().highlight
                   ? QStringLiteral("<div class=\"currententry\">")
                   : QStringLiteral("<div class=\"entry\">");
        ret.append(i.getAlternativeContent());

        if (!i.childList().empty()) {
            KeyTree const & tree = i.childList();

            BtConstModuleList const modules(collectModules(tree));

            if (modules.count() == 1)
                // insert the direction into the surrounding div:
                ret.insert(5,
                           QStringLiteral("dir=\"%1\" ")
                               .arg(modules.first()->textDirectionAsHtml()));

            for (auto const & item : tree)
                ret.append(renderEntry(item));
        }

        ret.append(QStringLiteral("</div>"));
        return ret; // WARNING: Return already here!
    }


    BtConstModuleList const & modules(i.modules());
    if (modules.isEmpty())
        return {}; // no module present for rendering

    std::unique_ptr<CSwordKey> scoped_key(
            !k ? modules.first()->createKey() : nullptr);
    CSwordKey * const key = k ? k : scoped_key.get();
    BT_ASSERT(key);

    CSwordVerseKey * const myVK = dynamic_cast<CSwordVerseKey *>(key);
    if (myVK)
        myVK->setIntros(true);

    bool const oneModule = modules.size() == 1;
    auto renderedText(oneModule
                      ? QStringLiteral("\n")
                      : QStringLiteral("\n\t\t<tr>\n"));
    // Only insert the table stuff if we are displaying parallel.

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

        auto & swModule = modulePtr->swordModule();
        auto const langAttr =
                QStringLiteral(" xml:lang=\"%1\" lang=\"%1\"").arg(
                    modulePtr->language()->abbrev());

        QString key_renderedText;
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
                        key_renderedText += ' ';
                        pk.setIndex(i + 1);
                        key_renderedText += pk.renderedText();
                    }
                }
            }
        } else {
            key_renderedText =
                    QStringLiteral("<span class=\"inactive\">&#8212;</span>");
        }

        QString entry;
        if (m_filterOptions.headings && key->isValid() && i.key() == key->key()) {

            // only process EntryAttributes, do not render, this might destroy the EntryAttributes again
            swModule.renderText(nullptr, -1, 0);

            for (auto const & vp
                 : swModule.getEntryAttributes()["Heading"]["Preverse"])
            {
                auto preverseHeading(QString::fromUtf8(vp.second.c_str()));
                if (preverseHeading.isEmpty())
                    continue;

                static QString const greaterOrS(
                            QStringLiteral(">\x20\x09\x0d\x0a"));
                for (auto i = preverseHeading.indexOf('<');
                     i >= 0;
                     i = preverseHeading.indexOf('<', i))
                {
                    auto ref = QStringView(preverseHeading).mid(i + 1);
                    if (ref.startsWith(QStringLiteral("title"))) {
                        ref = ref.mid(5); // strlen("title")
                        if (!ref.isEmpty() && greaterOrS.contains(ref[0])) {
                            auto const charsUntilTagEnd = ref.indexOf('>');
                            if (charsUntilTagEnd < 0)
                                break;
                            // Remove entire <title> tag:
                            preverseHeading.remove(i, charsUntilTagEnd + 7);
                        } else {
                            i += 7; // strlen("<title?")
                        }
                    } else if (ref.startsWith(QStringLiteral("/title"))) {
                        ref = ref.mid(6); // strlen("/title")
                        if (!ref.isEmpty() && greaterOrS.contains(ref[0])) {
                            auto const charsUntilTagEnd = ref.indexOf('>');
                            if (charsUntilTagEnd < 0)
                                break;
                            // Remove entire </title> tag:
                            preverseHeading.remove(i, charsUntilTagEnd + 8);
                        } else {
                            i += 8; // strlen("</title?")
                        }
                    } else if (ref.startsWith(QStringLiteral("div"))) {
                        ref = ref.mid(3); // strlen("div")
                        if (!ref.isEmpty() && greaterOrS.contains(ref[0])) {
                            auto const charsUntilTagEnd = ref.indexOf('>');
                            if (charsUntilTagEnd < 0)
                                break;
                            if (charsUntilTagEnd > 0) {
                                if (ref[charsUntilTagEnd - 1] == '/') {
                                    // Remove entire invalid empty <div/> tag:
                                    preverseHeading.remove(
                                                i,
                                                charsUntilTagEnd + 5);
                                } else {
                                    i += charsUntilTagEnd + 5;
                                }
                            } else {
                                i += 5; // strlen("<div>")
                            }
                        } else {
                            i += 5; // strlen("<div?")
                        }
                    } else {
                        i += 3; // strlen("<?>")
                    }
                }

                /// \todo Take care of the heading type!
                if (!preverseHeading.isEmpty())
                    entry = QStringLiteral(
                                "<div%1 class=\"sectiontitle\">%2</div>")
                            .arg(langAttr, preverseHeading);
            }
        }

        entry.append(m_displayOptions.lineBreaks
                     ? QStringLiteral("<div class=\"")
                     : QStringLiteral("<div class=\"inline "));
        if (oneModule && i.settings().highlight)
            entry.append(QStringLiteral("current"));
        entry.append(QStringLiteral("entry\""));
        auto const textDirectionAttribute =
                QStringLiteral(" dir=\"%1\">").arg(
                    modulePtr->textDirectionAsHtml());
        entry.append(langAttr).append(textDirectionAttribute);

        //keys should normally be left-to-right, but this doesn't apply in all cases
        if(key->isValid() && i.key() == key->key())
            entry.append(
                    QStringLiteral(
                            "<span class=\"entryname\" dir=\"ltr\">%1</span>")
                    .arg(entryLink(i, *modulePtr)));

        if (m_addText)
            entry.append(key_renderedText);

        for (auto const & item : i.childList())
            entry.append(renderEntry(item));

        entry.append(QStringLiteral("</div>"));

        if (oneModule) {
            renderedText.append(QStringLiteral("\t\t%1\n").arg(entry));
        } else {
            renderedText.append(
                        QStringLiteral("\t\t<td class=\"%1entry\"%2%3\n\t\t\t%4"
                                       "\n\t\t</td>\n")
                        .arg(i.settings().highlight
                             ? QStringLiteral("current")
                             : QString(),
                             langAttr,
                             textDirectionAttribute,
                             entry));
        }
    }

    if (!oneModule)
        renderedText.append(QStringLiteral("\t\t</tr>\n"));

    //  qDebug("CTextRendering: %s", renderedText.latin1());
    return renderedText;
}

QString CTextRendering::finishText(QString const & text, KeyTree const & tree)
    const
{
    CDisplayTemplateMgr::Settings settings;
    settings.modules = collectModules(tree);
    if (settings.modules.count() == 1) {
        CSwordModuleInfo const * const firstModule = settings.modules.first();
        settings.langAbbrev = firstModule->language()->abbrev();
        settings.textDirection = firstModule->textDirection();
    } else {
        settings.langAbbrev = QStringLiteral("unknown");
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
                                  CSwordModuleInfo const &) const
{ return item.key(); }
