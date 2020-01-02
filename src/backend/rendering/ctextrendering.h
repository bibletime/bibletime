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

#ifndef CTEXTRENDERING_H
#define CTEXTRENDERING_H

#include <QList>
#include <QSharedPointer>
#include <QString>

#include "../drivers/btmodulelist.h"


class CSwordKey;

namespace Rendering {

/**
 * CTextRendering is BibleTime's place where the actual rendering takes place.
 * It provides several methods to convert an abstract tree of items
 * into a string of html.
 *
 * See the implementations @ref CHTMLExportRendering and especially @ref CDisplayRendering.
 * @short Text rendering based on trees
 * @author The BibleTime team
*/
class CTextRendering {

    public: /* Types: */

        class KeyTreeItem;

        class KeyTreeSharedPointer: public QSharedPointer<KeyTreeItem> {
            public:
                inline KeyTreeSharedPointer(KeyTreeItem * i)
                    : QSharedPointer<KeyTreeItem>(i) {}

                inline operator const KeyTreeItem * () const { return data(); }
        };

        using KeyTree = QList<KeyTreeSharedPointer>;

        class KeyTreeItem {

            public: /* Types: */

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

            public: /* Methods: */

                KeyTreeItem(const QString &key,
                            const CSwordModuleInfo *module,
                            const Settings &settings);

                KeyTreeItem(const QString &key,
                            const BtConstModuleList &modules,
                            const Settings &settings);

                KeyTreeItem(const QString &startKey,
                            const QString &stopKey,
                            const CSwordModuleInfo *module,
                            const Settings &settings);

                KeyTreeItem(const QString &content, const Settings &settings);

                KeyTreeItem(const KeyTreeItem &i);

                inline const QString &getAlternativeContent() const {
                    return m_alternativeContent;
                }

                inline void setAlternativeContent(const QString& newContent) {
                    m_alternativeContent = newContent;
                }

                inline bool hasAlternativeContent() const {
                    return !m_alternativeContent.isNull();
                }

                inline const BtConstModuleList& modules() const {
                    return m_moduleList;
                }

                inline const QString& key() const {
                    return m_key;
                }

                inline const Settings& settings() const {
                    return m_settings;
                }

                inline KeyTree* childList() const {
                    return &m_childList;
                }

                inline void setMappedKey(CSwordKey const * key) const {
                    m_mappedKey = key;
                }

                inline CSwordKey const * mappedKey() const {
                    return m_mappedKey;
                }

            protected: /* Methods: */

                KeyTreeItem();

            private: /* Fields: */

                Settings m_settings;
                BtConstModuleList m_moduleList;
                QString m_key;
                mutable CSwordKey const * m_mappedKey = nullptr;
                mutable KeyTree m_childList;

                QString m_stopKey;
                QString m_alternativeContent;

        }; /* class KeyTreeItem */

    public: /* Methods: */

        virtual ~CTextRendering() {}

        const QString renderKeyTree(const KeyTree &tree);

        const QString renderKeyRange(
                const QString &start,
                const QString &stop,
                const BtConstModuleList &modules,
                const QString &hightlightKey = QString(),
                const KeyTreeItem::Settings &settings = KeyTreeItem::Settings());

        const QString renderSingleKey(
                const QString &key,
                const BtConstModuleList &modules,
                const KeyTreeItem::Settings &settings = KeyTreeItem::Settings());

    protected: /* Methods: */

        BtConstModuleList collectModules(const KeyTree &tree) const;
        virtual QString renderEntry(const KeyTreeItem &item, CSwordKey * key = nullptr) = 0;
        virtual QString finishText(const QString &text, const KeyTree &tree) = 0;
        virtual void initRendering() = 0;

}; /* class CTextRendering */

} /* namespace Rendering */

#endif
