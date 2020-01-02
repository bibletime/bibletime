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

#ifndef SEARCHCSEARCHANALYSISDIALOG_H
#define SEARCHCSEARCHANALYSISDIALOG_H

#include <QDialog>

#include "../../../backend/cswordmodulesearch.h"


class CSwordModuleInfo;
class QAbstractButton;
class QDialogButtonBox;

namespace Search {

class CSearchAnalysisView;
class CSearchAnalysisScene;

/**
    @author The BibleTime team <info@bibletime.info>
*/
class CSearchAnalysisDialog : public QDialog {
        Q_OBJECT
    public:
        CSearchAnalysisDialog(const CSwordModuleSearch::Results &results,
                              QWidget *parentDialog = nullptr);

    protected: // Protected methods
        /**
         * Initializes this dialog.
         */
        void initView();
        void resizeEvent(QResizeEvent* event) override;

    private slots:
        void buttonClicked(QAbstractButton* button);

    private:
        CSearchAnalysisScene* m_analysis;
        CSearchAnalysisView* m_analysisView;
        QDialogButtonBox* m_buttonBox;
};

}

#endif
