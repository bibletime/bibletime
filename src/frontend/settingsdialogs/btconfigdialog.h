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

#pragma once

#include <QDialog>


class QListWidget;
class QListWidgetItem;
class QStackedWidget;

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

        Page(QIcon const & icon, QWidget * const parent = nullptr);
        ~Page() noexcept;

        void setHeaderText(QString const & headerText);

        virtual void save() const = 0;

    private: /* Fields: */

        QListWidgetItem * const m_listWidgetItem;
        bool m_ownsListWidgetItem = true;

    };

public: /* Methods: */

    BtConfigDialog(QWidget * const parent = nullptr,
                   Qt::WindowFlags const flags = Qt::WindowFlags());

    /**
      \brief Adds the page to this dialog, taking ownership.
      \param[in] page pointer to the page to add.
    */
    void addPage(Page * const pageWidget);

    void save();

Q_SIGNALS:

    void signalSettingsChanged();

private: /* Fields: */

    QListWidget * const m_contentsList;
    QStackedWidget * const m_pageWidget;
    int m_maxItemWidth = 0;

};
