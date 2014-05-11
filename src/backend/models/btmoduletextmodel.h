/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTMODULETEXTMODEL_H
#define BTMODULETEXTMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include "btglobal.h"
#include "backend/keys/cswordversekey.h"
#include "backend/keys/cswordtreekey.h"
#include "backend/keys/cswordldkey.h"

class CSwordModuleInfo;

struct ModuleEntry {
    enum TextRoles {
        ReferenceRole = Qt::UserRole + 1,
        TextRole = Qt::UserRole + 2
    };
};

/**
    \brief Model that represents the entire text of a given module

    This model will display the entire text of a module. It is made to be
    used by a QML ListView and to view a small portion of the available text.
    It can be continuously scrolled to any location within the module.

    \note Currently Bible, Commentary, and Book modules are supported.
    \note Parallel Bible text not yet supported.
 */



class BtModuleTextModel: public QAbstractListModel {

    Q_OBJECT

public:

    BtModuleTextModel(QObject *parent = 0);

    /** Specifies one or more module names for use by the model */
    void setModules(const QStringList& modules);

    bool isBible() const;
    bool isBook() const;
    bool isCommentary() const;
    bool isLexicon() const;

    /** functions to convert from book or verse key to row index and back */
    CSwordTreeKey indexToBookKey(int index) const;
    CSwordVerseKey indexToVerseKey(int index) const;

    int verseKeyToIndex(const CSwordVerseKey& key) const;
    QString indexToKeyName(int index) const;

    /** Reimplemented from QAbstractItemModel. */
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QHash<int, QByteArray> roleNames() const;
    void setRoleNames(const QHash<int, QByteArray> &roleNames);
    void setHighlightWords(const QString& highlightWords);

private:

    /** returns text string for each model index */
    QVariant bookData(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QVariant verseData(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QVariant lexiconData(const QModelIndex & index, int role = Qt::DisplayRole) const;

    QList<const CSwordModuleInfo*> m_moduleInfoList;
    QHash<int, QByteArray> m_roleNames;
    QStringList m_modules;
    QString m_highlightWords;

    int m_firstEntry;
    int m_maxEntries;
    DisplayOptions m_displayOptions;
    FilterOptions m_filterOptions;
};


#endif
