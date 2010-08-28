/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CLEXICONKEYCHOOSER_H
#define CLEXICONKEYCHOOSER_H

#include "frontend/keychooser/ckeychooser.h"


class CKeyChooserWidget;
class CSwordLDKey;
class CSwordLexiconModuleInfo;
class CSwordModuleInfo;
class QHBoxLayout;
class QWidget;

/**
 * This class implements the KeyChooser for lexicons
 *
 * it inhertits @ref CKeyChooser
 * it uses 1 @ref CKeyChooserWidget to represent the lexicon keys
 *
  * @author The BibleTime team
  */
class CLexiconKeyChooser : public CKeyChooser {
        Q_OBJECT
    public:
        /**
        * The constructor
        *
        * you should not need to use this, use @ref CKeyChooser::createInstance instead
        */
        CLexiconKeyChooser(
            QList<CSwordModuleInfo*> modules, 
            BTHistory* history, 
            CSwordKey *key = 0, 
            QWidget *parent = 0);

    public slots:
        /**
        * see @ref CKeyChooser::getKey
        * @return Return the key object we use.
        */
        virtual CSwordKey* key();
        /**
        * see @ref CKeyChooser::setKey
        */
        virtual void setKey(CSwordKey* key);
        /**
        * used to react to changes in the @ref CKeyChooserWidget
        *
        * @param index not used
        **/
        virtual void activated(int index);
        /**
        * Reimplementation.
        */
        virtual void refreshContent();
        /**
        * Sets the module and refreshes the combo boxes of this keychooser.
        */
        virtual void setModules( const QList<CSwordModuleInfo*>& modules, const bool refresh = true );

    protected:
        CKeyChooserWidget *m_widget;
        CSwordLDKey* m_key;
        QList<CSwordLexiconModuleInfo*> m_modules;
        QHBoxLayout *m_layout;

        virtual void adjustFont();

    public slots: // Public slots
        virtual void updateKey(CSwordKey* key);

    protected slots:
        virtual void setKey(QString& newKey);

};

#endif
