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

/**
    \brief Searches the keyNames of a BtModuleTextModel to find an index.

    Does a binary search though a BtModuleTextModel with a given possible
    keyName. Finds the index for the keyName or the index just before it
    if there is not an exact match.

    Invokable from QML.
 */


#ifndef SEARCH_MODEL_H
#define SEARCH_MODEL_H

#include <QObject>

class QAbstractItemModel;
class QVariant;

namespace btm {

class SearchModel: public QObject {

    Q_OBJECT

public:
    SearchModel(QObject* parent = nullptr)
        : QObject(parent) { }

    Q_INVOKABLE static int indexMatchingKeyName(const QString& keyName, const QVariant& vModel);
};

}

#endif
