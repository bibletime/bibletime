/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTBOOKSHELFWIZARD_H
#define BTBOOKSHELFWIZARD_H

#include <QWizard>

#include <QStringList>
#include "btbookshelfwizardenums.h"
#include "../../util/btassert.h"


class BtBookshelfWorksPage;
class BtBookshelfInstallFinalPage;
class BtBookshelfLanguagesPage;
class BtBookshelfSourcesPage;
class BtBookshelfSourcesProgressPage;
class BtBookshelfTaskPage;
class BtBookshelfUpdatePage;
class BtModuleSet;
class QKeyEvent;
class QMessageBox;

/** \brief The Bookshelf Manager wizard. */
class BtBookshelfWizard final: public QWizard {

    Q_OBJECT

public: /* Methods: */

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

public slots:

    void accept() final override;

protected:
    virtual void keyPressEvent(QKeyEvent * event) override;

private: /* Methods: */

    void retranslateUi();
    void stopDownload();

private: /* Fields: */

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

#endif
