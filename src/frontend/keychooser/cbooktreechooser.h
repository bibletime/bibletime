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

#ifndef CBOOKTREECHOOSER_H
#define CBOOKTREECHOOSER_H

#include "ckeychooser.h"

#include "../../backend/keys/cswordtreekey.h"
#include "ckeychooserwidget.h"


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

        void refreshContent() override;

        void setModules(const BtConstModuleList &modules,
                        bool refresh = true) override;

        inline CSwordKey *key() override {
            return m_key;
        }

        void setKey(CSwordKey *key) override;


        void setKey(CSwordKey*, const bool emitSinal);

    public slots: // Public slots
        void updateKey( CSwordKey* ) override;

        void doShow();

    protected: // Protected methods
        /**
        * Creates the first level of the tree structure.
        */
        void setupTree();
        void adjustFont() override;
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
