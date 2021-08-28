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

#include "languageitem.h"

#include <QIcon>
#include "../../util/cresmgr.h"
#include "../language.h"


namespace BookshelfModel {

QVariant LanguageItem::data(int role) const {
    switch (role) {

        case Qt::DisplayRole:
            return m_language->translatedName();

        case Qt::DecorationRole:
            return CResMgr::modules::icon_moduleLanguage();

        default:
            return Item::data(role);

    }
}

} // namespace BookshelfModel
