/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
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

public: /* Methods: */

    CBibleKeyChooser(BtConstModuleList const & modules,
                     BTHistory * history,
                     CSwordKey * key = nullptr,
                     QWidget * parent = nullptr);

public Q_SLOTS:

    CSwordKey * key() final override;

    void setKey(CSwordKey * key) final override;

    void setModules(BtConstModuleList const & modules,
                    bool refresh = true) final override;

    /** \brief used to do actions after key changes. */
    void refChanged(CSwordVerseKey * key);

    void updateKey(CSwordKey * key) final override;
    void refreshContent() final override;

private: /* Methods: */

    void handleHistoryMoved(QString const & newKey) final override;

private: /* Fields: */

    BtBibleKeyWidget * w_ref;
    QList<CSwordBibleModuleInfo const *> m_modules;
    CSwordVerseKey * m_key;

}; /* class CBibleKeyChooser */
