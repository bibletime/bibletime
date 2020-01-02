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

#include "ctextrendering.h"

#include <memory>
#include <QRegExp>
#include <QtAlgorithms>
#include "../../util/btassert.h"
#include "../drivers/cswordmoduleinfo.h"
#include "../keys/cswordkey.h"
#include "../keys/cswordversekey.h"
#include "../managers/cdisplaytemplatemgr.h"
#include "../managers/referencemanager.h"

// Sword includes:
#include <swkey.h>


using namespace Rendering;

CTextRendering::KeyTreeItem::KeyTreeItem(const QString &key,
                                         const CSwordModuleInfo *module,
                                         const Settings &settings)
        : m_settings(settings),
        m_moduleList(),
        m_key( key ),
        m_childList(),
        m_stopKey( QString() ),
        m_alternativeContent( QString() ) {
    m_moduleList.append( const_cast<CSwordModuleInfo*>(module) ); //BAD CODE
}

CTextRendering::KeyTreeItem::KeyTreeItem(const QString &content,
                                         const Settings &settings)
        : m_settings( settings ),
        m_moduleList(),
        m_key( QString() ),
        m_childList(),
        m_stopKey( QString() ),
        m_alternativeContent( content ) {
}

CTextRendering::KeyTreeItem::KeyTreeItem(const QString &key,
                                         const BtConstModuleList &mods,
                                         const Settings &settings)
        : m_settings( settings ),
        m_moduleList( mods ),
        m_key( key ),
        m_childList(),
        m_stopKey( QString() ),
        m_alternativeContent( QString() ) {
}

CTextRendering::KeyTreeItem::KeyTreeItem()
        : m_settings(),
        m_moduleList(),
        m_key(QString()),
        m_childList(),
        m_stopKey(QString()),
        m_alternativeContent(QString()) {
}

CTextRendering::KeyTreeItem::KeyTreeItem(const KeyTreeItem& i)
        : m_settings( i.m_settings ),
        m_moduleList( i.m_moduleList ),
        m_key( i.m_key ),
        m_childList(),
        m_stopKey( i.m_stopKey ),
        m_alternativeContent( i.m_alternativeContent )
{
    const KeyTree &tree = *i.childList();
    Q_FOREACH (const KeyTreeItem * const item, tree) {
        m_childList.append(new KeyTreeItem((*item))); //deep copy
    }

}

CTextRendering::KeyTreeItem::KeyTreeItem(const QString &startKey,
                                         const QString &stopKey,
                                         const CSwordModuleInfo *module,
                                         const Settings &settings)
        : m_settings( settings ),
        m_moduleList(),
        m_key( startKey ),
        m_childList(),
        m_stopKey( stopKey ),
        m_alternativeContent( QString() ) {
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
                m_childList.append(
                            new KeyTreeItem(start.key(),
                                            module,
                                            KeyTreeItem::Settings{
                                                false,
                                                settings.keyRenderingFace}));
                ok = start.next(CSwordVerseKey::UseVerse);
            }
        }
        else if (m_key.isEmpty()) {
            m_childList.append(
                        new KeyTreeItem(startKey,
                                        module,
                                        KeyTreeItem::Settings{
                                            false,
                                            settings.keyRenderingFace}));
        }
    }
    else if ((module->type() == CSwordModuleInfo::Lexicon) || (module->type() == CSwordModuleInfo::Commentary) ) {
        m_childList.append(
                    new KeyTreeItem(startKey,
                                    module,
                                    KeyTreeItem::Settings{
                                        false,
                                        KeyTreeItem::Settings::NoKey}));
    }
    else if (module->type() == CSwordModuleInfo::GenericBook) {
        m_childList.append(
                    new KeyTreeItem(startKey,
                                    module,
                                    KeyTreeItem::Settings{
                                        false,
                                        KeyTreeItem::Settings::NoKey}));
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

BtConstModuleList CTextRendering::collectModules(const KeyTree &tree) const {
    //collect all modules which are available and used by child items
    BtConstModuleList modules;

    Q_FOREACH (const KeyTreeItem * const c, tree) {
        BT_ASSERT(c);
        Q_FOREACH (const CSwordModuleInfo * const mod, c->modules()) {
            if (!modules.contains(mod))
                modules.append(mod);
        }
    }
    return modules;
}

const QString CTextRendering::renderKeyTree(const KeyTree &tree) {
    initRendering();

    const BtConstModuleList modules = collectModules(tree);
    QString t;

    //optimization for entries with the same key

    if (modules.count() == 1) { //this optimizes the rendering, only one key created for all items
        std::unique_ptr<CSwordKey> key(
                CSwordKey::createInstance(modules.first()));
        Q_FOREACH (const KeyTreeItem * const c, tree) {
            key->setKey(c->key());
            t.append(renderEntry(*c, key.get()));
        }
    }
    else {
        Q_FOREACH (const KeyTreeItem * const c, tree) {
            t.append( renderEntry( *c ) );
        }
    }

    return finishText(t, tree);
}

const QString CTextRendering::renderKeyRange(
        const QString &start,
        const QString &stop,
        const BtConstModuleList &modules,
        const QString &highlightKey,
        const KeyTreeItem::Settings &keySettings)
{

    const CSwordModuleInfo *module = modules.first();
    //qWarning( "renderKeyRange start %s stop %s \n", start.latin1(), stop.latin1() );

    std::unique_ptr<CSwordKey> lowerBound( CSwordKey::createInstance(module) );
    lowerBound->setKey(start);

    std::unique_ptr<CSwordKey> upperBound( CSwordKey::createInstance(module) );
    upperBound->setKey(stop);

    sword::SWKey* sw_start = dynamic_cast<sword::SWKey*>(lowerBound.get());
    sword::SWKey* sw_stop = dynamic_cast<sword::SWKey*>(upperBound.get());

    BT_ASSERT((*sw_start == *sw_stop) || (*sw_start < *sw_stop));

    if (*sw_start == *sw_stop) { //same key, render single key
        return renderSingleKey(lowerBound->key(), modules);
    }
    else if (*sw_start < *sw_stop) { // Render range
        KeyTree tree;
        KeyTreeItem::Settings settings = keySettings;

        CSwordVerseKey* vk_start = dynamic_cast<CSwordVerseKey*>(lowerBound.get());
        BT_ASSERT(vk_start);

        CSwordVerseKey* vk_stop = dynamic_cast<CSwordVerseKey*>(upperBound.get());
        BT_ASSERT(vk_stop);

        while ((*vk_start < *vk_stop) || (*vk_start == *vk_stop)) {

            //make sure the key given by highlightKey gets marked as current key
            settings.highlight = (!highlightKey.isEmpty() ? (vk_start->key() == highlightKey) : false);

            /**
                \todo We need to take care of linked verses if we render one or
                      (esp) more modules. If the verses 2,3,4,5 are linked to 1,
                      it should be displayed as one entry with the caption 1-5.
            */

            if (vk_start->getChapter() == 0) { // range was 0:0-1:x, render 0:0 first and jump to 1:0
                vk_start->setVerse(0);
                tree.append( new KeyTreeItem(vk_start->key(), modules, settings) );
                vk_start->setChapter(1);
                vk_start->setVerse(0);
            }
            tree.append( new KeyTreeItem(vk_start->key(), modules, settings) );
            if (!vk_start->next(CSwordVerseKey::UseVerse)) {
                /// \todo Notify the user about this failure.
                break;
            }
        }
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
    tree.append( new KeyTreeItem(key, modules, settings) );

    return renderKeyTree(tree);
}
