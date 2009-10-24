/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/rendering/ctextrendering.h"

#include <boost/scoped_ptr.hpp>
#include <QRegExp>
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordkey.h"
#include "backend/keys/cswordversekey.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "backend/managers/creferencemanager.h"

// Sword includes:
#include <swkey.h>


using namespace Rendering;

CTextRendering::KeyTreeItem::KeyTreeItem(const QString& key, CSwordModuleInfo const * mod, const Settings settings )
        : m_settings( settings ),
        m_moduleList(),
        m_key( key ),
        m_childList(),
        m_stopKey( QString::null ),
        m_alternativeContent( QString::null ) {
    m_moduleList.append( const_cast<CSwordModuleInfo*>(mod) ); //BAD CODE
}

CTextRendering::KeyTreeItem::KeyTreeItem(const QString& content, const Settings settings )
        : m_settings( settings ),
        m_moduleList(),
        m_key( QString::null ),
        m_childList(),
        m_stopKey( QString::null ),
        m_alternativeContent( content ) {
}

CTextRendering::KeyTreeItem::KeyTreeItem(const QString& key, const QList<CSwordModuleInfo*>& mods, const Settings settings )
        : m_settings( settings ),
        m_moduleList( mods ),
        m_key( key ),
        m_childList(),
        m_stopKey( QString::null ),
        m_alternativeContent( QString::null ) {
}

CTextRendering::KeyTreeItem::KeyTreeItem()
        : m_settings(),
        m_moduleList(),
        m_key(QString::null),
        m_childList(),
        m_stopKey(QString::null),
        m_alternativeContent(QString::null) {
}

CTextRendering::KeyTreeItem::KeyTreeItem(const KeyTreeItem& i)
        : m_settings( i.m_settings ),
        m_moduleList( i.m_moduleList ),
        m_key( i.m_key ),
        m_childList(),
        m_stopKey( i.m_stopKey ),
        m_alternativeContent( i.m_alternativeContent ) {
    foreach(KeyTreeItem* item, (*i.childList())) {
        m_childList.append(new KeyTreeItem((*item))); //deep copy
    }

}

CTextRendering::KeyTreeItem::~KeyTreeItem() {
    qDeleteAll(m_childList);
}

CTextRendering::KeyTreeItem::KeyTreeItem(const QString& startKey, const QString& stopKey, CSwordModuleInfo* module, const Settings settings)
        : m_settings( settings ),
        m_moduleList(),
        m_key( startKey ),
        m_childList(),
        m_stopKey( stopKey ),
        m_alternativeContent( QString::null ) {
    Q_ASSERT(module);
    m_moduleList.append(module);

    //use the start and stop key to ceate our child items

    if (module->type() == CSwordModuleInfo::Bible) {
        CSwordVerseKey start(module);
        start.key(startKey);

        CSwordVerseKey stop(module);
        stop.key(stopKey);

        if (!m_key.isEmpty() && !m_stopKey.isEmpty()) { //we have a range of keys
            bool ok = true;

            while (ok && ((start < stop) || (start == stop)) ) { //range
                m_childList.append(
                    new KeyTreeItem(start.key(), module, KeyTreeItem::Settings(false, settings.keyRenderingFace))
                );


                ok = start.next(CSwordVerseKey::UseVerse);
            }
        }
        else if (m_key.isEmpty()) {
            m_childList.append( new KeyTreeItem(startKey, module, KeyTreeItem::Settings(false, settings.keyRenderingFace)) );
        }
    }
    else if ((module->type() == CSwordModuleInfo::Lexicon) || (module->type() == CSwordModuleInfo::Commentary) ) {
        m_childList.append( new KeyTreeItem(startKey, module, KeyTreeItem::Settings(false, KeyTreeItem::Settings::NoKey)) );
    }
    else if (module->type() == CSwordModuleInfo::GenericBook) {
        m_childList.append( new KeyTreeItem(startKey, module, KeyTreeItem::Settings(false, KeyTreeItem::Settings::NoKey)) );
    }

    //make it into "<simple|range> (modulename)"

    if (startKey == stopKey) {
        m_alternativeContent = startKey;
    }
    else {
        sword::VerseKey vk(startKey.toUtf8().constData(), stopKey.toUtf8().constData());

        if (vk.LowerBound().Book() != vk.UpperBound().Book()) {
            m_alternativeContent = QString::fromUtf8(vk.getRangeText());
        }
        else if (vk.LowerBound().Chapter() != vk.UpperBound().Chapter()) {
            m_alternativeContent = QString("%1 - %2:%3")
                                   .arg(QString::fromUtf8(vk.LowerBound().getText()))
                                   .arg(vk.UpperBound().Chapter())
                                   .arg(vk.UpperBound().Verse());
        }
        else { //only verses differ (same book, same chapter)
            m_alternativeContent = QString("%1 - %2")
                                   .arg(QString::fromUtf8(vk.LowerBound().getText()))
                                   .arg(vk.UpperBound().Verse());
        }
    }

    m_alternativeContent.append(" (").append(module->name()).append(")");
    m_alternativeContent.prepend("<div class=\"rangeheading\" dir=\"ltr\">").append("</div>"); //insert the right tags
}

