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

        RangeItem(QString const & caption,
                  QString const & range = QString(),
                  QListWidget * parent = nullptr)
            : QListWidgetItem(caption, parent)
            , m_range(range) {}

        QString caption() const { return text(); }
        void setCaption(QString const & caption) { setText(caption); }
        QString const & range() const { return m_range; }
        void setRange(QString const & range) { m_range = range; }

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

private Q_SLOTS:

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
