/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "mobile/models/roleitemmodel.h"
#include <QList>
#include <QString>
#include "backend/drivers/cswordmoduleinfo.h"

class CSwordVerseKey;

namespace btm {

class BibleTextModelBuilder {
public:
    BibleTextModelBuilder(RoleItemModel* model);

    void updateModel(
            QList<const CSwordModuleInfo*> modules,
            const QString& keyText);

    void updateKeyRange(
            QList<const CSwordModuleInfo*> modules,
            const QString& startKey,
            const QString& endKey,
            const QString& highlightKey);

    int getCurrentModelIndex() const;

private:
    void addVerseToModel(CSwordVerseKey* key);

    RoleItemModel* m_model;
    int m_currentVerse;
    int m_currentModelIndex;
};

}
