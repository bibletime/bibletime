#ifndef CSWORDSETUPINSTALLSOURCESDIALOG_H
#define CSWORDSETUPINSTALLSOURCESDIALOG_H

#include "btinstallmgr.h"
#include <qdialog.h>

class QLabel;
class QComboBox;
class QLineEdit;

namespace BookshelfManager {

class CSwordSetupInstallSourcesDialog : public QDialog  {
		Q_OBJECT

public:
		static sword::InstallSource getSource();

protected:
		CSwordSetupInstallSourcesDialog();

protected slots:
		void slotOk();
		void slotProtocolChanged();

private:
		QLabel    *m_serverLabel;
		QLineEdit *m_captionEdit, *m_serverEdit, *m_pathEdit;
		QComboBox *m_protocolCombo;
	};

} //namespace

#endif //CSWORDSETUPINSTALLSOURCESDIALOG_H
