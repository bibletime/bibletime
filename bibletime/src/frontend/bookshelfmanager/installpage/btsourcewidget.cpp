/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfmanager/installpage/btsourcewidget.h"

#include <boost/scoped_ptr.hpp>
#include <QApplication>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressDialog>
#include <QPushButton>
#include <QString>
#include <QTabBar>
#include <QTabWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QWidget>
#include "frontend/bookshelfmanager/btinstallmgr.h"
#include "frontend/bookshelfmanager/btmodulemanagerdialog.h"
#include "frontend/bookshelfmanager/cswordsetupinstallsourcesdialog.h"
#include "frontend/bookshelfmanager/installpage/btinstallmodulechooserdialog.h"
#include "frontend/bookshelfmanager/installpage/btinstallpage.h"
#include "frontend/bookshelfmanager/installpage/btinstallprogressdialog.h"
#include "frontend/bookshelfmanager/installpage/btsourcearea.h"
#include "frontend/bookshelfmanager/instbackend.h"
#include "util/dialogutil.h"


/**
* Tab Widget that holds source widgets
*/
BtSourceWidget::BtSourceWidget(BtInstallPage *parent)
        : QTabWidget(parent), m_page(parent)
{
    qDebug() << "BtSourceWidget::BtSourceWidget start";
    initSources();

    /// \todo choose the active tab from config
}

BtSourceArea* BtSourceWidget::area() {
    return dynamic_cast<BtSourceArea*>(currentWidget());
}

QString BtSourceWidget::currentSourceName() {
    qDebug() << "BtSourceWidget::currentSourceName: " << m_sourceNameList.at(currentIndex());
    return m_sourceNameList.at(currentIndex());
}

void BtSourceWidget::slotDelete() {
    qDebug() << "void BtSourceWidget::slotDelete() start";
    // ask for confirmation
    int ret = util::showWarning(this, tr("Delete Source?"),
                                tr("Do you really want to delete this source?"),
                                QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        instbackend::deleteSource(currentSourceName());
        initSources();
    }
}

void BtSourceWidget::slotAdd() {

    boost::scoped_ptr<CSwordSetupInstallSourcesDialog> dlg( new CSwordSetupInstallSourcesDialog() );
    sword::InstallSource newSource(""); //empty, invalid Source

    if (dlg->exec() == QDialog::Accepted) {
        if (!dlg->wasRemoteListAdded()) {
            newSource = dlg->getSource();
            if ( !((QString)newSource.type.c_str()).isEmpty() ) { // we have a valid source to add
                instbackend::addSource(newSource);
            }
        }
        initSources();
    }
}


void BtSourceWidget::slotRefresh() {
    qDebug() << "void BtSourceWidget::slotRefresh() start";
    // (re)build the module cache for the source

    QString sourceName = currentSourceName();

    // quick enough, make it modal so that we don't need to take care of anything else
    m_progressDialog = new QProgressDialog("", tr("Cancel"), 0 , 100, this);
    m_progressDialog->setWindowTitle(tr("Refreshing Source"));
    m_progressDialog->setMinimumDuration(0);

    /// \todo get rid of the backend code, BtInstallMgr and progressdialog could handle this
    //write method BtInstallMgr::slotRefreshCanceled()
    connect(m_progressDialog, SIGNAL(canceled()), SLOT(slotRefreshCanceled()));

    // BACKEND CODE **********************************************************
    // would this be possible: instbackend::refreshSource( arguments );
    qDebug() << "void BtSourceWidget::slotRefresh 1";
    BtInstallMgr iMgr;
    m_currentInstallMgr = &iMgr; //for the progress dialog
    sword::InstallSource is = instbackend::source(sourceName);
    bool success = false;
    qDebug() << "void BtSourceWidget::slotRefresh 2";
    // connect this directly to the dialog setValue(int) if possible
    connect(&iMgr, SIGNAL(percentCompleted(const int, const int)), SLOT(slotRefreshCompleted(const int, const int)));

    if (instbackend::isRemote(is)) {
        m_progressDialog->show();
        qApp->processEvents();
        this->slotRefreshCompleted(0, 0);
        m_progressDialog->setLabelText(tr("Connecting..."));
        m_progressDialog->setValue(0);
        qApp->processEvents();
        qDebug() << "void BtSourceWidget::slotRefresh 3";
        bool successful = iMgr.refreshRemoteSource( &is );
        if (!successful ) { //make sure the sources were updated sucessfully
            success = true;
            m_progressDialog->setValue(100); //make sure the dialog closes
        }
        else {
            qWarning("InstallMgr: refreshRemoteSources returned an error.");
            success = false;
        }
    }
    else {
        // Local source, update the list
        success = true;
    }

    delete m_progressDialog;
    m_progressDialog = 0;

    // rebuild the view tree and refresh the view
    if (success) {
        qDebug() << "void BtSourceWidget::slotRefresh 4";
        area()->createModuleTree();
    }
}

