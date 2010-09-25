#include "frontend/bookshelfmanager/installpage/btrefreshprogressdialog.h"

#include <QApplication>
#include <QDebug>
#include "backend/btinstallbackend.h"
#include "util/dialogutil.h"


BtRefreshProgressDialog::BtRefreshProgressDialog(sword::InstallSource &source,
                                                 QWidget *parent,
                                                 Qt::WindowFlags flags)
             : QProgressDialog(parent, flags)
             , m_source(source)
{
    qDebug() << "Creating BtRefreshProgressDialog for source" << source.caption;

    Q_ASSERT(BtInstallBackend::isRemote(source));
    setWindowTitle(tr("Refreshing Source %1").arg(QString(source.caption)));
    setCancelButtonText(tr("&Cancel"));
    setLabelText(tr("Connecting..."));
    Q_ASSERT(minimum() == 0);
    setMaximum(100);
    setValue(0);
    setWindowModality(Qt::ApplicationModal);
    setMinimumDuration(1000);

    connect(this, SIGNAL(canceled()),
            this, SLOT(slotCanceled()));
    connect(&m_installMgr, SIGNAL(percentCompleted(int,int)),
            this,          SLOT(slotPercentCompleted(int,int)));
}

void BtRefreshProgressDialog::slotPercentCompleted(int, int current) {
    qDebug() << "BtRefreshProgressDialog progress:" << current;

    setValue(current);
    qApp->processEvents();
}

void BtRefreshProgressDialog::slotCanceled() {
    qDebug() << "BtRefreshProgressDialog cancel clicked.";

    m_installMgr.terminate();
}

bool BtRefreshProgressDialog::runAndDelete() {
    qDebug() << "BtRefreshProgressDialog runAndDelete()";

    show();
    qApp->processEvents();
    bool r = (m_installMgr.refreshRemoteSource(&m_source) == 0);
    if (r) {
        setValue(100);
        qApp->processEvents();
    } else {
        util::showWarning(this, tr("Warning"),
                          tr("Failed to refresh source %1")
                              .arg(QString(m_source.caption)));
    }
    deleteLater();
    return r;
}
