/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef SEARCHCSEARCHANALYSISDIALOG_H
#define SEARCHCSEARCHANALYSISDIALOG_H


class CSwordModuleInfo;

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
	CSearchAnalysisDialog(QList<CSwordModuleInfo*> modules, QWidget* parentDialog);
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
