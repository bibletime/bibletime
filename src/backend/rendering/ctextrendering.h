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

#pragma once

#include <list>
#include <QString>
#include "../btglobal.h"
#include "../config/btconfig.h"
#include "../drivers/btmodulelist.h"
#include "../keys/cswordkey.h"


class CSwordKey;
class CSwordVerseKey;

namespace Rendering {

/**
 * CTextRendering is BibleTime's place where the actual rendering takes place.
 * It provides several methods to convert an abstract tree of items
 * into a string of html.
 *
 * @short Text rendering based on trees
 * @author The BibleTime team
*/
class CTextRendering {

    public: // types:

        class KeyTreeItem;

        using KeyTree = std::list<KeyTreeItem>;

        class KeyTreeItem {

            public: // types:

                struct Settings {

                    enum KeyRenderingFace {
                        NoKey,         ///< means no key shown at all
                        SimpleKey,     ///< means only versenumber or only lexicon entry name
                        CompleteShort, ///< means key like "Gen 1:1"
                        CompleteLong,  ///< means "Genesis 1:1"
                        ExpandedShort, /// means "KJV:Gen 1:1"
                        ExpandedLong   /// means "Genesis 1:1 (KJV)"
                    };

                    bool highlight;
                    KeyRenderingFace keyRenderingFace;

                }; /* struct Settings */

            public: // methods:

                KeyTreeItem(KeyTreeItem &&) = delete;
                KeyTreeItem(KeyTreeItem const &&) = delete;

                KeyTreeItem(const QString &key,
                            const CSwordModuleInfo *module,
                            const Settings &settings);

                KeyTreeItem(const QString &key,
                            const BtConstModuleList &modules,
                            const Settings &settings);

                KeyTreeItem(const CSwordKey * key,
                            const BtConstModuleList & modules,
                            const Settings & settings);

                KeyTreeItem(const QString &startKey,
                            const QString &stopKey,
                            const CSwordModuleInfo *module,
                            const Settings &settings);

                KeyTreeItem(const QString &content, const Settings &settings);

                KeyTreeItem & operator=(KeyTreeItem &&) = delete;
                KeyTreeItem & operator=(KeyTreeItem const &&) = delete;

                QString const & getAlternativeContent() const
                { return m_alternativeContent; }

                bool hasAlternativeContent() const
                { return !m_alternativeContent.isNull(); }

                BtConstModuleList const & modules() const
                { return m_moduleList; }

                QString const & key() const { return m_key; }
                
                inline const CSwordKey * swordKey() const {
                    return m_swordKey.get();
                }

                Settings const & settings() const { return m_settings; }

                KeyTree & childList() const noexcept { return m_childList; }

            private: // fields:

                Settings m_settings;
                BtConstModuleList m_moduleList;
                QString m_key; // TODO optimize
                std::unique_ptr<CSwordKey> m_swordKey = nullptr;

                mutable KeyTree m_childList;

                QString m_stopKey;
                QString m_alternativeContent;

        }; /* class KeyTreeItem */

    public: // methods:

        CTextRendering(
            bool addText,
            DisplayOptions const & displayOptions =
                    btConfig().getDisplayOptions(),
            FilterOptions const & filterOptions =
                    btConfig().getFilterOptions());

        virtual ~CTextRendering() {}

        const QString renderKeyTree(const KeyTree &tree);

        const QString renderKeyRange(
                CSwordVerseKey const & lowerBound,
                CSwordVerseKey const & upperBound,
                const BtConstModuleList &modules,
                const QString &hightlightKey = QString(),
                const KeyTreeItem::Settings &settings = KeyTreeItem::Settings());

        const QString renderSingleKey(
                const QString &key,
                const BtConstModuleList &modules,
                const KeyTreeItem::Settings &settings = KeyTreeItem::Settings());

    protected: // methods:

        BtConstModuleList collectModules(const KeyTree &tree) const;
        virtual QString renderEntry(const KeyTreeItem &item);
        virtual QString finishText(const QString &text, const KeyTree &tree);

        /** \returns link for given key in context of render item */
        virtual QString entryLink(KeyTreeItem const & item,
                                  CSwordKey const * key);

    protected: // fields:

        DisplayOptions const m_displayOptions;
        FilterOptions const m_filterOptions;
        bool const m_addText;

}; /* class CTextRendering */

} /* namespace Rendering */
