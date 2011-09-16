/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfmanager/installpage/btinstallpathdialog.h"

#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "backend/btinstallbackend.h"
#include "util/dialogutil.h"
#include "util/directory.h"
#include "util/cresmgr.h"
#include "util/tool.h"


BtInstallPathDialog::BtInstallPathDialog() {
    namespace DU = util::directory;

    setWindowTitle(tr("Bookshelf Folders"));

    QVBoxLayout *mainLayout;
    QHBoxLayout *viewLayout;

    mainLayout = new QVBoxLayout(this);
    viewLayout = new QHBoxLayout();

    QString l1 = tr("Works can be installed in one or more folders. After setting up folders here you can choose one of them in Install page.");
    /// \bug The following string has an extra space character:
    QString l2 = tr("BibleTime and the SWORD library find the works from  all of these folders. If a folder is removed here it still exists in the system with all the works in it.");

    QLabel* mainLabel = util::tool::explanationLabel(this,
                        tr("Configure bookshelf folders"), l1 + QString("<small><br/><br/>") + l2 + QString("</small>"));
    mainLayout->addWidget(mainLabel);

    QString swordConfPath = BtInstallBackend::swordConfigFilename();
    /// \todo After releasing 2.4, change the following line to: QLabel *confPathLabel = new QLabel(tr("Configuration file for the folders is: <b>%1</b>").arg(swordConfPath), this);
    QLabel* confPathLabel = new QLabel(tr("Configuration file for the folders is: ").append("<b>%1</b>").arg(swordConfPath), this);
    confPathLabel->setWordWrap(true);
    mainLayout->addWidget(confPathLabel);


    m_swordPathListBox = new QTreeWidget(this);
    m_swordPathListBox->setHeaderHidden(true);

    QString rwfolderitem(tr("Folders where new works can be installed"));
    m_writableItem = new QTreeWidgetItem(m_swordPathListBox, QStringList(rwfolderitem));;
    m_writableItem->setFlags(Qt::ItemIsEnabled);
    m_readableItem = new QTreeWidgetItem(m_swordPathListBox, QStringList(tr("Read-only folders")));;
    m_readableItem->setFlags(Qt::ItemIsEnabled);
    m_nonexistingItem = new QTreeWidgetItem(m_swordPathListBox, QStringList(tr("Nonexistent folders")));;
    m_nonexistingItem->setFlags(Qt::ItemIsEnabled);

    QStringList targets = BtInstallBackend::targetList();

    foreach (QString pathname, targets)  {
        addPathToList(pathname);
    }
    updateTopLevelItems();

    viewLayout->addWidget(m_swordPathListBox);

    QVBoxLayout* buttonLayout = new QVBoxLayout();

    m_addButton = new QPushButton(tr("&Add..."), this);
    m_addButton->setToolTip(tr("Add new folder"));
    m_addButton->setIcon(DU::getIcon(CResMgr::bookshelfmgr::paths::add_icon));
    connect(m_addButton, SIGNAL(clicked()), this, SLOT(slotAddClicked()));
    buttonLayout->addWidget(m_addButton);

    m_editButton = new QPushButton(tr("&Edit..."), this);
    m_editButton->setToolTip(tr("Edit the selected folder"));
    m_editButton->setIcon(DU::getIcon(CResMgr::bookshelfmgr::paths::edit_icon));
    connect(m_editButton, SIGNAL(clicked()), this, SLOT(slotEditClicked()));
    buttonLayout->addWidget(m_editButton);

    m_removeButton = new QPushButton(tr("&Remove"), this);
    m_removeButton->setToolTip(tr("Remove the selected folder"));
    m_removeButton->setIcon(DU::getIcon(CResMgr::bookshelfmgr::paths::remove_icon));
    connect(m_removeButton, SIGNAL(clicked()), this, SLOT(slotRemoveClicked()));
    buttonLayout->addWidget(m_removeButton);

    QSpacerItem* spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    buttonLayout->addItem(spacerItem);

    viewLayout->addLayout(buttonLayout);
    mainLayout->addLayout(viewLayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::NoButton | QDialogButtonBox::Ok);
    util::prepareDialogBox(buttonBox);
    mainLayout->addWidget(buttonBox);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    //clumsy way to set width. Could someone please fix Qt to have an easy way to set widget sizes?
    int textWidth = fontMetrics().width(rwfolderitem.append("MMMMMMMMMM"));
    int buttonWidth = m_addButton->width();
    resize(textWidth + buttonWidth, size().height());
}

