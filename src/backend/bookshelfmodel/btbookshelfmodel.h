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

#ifndef BTBOOKSHELFMODEL_H
#define BTBOOKSHELFMODEL_H

#include <QAbstractListModel>

#include "../drivers/btconstmoduleset.h"
#include "../drivers/btmoduleset.h"
#include "../drivers/cswordmoduleinfo.h"


/**
  Implements a simple list model projecting CSwordModuleInfo instances. This model is mostly
  implemented to provide an interface the the underlying data and to provide notifications
  when modules are added, removed or changed. If you want to use a model for widgets, the
  BtBookshelfTreeModel might be a better choice, since it also provides sorting and grouping.
*/
class BtBookshelfModel: public QAbstractListModel {

    Q_OBJECT

public: /* Types: */

    enum ModuleRole {
        ModuleNameRole = Qt::UserRole,
        ModuleIconRole,
        ModulePointerRole,
        ModuleCategoryRole,
        ModuleLanguageRole,
        ModuleHiddenRole,
        ModuleInstallPathRole,
        ModuleHasIndexRole,
        ModuleIndexSizeRole,
        ModuleDescriptionRole,
        UserRole
    };

public: /* Methods: */

    inline BtBookshelfModel(QObject * const parent = nullptr)
        : QAbstractListModel(parent) {}

    // Virtual methods implemented from QAbstractListModel:
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant data(CSwordModuleInfo * module, int role) const;
    QVariant data(const QModelIndex & index, int role) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex & index,
                 const QVariant & value,
                 int role = ModuleHiddenRole) override;

    /**
      Given an index of this model, this method returns a pointer to the underlying
      CSwordModuleInfo instance corresponding to the given index.
      \param[in] index An index to this model.
    */
    inline CSwordModuleInfo * module(const QModelIndex & index) const {
        return static_cast<CSwordModuleInfo *>(
               data(index,
                    BtBookshelfModel::ModulePointerRole).value<void *>());
    }

    /**
      Clears the data of the whole model by removing all items.
      \param[in] destroy If true, all CSwordModuleInfo instances in this model are also
                         destroyed.
    */
    void clear(bool destroy = false);

    /**
      Appends the given module to this model.
      \param[in] module Module to add.
    */
    void addModule(CSwordModuleInfo * const module);

    /**
      Appends the all the modules in the given set to this model.
      \param[in] modules Set of modules to add.
    */
    void addModules(BtModuleSet const & modules);

    /**
      Removes the given module from this model and optionally destroys it.
      \param[in] module The module to remove from this model.
      \param[in] destroy If true, the given CSwordModuleInfo instance is destroyed.
    */
    void removeModule(CSwordModuleInfo * const module,
                      bool destroy = false);

    /**
      Removes all modules from the given set from this model and optionally destroys
      them.
      \param[in] modules The set of modules to remove from this model.
      \param[in] destroy If true, the given CSwordModuleInfo instances are destroyed.
    */
    void removeModules(BtConstModuleSet const & modules,
                       bool destroy = false);

    /**
      Removes all modules from the given list from this model and optionally destroys
      them.
      \param[in] modules The list of modules to remove from this model.
      \param[in] destroy If true, the given CSwordModuleInfo instances are destroyed.
    */
    void removeModules(const QList<CSwordModuleInfo *> & modules,
                       bool destroy = false);

    /**
      Returns the first module found with the given name.
      \param[in] name Name of the module to find.
    */
    CSwordModuleInfo* getModule(const QString & name) const;

    /**
      Returns the list of handled modules as a list of CSwordModuleInfo* pointers.
    */
    inline const QList<CSwordModuleInfo *> & moduleList() const {
        return m_data;
    }

protected slots:

    /**
      Slot DIRECTLY called by CSwordModuleInfo when the hidden status of the respective
      module changes.
      \param[in] hidden True, if the module was hidden; false, if the module was shown.
    */
    void moduleHidden(bool hidden);

    /**
      Slot DIRECTLY called by CSwordModuleInfo when the indexed status of the respective
      module changes.
      \param[in] indexed True, if the module was indexed; false if the index was deleted.
    */
    void moduleIndexed(bool indexed);

    /**
      Slot DIRECTLY called by CSwordModuleInfo when the locked status of the respective
      module changes.
      \param[in] unlocked True, if the module was unlocked; false if the module was
                 locked.
    */
    void moduleUnlocked(bool unlocked);

private: /* Methods: */

    /**
      Called internally when module data changes. This method emits any neccessary
      signals for this model.
      \pre The givem module is handled by this model.
      \param[in] module The module that changed status.
    */
    void moduleDataChanged(CSwordModuleInfo * module);

private: /* Fields: */

    /**
      The underlying data as a list of pointers to the respective CSwordModuleInfo
      instances.
    */
    QList<CSwordModuleInfo *> m_data;

};

#endif // BTBOOKSHELFMODEL_H
