/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CBOOKTREECHOOSER_H
#define CBOOKTREECHOOSER_H

#include "frontend/keychooser/ckeychooser.h"

#include "backend/keys/cswordtreekey.h"
#include "frontend/keychooser/ckeychooserwidget.h"


namespace sword {
class TreeKeyIdx;
}
class CSwordBookModuleInfo;
class CSwordKey;
class QTreeWidget;
class QTreeWidgetItem;
class BTHistory;

/** The keychooser implementation for books.
  * @author The BibleTime team
  */
class CBookTreeChooser : public CKeyChooser {
        Q_OBJECT
    public:
        CBookTreeChooser(const QList<const CSwordModuleInfo*> &modules,
                         BTHistory *history, CSwordKey *key = 0,
                         QWidget *parent = 0);

        /**
          Reimplemented from CKeyChooser::refreshContent().
        */
        virtual void refreshContent();

        /**
          Reimplemented from CKeyChooser::setModules().
        */
        virtual void setModules(const QList<const CSwordModuleInfo*> &modules,
                                bool refresh = true);

        /**
          Reimplemented from CKeyChooser::key().
        */
        virtual inline CSwordKey *key() {
            return m_key;
        }

        /**
          Reimplemented from CKeyChooser::setKey().
        */
        virtual void setKey(CSwordKey *key);


        void setKey(CSwordKey*, const bool emitSinal);

    public slots: // Public slots
        virtual void updateKey( CSwordKey* );
        /**
        * Reimplementation to handle tree creation on show.
        */
        virtual void show();


    protected: // Protected methods
        /**
        * Creates the first level of the tree structure.
        */
        void setupTree();
        virtual void adjustFont();
        void addKeyChildren(CSwordTreeKey* key, QTreeWidgetItem* item);

    protected slots: // Protected slots
        void itemActivated( QTreeWidgetItem* item );
        void setKey(QString& newKey);

    private:
        QList<const CSwordBookModuleInfo*> m_modules;
        CSwordTreeKey* m_key;
        QTreeWidget* m_treeView;
};

#endif
