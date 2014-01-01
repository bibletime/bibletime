/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CBIBLEKEYCHOOSER_H
#define CBIBLEKEYCHOOSER_H

#include "frontend/keychooser/ckeychooser.h"

#include <QList>
#include "backend/drivers/cswordbiblemoduleinfo.h"


class QWidget;

class BtBibleKeyWidget;
class CSwordVerseKey;
class CSwordBibleModuleInfo;

/** This class implements the KeyChooser for bibles and commentaries
 *
 * it inhertits @ref CKeyChooser
 *
 * it uses a BtBibleKeyWidget to represent the bible keys
 *
  * @author The BibleTime team
  */

class CBibleKeyChooser : public CKeyChooser  {
        Q_OBJECT

    public:
        CBibleKeyChooser(const QList<const CSwordModuleInfo*> &modules,
                         BTHistory *history, CSwordKey *key = 0,
                         QWidget *parent = 0);

    public slots:
        /**
          Reimplemented from CKeyChooser::key().
        */
        CSwordKey* key();

        /**
          Reimplemented from CKeyChooser::setKey().
        */
        virtual void setKey(CSwordKey *key);

        /**
          Reimplemented from CKeyChooser::setModules().
        */
        virtual void setModules(const QList<const CSwordModuleInfo*> &modules,
                                bool refresh = true);

        /**
        * used to do actions before key changes
        */
        void beforeRefChange(CSwordVerseKey *key);
        /**
        * used to do actions after key changes
        */
        void refChanged(CSwordVerseKey *key);

        void updateKey(CSwordKey* key);
        void adjustFont();
        void refreshContent();

    protected slots:

        virtual void setKey(const QString & newKey);

    private:
        BtBibleKeyWidget* w_ref;
        QList<const CSwordBibleModuleInfo*> m_modules;
        CSwordVerseKey *m_key;
};

#endif
