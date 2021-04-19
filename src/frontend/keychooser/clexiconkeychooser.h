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


class CKeyChooserWidget;
class CSwordLDKey;
class CSwordLexiconModuleInfo;
class CSwordModuleInfo;
class QHBoxLayout;
class QWidget;

class CLexiconKeyChooser final : public CKeyChooser {

    Q_OBJECT

public: /* Methods: */

    CLexiconKeyChooser(BtConstModuleList const & modules,
                       BTHistory * history,
                       CSwordKey * key = nullptr,
                       QWidget * parent = nullptr);

public: /* Methods: */

    CSwordKey * key() final override;

    void setKey(CSwordKey * key) final override;

    void refreshContent() final override;

    void setModules(BtConstModuleList const & modules,
                    bool refresh = true) final override;

public Q_SLOTS:

    void updateKey(CSwordKey* key) final override;

private: /* Methods: */

    void handleHistoryMoved(QString const & newKey) final override;

private: /* Fields: */

    CKeyChooserWidget * m_widget;
    CSwordLDKey * m_key;
    QList<CSwordLexiconModuleInfo const *> m_modules;
    QHBoxLayout * m_layout;

}; /* class CLexiconKeyChooser */
