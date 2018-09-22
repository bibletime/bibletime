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

/**
    \brief Model that represents the entire text of a given module

    This model will display the entire text of a module. It is made to be
    used by a QML ListView and to view a small portion of the available text.
    It can be continuously scrolled to any location within the module.

    \note Currently Bible, Commentary, and Book modules are supported.
    \note Personal commentary and up to 4 parallel modules are supported.
 */


class BtModuleTextFilter {

public:
    virtual ~BtModuleTextFilter() {}
    virtual QString processText(const QString& text) = 0;
};

class BtModuleTextModel: public QAbstractListModel {

    Q_OBJECT

public:

    BtModuleTextModel(QObject *parent = nullptr);

    /** Specifies one or more module names for use by the model */
    void setModules(const QStringList& modules);

    virtual bool setData(QModelIndex const & index,
                         QVariant const & value,
                         int role = Qt::EditRole) override;

    bool isBible() const;
    bool isBook() const;
    bool isCommentary() const;
    bool isLexicon() const;

    /** functions to convert from book or verse key to row index and back */
    CSwordTreeKey indexToBookKey(int index) const;
    CSwordVerseKey indexToVerseKey(int index) const;

    int keyToIndex(const CSwordKey* key) const;
    int verseKeyToIndex(const CSwordVerseKey& key) const;
    QString indexToKeyName(int index) const;

    /** Reimplemented from QAbstractItemModel. */
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    void setRoleNames(const QHash<int, QByteArray> &roleNames);
    void setHighlightWords(const QString& highlightWords);

    static void setLinkColor(const QColor& color);
    static void setHighlightColor(const QColor& color);
    static void setJesusWordsColor(const QColor& color);

    FilterOptions getFilterOptions() const;
    void setFilterOptions(FilterOptions filterOptions);

    void clearTextFilter();
    void setTextFilter(BtModuleTextFilter * textFilter);

private:

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
};


#endif
