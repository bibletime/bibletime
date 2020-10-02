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

#ifndef BTCONFIGDIALOG_H
#define BTCONFIGDIALOG_H

#include <QDialog>

#include <QIcon>
#include <QListWidgetItem>


class QDialogButtonBox;
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

public: /* Types: */

    /** Base class for configuration dialog pages. */
    class Page : public QWidget {

        friend class BtConfigDialog;

    public: /* Methods: */

        inline Page(QIcon const & icon, QWidget * const parent)
            : QWidget(parent)
            , m_icon(icon)
        {}

        inline void setHeaderText(QString const & headerText) {
            m_headerText = headerText;
            if (m_listWidgetItem)
                m_listWidgetItem->setText(headerText);
        }

    private: /* Methods: */

        void setListWidgetItem(QListWidgetItem * const item) noexcept {
            m_listWidgetItem = item;
            item->setIcon(m_icon);
            item->setText(m_headerText);
        }

    private: /* Fields: */

        QIcon const m_icon;
        QString m_headerText;
        QListWidgetItem * m_listWidgetItem = nullptr;

    };

public: /* Methods: */

    BtConfigDialog(QWidget * const parent = nullptr,
                   Qt::WindowFlags const flags = Qt::WindowFlags());

    /** Adds a BtConfigPage to the paged widget stack. The new page will be the current page.*/
    void addPage(Page * const pageWidget);

    /** Adds a button box to the lower edge of the dialog. */
    void setButtonBox(QDialogButtonBox * const buttonBox);

    /** Changes the current page using the given index number. */
    void setCurrentPage(int const newIndex);

private: /* Fields: */

    QListWidget * const m_contentsList;
    QStackedWidget * const m_pageWidget;
    QVBoxLayout * m_pageLayout;
    int m_maxItemWidth = 0;

};

#endif
