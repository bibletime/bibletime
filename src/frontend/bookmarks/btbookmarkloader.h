/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTBOOKMARKLOADER_H
#define BTBOOKMARKLOADER_H

#include "util/directory.h"

#include <QDomElement>
#include <QList>
#include <QString>


class QTreeWidgetItem;

/**
* Class for loading and saving bookmarks.
*/
class BtBookmarkLoader {
    public:
        /** Loads a list of items (with subitem trees) from a named file
        * or from the default bookmarks file. */
        QList<QTreeWidgetItem*> loadTree(QString fileName = QString::null);

        /** Takes one item and saves the tree which is under it to a named file
        * or to the default bookmarks file, asking the user about overwriting if necessary. */
        void saveTreeFromRootItem(QTreeWidgetItem* rootItem, QString fileName = QString::null, bool forceOverwrite = true);

    private:
        /** Create a new item from a document element. */
        QTreeWidgetItem* handleXmlElement(QDomElement& element, QTreeWidgetItem* parent);

        /** Writes one item to a document element. */
        void saveItem(QTreeWidgetItem* item, QDomElement& parentElement);

        /** Loads a bookmark XML document from a named file or from the default bookmarks file. */
        QString loadXmlFromFile(QString fileName = QString::null);
};

#endif
