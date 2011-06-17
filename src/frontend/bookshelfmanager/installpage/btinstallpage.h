/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTINSTALLPAGE_H
#define BTINSTALLPAGE_H

#include "frontend/bookshelfmanager/btconfigdialog.h"

#include "backend/bookshelfmodel/btbookshelftreemodel.h"


namespace sword {
class InstallSource;
}

class BtInstallPageWorksWidget;
class QComboBox;
class QGroupBox;
class QLabel;
class QPushButton;
class QStackedLayout;
class QToolButton;

/**
* The Install page includes module path chooser, source/module handler and install button.
*/
class BtInstallPage: public BtConfigPage {
        Q_OBJECT

    public:
        BtInstallPage(QWidget *parent = 0);

        /** Reimplemented from BtConfigPage. */
        virtual const QIcon &icon() const;

        /** Reimplemented from BtConfigPage. */
        virtual QString header() const;

        void setInstallEnabled(bool b);

        QString selectedInstallPath();

    public slots:
        void slotSwordSetupChanged();

    private:
        void initView();
        void initConnections();
        void initPathCombo();
        void initSourcesCombo();

        void activateSource(const sword::InstallSource &src);
        void retranslateInstallGroupBox();

    private slots:
        void slotGroupingOrderChanged(const BtBookshelfTreeModel::Grouping &g);
        void slotHeaderChanged();
        void slotInstall();
        void slotPathChanged(const QString& pathText);
        void slotEditPaths();
        void slotSourceAdd();
        void slotSourceDelete();
        void slotSourceIndexChanged(int index);
        void slotSelectedModulesChanged();

    private:
        BtBookshelfTreeModel::Grouping m_groupingOrder;
        QByteArray m_headerState;

        QMap<QString, BtInstallPageWorksWidget*> m_sourceMap;

        QLabel *m_warningLabel;

        QGroupBox *m_sourceGroupBox;
            QComboBox   *m_sourceComboBox;
            QPushButton *m_sourceAddButton;
            QPushButton *m_sourceDeleteButton;

        QGroupBox *m_worksGroupBox;
            QStackedLayout *m_worksLayout;

        QGroupBox *m_installGroupBox;
            QLabel      *m_pathLabel;
            QComboBox   *m_pathCombo;
            QToolButton *m_configurePathButton;
            QPushButton *m_installButton;

        unsigned m_modulesSelected;
        unsigned m_modulesSelectedSources;
};

#endif
