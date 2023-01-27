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

#include "../ckeychooser.h"

#include <QList>
#include "../../../backend/drivers/cswordbiblemoduleinfo.h"


class BtBibleKeyWidget;
class CSwordVerseKey;
class CSwordBibleModuleInfo;
class QWidget;

/** \brief A key chooser for bibles and commentaries. */
class CBibleKeyChooser final : public CKeyChooser {

    Q_OBJECT

public: // methods:

    CBibleKeyChooser(BtConstModuleList const & modules,
                     CSwordKey * key = nullptr,
                     QWidget * parent = nullptr);

    CSwordKey * key() final override;

    void setKey(CSwordKey * key) final override;

    void setModules(BtConstModuleList const & modules,
                    bool refresh = true) final override;

    void refreshContent() final override;

public Q_SLOTS:

    void updateKey(CSwordKey * key) final override;

private: // fields:

    BtBibleKeyWidget * m_widget = nullptr;
    QList<CSwordBibleModuleInfo const *> m_modules;
    CSwordVerseKey * m_key;

}; /* class CBibleKeyChooser */
