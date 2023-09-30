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

#include <QHash>
#include <QObject>
#include <QSize>
#include <QString>
#include <QStringList>
#include "../../backend/cswordmodulesearch.h"
#include "btsearchresultarea.h"


class CSwordModuleInfo;
class QAction;
class QContextMenuEvent;
class QMenu;
class QWidget;

namespace Search {

class StrongsResultList;

class CModuleResultView : public QTreeWidget {
        Q_OBJECT
    public:
        CModuleResultView(QWidget* parent);
        ~CModuleResultView() override;

        /**
          Setups the tree using the given list of modules.
        */
        void setupTree(const CSwordModuleSearch::Results &results,
                       const QString &searchedText);

        /**
        * Returns the currently active module.
        */
        CSwordModuleInfo* activeModule();

        QSize sizeHint() const override {
            return m_size;
        }

    protected:
        /**
        * Initializes this widget.
        */
        void initView();
        /**
        * Initializes the connections of this widget
        */
        void initConnections();

    protected Q_SLOTS:
        /**
        * Is executed when an item was selected in the list.
        */
        void executed(QTreeWidgetItem*, QTreeWidgetItem*);
        /**
        * This event handler (reimplemented from QWidget) opens the popup menu at the given position.
        */
        void contextMenuEvent( QContextMenuEvent * event ) override;

    Q_SIGNALS:
        void moduleSelected(CSwordModuleInfo const *,
                            CSwordModuleSearch::ModuleResultList const &);
        void moduleChanged();
        void strongsSelected(CSwordModuleInfo*, const QStringList&);

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

        } m_actions;

        QMenu* m_popup;

        QHash<CSwordModuleInfo const *, CSwordModuleSearch::ModuleResultList>
                m_results;
        QHash<CSwordModuleInfo const *, QList<StrongsResult>> m_strongsResults;
        QSize m_size;
};


} //end of namespace Search
