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

#include <optional>
#include <QAbstractListModel>
#include <QColor>
#include <QStringList>
#include "../btglobal.h"
#include "../drivers/btmodulelist.h"
#include "../keys/cswordversekey.h"
#include "../keys/cswordtreekey.h"
#include "../rendering/cdisplayrendering.h"


class CSwordModuleInfo;

/** For the BtFindWidget buttons (previous, next) */
struct FindState {
    // Model row for item with highlight
    int index;

    // 1st, 2nd, etc. word highlighted within the item
    int subIndex;
};

struct ModuleEntry {
    enum TextRoles {
        ReferenceRole = Qt::UserRole + 1,
        TextRole = Qt::UserRole + 2,

        Text0Role = Qt::UserRole + 10,
        Text1Role = Qt::UserRole + 11,
        Text2Role = Qt::UserRole + 12,
        Text3Role = Qt::UserRole + 13,
        Text4Role = Qt::UserRole + 14,
        Text5Role = Qt::UserRole + 15,
        Text6Role = Qt::UserRole + 16,
        Text7Role = Qt::UserRole + 17,
        Text8Role = Qt::UserRole + 18,
        Text9Role = Qt::UserRole + 19,

        Title0Role = Qt::UserRole + 20,
        Title1Role = Qt::UserRole + 21,
        Title2Role = Qt::UserRole + 22,
        Title3Role = Qt::UserRole + 23,
        Title4Role = Qt::UserRole + 24,
        Title5Role = Qt::UserRole + 25,
        Title6Role = Qt::UserRole + 26,
        Title7Role = Qt::UserRole + 27,
        Title8Role = Qt::UserRole + 28,
        Title9Role = Qt::UserRole + 29
    };
};

class BtModuleTextFilter {

public:
    virtual ~BtModuleTextFilter() = 0;
    virtual QString processText(const QString& text) = 0;
};


/**
  * @page modelviewmodel Details about the module text model
  * <p>
  * The BtModuleTextModel represents the text of a sword module. It uses
  * an integer index to address each entry (verse, paragraph, etc).
  * The html text is obtained using backend rendering functions.
  * It is then post processed to fix a few things so that the QML
  * ListView works correctly. The ListView supports a "rich text"
  * that is a subset of html. The post processing is done in
  * the class BtTextFilter which is installed into BtModuleTextModel
  * by the frontend.
  * </p>
  */


/**
    \brief Model that represents the entire text of a given module

    It is used by a QML ListView to view a small portion of the
    available module text. Only the portions of the module that are
    currently displayed and some just above or below this, are
    instantiated.

    \note Four parallel modules are supported.
 */


class BtModuleTextModel: public QAbstractListModel {

    Q_OBJECT

public:

    BtModuleTextModel(QObject *parent = nullptr);

    /** Convert index(row) into CSwordVerseKey. */
    CSwordVerseKey indexToVerseKey(int index) const;

    /** Convert index(row) into verse. */
    int indexToVerse(int index) const;

    /** Convert index(row) into CSwordVerseKey. */
    CSwordVerseKey indexToVerseKey(int index,
                                   CSwordModuleInfo const & module) const;

    /** Convert index(row) into CSwordVerseKey. */
    CSwordKey* indexToKey(int index, int moduleNum) const;

    /** Convert CSwordKey into index. */
    int keyToIndex(CSwordKey const & key) const;

    /** Convert CSwordVerseKey into index(row). */
    int verseKeyToIndex(const CSwordVerseKey& key) const;

    /** Get reference name from index(row). */
    QString indexToKeyName(int index) const;

    /** Get index offset of first entry of module */
    int firstEntryIndex() const noexcept { return m_firstEntry; }

    /** Reimplemented from QAbstractItemModel. */
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;

    /** Reimplemented from QAbstractItemModel. */
    QVariant data(const QModelIndex & index,
                  int role = Qt::DisplayRole) const override;

    /** Reimplemented from QAbstractItemModel. */
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    /** Reimplemented from QAbstractItemModel. */
    virtual bool setData(const QModelIndex &index,
                         const QVariant &value, int role = Qt::EditRole) override;

    void setDisplayOptions(const DisplayOptions & displayOptions);

    /** Set the state of the currently found word functionality */
    void setFindState(std::optional<FindState> findState);

    /** Set the color of word that are highlighted */
    void setHighlightWords(const QString& highlightWords, bool caseSensitive);

    /** Used by model to get the roleNames and corresponding role numbers. */
    QHash<int, QByteArray> roleNames() const override;

    /** Set the filter options used for rendering module text. */
    void setFilterOptions(FilterOptions filterOptions);

    /** Specifies one or more module names for use by the model */
    void setModules(const QStringList& modules);

    /** Load module pointers from module names */
    void reloadModules();

    /** Set the text options used for rendering module text. */
    void setTextFilter(BtModuleTextFilter * textFilter);

private:

    CSwordTreeKey indexToBookKey(int index) const;

    bool isBible() const;
    bool isBook() const;
    bool isCommentary() const;
    bool isLexicon() const;
    bool isSelected(int index) const;

    /** returns text string for each model index */
    QString bookData(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QString verseData(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QString lexiconData(const QModelIndex & index, int role = Qt::DisplayRole) const;

    BtConstModuleList m_moduleInfoList;
    QStringList m_modules;
    QString m_highlightWords;

    int m_firstEntry;
    int m_maxEntries;
    BtModuleTextFilter * m_textFilter;
    Rendering::CDisplayRendering m_displayRendering;
    std::optional<FindState> m_findState;
};