const QString& CTextRendering::KeyTreeItem::getAlternativeContent() const {
    return m_alternativeContent;
}

const QList<CSwordModuleInfo*> CTextRendering::collectModules(KeyTree* const tree) const {
    //collect all modules which are available and used by child items
    QList<CSwordModuleInfo*> modules;

    foreach (KeyTreeItem* c, (*tree)) {
        Q_ASSERT(c);
        foreach (CSwordModuleInfo* mod, c->modules()) {
            if (!modules.contains(mod)) {
                modules.append(mod);
            }
        }
    }
    return modules;
}

const QString CTextRendering::renderKeyTree( KeyTree& tree ) {
    initRendering();

    QList<CSwordModuleInfo*> modules = collectModules(&tree);
    QString t;

    //optimization for entries with the same key
    boost::scoped_ptr<CSwordKey> key(
        (modules.count() == 1) ? CSwordKey::createInstance(modules.first()) : 0
    );

    foreach (KeyTreeItem* c, tree) {
        if (modules.count() == 1) { //this optimizes the rendering, only one key created for all items
            key->key( c->key() );
            t.append( renderEntry( *c, key.get()) );
        }
        else {
            t.append( renderEntry( *c ) );
        }
    }

    return finishText(t, tree);
}

const QString CTextRendering::renderKeyRange( const QString& start, const QString& stop, const QList<CSwordModuleInfo*>& modules, const QString& highlightKey, const KeyTreeItem::Settings& keySettings ) {

    CSwordModuleInfo* module = modules.first();
    //qWarning( "renderKeyRange start %s stop %s \n", start.latin1(), stop.latin1() );

    boost::scoped_ptr<CSwordKey> lowerBound( CSwordKey::createInstance(module) );
    lowerBound->key(start);

    boost::scoped_ptr<CSwordKey> upperBound( CSwordKey::createInstance(module) );
    upperBound->key(stop);

    sword::SWKey* sw_start = dynamic_cast<sword::SWKey*>(lowerBound.get());
    sword::SWKey* sw_stop = dynamic_cast<sword::SWKey*>(upperBound.get());

    Q_ASSERT((*sw_start == *sw_stop) || (*sw_start < *sw_stop));

    if (*sw_start == *sw_stop) { //same key, render single key
        return renderSingleKey(lowerBound->key(), modules);
    }
    else if (*sw_start < *sw_stop) { // Render range
        KeyTree tree;
        KeyTreeItem::Settings settings = keySettings;

        CSwordVerseKey* vk_start = dynamic_cast<CSwordVerseKey*>(lowerBound.get());
        Q_ASSERT(vk_start);

        CSwordVerseKey* vk_stop = dynamic_cast<CSwordVerseKey*>(upperBound.get());
        Q_ASSERT(vk_stop);

        bool ok = true;
        while (ok && ((*vk_start < *vk_stop) || (*vk_start == *vk_stop))) {
            //make sure the key given by highlightKey gets marked as current key
            settings.highlight = (!highlightKey.isEmpty() ? (vk_start->key() == highlightKey) : false);

            /*TODO: We need to take care of linked verses if we render one or (esp) more modules
            If the verses 2,3,4,5 are linked to 1, it should be displayed as one entry with the caption 1-5 */

            if (vk_start->Chapter() == 0) { //range was 0:0-1:x, render 0:0 first and jump to 1:0
                vk_start->Verse(0);
                tree.append( new KeyTreeItem(vk_start->key(), modules, settings) );
                vk_start->Chapter(1);
                vk_start->Verse(0);
            }
            /// \bug Valgrind reports memory leak with allocation:
            tree.append( new KeyTreeItem(vk_start->key(), modules, settings) );
            ok = vk_start->next(CSwordVerseKey::UseVerse);
        }

        return renderKeyTree(tree);
    }

    return QString::null;
}

const QString CTextRendering::renderSingleKey( const QString& key, const QList<CSwordModuleInfo*>& moduleList, const KeyTreeItem::Settings& settings ) {
    KeyTree tree;
    tree.append( new KeyTreeItem(key, moduleList, settings) );

    return renderKeyTree(tree);
}


