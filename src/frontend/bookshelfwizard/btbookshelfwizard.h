/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTBOOKSHELFWIZARD_H
#define BTBOOKSHELFWIZARD_H

#include <QWizard>

#include <QStringList>
#include "frontend/bookshelfwizard/btbookshelfwizardenums.h"
#include "frontend/bookshelfwizard/btbookshelfworkspage.h"


class BtBookshelfWorksPage;
class BtBookshelfInstallFinalPage;
class BtBookshelfLanguagesPage;
class BtBookshelfRemoveFinalPage;
class BtBookshelfSourcesPage;
class BtBookshelfSourcesProgressPage;
class BtBookshelfTaskPage;
class BtBookshelfUpdatePage;
class BtModuleSet;

/** \brief The Bookshelf Manager wizard. */
class BtBookshelfWizard: public QWizard {

    Q_OBJECT

public: /* Methods: */

    BtBookshelfWizard(QWidget * parent = nullptr, Qt::WindowFlags flags = 0);

    QStringList selectedSources() const;
    QStringList selectedLanguages() const;
    BtModuleSet selectedWorks() const;
    WizardTaskType taskType() const;
    QString installPath() const;

    BtBookshelfLanguagesPage & languagesPage() const noexcept
    { return *m_languagesPage; }

public slots:

    void accept();

private: /* Methods: */

    void retranslateUi();

private: /* Fields: */

    BtBookshelfTaskPage * m_taskPage;
    BtBookshelfSourcesProgressPage * m_sourcesProgressPage;
    BtBookshelfSourcesPage * m_sourcesPage;
    BtBookshelfLanguagesPage * m_languagesPage;
    BtBookshelfWorksPage * m_installWorksPage;
    BtBookshelfWorksPage * m_updateWorksPage;
    BtBookshelfWorksPage * m_removeWorksPage;
    BtBookshelfRemoveFinalPage * m_removeFinalPage;
    BtBookshelfInstallFinalPage * m_installFinalPage;

}; /* class BtBookshelfWizard */

#endif
