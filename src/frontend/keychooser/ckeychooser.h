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

#include <QWidget>

#include "../../backend/drivers/btmodulelist.h"


class CSwordKey;
class QAction;

/**
 * The base class for the KeyChooser.
 * Do not use directly, create a KeyChooser with
 * @ref #createInstance , this will create the proper one
 * of the classes that inherit from @ref CKeyChooser
 */
class CKeyChooser: public QWidget {

    Q_OBJECT

public: // methods:

    /**
    * Creates a proper Instance, either
    * @ref CLexiconKeyChooser or
    * @ref CBibleKeyChooser
    * @param info the @ref CModuleInfo to be represented by the KeyChooser
    * @param key if not NULL, the @ref CKey the KeyChooser should be set to
    * @param parent the parent of the widget to create
    */
    static CKeyChooser * createInstance(const BtConstModuleList & modules,
                                        CSwordKey * key,
                                        QWidget * parent);

    /**
      Sets the CKey
      \param key the key which the widget should be set to.
    */
    virtual void setKey(CSwordKey * key) = 0;

    /**
      \returns the current CKey.
    */
    virtual CSwordKey * key() = 0;

    /**
      Sets the module of this keychooser and refreshes the comboboxes
    */
    virtual void setModules(const BtConstModuleList & modules,
                            bool refresh = true) = 0;

    /**
      Refreshes the content of the different key chooser parts.
    */
    virtual void refreshContent() = 0;

public Q_SLOTS:

    /**
      Updates the CKey.
      \param key the key which the widget should be set to.
    */
    virtual void updateKey(CSwordKey * key) = 0;

    void handleHistoryMoved(QString const & newKey);

Q_SIGNALS:

    /**
    * is emitted if the @ref CKey was changed by the user
    */
    void keyChanged(CSwordKey * newKey);

protected: // methods:

    CKeyChooser(QWidget * parent = nullptr);

};
