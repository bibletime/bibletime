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


namespace sword { class TreeKeyIdx; }
class BTHistory;
class CSwordBookModuleInfo;
class CSwordKey;
class QTreeWidget;
class QTreeWidgetItem;

class CBookTreeChooser final : public CKeyChooser {

    Q_OBJECT

public: /* Methods: */

    CBookTreeChooser(BtConstModuleList const & modules,
                     BTHistory * history,
                     CSwordKey * key = nullptr,
                     QWidget * parent = nullptr);

    void refreshContent() final override;
    void setModules(BtConstModuleList const & modules,
                    bool refresh = true) final override;

    CSwordKey * key() final override { return m_key; }
    void setKey(CSwordKey * key) final override;
    void setKey(CSwordKey *, const bool emitSinal);

public Q_SLOTS:

    void updateKey(CSwordKey *) final override;
    void doShow();

private: /* Methods: */

    /** \brief Creates the first level of the tree structure. */
    void setupTree();
    void addKeyChildren(CSwordTreeKey * key, QTreeWidgetItem * item);
    void adjustFont();
    void handleHistoryMoved(QString const & newKey) final override;

private Q_SLOTS:

    void itemActivated(QTreeWidgetItem * item);

private: /* Fields: */

    QList<CSwordBookModuleInfo const *> m_modules;
    CSwordTreeKey * m_key;
    QTreeWidget * m_treeView;

}; /* class CBookTreeChooser */

#endif /* CBOOKTREECHOOSER_H */