/// \todo try to move this to BtInstallMgr
void BtSourceWidget::slotRefreshCanceled() {
    qDebug() << "BtSourceWidget::slotRefreshCanceled";
    Q_ASSERT(m_currentInstallMgr);
    if (m_currentInstallMgr) {
        m_currentInstallMgr->terminate();
    }
    qApp->processEvents();
}

/// \todo try to move this to progress dialog
void BtSourceWidget::slotRefreshCompleted(const int, const int current) {
    qDebug() << "BtSourceWidget::slotRefreshCompleted";
    if (m_progressDialog) {
        if (m_progressDialog->labelText() != tr("Refreshing...")) {
            m_progressDialog->setLabelText(tr("Refreshing..."));
        }
        m_progressDialog->setValue(current);
    }
    qApp->processEvents();
}

// init the tabbar, setup the module tree for the current source
void BtSourceWidget::initSources() {
    qDebug() << "void BtSourceWidget::initSources() start";

    //first clear all sources
    //int i = count();
    for (int i = count() - 1; i >= 0; i--) {
        BtSourceArea* a = dynamic_cast<BtSourceArea*>(widget(i));
        a->prepareRemove();
    }
    for (int i = count() - 1; i >= 0; i--) {
        qDebug() << "remove tab" << tabText(i);
        QWidget* w = widget(i);
        removeTab(i);
        delete w;
        qDebug() << "deleted";
    }
    m_sourceNameList.clear();

    // ***** Use the backend to get the list of sources *****
    instbackend::initPassiveFtpMode();
    QStringList sourceList = instbackend::sourceList();
    qDebug() << "got the source list from backend:" << sourceList;
    // Add a default entry, the Crosswire main repository
    if (!sourceList.count()) {
        /// \todo Open a dialog which asks whether to get list from server and add sources
        sword::InstallSource is("FTP");   //default return value
        is.caption = "CrossWire Bible Society";
        is.source = "ftp.crosswire.org";
        is.directory = "/pub/sword/raw";
        // passive ftp is not needed here, it's the default

        instbackend::addSource(is);

        sourceList = instbackend::sourceList();
        //Q_ASSERT( sourceList.count() > 0 );
    }
    qDebug() << "void BtSourceWidget::initSources 1";
    // Add the sources to the widget
    foreach (QString sourceName, sourceList) {
        addSource(sourceName);
    }
    // connect this after the tabs have been created,
    // otherwise the signal is caught too early.
    QObject::connect(this, SIGNAL(currentChanged(int)), this, SLOT(slotTabSelected(int)));
    qDebug() << "void BtSourceWidget::initSources end";
    /// \todo select the current source from the config
    // It's important to choose something because the tree is not initialized until now
    setCurrentIndex(0);
    slotTabSelected(0); // setting the index wasn't enough if there were only 1 tab

    if (sourceList.count() == 0) {
        QHBoxLayout* l = new QHBoxLayout(this);
        QLabel* message = new QLabel(QString("<i>") + tr("No sources were found in the SWORD configuration and BibleTime couldn't create a default source. Check your SWORD configuration and that the configuration path is writable. Then restart the Bookshelf Manager.") + QString("</i>"), this);
        message->setWordWrap(true);
        l->addWidget(message);
    }
}

