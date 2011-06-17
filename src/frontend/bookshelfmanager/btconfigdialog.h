/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTCONFIGDIALOG_H
#define BTCONFIGDIALOG_H

#include <QDialog>


class BtConfigPage;
class QDialogButtonBox;
class QFrame;
class QListWidget;
class QStackedWidget;
class QVBoxLayout;

/**
* Base class for configuration dialogs. A dialog which has a page chooser (icons
* + text) at the left, widget pages and a buttonbox.
*
* Usage: add BtConfigPage pages with addPage(), add a button box with addButtonBox().
* Connect the button box signals. Use setAttribute(Qt::WA_DeleteOnClose) if you want
* an auto-destroying window.
*/
class BtConfigDialog : public QDialog {

        Q_OBJECT

    public: /* Types: */

        /** Base class for configuration dialog pages. */
        class Page : public QWidget {

            public: /* Methods: */

                inline Page(BtConfigDialog *parent) : QWidget(parent) {}

                virtual const QIcon &icon() const = 0;

                virtual QString header() const = 0;

        };

    public: /* Methods: */

        BtConfigDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);

        /** Adds a BtConfigPage to the paged widget stack. The new page will be the current page.*/
        void addPage(Page *pageWidget);

        /** Adds a button box to the lower edge of the dialog. */
        void setButtonBox(QDialogButtonBox* buttonBox);

        /** Changes the current page using the given index number. */
        void setCurrentPage(int newIndex);

    private slots:

        void slotChangePage(int newIndex);

    private: /* Fields: */

        QListWidget* m_contentsList;
        QStackedWidget* m_pageWidget;
        QVBoxLayout* m_pageLayout;
        QFrame *m_buttonBoxRuler;
        QDialogButtonBox *m_buttonBox;
        int m_maxItemWidth;
        int m_previousPageIndex;

};

#endif
