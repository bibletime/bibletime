/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CTEXTRENDERING_H
#define CTEXTRENDERING_H

#include <QList>
#include <QString>


class CSwordKey;
class CSwordModuleInfo;

/**
 * CTextRendering is BibleTime's place where the actual rendering takes place.
 * It provides several methods to convert an abstract tree of items
 * into a string of html.
 *
 * See the implementations @ref CHTMLExportRendering and especially @ref CDisplayRendering.
 * @short Text rendering based on trees
 * @author The BibleTime team
*/

namespace Rendering {

class CTextRendering {

    public:

        class KeyTreeItem;
        typedef QList<KeyTreeItem*> KeyTree;

        class KeyTreeItem {
            public:

                struct Settings {
                    enum KeyRenderingFace {
                        NoKey, //< means no key shown at all
                        SimpleKey, //< means only versenumber or only lexicon entry name
                        CompleteShort, //< means key like "Gen 1:1"
                        CompleteLong //< means "Genesis 1:1"
                    };

                    Settings(const bool highlight = false, KeyRenderingFace keyRendering = SimpleKey) : highlight(highlight), keyRenderingFace(keyRendering) {}

                    bool highlight;
                    KeyRenderingFace keyRenderingFace;
                };

                KeyTreeItem(const QString &key,
                            const CSwordModuleInfo *module,
                            const Settings &settings);

                KeyTreeItem(const QString &key,
                            const QList<const CSwordModuleInfo*> &modules,
                            const Settings &settings);

                KeyTreeItem(const QString &startKey,
                            const QString &stopKey,
                            const CSwordModuleInfo *module,
                            const Settings &settings);

                KeyTreeItem(const QString &content, const Settings &settings);

                KeyTreeItem(const KeyTreeItem &i);

                virtual inline ~KeyTreeItem() {
                    qDeleteAll(m_childList);
                }

                const QString& getAlternativeContent() const;
                inline void setAlternativeContent(const QString& newContent) {
                    m_alternativeContent = newContent;
                };

                inline bool hasAlternativeContent() const {
                    return !m_alternativeContent.isNull();
                };

                inline const QList<const CSwordModuleInfo*>& modules() const {
                    return m_moduleList;
                };

                inline const QString& key() const {
                    return m_key;
                };

                inline const Settings& settings() const {
                    return m_settings;
                };

                inline KeyTree* childList() const;
//        inline const bool hasChildItems() const;

            protected:
                KeyTreeItem();

                Settings m_settings;
                QList<const CSwordModuleInfo*> m_moduleList;
                QString m_key;
                mutable KeyTree m_childList;

                QString m_stopKey;
                QString m_alternativeContent;
        };

        virtual ~CTextRendering() {}

        const QString renderKeyTree( KeyTree& );

        const QString renderKeyRange(
                const QString &start,
                const QString &stop,
                const QList<const CSwordModuleInfo*> &modules,
                const QString &hightlightKey = QString::null,
                const KeyTreeItem::Settings &settings = KeyTreeItem::Settings());

        const QString renderSingleKey(
                const QString &key,
                const QList<const CSwordModuleInfo*> &modules,
                const KeyTreeItem::Settings &settings = KeyTreeItem::Settings());

    protected:
        const QList<const CSwordModuleInfo*> collectModules(const KeyTree * const tree) const;
        virtual const QString renderEntry( const KeyTreeItem&, CSwordKey* = 0 ) = 0;
        virtual const QString finishText( const QString&, KeyTree& tree ) = 0;
        virtual void initRendering() = 0;
};

inline CTextRendering::KeyTree* CTextRendering::KeyTreeItem::childList() const {
    return &m_childList;
}
//
//inline const bool CTextRendering::KeyTreeItem::hasChildItems() const {
//    return !m_childList.isEmpty();
//}

}

#endif