void BtSourceWidget::addSource(const QString& sourceName) {
    qDebug() << "void BtSourceWidget::addSource(const QString& sourceName) start, with name" << sourceName;
    // The source has already been added to the backend.

    QString type;
    QString server;
    QString path;
    sword::InstallSource is = instbackend::source(sourceName);
    if (instbackend::isRemote(is)) {
        type = tr("Remote:");
        server = is.source.c_str();
        path = is.directory.c_str();
    }
    else { // local source
        type = tr("Local:");
        QFileInfo fi( is.directory.c_str() );
        path = is.directory.c_str();
        if (!(fi.isDir() )) {
            path = path + QString(" ") + tr("Not a directory!"); /// \todo change this
        }
        if (!fi.isReadable()) {
            path = path + QString(" ") + tr("Not readable!"); /// \todo change this
        }
    }

    // Here the tab UI is created and added to the tab widget
    BtSourceArea* area = new BtSourceArea(sourceName);
    int tabNumber = this->addTab(area, sourceName);

    /// \todo add "remote/local", server, path etc.
    QString toolTip(QString("<p style='white-space:pre'>") + sourceName + QString("<br/><b>") + type + QString("</b> ") + server + path + QString("</p>"));
    tabBar()->setTabToolTip(tabNumber, toolTip);

    //select the new tab
    setCurrentIndex(tabNumber);
    m_sourceNameList.append(sourceName);

    // Initialize source tab connections:
    connect(area->m_refreshButton, SIGNAL(clicked()), SLOT(slotRefresh()));
    connect(area->m_deleteButton, SIGNAL(clicked()), SLOT(slotDelete()), Qt::QueuedConnection);
    connect(area->m_addButton, SIGNAL(clicked()), SLOT(slotAdd()));
    connect(area, SIGNAL(signalSelectionChanged(QString, int)), SLOT(slotModuleSelectionChanged(QString, int)) );

    qDebug() << "BtSourceWidget::addSource end";
}

//
void BtSourceWidget::slotModuleSelectionChanged(QString sourceName, int selectedCount) {
    /// \todo editing sources should update the map also
    qDebug() << "BtSourceWidget::slotModuleSelectionChanged start";

    int overallCount = 0;
    m_selectedModulesCountMap.insert(sourceName, selectedCount);
    foreach (int count, m_selectedModulesCountMap) {
        qDebug() << "add" << count << "to overall count of selected modules";
        overallCount += count;
    }

    if (overallCount > 0) {
        m_page->setInstallEnabled(true);
    }
    else {
        m_page->setInstallEnabled(false);
    }
}

void BtSourceWidget::slotTabSelected(int /*index*/) {
    BtSourceArea* area = dynamic_cast<BtSourceArea*>(currentWidget());
    if (area) area->initTreeFirstTime();
}

void BtSourceWidget::slotInstall() {
    qDebug() << "void BtInstallPage::slotInstall start";

    // check that the destination path is writable, do nothing if not and user doesn't want to continue
    QDir dir = QDir(dynamic_cast<BtInstallPage*>(parent())->selectedInstallPath());
    bool canWrite = true;
    if (dir.isReadable()) {
        const QFileInfo fi( dir.canonicalPath() );
        if (!fi.exists() || !fi.isWritable()) {
            canWrite = false;
        }
    }
    else {
        canWrite = false;
    }
    if (!canWrite) {
        const int result = util::showWarning(this, tr("Warning"), tr("The destination directory is not writable or does not exist. Installation will fail unless this has first been fixed."), QMessageBox::Ignore | QMessageBox::Cancel, QMessageBox::Cancel);
        if (result != QMessageBox::Ignore) {
            return;
        }
    }

    // create the confirmation dialog
    BtInstallModuleChooserDialog *dlg = new BtInstallModuleChooserDialog(this);

    // Add all checked modules from all tabs:
    for (int tab = 0; tab < count(); tab++) {
        BtSourceArea *sArea = static_cast<BtSourceArea*>(widget(tab));
        foreach (CSwordModuleInfo *module, sArea->selectedModules()) {
            dlg->addModuleItem(module, m_sourceNameList.at(tab));
        }
    }

    if (dlg->exec() == QDialog::Accepted) {
        slotInstallAccepted(dlg->checkedModules());
    }
    delete dlg;
}

void BtSourceWidget::slotInstallAccepted(const QSet<CSwordModuleInfo*> &mi) {
    qDebug() << "BtSourceWidget::slotInstallAccepted";

    if (mi.empty()) return;

    /// \todo first remove all modules which will be updated from the module list
    // but what modules? all with the same real name? (there may be _n modules...)

    BtModuleManagerDialog *parentDialog = dynamic_cast<BtModuleManagerDialog*>(m_page->parentDialog());

    BtInstallProgressDialog *dlg = new BtInstallProgressDialog(mi, m_page->selectedInstallPath(), parentDialog);

    if (!parentDialog) qDebug() << "error, wrong parent!";

    m_page->setInstallEnabled(false);
    // the progress dialog is now modal, it can be made modeless later.
    dlg->exec();

    qDebug() << "BtSourceWidget::slotInstallAccepted end";
}
