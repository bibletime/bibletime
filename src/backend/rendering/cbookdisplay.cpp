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

#include "cbookdisplay.h"

#include <memory>
#include <QtAlgorithms>
#include "../../util/btassert.h"
#include "../drivers/cswordbookmoduleinfo.h"
#include "../keys/cswordtreekey.h"
#include "cdisplayrendering.h"


const QString Rendering::CBookDisplay::text(
        const BtConstModuleList &modules,
        const QString &keyName,
        const DisplayOptions &displayOptions,
        const FilterOptions &filterOptions)
{
    using CSBMI = CSwordBookModuleInfo;

    const CSBMI* book = dynamic_cast<const CSBMI*>(modules.first());
    BT_ASSERT(book);

    DisplayOptions dOpts = displayOptions;
    dOpts.lineBreaks = true; //books should render with blocks, not with inlined sections

    CDisplayRendering render(dOpts, filterOptions);
    CDisplayRendering::KeyTree tree;
    CDisplayRendering::KeyTreeItem::Settings itemSettings;

    // the number of levels which should be display together, 1 means display no entries together
    int displayLevel = book->config( CSwordModuleInfo::DisplayLevel ).toInt();

    std::unique_ptr<CSwordTreeKey> key(
            dynamic_cast<CSwordTreeKey *>(CSwordKey::createInstance(book)));
    key->setKey(keyName); //set the key to position we'd like to get

    const unsigned long offset = key->getOffset();

    // standard of DisplayLevel, display nothing together
    // if the current key is the root entry don't display anything together!

    if ((displayLevel <= 1) || (key->key().isEmpty() || (key->key() == "/") )) {
        tree.append( new CDisplayRendering::KeyTreeItem( key->key(), modules, itemSettings ) );

        const QString renderedText = render.renderKeyTree(tree);
        key->setOffset( offset );
        return renderedText;
    };

    /**
    * Check whether displaying displayLevel levels together is possible.
    * For this count the childs and parents
    * of the required position
    */

    int possibleLevels = 1; //we start with the default value of displayLevel, which means no entries together

    while ( key->sword::TreeKeyIdx::parent() && (key->key() != "/") && !key->key().isEmpty() ) {//add parents
        ++possibleLevels;
    };

    //   key->key(keyName); //set the key to the start position

    key->setOffset( offset );

    while ( key->firstChild( )) { //add childs
        ++possibleLevels;
    };

    if (possibleLevels < displayLevel) { //too few levels available!
        //display current level, we could also decide to display the available levels together
        tree.append( new CDisplayRendering::KeyTreeItem( key->key(), modules, itemSettings ) );

        const QString renderedText = render.renderKeyTree(tree);
        key->setOffset( offset );
        return renderedText;
    };

    if ((displayLevel > 2) && (displayLevel == possibleLevels)) { //fix not to diplay the whole module
        --displayLevel;
    }

    // at this point we're sure that we can display the required levels toogether
    // at the moment we're at the lowest level, so we only have to go up!
    for (int currentLevel = 1; currentLevel < displayLevel; ++currentLevel) { //we start again with 1 == standard of displayLevel

        if ( !key->sword::TreeKeyIdx::parent() ) { //something went wrong although we checked before! Be safe and return entry's text
            tree.append( new CDisplayRendering::KeyTreeItem( key->key(), modules, itemSettings ) );

            const QString renderedText = render.renderKeyTree(tree);
            key->setOffset( offset );
            return renderedText;
        };
    };

    // no we can display all sub levels together! We checked before that this is possible!
    itemSettings.highlight = (key->key() == keyName);

    tree.append( new CDisplayRendering::KeyTreeItem( key->key(), modules, itemSettings ) );

    //const bool hasToplevelText = !key->strippedText().isEmpty();
    key->firstChild(); //go to the first sibling on the same level

    setupRenderTree(key.get(), &tree, keyName);

    const QString renderedText = render.renderKeyTree(tree);

    key->setOffset( offset ); //restore key
    return renderedText;
}

void Rendering::CBookDisplay::setupRenderTree(CSwordTreeKey * swordTree, CTextRendering::KeyTree * renderTree, const QString& highlightKey) {

    const QString key = swordTree->key();
    const unsigned long offset = swordTree->getOffset();

    CTextRendering::KeyTreeItem::Settings settings;
    settings.highlight = (key == highlightKey);

    /// \todo Check whether this is correct:
    CTextRendering::KeyTreeItem *item = new CTextRendering::KeyTreeItem(
            key, swordTree->module(), settings);
    renderTree->append( item );

    if (swordTree->hasChildren()) { //print tree for the child items
        swordTree->firstChild();
        setupRenderTree(swordTree, item->childList(), highlightKey);
        swordTree->setOffset( offset ); //go back where we came from
    }

    if (swordTree->nextSibling()) { //print tree for next entry on the same depth
        setupRenderTree(swordTree, renderTree, highlightKey);
        swordTree->setOffset( offset ); //return to the value we had at the beginning of this block!
    }
}
