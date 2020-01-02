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

#ifndef CRANGECHOOSERDIALOG_H
#define CRANGECHOOSERDIALOG_H

#include <QDialog>

#include <QListWidgetItem>


class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QTextEdit;

namespace Search {

/** \todo Redesign this dialog and rename to have a Bt prefix. */
class CRangeChooserDialog: public QDialog {

        Q_OBJECT

private: /* Types: */

    class RangeItem: public QListWidgetItem {

    public:

        inline RangeItem(const QString &caption,
                         const QString &range = QString(),
                         QListWidget * parent = nullptr)
            : QListWidgetItem(caption, parent)
            , m_range(range) {}

        inline const QString caption() const { return text(); }
        inline void setCaption(const QString &caption) { setText(caption); }
        inline const QString &range() const { return m_range; }
        inline void setRange(const QString &range) { m_range = range; }

    private: /* Fields: */

        QString m_range;

    }; /* class RangeItem */

public: /* Methods: */

    CRangeChooserDialog(const QStringList& rangeScopeModule, QWidget *parentDialog = nullptr);

    void accept() override;

private: /* Methods: */

    /**
      Initializes widgets.
    */
    void initView();

    /**
      Initializes connections.
    */
    void initConnections();

    void retranslateUi();

    /**
      Stores the values from the current edit view to the given RangeItem.
      \param[out] i The RangeItem object to store the values to.
    */
    void saveCurrentToRange(RangeItem * i);

    /**
      Resets the editing controls based on whether a range is selected in the
      range list.
    */
    void resetEditControls();

private slots:

    /**
      Adds a new range to the list.
    */
    void addNewRange();

    /**
      Handles changes in m_rangeList.
    */
    void selectedRangeChanged(QListWidgetItem * current,
                              QListWidgetItem * previous);

    /**
      Parses the entered text and prints out the result in the list box below
      the edit area.
    */
    void updateResultList();

    /**
      Deletes the selected range.
    */
    void deleteCurrentRange();

    /**
      Restores the default ranges.
    */
    void restoreDefaults();

    /**
      Called when m_nameEdit changes.
    */
    void nameEditTextChanged(const QString &newText);

private:

    QLabel      *m_rangeListLabel;
    QListWidget *m_rangeList;

    QLabel    *m_nameEditLabel;
    QLineEdit *m_nameEdit;

    QLabel    *m_rangeEditLabel;
    QTextEdit *m_rangeEdit;

    QLabel      *m_resultListLabel;
    QListWidget *m_resultList;

    QPushButton *m_newRangeButton;
    QPushButton *m_deleteRangeButton;

    QDialogButtonBox *m_buttonBox;

    QStringList m_scopeModules;

}; /* class CRangeChooserDialog */

} /* namespace Search */

#endif
