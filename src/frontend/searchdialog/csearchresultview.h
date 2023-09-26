/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include <QTreeWidget>

#include "../../backend/cswordmodulesearch.h"


class CSwordModuleInfo;
class CReadDisplay;
class QAction;
class QMenu;

namespace Search {

class CSearchResultView  : public QTreeWidget {
        Q_OBJECT
    public:
        CSearchResultView(QWidget* parent);

        /**
          \returns the module which is currently used.
        */
        CSwordModuleInfo const * module() const { return m_module; }

    protected: // methods:
        /**
        * Initializes the view of this widget.
        */
        void initView();
        void initConnections();

        //from QTreeWidget
        QMimeData *
        #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        mimeData(QList<QTreeWidgetItem *> const items) const override;
        #else
        mimeData(QList<QTreeWidgetItem *> const & items) const override;
        #endif
        QStringList mimeTypes () const override;

    public Q_SLOTS:

        /**
          Setups the list with the given module.
        */
        void setupTree(CSwordModuleInfo const * m,
                       CSwordModuleSearch::ModuleResultList const & results);

        void setupStrongsTree(CSwordModuleInfo*, const QStringList&);

        void contextMenuEvent(QContextMenuEvent* event) override;

    private:
        struct {
            QMenu* saveMenu;
            struct {
                QAction* result;
                QAction* resultWithText;
            }
            save;

            QMenu* printMenu;
            struct {
                QAction* result;
            }
            print;

            QMenu* copyMenu;
            struct {
                QAction* result;
                QAction* resultWithText;
            }
            copy;
        }
        m_actions;

        QMenu* m_popup;
        const CSwordModuleInfo *m_module;

    Q_SIGNALS:
        void keySelected(const QString&);
        void keyDeselected();
};

} //end of namespace Search
