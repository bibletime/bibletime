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

#include <QObject>
#include <QString>
#include <QList>
#include "../../backend/drivers/btmodulelist.h"


class CKeyChooserWidget;
class CSwordBookModuleInfo;
class CSwordKey;
class CSwordTreeKey;
class QHBoxLayout;
class QWidget;

class CBookKeyChooser final : public CKeyChooser {

    Q_OBJECT

public:

    CBookKeyChooser(BtConstModuleList const & modules,
                    CSwordKey * key = nullptr,
                    QWidget * parent = nullptr);

    void refreshContent() final override;

    void setModules(BtConstModuleList const & modules,
                    bool refresh = false) final override;
    CSwordKey * key() final override;

    void setKey(CSwordKey * key) final override;

    /** \brief Sets a new key to this keychooser. */
    void setKey(CSwordKey * key, bool const emitSignal);

public Q_SLOTS:

    /** \brief Updates the keychoosers for the given key but emit no signal. */
    void updateKey(CSwordKey * key) final override;

protected: // methods:

    /**
       Fills the combo given by depth with the items from the key having depth
       "depth". The parent sibling is given by key.
    */
    void setupCombo(QString const & key,
                    int const depth,
                    int const currentItem);

private: // methods:

    void adjustFont();

private Q_SLOTS:

    /** \brief A keychooser changed. Update and emit a signal if necessary. */
    void keyChooserChanged(int);

private: // fields:

    QList<CKeyChooserWidget *> m_chooserWidgets;
    QList<CSwordBookModuleInfo const *> m_modules;
    CSwordTreeKey * m_key;
    QHBoxLayout * m_layout;

}; /* class CBookKeyChooser */
