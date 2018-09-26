/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTMODULETEXTMODEL_H
#define BTMODULETEXTMODEL_H

#include <QAbstractListModel>
#include <QColor>
#include <QStringList>
#include "../btglobal.h"
#include "../drivers/btmodulelist.h"
#include "../keys/cswordversekey.h"
#include "../keys/cswordtreekey.h"
#include "../keys/cswordldkey.h"


/** For the BtFindWidget buttons (previous, next) */
struct FindState {

    // Prev/Next word highlighting enabled
    bool enabled;

    // Model row for item with highlight
    int index;

    // 1st, 2nd, etc. word highlighted within the item
    int subIndex;
};

struct ModuleEntry {
    enum TextRoles {
        ReferenceRole = Qt::UserRole + 1,
        TextRole = Qt::UserRole + 2,
        Text1Role = Qt::UserRole + 3,
        Text2Role = Qt::UserRole + 4,
        Text3Role = Qt::UserRole + 5,
        Text4Role = Qt::UserRole + 6
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

    /** Get color of Jesus's words */
    static QColor getJesusWordsColor();

    /** Convert index(row) into CSwordVerseKey. */
    CSwordVerseKey indexToVerseKey(int index) const;

    /** Convert CSwordKey into index(row). */
    int keyToIndex(const CSwordKey* key) const;

    /** Convert CSwordVerseKey into index(row). */
    int verseKeyToIndex(const CSwordVerseKey& key) const;

    /** Get reference name from index(row). */
    QString indexToKeyName(int index) const;

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

    /** Set the state of the currently found word functionality */
    void setFindState(const FindState& findState);

    /** Set the color of word that are highlighted */
    void setHighlightWords(const QString& highlightWords, bool caseSensitive);

    /** Used by model to get the roleNames and corresponding role numbers. */
    QHash<int, QByteArray> roleNames() const override;

    /** Sets names that correspond to role numbers. Qml
     * ListView uses names instead of roles to get data. */
    void setRoleNames(const QHash<int, QByteArray> &roleNames);


    /** Set the color used for Jesus words. (Used by BT Mobile) */
    static void setJesusWordsColor(const QColor& color);

    /** Set the color used for highlighting words found by searching. (Used by BT Mobile) */
    static void setHighlightColor(const QColor& color);

    /** Set the color used for Bible references. (Used by BT Mobile) */
    static void setLinkColor(const QColor& color);

    /** Set the filter options used for rendering module text. */
    void setFilterOptions(FilterOptions filterOptions);

    /** Specifies one or more module names for use by the model */
    void setModules(const QStringList& modules);

    /** Specifies one or more modules for use by the model */
    void setModules(const BtConstModuleList &modules);

    /** Set the text options used for rendering module text. */
    void setTextFilter(BtModuleTextFilter * textFilter);

private:

    CSwordTreeKey indexToBookKey(int index) const;

    QString highlightFindPreviousNextField(const QString& text) const;
    bool isBible() const;
    bool isBook() const;
    bool isCommentary() const;
    bool isLexicon() const;

    /** returns text string for each model index */
    QString bookData(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QString verseData(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QString lexiconData(const QModelIndex & index, int role = Qt::DisplayRole) const;

    QString replaceColors(const QString& text) const;

    BtConstModuleList m_moduleInfoList;
    QHash<int, QByteArray> m_roleNames;
    QStringList m_modules;
    QString m_highlightWords;

    int m_firstEntry;
    int m_maxEntries;
    BtModuleTextFilter * m_textFilter;
    DisplayOptions m_displayOptions;
    FilterOptions m_filterOptions;
    FindState m_findState;
};


#endif
