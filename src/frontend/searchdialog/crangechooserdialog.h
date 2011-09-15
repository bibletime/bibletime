/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CRANGECHOOSERDIALOG_H
#define CRANGECHOOSERDIALOG_H

#include <QDialog>

#include <QListWidgetItem>


class QDialogButtonBox;
class QLineEdit;
class QListWidget;
class QPushButton;
class QTextEdit;

namespace Search {

class CRangeChooserDialog : public QDialog {
        Q_OBJECT
    public:
        CRangeChooserDialog(QWidget* parentDialog);

    protected: // Protected methods
        class RangeItem : public QListWidgetItem {
            public:
                RangeItem(QListWidget*, QListWidgetItem* afterThis = 0, const QString caption = QString::null, const QString range = QString::null);

                const QString& range() const;
                QString caption() const;
                void setRange(QString range);
                void setCaption(const QString);
            private:
                QString m_range;
        };

        /**
        * Initializes the connections of this widget.
        */
        void initConnections();
        /**
        * Initializes the view of this object.
        */
        void initView();

    protected slots: // Protected slots
        /**
        * Adds a new range to the list.
        */
        void addNewRange();
        void editRange(QListWidgetItem*);
        /**
        * Parses the entered text and prints out the result in the list box below the edit area.
        */
        void parseRange();
        void nameChanged(const QString&);
        void rangeChanged();
        /**
        * Deletes the selected range.
        */
        void deleteCurrentRange();
        virtual void slotDefault();
        virtual void slotOk();

    private:
        QListWidget* m_rangeList;
        QListWidget* m_resultList;
        QLineEdit* m_nameEdit;
        QTextEdit* m_rangeEdit;
        QPushButton* m_newRangeButton;
        QPushButton* m_deleteRangeButton;
        QDialogButtonBox* m_buttonBox;
};


} //end of namespace Search


#endif
