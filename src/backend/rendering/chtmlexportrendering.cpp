/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/rendering/chtmlexportrendering.h"

#include <QSharedPointer>

#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordkey.h"
#include "backend/keys/cswordversekey.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "backend/managers/clanguagemgr.h"


#if 0
#include <QDebug>

namespace {

/** Helper function to dump a verse with all its enty attributes. */
void dumpEntryAttributes(sword::SWModule *module) {
    qDebug() << "Attributes for key: " << module->getKeyText();
    sword::AttributeTypeList::iterator i1;
    sword::AttributeList::iterator i2;
    sword::AttributeValue::iterator i3;
    for (i1 = module->getEntryAttributes().begin(); i1 != module->getEntryAttributes().end(); i1++) {
        qDebug() << "[ " << i1->first << " ]";
        for (i2 = i1->second.begin(); i2 != i1->second.end(); i2++) {
            qDebug() << "\t[ " << i2->first << " ]";
            for (i3 = i2->second.begin(); i3 != i2->second.end(); i3++) {
               qDebug() << "\t\t" << i3->first << " = " << i3->second;
            }
        }
    }
}

} // anonymous namespace
#endif

namespace Rendering {

CHTMLExportRendering::CHTMLExportRendering(
        bool addText,
        const DisplayOptions &displayOptions,
        const FilterOptions &filterOptions)
    : m_displayOptions(displayOptions)
    , m_filterOptions(filterOptions)
    , m_addText(addText)
{
    // Intentionally empty
}

QString CHTMLExportRendering::renderEntry(const KeyTreeItem& i, CSwordKey* k) {

    if (i.hasAlternativeContent()) {
        QString ret = i.settings().highlight
                      ? "<div class=\"currententry\">"
                      : "<div class=\"entry\">";
        ret.append(i.getAlternativeContent());

        //   Q_ASSERT(i.hasChildItems());

        if (!i.childList()->isEmpty()) {
            const KeyTree & tree = *i.childList();

            const QList<const CSwordModuleInfo*> modules = collectModules(tree);

            if (modules.count() == 1) { //insert the direction into the surrounding div
                ret.insert( 5, QString("dir=\"%1\" ").arg((modules.first()->textDirection() == CSwordModuleInfo::LeftToRight) ? "ltr" : "rtl" ));
            }

            Q_FOREACH (const KeyTreeItem * const item, tree) {
                ret.append(renderEntry(*item));
            }
        }

        ret.append("</div>");
        return ret; //WARNING: Return already here!
    }


    const QList<const CSwordModuleInfo*> &modules(i.modules());
    if (modules.isEmpty()) {
        return ""; //no module present for rendering
    }

    QSharedPointer<CSwordKey> scoped_key( !k ? CSwordKey::createInstance(modules.first()) : 0 );
    CSwordKey* key = k ? k : scoped_key.data();
    Q_ASSERT(key);

    CSwordVerseKey* myVK = dynamic_cast<CSwordVerseKey*>(key);

    if (myVK) {
        myVK->setIntros(true);
    }

    QString renderedText( (modules.count() > 1) ? "\n\t\t<tr>\n" : "\n" );
    // Only insert the table stuff if we are displaying parallel.

    //declarations out of the loop for optimization
    QString entry;
    bool isRTL;
    QString preverseHeading;
    QString langAttr;
    QString key_renderedText;

    QList<const CSwordModuleInfo*>::const_iterator end_modItr = modules.end();

    for (QList<const CSwordModuleInfo*>::const_iterator mod_Itr(modules.begin()); mod_Itr != end_modItr; ++mod_Itr) {
        key->setModule(*mod_Itr);
        key->setKey(i.key());

        isRTL = ((*mod_Itr)->textDirection() == CSwordModuleInfo::RightToLeft);
        entry = QString::null;

        if ((*mod_Itr)->language()->isValid()) {
            langAttr = QString("xml:lang=\"")
                       .append((*mod_Itr)->language()->abbrev())
                       .append("\" lang=\"")
                       .append((*mod_Itr)->language()->abbrev())
                       .append("\"");
        }
        else {
            langAttr = QString("xml:lang=\"")
                       .append((*mod_Itr)->module()->getLanguage())
                       .append("\" lang=\"")
                       .append((*mod_Itr)->module()->getLanguage())
                       .append("\"");
        }

        key_renderedText = key->renderedText();

        if (m_filterOptions.headings) {

            // only process EntryAttributes, do not render, this might destroy the EntryAttributes again
            (*mod_Itr)->module()->RenderText(0, -1, 0);

            sword::AttributeValue::const_iterator it =
                (*mod_Itr)->module()->getEntryAttributes()["Heading"]["Preverse"].begin();
            const sword::AttributeValue::const_iterator end =
                (*mod_Itr)->module()->getEntryAttributes()["Heading"]["Preverse"].end();

            for (; it != end; ++it) {
                QString unfiltered = QString::fromUtf8(it->second.c_str());

                /// \todo This is only a preliminary workaround to strip the tags:
                QRegExp filter("(.*)<title[^>]*>(.*)</title>(.*)");
                while(filter.indexIn(unfiltered) >= 0) {
                    unfiltered = filter.cap(1) + filter.cap(2) + filter.cap(3);
                }
                // Fiter out offending self-closing div tags, which are bad HTML
                QRegExp ofilter("(.*)<div[^>]*/>(.*)");
                while(ofilter.indexIn(unfiltered) >= 0) {
                    unfiltered = ofilter.cap(1) + ofilter.cap(2);
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

        entry.append(m_displayOptions.lineBreaks  ? "<div "  : "<div style=\"display: inline;\" ");

        if (modules.count() == 1) { //insert only the class if we're not in a td
            entry.append( i.settings().highlight  ? "class=\"currententry\" " : "class=\"entry\" " );
        }

        entry.append(langAttr).append(isRTL ? " dir=\"rtl\">" : " dir=\"ltr\">");

        //keys should normally be left-to-right, but this doesn't apply in all cases
        entry.append("<span class=\"entryname\" dir=\"ltr\">").append(entryLink(i, *mod_Itr)).append("</span>");

        if (m_addText) {
            //entry.append( QString::fromLatin1("<span %1>%2</span>").arg(langAttr).arg(key_renderedText) );
            entry.append( key_renderedText );
        }

        if (!i.childList()->isEmpty()) {
            KeyTree* tree(i.childList());

            Q_FOREACH (const KeyTreeItem * const c, *tree) {
                entry.append( renderEntry(*c) );
            }
        }

        entry.append("</div>");

        if (modules.count() == 1) {
            renderedText.append( "\t\t" ).append( entry ).append("\n");
        }
        else {
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

    if (modules.count() > 1) {
        renderedText.append("\t\t</tr>\n");
    }

    //  qDebug("CHTMLExportRendering: %s", renderedText.latin1());
    return renderedText;
}

void CHTMLExportRendering::initRendering() {
    //CSwordBackend::instance()()->setDisplayOptions( m_displayOptions );
    CSwordBackend::instance()->setFilterOptions( m_filterOptions );
}

QString CHTMLExportRendering::finishText(const QString &text, const KeyTree &tree) {
    typedef CDisplayTemplateMgr CDTM;

    CDTM::Settings settings;
    settings.modules = collectModules(tree);
    if (settings.modules.count() == 1) {
        const CSwordModuleInfo * const firstModule = settings.modules.first();
        const CLanguageMgr::Language * const lang = firstModule->language();
        settings.langAbbrev = lang->isValid() ? lang->abbrev() : "unknown";
        if (firstModule->textDirection() == CSwordModuleInfo::RightToLeft) {
            settings.pageDirection = "rtl";
        }
    } else {
        settings.langAbbrev = "unknown";
        settings.pageDirection = QString::null;
    }

    return CDTM::instance()->fillTemplate(QObject::tr("Export"), text, settings);
}

/*!
    \fn CHTMLExportRendering::entryLink( KeyTreeItem& item )
 */
QString CHTMLExportRendering::entryLink(const KeyTreeItem &item,
                                        const CSwordModuleInfo * module)
{
    Q_UNUSED(module);

    return item.key();
}

}//end of namespace "Rendering"