BtInstallPathDialog::~BtInstallPathDialog() {
    m_swordPathListBox->invisibleRootItem()->takeChildren();
    delete m_readableItem;
    delete m_writableItem;
    delete m_nonexistingItem;
}

void BtInstallPathDialog::updateTopLevelItems() {
    if (m_writableItem->childCount()) {
        m_writableItem->setHidden(false);
        m_swordPathListBox->expandItem(m_writableItem);
    }
    else {
        m_writableItem->setHidden(true);
    }
    if (m_readableItem->childCount()) {
        m_readableItem->setHidden(false);
        m_swordPathListBox->expandItem(m_readableItem);
    }
    else {
        m_readableItem->setHidden(true);
    }
    if (m_nonexistingItem->childCount()) {
        m_nonexistingItem->setHidden(false);
        m_swordPathListBox->expandItem(m_nonexistingItem);
    }
    else {
        m_nonexistingItem->setHidden(true);
    }
}


void BtInstallPathDialog::addPathToList(QString pathname) {
    if (pathname.isEmpty()) return;
    QTreeWidgetItem* i = 0;
    QDir dir(pathname);
    if (!dir.exists()) {
        i = new QTreeWidgetItem(m_nonexistingItem, QStringList(pathname) );
    }
    else if (dir.isReadable()) {
        const QFileInfo fi( dir.canonicalPath() );
        if (fi.isWritable()) {
            i = new QTreeWidgetItem(m_writableItem, QStringList(pathname) );
        }
        else {
            i = new QTreeWidgetItem(m_readableItem, QStringList(pathname) );
        }
    }
    if (i && QDir(pathname) == BtInstallBackend::swordDir()) {
        i->setFlags(Qt::NoItemFlags);
        i->setToolTip(0, tr("This default folder in your home directory can't be removed"));
    }
}

void BtInstallPathDialog::slotEditClicked() {
    if (QTreeWidgetItem* i = m_swordPathListBox->currentItem()) {
        QString dirname = QFileDialog::getExistingDirectory(this, tr("Choose Folder"), i->text(0), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if (dirname.isEmpty()) { // if user cancelled the dialog
            return;
        }
        QDir dir = QDir(dirname);
        if (dir.isReadable()) {
            const QFileInfo fi( dir.canonicalPath() );
            if (!fi.exists() || !fi.isWritable()) {
                const int result = util::showWarning(this, tr("Use Folder?"), tr("This folder is not writable, so works can not be installed here using BibleTime. Do you want to use this folder instead of the previous value?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
                if (result != QMessageBox::Yes) return;
            }
            //i->setText(0, dir.absolutePath()); // absolute, not canonical
            addPathToList(dir.absolutePath());
            delete i;
            updateTopLevelItems();
        }
    }
}

void BtInstallPathDialog::slotAddClicked() {
    QString dirname = QFileDialog::getExistingDirectory(this, tr("Choose Folder"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dirname.isEmpty()) { // if user cancelled the dialog
        return;
    }
    QDir dir = QDir(dirname);
    if (dir.isReadable()) {
        const QFileInfo fi( dir.canonicalPath() );
        if (!fi.exists() || !fi.isWritable()) {
            const int result = util::showWarning(this, tr("Use Folder?"), tr("This folder is not writable, so works can not be installed here using BibleTime. Do you still want to add it to the list of bookshelf folders?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            if (result != QMessageBox::Yes) {
                return;
            }
        }
        addPathToList(util::directory::convertDirSeparators(dir.canonicalPath()));
        updateTopLevelItems();
    }
}

void BtInstallPathDialog::slotRemoveClicked() {
    QTreeWidgetItem* i = m_swordPathListBox->selectedItems().value(0);
    if (i && i->parent() != m_swordPathListBox->invisibleRootItem()) {
        delete i;
    }
    updateTopLevelItems();
}

void BtInstallPathDialog::writeSwordConfig() {
    QStringList targets;
    QTreeWidgetItemIterator it(m_swordPathListBox, QTreeWidgetItemIterator::NoChildren | QTreeWidgetItemIterator::Enabled | QTreeWidgetItemIterator::NotHidden);
    while (*it) {
        if (!(*it)->text(0).isEmpty()) {
            targets << (*it)->text(0);
        }
        ++it;
    }
    qDebug() << "save the target list" << targets;
    BtInstallBackend::setTargetList(targets); //creates new Sword config
}

void BtInstallPathDialog::accept() {
    writeSwordConfig();
    QDialog::accept();
}
