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

#include "csearchanalysisdialog.h"

#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QRect>
#include <QScreen>
#include <QtGlobal>
#include <QVBoxLayout>
#include <utility>
#include "../../../backend/cswordmodulesearch.h"
#include "../../../util/btconnect.h"
#include "../../messagedialog.h"
#include "csearchanalysisscene.h"
#include "csearchanalysisview.h"


namespace Search {

static const int DIALOG_HEIGHT = 400;
static const int DIALOG_BORDER = 30;

CSearchAnalysisDialog::CSearchAnalysisDialog(
        QString searchedText,
        const CSwordModuleSearch::Results &results,
        QWidget *parentDialog)
        : QDialog(parentDialog)
{
    QVBoxLayout *vboxLayout = new QVBoxLayout(this);

    m_analysis =
            new CSearchAnalysisScene(std::move(searchedText), results, this);
    m_analysisView = new CSearchAnalysisView(m_analysis, this);
    ////    m_analysisView->show();
    vboxLayout->addWidget(m_analysisView);

    m_buttonBox = new QDialogButtonBox(
                        QDialogButtonBox::Save | QDialogButtonBox::Close,
                        this);
    //tr("Save as HTML"),
    message::prepareDialogBox(m_buttonBox);
    vboxLayout->addWidget(m_buttonBox);

    BT_CONNECT(m_buttonBox, &QDialogButtonBox::rejected,
               this, &CSearchAnalysisDialog::reject);
    BT_CONNECT(m_buttonBox, &QDialogButtonBox::accepted,
               [this]{ m_analysis->saveAsHTML(); });

    retranslateUi();
}

void CSearchAnalysisDialog::resizeEvent(QResizeEvent* event) {
    QDialog::resizeEvent(event);
    m_analysis->resizeHeight(height());
}

void CSearchAnalysisDialog::showEvent(QShowEvent * const event) {
    QDialog::showEvent(event);

    /* Set initial width based on the search data, but limit to the width of the
       desktop. */
    if (m_shown)
        return;
    m_shown = true;
    int const width = static_cast<int>(m_analysis->width() + DIALOG_BORDER);
    int const desktopWidth = screen()->availableSize().width();
    resize(qMin(width, desktopWidth), DIALOG_HEIGHT);
}

void CSearchAnalysisDialog::retranslateUi()
{ setWindowTitle(tr("Analysis of search results")); }

}
