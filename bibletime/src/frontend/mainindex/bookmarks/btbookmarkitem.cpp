/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/


#include "btbookmarkitem.h"
#include "btbookmarkfolder.h"

#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordversekey.h"
#include "frontend/cinputdialog.h"
#include "util/cresmgr.h"
#include "util/directoryutil.h"
#include "backend/config/cbtconfig.h"
#include "util/cpointers.h"

#include <QDebug>

#include <boost/scoped_ptr.hpp>


BtBookmarkItem::BtBookmarkItem(CSwordModuleInfo* module, QString key, QString& description)
        : m_description(description),
        m_moduleName(module ? module->name() : QString::null) {
    if (((module && (module->type() == CSwordModuleInfo::Bible)) || (module->type() == CSwordModuleInfo::Commentary))  ) {
        CSwordVerseKey vk(0);
        vk.key(key);
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
        m_moduleName(other.m_moduleName) {
    update();
}

CSwordModuleInfo* BtBookmarkItem::module() {
    CSwordModuleInfo* const m = CPointers::backend()->findModuleByName(m_moduleName);
    return m;
}

QString BtBookmarkItem::key() {
    const QString englishKeyName = englishKey();
    if (!module()) {
        return englishKeyName;
    }

    QString returnKeyName = englishKeyName;
    if ((module()->type() == CSwordModuleInfo::Bible) || (module()->type() == CSwordModuleInfo::Commentary)) {
        CSwordVerseKey vk(0);
        vk.key(englishKeyName);
        vk.setLocale(CPointers::backend()->booknameLanguage().toLatin1() );

        returnKeyName = vk.key(); //the returned key is always in the currently set bookname language
    }

    return returnKeyName;
}

const QString& BtBookmarkItem::description() {
    return m_description;
}

void BtBookmarkItem::setDescription(QString text) {
    m_description = text;
}

QString BtBookmarkItem::toolTip() {
    if (!module()) {
        return QString::null;
    }

    CSwordBackend::FilterOptions filterOptions = CBTConfig::getFilterOptionDefaults();
    filterOptions.footnotes = false;
    filterOptions.scriptureReferences = false;
    CPointers::backend()->setFilterOptions(filterOptions);

    QString ret;
    boost::scoped_ptr<CSwordKey> k( CSwordKey::createInstance(module()) );
    k->key(this->key());

    const CLanguageMgr::Language* lang = module()->language();
    CBTConfig::FontSettingsPair fontPair = CBTConfig::get
                                           (lang);

    Q_ASSERT(k.get());
    if (fontPair.first) { //use a special font
        ret = QString::fromLatin1("<b>%1 (%2)</b><hr>%3")
              .arg(key())
              .arg(module()->name())
              .arg(description())
              ;
    }
    else {
        ret = QString::fromLatin1("<b>%1 (%2)</b><hr>%3")
              .arg(key())
              .arg(module()->name())
              .arg(description())
              ;
    }

    return ret;
}

bool BtBookmarkItem::enableAction(MenuAction action) {
    if (action == ChangeBookmark || (module() && (action == PrintBookmarks)) || action == DeleteEntries)
        return true;

    return false;
}

void BtBookmarkItem::rename() {
    bool ok  = false;
    const QString newDescription = CInputDialog::getText(QObject::tr("Change description ..."), QObject::tr("Enter a new description for the chosen bookmark."), description(), &ok, treeWidget());

    if (ok) {
        m_description = newDescription;
        update();
    }
}

QString BtBookmarkItem::englishKey() const {
    return m_key;
}

void BtBookmarkItem::update() {
    namespace DU = util::filesystem::directoryutil;

    qDebug() << "BtBookmarkItem::update";
    setIcon(0, DU::getIcon(CResMgr::mainIndex::bookmark::icon));

    const QString title = QString::fromLatin1("%1 (%2)").arg(key()).arg(module() ? module()->name() : QObject::tr("unknown"));
    setText(0, title);
    setToolTip(0, toolTip());
}

