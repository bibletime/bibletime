//
// C++ Interface: csearchanalysisdialog
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SEARCHCSEARCHANALYSISDIALOG_H
#define SEARCHCSEARCHANALYSISDIALOG_H


#include "backend/drivers/cswordmoduleinfo.h"

#include <QDialog>


class QAbstractButton;
class QDialogButtonBox;

namespace Search {

class CSearchAnalysisView;
class CSearchAnalysisScene;



/**
	@author The BibleTime team <info@bibletime.info>
*/
class CSearchAnalysisDialog : public QDialog
{
	Q_OBJECT

public:
	CSearchAnalysisDialog(ListCSwordModuleInfo modules, QWidget* parentDialog);
	~CSearchAnalysisDialog() {}

protected: // Protected methods
	void buttonClicked(QAbstractButton* button);
	/**
	 * Initializes this dialog.
	 */
	void initView();

private:
	CSearchAnalysisScene* m_analysis;
	CSearchAnalysisView* m_analysisView;
	QDialogButtonBox* m_buttonBox;
};

}

#endif
