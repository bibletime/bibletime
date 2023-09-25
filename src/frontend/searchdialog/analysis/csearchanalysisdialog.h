/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include <QDialog>

#include <QObject>
#include <QString>
#include "../../../backend/cswordmodulesearch.h"


class QDialogButtonBox;
class QResizeEvent;
class QWidget;

namespace Search {

class CSearchAnalysisView;
class CSearchAnalysisScene;

class CSearchAnalysisDialog : public QDialog {
        Q_OBJECT
    public:
        CSearchAnalysisDialog(QString searchedText,
                              CSwordModuleSearch::Results const & results,
                              QWidget * parentDialog = nullptr);

    protected: // methods:

        void resizeEvent(QResizeEvent* event) override;
        void showEvent(QShowEvent * event) override;

    private: // Methods:

        void retranslateUi();

    private:
        CSearchAnalysisScene* m_analysis;
        CSearchAnalysisView* m_analysisView;
        QDialogButtonBox* m_buttonBox;
        bool m_shown = false;
};

}
