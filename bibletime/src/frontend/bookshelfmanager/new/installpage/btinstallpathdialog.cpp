/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btinstallpathdialog.h"
#include "btinstallpathdialog.moc"

#include "frontend/bookshelfmanager/new/backend.h"

#include "util/ctoolclass.h"
#include "util/directoryutil.h"

#include <QString>
#include <QDialog>
#include <QDir>
#include <QGridLayout>
#include <QLabel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QDialogButtonBox>

BtInstallPathDialog::BtInstallPathDialog()
{

    QVBoxLayout *mainLayout;
    QHBoxLayout *viewLayout;

    mainLayout = new QVBoxLayout(this);
    viewLayout = new QHBoxLayout();

	QLabel* mainLabel = CToolClass::explanationLabel(this,
						tr("Configure install paths"),
						tr("New works can be installed in one or more directories. After setting up directories here you can choose one of them in Install page.")
													);
	mainLayout->addWidget(mainLabel);

	QString swordConfPath = backend::configFilename();
	QLabel* confPathLabel = new QLabel(tr("Configuration file for the paths is: ").append("<b>%1</b>").arg(swordConfPath), this);
	confPathLabel->setWordWrap(true);
	mainLayout->addWidget(confPathLabel);


    m_swordPathListBox = new QTreeWidget(this);
	m_swordPathListBox->header()->hide();

	QStringList targets = backend::targetList();
	for (QStringList::iterator it = targets.begin(); it != targets.end(); ++it)  {
		if ((*it).isEmpty()) continue;
		new QTreeWidgetItem(m_swordPathListBox, QStringList(*it) );
	}
	
	viewLayout->addWidget(m_swordPathListBox);

    QVBoxLayout* buttonLayout = new QVBoxLayout();

	m_addButton = new QPushButton(tr("Add..."), this);
	m_addButton->setIcon(util::filesystem::DirectoryUtil::getIcon("edit_add"));
	connect(m_addButton, SIGNAL(clicked()), this, SLOT(slotAddClicked()));
    buttonLayout->addWidget(m_addButton);

	m_editButton = new QPushButton(tr("Edit..."), this);
	m_editButton->setIcon(util::filesystem::DirectoryUtil::getIcon("edit"));
	connect(m_editButton, SIGNAL(clicked()), this, SLOT(slotEditClicked()));
	buttonLayout->addWidget(m_editButton);

	m_removeButton = new QPushButton(tr("Remove"), this);
	m_removeButton->setIcon(util::filesystem::DirectoryUtil::getIcon("editdelete"));
	connect(m_removeButton, SIGNAL(clicked()), this, SLOT(slotRemoveClicked()));
    buttonLayout->addWidget(m_removeButton);

	QSpacerItem* spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	buttonLayout->addItem(spacerItem);

	viewLayout->addLayout(buttonLayout);
	mainLayout->addLayout(viewLayout);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);
    mainLayout->addWidget(buttonBox);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

}

void BtInstallPathDialog::slotEditClicked() {
	if (QTreeWidgetItem* i = m_swordPathListBox->currentItem()) {
		QString dirname = QFileDialog::getExistingDirectory(this, tr("Choose directory"), i->text(0));
	if (dirname.isEmpty()) { // if user cancelled the dialog
		return;
	}
		QDir dir = QDir(dirname);
		if (dir.isReadable()) {
			const QFileInfo fi( dir.canonicalPath() );
			if (!fi.exists() || !fi.isWritable()) {
				const int result = QMessageBox::warning(this, tr("Confirmation"), tr("This directory is not writable, so works can not be installed here using BibleTime. Do you want to use this directory instead of the previous value?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
				if (result != QMessageBox::Yes) return;
			}
			i->setText(0, dir.canonicalPath());

			writeSwordConfig(); //to make sure other parts work with the new setting
		}
	}
}

void BtInstallPathDialog::slotAddClicked() {
	QString dirname = QFileDialog::getExistingDirectory(this, tr("Choose directory"), "");
	if (dirname.isEmpty()) { // if user cancelled the dialog
		return;
	}
	QDir dir = QDir(dirname);
	if (dir.isReadable()) {
		const QFileInfo fi( dir.canonicalPath() );
		if (!fi.exists() || !fi.isWritable()) {
			const int result = QMessageBox::warning(this, tr("Warning"), tr("This directory is not writable, so works can not be installed here using BibleTime. Do you still want to add it to the list of bookshelf directories?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
			if (result != QMessageBox::Yes) {
				return;
			}
		}
		new QTreeWidgetItem(m_swordPathListBox, QStringList(dir.canonicalPath()) );


		writeSwordConfig(); //to make sure other parts work with the new setting
	}
}

void BtInstallPathDialog::slotRemoveClicked() {
	QTreeWidgetItem* i = m_swordPathListBox->currentItem();
	if (i) {
		delete i;


		writeSwordConfig(); //to make sure other parts work with the new setting
	}
}

void BtInstallPathDialog::writeSwordConfig()
{
	//TODO: send signal
}
