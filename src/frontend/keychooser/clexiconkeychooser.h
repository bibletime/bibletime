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

#include "ckeychooser.h"

#include <QList>
#include <QObject>
#include <QString>
#include "../../backend/drivers/btmodulelist.h"


class CKeyChooserWidget;
class CSwordKey;
class CSwordLDKey;
class CSwordLexiconModuleInfo;
class QHBoxLayout;
class QWidget;

class CLexiconKeyChooser final : public CKeyChooser {

    Q_OBJECT

public: // methods:

    CLexiconKeyChooser(BtConstModuleList const & modules,
                       CSwordKey * key = nullptr,
                       QWidget * parent = nullptr);

public: // methods:

    CSwordKey * key() final override;

    void setKey(CSwordKey * key) final override;

    void refreshContent() final override;

    void setModules(BtConstModuleList const & modules,
                    bool refresh = true) final override;

public Q_SLOTS:

    void updateKey(CSwordKey* key) final override;

private: // fields:

    CKeyChooserWidget * m_widget;
    CSwordLDKey * m_key;
    QList<CSwordLexiconModuleInfo const *> m_modules;

}; /* class CLexiconKeyChooser */
