/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CBOOKKEYCHOOSER_H
#define CBOOKKEYCHOOSER_H

#include "frontend/keychooser/ckeychooser.h"

#include <QList>
#include "frontend/keychooser/ckeychooserwidget.h"


class CSwordBookModuleInfo;
class CSwordKey;
class CSwordTreeKey;

/**
  \brief The keychooser implementation for books.
*/
class CBookKeyChooser: public CKeyChooser {

    Q_OBJECT

public:

    CBookKeyChooser(const BtConstModuleList & modules,
                    BTHistory * history,
                    CSwordKey * key = nullptr,
                    QWidget * parent = nullptr);

    /**
      Reimplemented from CKeyChooser.
    */
    virtual void refreshContent() override;

    /**
    * Sets another module to this keychooser
    */
    virtual void setModules(const BtConstModuleList & modules,
                            bool refresh = false) override;
    /**
    * Returns the key of this keychooser
    */
    virtual CSwordKey * key() override;

    /**
    * Sets a new key to this keychooser
    */
    virtual void setKey(CSwordKey * key) override;

    /**
    * Sets a new key to this keychooser
    */
    void setKey(CSwordKey * key, const bool emitSignal);

public slots: // Public slots

    /**
    * Updates the keychoosers for the given key but emit no signal.
    */
    void updateKey(CSwordKey * key) override;

protected: /* Methods: */

    /**
    * Fills the combo given by depth with the items from the key having depth "depth".
    * The parent sibling is given by key.
    */
    void setupCombo(const QString & key, const int depth, const int currentItem);

    /** No descriptions */
    virtual void adjustFont() override;

protected slots:

    /**
    * A keychooser changed. Update and emit a signal if necessary.
    */
    void keyChooserChanged(int);

    virtual void setKey(const QString & newKey) override;

private: /* Fields: */

    QList<CKeyChooserWidget *> m_chooserWidgets;
    QList<const CSwordBookModuleInfo *> m_modules;
    CSwordTreeKey * m_key;
    QHBoxLayout * m_layout;

};

#endif
