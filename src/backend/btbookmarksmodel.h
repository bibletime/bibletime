/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2026 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include <QAbstractItemModel>
#include <QList>
#include <QString>
#include <QStringList>


class BtBookmarksModelPrivate;
class CSwordModuleInfo;

/**
  Model to load and display bookmarks. It is saved periodically if it was loaded
  from default bookmarks file. No more one such model allowed at time.
*/
class BtBookmarksModel: public QAbstractItemModel {

    Q_OBJECT


public: // methods:

    enum BookmarksRoles {
        TypeRole = Qt::UserRole + 1
    };

    /** A bookmark as synchronization sees it. */
    struct SyncEntry {
        QString id;
        /** Names of the folders it is in, from the top level down. */
        QStringList folder;
        QString module;
        /** The key in English. */
        QString key;
        QString description;
        /** Empty when the bookmark shows its default title. */
        QString title;
        /** Seconds since the epoch. */
        qint64 created = 0;
        qint64 modified = 0;
    };

    BtBookmarksModel(QObject * parent = nullptr);

    /**
      \brief Constructor/destructor for new bookmarks model, data is loaded on first
          constructor call and unloaded on last destructor call.

      \param[in] fileName loads a list of items (with subitem trees) from a named file
          or from the default bookmarks file if empty.
    */
    BtBookmarksModel(QString const & fileName = QString(),
                     QObject * parent = nullptr);
    ~BtBookmarksModel() override;

    /** Reimplemented from QAbstractItemModel */
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;
    bool hasChildren(const QModelIndex & parent = QModelIndex()) const override;
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex & index) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex & index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
    bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex()) override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    /**
      \brief add new item with given parameters
    */
    QModelIndex addBookmark(int const row,
                            QModelIndex const & parent,
                            CSwordModuleInfo const & module,
                            QString const & key,
                            QString const & description = QString(),
                            QString const & title = QString());

    /**
      \brief add new folder.
    */
    QModelIndex addFolder(int row, const QModelIndex & parent, const QString & name = QString());

    /**
      \brief Copies item to target position.

      \param[in] row new item will occupy given row.
      \param[in] parent if invalid new item will be placed on top level.
      \param[in] toCopy item to copy.
      \param[in] keepIds whether the copied bookmarks keep their ids (when the
          originals are removed afterwards, i.e. the items are moved).
    */
    void copyItems(int row,
                   QModelIndex const & parent,
                   QModelIndexList const & toCopy,
                   bool keepIds = false);

    /**
      \returns whether item of index is a folder.
     */
    bool isFolder(const QModelIndex & index) const;

    /**
      \returns whether item of index is a bookmark.
     */
    bool isBookmark(const QModelIndex & index) const;

    /**
      \returns true if the testIndex is baseIndex or a direct or indirect subitem of baseIndex.
    */
    bool hasDescendant(const QModelIndex & baseIndex, const QModelIndex & testIndex) const;

    /**
      \returns description for specified index if have.
     */
    QString description(const QModelIndex & index) const;

    /**
      \brief set descritpion for index.
     */
    void setDescription(const QModelIndex & index, const QString & description);

    /**
      \returns sword module for specified index if have.
     */
    CSwordModuleInfo * module(const QModelIndex & index) const;

    /**
      \returns key for specified index if have.
     */
    QString key(const QModelIndex & index) const;

    /**
      \param[in] parent sort items under specified index, if invalid sort all
                        items.
    */
    void sortItems(QModelIndex const & parent = QModelIndex(),
                   Qt::SortOrder const order = Qt::AscendingOrder);

    /**
     \returns roleNames needed for QML use
    */
    QHash<int, QByteArray> roleNames() const override;

    /** \returns the model of the default bookmarks file, if loaded. */
    static BtBookmarksModel * defaultModel() noexcept;

    /** \returns all bookmarks, folder by folder. */
    QList<SyncEntry> syncEntries() const;

    /**
      \returns the paths of the folders that hold bookmarks or no other
          folders (the others are implied by these paths).
    */
    QList<QStringList> syncFolders() const;

    /** Creates the folders of \a path that are missing. */
    QModelIndex ensureSyncFolder(QStringList const & path);

    /**
      Removes the folder at \a path if it is empty.
      \returns false when the folder still has items.
    */
    bool removeSyncFolderIfEmpty(QStringList const & path);

    /** Adds or updates (and moves) the bookmark with the entry's id. */
    void setSyncEntry(SyncEntry const & entry);

    /** Removes the bookmark with the given id. */
    bool removeSyncEntry(QString const & id);

Q_SIGNALS:

    /** Emitted whenever bookmarks or folders were changed. */
    void bookmarksChanged();

public Q_SLOTS:

    /**
      \brief Save bookmarks or specified branch to file.

      \param[in] fileName use file or save to the default bookmarks file if it is empty,
          file will be overwriten if it exists.
      \param[in] rootItem is used to save specified branch of bookmark items or save all
          bookmarks if it is empty.
      \returns true if success.
    */
    bool save(QString fileName = QString(), const QModelIndex & rootItem = QModelIndex());

    /**
      \brief Import bookmarks from file.

      \param[in] fileName file to load bookmarks.
      \param[in] rootItem bookmarks will be loaded under specified item, if empty, items
          will be loaded on top level. Items will be placed in append mode.
      \returns true if success.
    */
    bool load(QString fileName = QString(), const QModelIndex & rootItem = QModelIndex());

private:

    bool slotSave() { return save(); }
    QModelIndex indexOfItem(void * item) const;

private: // fields:
    Q_DECLARE_PRIVATE(BtBookmarksModel)
    BtBookmarksModelPrivate * const d_ptr;

};
