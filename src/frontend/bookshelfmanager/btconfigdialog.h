/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTCONFIGDIALOG_H
#define BTCONFIGDIALOG_H

#include <QDialog>

#include <QDebug>
#include <QWidget>


class BtConfigPage;
class QDialogButtonBox;
class QLabel;
class QListWidget;
class QListWidgetItem;
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
    public:
        BtConfigDialog(QWidget *parent = 0);
        virtual ~BtConfigDialog();

        /** Adds a BtConfigPage to the paged widget stack. The new page will be the current page.*/
        void addPage(BtConfigPage* pageWidget);
        /** Adds a button box to the lower edge of the dialog. */
        void addButtonBox(QDialogButtonBox* buttonBox);

    public slots:
        /** Changes the current page using the given index number. */
        void slotChangePage(int newIndex);

    private:
        QListWidget* m_contentsList;
        QStackedWidget* m_pageWidget;
        QVBoxLayout* m_pageLayout;
        int m_maxItemWidth;
        int m_previousPageIndex;
};



/**
* Base class for configuration dialog pages.
*/
class BtConfigPage : public QWidget {
        Q_OBJECT
        friend class BtConfigDialog;

    public:
        /**
          Constructs a configuration dialog base, with QVBoxLayout as layout() and a header
          label as the first widget in this layout.
          \param[in] parent The parent widget.
        */
        BtConfigPage(QWidget *parent = 0);
        virtual ~BtConfigPage();

        /** Implement these to return the correct values.
        * For example: header(){return tr("General");}
        */
        virtual const QIcon &icon() const = 0;
        virtual QString label() const = 0;
        virtual QString header() const = 0;

        inline BtConfigDialog *parentDialog() const {
            return m_parentDialog;
        }

    protected:
        /** Reimplemented from QWidget. */
        virtual void showEvent(QShowEvent *event);

    private:
        void initializeLabel();

    private:
        BtConfigDialog *m_parentDialog;
        QLabel *m_headerLabel;
        bool m_labelInitialized;
};


#endif

