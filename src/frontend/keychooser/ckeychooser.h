/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CKEYCHOOSER_H
#define CKEYCHOOSER_H

#include <QWidget>


class BTHistory;
class CSwordKey;
class CSwordModuleInfo;
class QAction;

/**
 * The base class for the KeyChooser.
 * Do not use directly, create a KeyChooser with
 * @ref #createInstance , this will create the proper one
 * of the classes that inherit from @ref CKeyChooser
 *
  * @author The BibleTime team
  */
class CKeyChooser : public QWidget {
        Q_OBJECT
    public:

        /**
        * Creates a proper Instance, either
        * @ref CLexiconKeyChooser or
        * @ref CBibleKeyChooser
        * @param info the @ref CModuleInfo to be represented by the KeyChooser
        * @param key if not NULL, the @ref CKey the KeyChooser should be set to
        * @param parent the parent of the widget to create
        */
        static CKeyChooser *createInstance(
            const QList<const CSwordModuleInfo*> &modules,
            BTHistory *history, CSwordKey *key, QWidget *parent);


    public slots:
        /**
          Sets the CKey
          \param key the key which the widget should be set to.
        */
        virtual void setKey(CSwordKey* key) = 0;

        /**
          Updates the CKey.
          \param key the key which the widget should be set to.
        */
        virtual void updateKey(CSwordKey *key) = 0;

        /**
          \returns the current CKey.
        */
        virtual CSwordKey *key() = 0;

        /**
          Sets the module of this keychooser and refreshes the comboboxes
        */
        virtual void setModules(const QList<const CSwordModuleInfo*> &modules,
                                bool refresh = true) = 0;

        /**
          Refreshes the content of the different key chooser parts.
        */
        virtual void refreshContent() = 0;

        /**
          \returns the history object of this keychooser.
        */
        inline BTHistory *history() const { return m_history; }

    signals:

        /**
        * is emitted if the @ref CKey was changed by the user
        */
        void keyChanged(CSwordKey* newKey);
        /**
        * Is emitted before the key is changed!
        */
        void beforeKeyChange(const QString& key);

    protected:

        CKeyChooser(const QList<const CSwordModuleInfo*> &info,
                    BTHistory *history, CSwordKey *key = 0,
                    QWidget *parent = 0);

        virtual inline ~CKeyChooser() {}

        /**
          Resets the appropriate font to for the modules.
        */
        virtual void adjustFont() = 0;

    protected slots:
        virtual void setKey(QString &newKey) = 0;

    private:
        BTHistory *m_history;

};

#endif
