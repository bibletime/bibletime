/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
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

#include <QIcon>
#include <QListWidgetItem>


class BtConfigPage;
class QDialogButtonBox;
class QFrame;
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

            friend class BtConfigDialog;

            public: /* Methods: */

                inline Page(BtConfigDialog *parent)
                    : QWidget(parent), m_listWidgetItem(0) {}
                inline Page(const QIcon &icon, BtConfigDialog *parent)
                    : QWidget(parent), m_icon(icon), m_listWidgetItem(0) {}

                inline const QIcon &icon() const { return m_icon; }
                inline void setIcon(const QIcon &icon) {
                    m_icon = icon;
                    if (m_listWidgetItem != 0)
                        m_listWidgetItem->setIcon(icon);
                }

                inline const QString &headerText() const { return m_headerText; }
                inline void setHeaderText(const QString &headerText) {
                    m_headerText = headerText;
                    if (m_listWidgetItem != 0)
                        m_listWidgetItem->setText(headerText);
                }

            private: /* Methods: */

                void setListWidgetItem(QListWidgetItem *item) {
                    m_listWidgetItem = item;
                }

            private: /* Fields: */

                QIcon m_icon;
                QString m_headerText;
                QListWidgetItem *m_listWidgetItem;

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
