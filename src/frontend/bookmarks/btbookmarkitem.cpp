/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookmarks/btbookmarkitem.h"

#include <QSharedPointer>
#include "backend/config/btconfig.h"
#include "backend/managers/cswordbackend.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordversekey.h"
#include "btglobal.h"
#include "frontend/bookmarks/btbookmarkfolder.h"
#include "frontend/bookmarks/bteditbookmarkdialog.h"
#include "util/cresmgr.h"
#include "util/directory.h"


BtBookmarkItem::BtBookmarkItem(const CSwordModuleInfo *module,
                               const QString &key,
                               const QString &description,
                               const QString &title)
        : m_description(description),
          m_moduleName(module ? module->name() : QString::null),
          m_title(title)
{
    if (((module && (module->type() == CSwordModuleInfo::Bible)) || (module->type() == CSwordModuleInfo::Commentary))  ) {
        CSwordVerseKey vk(0);
        vk.setKey(key);
        vk.setLocale("en");
        m_key = vk.key(); //the m_key member is always the english key!
    }
    else {
        m_key = key;
    };

    update();
}

BtBookmarkItem::BtBookmarkItem(QTreeWidgetItem* parent)
        : BtBookmarkItemBase(parent) {}

BtBookmarkItem::BtBookmarkItem(const BtBookmarkItem& other)
        : BtBookmarkItemBase(0),
        m_key(other.m_key),
        m_description(other.m_description),
        m_moduleName(other.m_moduleName),
        m_title(other.m_title)
{
    update();
}

CSwordModuleInfo *BtBookmarkItem::module() const {
    return CSwordBackend::instance()->findModuleByName(m_moduleName);
}

QString BtBookmarkItem::key() const {
    const QString englishKeyName = englishKey();
    if (!module()) {
        return englishKeyName;
    }

    QString returnKeyName = englishKeyName;
    if ((module()->type() == CSwordModuleInfo::Bible) || (module()->type() == CSwordModuleInfo::Commentary)) {
        CSwordVerseKey vk(0);
        vk.setKey(englishKeyName);
        vk.setLocale(CSwordBackend::instance()->booknameLanguage().toLatin1() );

        returnKeyName = vk.key(); //the returned key is always in the currently set bookname language
    }

    return returnKeyName;
}

QString BtBookmarkItem::toolTip() const {
    if (!module()) {
        return QString::null;
    }

    FilterOptions filterOptions = getBtConfig().getFilterOptions();
    filterOptions.footnotes = false;
    filterOptions.scriptureReferences = false;
    CSwordBackend::instance()->setFilterOptions(filterOptions);

    QString ret;
    QSharedPointer<CSwordKey> k( CSwordKey::createInstance(module()) );
    k->setKey(key());

    // const CLanguageMgr::Language* lang = module()->language();
    // BtConfig::FontSettingsPair fontPair = getBtConfig().getFontForLanguage(lang);

    Q_ASSERT(k.data());
    QString header = QString::fromLatin1("%1 (%2)")
              .arg(key())
              .arg(module()->name());
    if (title() != header) {
        ret = QString::fromLatin1("<b>%1</b><br>%2<hr>%3")
              .arg(header)
              .arg(title())
              .arg(description())
              ;
    }
    else {
        ret = QString::fromLatin1("<b>%1</b><hr>%2")
              .arg(header)
              .arg(description())
              ;
    }

    return ret;
}

bool BtBookmarkItem::enableAction(MenuAction action) {
    if (action == EditBookmark || (module() && (action == PrintBookmarks)) || action == DeleteEntries)
        return true;

    return false;
}

void BtBookmarkItem::rename() {
    BtEditBookmarkDialog d(QString::fromLatin1("%1 (%2)").arg(key()).arg(module() ? module()->name() : QObject::tr("unknown")),
                           m_title,
                           m_description, treeWidget());

    if (d.exec() == QDialog::Accepted) {
        m_title = d.titleText();
        m_description = d.descriptionText();
        update();
    }
}

void BtBookmarkItem::update() {
    namespace DU = util::directory;

    setIcon(0, DU::getIcon(CResMgr::mainIndex::bookmark::icon));

    if (m_title.isEmpty()) {
      m_title = QString::fromLatin1("%1 (%2)").arg(key()).arg(module() ? module()->name() : QObject::tr("unknown"));
    }
    setText(0,m_title);
    setToolTip(0, toolTip());
}

