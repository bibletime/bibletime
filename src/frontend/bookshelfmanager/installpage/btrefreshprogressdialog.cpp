#include "frontend/bookshelfmanager/installpage/btrefreshprogressdialog.h"

#include <QApplication>
#include "backend/btinstallbackend.h"
#include "frontend/messagedialog.h"
#include "util/btassert.h"
#include "util/btconnect.h"


BtRefreshProgressDialog::BtRefreshProgressDialog(sword::InstallSource &source,
                                                 QWidget *parent,
                                                 Qt::WindowFlags flags)
             : QProgressDialog(parent, flags)
             , m_source(source)
{
    BT_ASSERT(BtInstallBackend::isRemote(source));
    setWindowTitle(tr("Refreshing source %1").arg(QString(source.caption)));
    setCancelButtonText(tr("&Cancel"));
    setLabelText(tr("Connecting..."));
    BT_ASSERT(minimum() == 0);
    setMaximum(100);
    setValue(0);
    setWindowModality(Qt::ApplicationModal);
    setMinimumDuration(1000);

    BT_CONNECT(this, SIGNAL(canceled()),
               this, SLOT(slotCanceled()));
    BT_CONNECT(&m_installMgr, SIGNAL(percentCompleted(int, int)),
               this,          SLOT(slotPercentCompleted(int, int)));
}

void BtRefreshProgressDialog::slotPercentCompleted(int, int current) {
    setValue(current);
    qApp->processEvents();
}

void BtRefreshProgressDialog::slotCanceled() {
    m_installMgr.terminate();
}

bool BtRefreshProgressDialog::runAndDelete() {
    show();
    qApp->processEvents();
    bool r = (m_installMgr.refreshRemoteSource(&m_source) == 0);
    if (r) {
        setValue(100);
        qApp->processEvents();
    } else {
        message::showWarning(this, tr("Warning"),
                          tr("Failed to refresh source %1")
                              .arg(QString(m_source.caption)));
    }
    deleteLater();
    return r;
}
