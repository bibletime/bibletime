/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTBOOKSHELFDOCKWIDGET_H
#define BTBOOKSHELFDOCKWIDGET_H

#include <QDockWidget>


class CSwordModuleInfo;
class BtBookshelfWidget;
class QAction;
class QActionGroup;
class QMenu;

class BtBookshelfDockWidget: public QDockWidget {
        Q_OBJECT
    public:
        BtBookshelfDockWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);

    signals:
        void moduleOpenTriggered(CSwordModuleInfo *module);
        void moduleSearchTriggered(CSwordModuleInfo *module);
        void moduleEditPlainTriggered(CSwordModuleInfo *module);
        void moduleEditHtmlTriggered(CSwordModuleInfo *module);
        void moduleUnlockTriggered(CSwordModuleInfo *module);
        void moduleAboutTriggered(CSwordModuleInfo *module);

    protected:
        void initMenus();
        void retranslateUi();

    protected slots:
        void slotModuleChecked(CSwordModuleInfo *module, bool checked);
        void slotItemActionTriggered(QAction *action);
        void slotPrepareItemContextMenu();
        void slotEditHiddenModules(bool enable);

    protected:
        BtBookshelfWidget *m_bookshelfWidget;

        // Item context menu:
        QMenu *m_itemContextMenu;
        QActionGroup *m_itemActionGroup;
        QAction *m_itemOpenAction;
        QAction *m_itemSearchAction;
        QMenu   *m_itemEditMenu;
        QAction *m_itemEditPlainAction;
        QAction *m_itemEditHtmlAction;
        QAction *m_itemUnlockAction;
        QAction *m_itemAboutAction;
};

#endif // BTBOOKSHELFDOCKWIDGET_H
