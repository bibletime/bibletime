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

#include <QWizard>

#include <QObject>
#include <QString>
#include <QStringList>
#include <Qt>
#include "btbookshelfwizardenums.h"


class BtBookshelfWorksPage;
class BtBookshelfInstallFinalPage;
class BtBookshelfLanguagesPage;
class BtBookshelfSourcesPage;
class BtBookshelfSourcesProgressPage;
class BtBookshelfTaskPage;
class BtModuleSet;
class QKeyEvent;
class QMessageBox;
class QWidget;

/** \brief The Bookshelf Manager wizard. */
class BtBookshelfWizard final: public QWizard {

    Q_OBJECT

public: // methods:

    BtBookshelfWizard(QWidget * parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    QStringList selectedSources() const;
    QStringList selectedLanguages() const;
    BtModuleSet selectedWorks() const;
    WizardTaskType taskType() const;
    QString installPath() const;

    static void setAutoUpdateSources(bool value);
    static bool autoUpdateSources();

    BtBookshelfLanguagesPage & languagesPage() const noexcept {
        return *m_languagesPage;
    }

    void downloadStarted();
    void downloadFinished();

public Q_SLOTS:

    void accept() final override;

protected:
    virtual void keyPressEvent(QKeyEvent * event) override;

private: // methods:

    void retranslateUi();
    void stopDownload();

private: // fields:

    bool m_downloadInProgress;
    bool m_closeRequested;
    QMessageBox * m_closeMessageBox;

    BtBookshelfTaskPage * const m_taskPage;
    BtBookshelfSourcesPage * const m_sourcesPage;
    BtBookshelfSourcesProgressPage * const m_sourcesProgressPage;
    BtBookshelfLanguagesPage * const m_languagesPage;
    BtBookshelfWorksPage * const m_installWorksPage;
    BtBookshelfWorksPage * const m_updateWorksPage;
    BtBookshelfWorksPage * const m_removeWorksPage;
    BtBookshelfInstallFinalPage * const m_installFinalPage;

}; /* class BtBookshelfWizard */
