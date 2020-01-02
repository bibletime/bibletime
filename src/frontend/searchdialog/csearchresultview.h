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

#ifndef CSEARCHRESULTSVIEW_H
#define CSEARCHRESULTSVIEW_H

#include <QTreeWidget>

// Sword includes
#include <listkey.h>


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
        inline const CSwordModuleInfo *module() const {
            return m_module;
        }

    protected: // Protected methods
        /**
        * Initializes the view of this widget.
        */
        void initView();
        void initConnections();

        //from QTreeWidget
        QMimeData * mimeData ( const QList<QTreeWidgetItem *> items ) const override;
        QStringList mimeTypes () const override;

    public slots: // Public slots
        void saveItems();

        /**
          Setups the list with the given module.
        */
        void setupTree(const CSwordModuleInfo *m, const sword::ListKey &results);

        void setupStrongsTree(CSwordModuleInfo*, const QStringList&);
        void copyItemsWithText();
        void copyItems();
        void saveItemsWithText();

        void contextMenuEvent(QContextMenuEvent* event) override;

    protected slots: // Protected slots
        void printItems();

        /**
        * Is connected to the signal which is emitted when a new item was chosen.
        */
        void executed(QTreeWidgetItem* current, QTreeWidgetItem*);

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

    signals: // Signals
        void keySelected(const QString&);
        void keyDeselected();
};

} //end of namespace Search

#endif

