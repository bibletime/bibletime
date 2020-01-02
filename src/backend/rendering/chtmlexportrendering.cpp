/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "chtmlexportrendering.h"

#include <memory>
#include "../../util/btassert.h"
#include "../drivers/cswordmoduleinfo.h"
#include "../keys/cswordkey.h"
#include "../keys/cswordversekey.h"
#include "../managers/cdisplaytemplatemgr.h"
#include "../managers/clanguagemgr.h"


namespace Rendering {

CHTMLExportRendering::CHTMLExportRendering(
        bool addText,
        DisplayOptions const & displayOptions,
        FilterOptions const & filterOptions)
    : m_displayOptions(displayOptions)
    , m_filterOptions(filterOptions)
    , m_addText(addText)
{}

QString CHTMLExportRendering::renderEntry(KeyTreeItem const & i, CSwordKey * k)
{
    if (i.hasAlternativeContent()) {
        QString ret = i.settings().highlight
                      ? "<div class=\"currententry\">"
                      : "<div class=\"entry\">";
        ret.append(i.getAlternativeContent());

        if (!i.childList()->isEmpty()) {
            KeyTree const & tree = *i.childList();

            BtConstModuleList const modules(collectModules(tree));

            if (modules.count() == 1)
                // insert the direction into the surrounding div:
                ret.insert(5,
                           QString("dir=\"%1\" ")
                               .arg(modules.first()->textDirectionAsHtml()));

            Q_FOREACH (KeyTreeItem const * const item, tree)
                ret.append(renderEntry(*item));
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
                    for (auto i = vk->getLowerBound().getIndex();
                         i < vk->getUpperBound().getIndex();
                         ++i)
                    {
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
            entry.append("<span class=\"entryname\" dir=\"ltr\">").append(entryLink(i, modulePtr)).append("</span>");

        if (m_addText)
            entry.append(key_renderedText);

        if (!i.childList()->isEmpty())
            Q_FOREACH (KeyTreeItem const * const c, *(i.childList()))
                entry.append(renderEntry(*c));

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

    //  qDebug("CHTMLExportRendering: %s", renderedText.latin1());
    return renderedText;
}

void CHTMLExportRendering::initRendering() {
    //CSwordBackend::instance()()->setDisplayOptions( m_displayOptions );
    CSwordBackend::instance()->setFilterOptions(m_filterOptions);
}

QString CHTMLExportRendering::finishText(QString const & text,
                                         KeyTree const & tree)
{
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
    \fn CHTMLExportRendering::entryLink( KeyTreeItem& item )
 */
QString CHTMLExportRendering::entryLink(KeyTreeItem const & item,
                                        CSwordModuleInfo const *)
{ return item.key(); }

}//end of namespace "Rendering"
