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

#ifndef BTINSTALLPAGEWORKSWIDGET_H
#define BTINSTALLPAGEWORKSWIDGET_H


#include "frontend/btbookshelfwidget.h"

// Sword includes
#include <installmgr.h>


class BtInstallPage;

class BtInstallPageWorksWidget: public BtBookshelfWidget {
    Q_OBJECT

    public: /* Methods: */

        BtInstallPageWorksWidget(const sword::InstallSource &source,
                                 const BtBookshelfTreeModel::Grouping &g,
                                 BtInstallPage *parent, Qt::WindowFlags f = 0);

        ~BtInstallPageWorksWidget();

        inline const sword::InstallSource &installSource() const {
            return m_source;
        }
        void deleteSource();

    private: /* Methods: */

        void updateTree();

    private slots:

        void slotSourceRefresh();

    private: /* Fields: */

        sword::InstallSource m_source;
        BtInstallPage *m_parent;
        QToolButton *m_sourceRefreshButton;
        CSwordBackend *m_backend;
        BtBookshelfModel *m_myModel;
};

#endif // BTINSTALLPAGEWORKSWIDGET_H
