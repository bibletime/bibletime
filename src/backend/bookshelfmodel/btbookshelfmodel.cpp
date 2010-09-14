/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "backend/bookshelfmodel/btbookshelfmodel.h"

#include <QSet>
#include "util/cresmgr.h"
#include "util/directoryutil.h"

BtBookshelfModel::BtBookshelfModel(QObject *parent)
        : QAbstractListModel(parent) {
    // Intentionally empty
}

BtBookshelfModel::~BtBookshelfModel() {
    // Intentionally empty
}

int BtBookshelfModel::rowCount(const QModelIndex &parent) const {
    return m_data.size();
}

QVariant BtBookshelfModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.column() != 0 || index.parent().isValid()) {
        return QVariant();
    }
    int row(index.row());
    if (row >= m_data.size()) return QVariant();

    switch (role) {
        case ModuleNameRole: // Qt::DisplayRole
            return m_data.at(row)->name();
        case ModuleIconRole: // Qt::DecorationRole
            return moduleIcon(m_data.at(row));
        case ModulePointerRole:
            return qVariantFromValue((void*) m_data.at(row));
        default:
            return QVariant();
    }
}

QVariant BtBookshelfModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal &&
            section == 0) {
        return tr("Module");
    }

    return QVariant();
}

QIcon BtBookshelfModel::moduleIcon(const CSwordModuleInfo *m) {
    typedef util::filesystem::DirectoryUtil DU;

    /// \todo Make CSwordModuleInfo::isLocked() const and remove const_cast:
    CSwordModuleInfo *module(const_cast<CSwordModuleInfo*>(m));

    CSwordModuleInfo::Category cat(module->category());
    switch (cat) {
        case CSwordModuleInfo::Bibles:
            if (module->isLocked()) {
                return DU::getIcon(CResMgr::modules::bible::icon_locked);
            } else {
                return DU::getIcon(CResMgr::modules::bible::icon_unlocked);
            }
        case CSwordModuleInfo::Commentaries:
            if (module->isLocked()) {
                return DU::getIcon(CResMgr::modules::commentary::icon_locked);
            } else {
                return DU::getIcon(CResMgr::modules::commentary::icon_unlocked);
            }
        case CSwordModuleInfo::Lexicons:
            if (module->isLocked()) {
                return DU::getIcon(CResMgr::modules::lexicon::icon_locked);
            } else {
                return DU::getIcon(CResMgr::modules::lexicon::icon_unlocked);
            }
        case CSwordModuleInfo::Books:
            if (module->isLocked()) {
                return DU::getIcon(CResMgr::modules::book::icon_locked);
            } else {
                return DU::getIcon(CResMgr::modules::book::icon_unlocked);
            }
        case CSwordModuleInfo::Cult:
        case CSwordModuleInfo::Images:
        case CSwordModuleInfo::DailyDevotional:
        case CSwordModuleInfo::Glossary:
        case CSwordModuleInfo::UnknownCategory:
        default:
            return categoryIcon(cat);
    }
}

QIcon BtBookshelfModel::categoryIcon(const CSwordModuleInfo::Category &category) {
    typedef util::filesystem::DirectoryUtil DU;

    switch (category) {
        case CSwordModuleInfo::Bibles:
            return DU::getIcon(CResMgr::categories::bibles::icon);
        case CSwordModuleInfo::Commentaries:
            return DU::getIcon(CResMgr::categories::commentaries::icon);
        case CSwordModuleInfo::Books:
            return DU::getIcon(CResMgr::categories::books::icon);
        case CSwordModuleInfo::Cult:
            return DU::getIcon(CResMgr::categories::cults::icon);
        case CSwordModuleInfo::Images:
            return DU::getIcon(CResMgr::categories::images::icon);
        case CSwordModuleInfo::DailyDevotional:
            return DU::getIcon(CResMgr::categories::dailydevotional::icon);
        case CSwordModuleInfo::Lexicons:
            return DU::getIcon(CResMgr::categories::lexicons::icon);
        case CSwordModuleInfo::Glossary:
            return DU::getIcon(CResMgr::categories::glossary::icon);
        case CSwordModuleInfo::UnknownCategory:
        default:
            return QIcon();
    }
}

QString BtBookshelfModel::categoryName(
    const CSwordModuleInfo::Category &category) {
    switch (category) {
        case CSwordModuleInfo::Bibles:
            return tr("Bibles");
        case CSwordModuleInfo::Commentaries:
            return tr("Commentaries");
        case CSwordModuleInfo::Books:
            return tr("Books");
        case CSwordModuleInfo::Cult:
            return tr("Cults/Unorthodox");
        case CSwordModuleInfo::Images:
            return tr("Maps and Images");
        case CSwordModuleInfo::DailyDevotional:
            return tr("Daily Devotionals");
        case CSwordModuleInfo::Lexicons:
            return  tr("Lexicons and Dictionaries");
        case CSwordModuleInfo::Glossary:
            return tr("Glossaries");
        default:
            return tr("Unknown");
    }
}

QString BtBookshelfModel::languageName(
    const CLanguageMgr::Language *language) {
    return language->translatedName();
}

void BtBookshelfModel::clear(bool destroy) {
    if (m_data.size() <= 0) return;

    beginRemoveRows(QModelIndex(), 0, m_data.size() - 1);
    if (destroy) {
        qDeleteAll(m_data);
    }
    m_data.clear();
    endRemoveRows();
}

void BtBookshelfModel::addModule(CSwordModuleInfo * const module) {
    Q_ASSERT(module != 0);

    if (m_data.contains(module)) return;

    const int index(m_data.size());
    beginInsertRows(QModelIndex(), index, index);
    m_data.append(module);
    endInsertRows();
}

void BtBookshelfModel::addModules(const QList<CSwordModuleInfo *> &modules) {
    addModules(modules.toSet());
}

void BtBookshelfModel::addModules(const QSet<CSwordModuleInfo *> &modules) {
    QList<CSwordModuleInfo *> newModules;
    Q_FOREACH(CSwordModuleInfo *module, modules) {
        if (!m_data.contains(module)) {
            newModules.append(module);
        }
    }

    if (newModules.isEmpty()) return;

    beginInsertRows(QModelIndex(), m_data.size(),
                    m_data.size() + newModules.size() - 1);
#if QT_VERSION >= 0x040500
    m_data.append(newModules);
#else
    Q_FOREACH(CSwordModuleInfo *module, newModules) {
        m_data.append(module);
    }
#endif
    endInsertRows();
}

void BtBookshelfModel::removeModule(CSwordModuleInfo * const module,
                                    bool destroy)
{
    const int index(m_data.indexOf(module));
    if (index == -1) return;

    beginRemoveRows(QModelIndex(), index, index);
    m_data.removeAt(index);
    endRemoveRows();
    if (destroy) delete module;
}

void BtBookshelfModel::removeModules(const QList<CSwordModuleInfo *> &modules,
                                     bool destroy)
{
    removeModules(modules.toSet(), destroy);
}

void BtBookshelfModel::removeModules(const QSet<CSwordModuleInfo *> &modules,
                                     bool destroy)
{
    // This is inefficient, since signals are emitted for each removed module:
    Q_FOREACH(CSwordModuleInfo *module, modules) {
        removeModule(module, destroy);
    }
}

CSwordModuleInfo* BtBookshelfModel::getModule(const QString &name) const {
    Q_FOREACH(CSwordModuleInfo *module, m_data) {
        if (module->name() == name) return module;
    }
    return 0;
}
