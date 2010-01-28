/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CBIBLEKEYCHOOSER_H
#define CBIBLEKEYCHOOSER_H

#include "frontend/keychooser/ckeychooser.h"

#include <QList>
#include "backend/drivers/cswordbiblemoduleinfo.h"


class QWidget;

class CKeyReferenceWidget;
class CSwordVerseKey;
class CSwordBibleModuleInfo;

/** This class implements the KeyChooser for bibles and commentaries
 *
 * it inhertits @ref CKeyChooser
 *
 * it uses a CKeyReferenceWidget 's to represent the bible keys
 *
  * @author The BibleTime team
  */

class CBibleKeyChooser : public CKeyChooser  {
        Q_OBJECT

    public:
        /**
        * the constructor
        * you should not need to use this, use @ref CKeyChooser::createInstance instead
        */
        CBibleKeyChooser(QList<CSwordModuleInfo*> modules,
            BTHistory* history,
            CSwordKey *key = 0, 
            QWidget *parent = 0);

    public slots:
        /**
        * see @ref CKeyChooser::getKey
        */
        CSwordKey* key();
        /**
        * see @ref CKeyChooser::setKey
        */
        virtual void setKey(CSwordKey *key);
        /**
        * Sets the module
        */
        virtual void setModules(const QList<CSwordModuleInfo*>& modules, const bool refresh = true);
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
        virtual void setKey(QString& newKey);

    private:
        CKeyReferenceWidget* w_ref;
        QList<CSwordBibleModuleInfo*> m_modules;
        CSwordVerseKey *m_key;
};

#endif
