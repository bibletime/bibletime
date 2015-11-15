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
        CBookTreeChooser(const BtConstModuleList &modules,
                         BTHistory *history, CSwordKey *key = nullptr,
                         QWidget *parent = nullptr);

        /**
          Reimplemented from CKeyChooser::refreshContent().
        */
        virtual void refreshContent() override;

        /**
          Reimplemented from CKeyChooser::setModules().
        */
        virtual void setModules(const BtConstModuleList &modules,
                                bool refresh = true) override;

        /**
          Reimplemented from CKeyChooser::key().
        */
        virtual inline CSwordKey *key() override {
            return m_key;
        }

        /**
          Reimplemented from CKeyChooser::setKey().
        */
        virtual void setKey(CSwordKey *key) override;


        void setKey(CSwordKey*, const bool emitSinal);

    public slots: // Public slots
        virtual void updateKey( CSwordKey* ) override;
        /**
        * Reimplementation to handle tree creation on show.
        */
        virtual void show();


    protected: // Protected methods
        /**
        * Creates the first level of the tree structure.
        */
        void setupTree();
        virtual void adjustFont() override;
        void addKeyChildren(CSwordTreeKey* key, QTreeWidgetItem* item);

    protected slots: // Protected slots
        void itemActivated( QTreeWidgetItem* item );
        void setKey(const QString & newKey) override;

    private:
        QList<const CSwordBookModuleInfo*> m_modules;
        CSwordTreeKey* m_key;
        QTreeWidget* m_treeView;
};

#endif
