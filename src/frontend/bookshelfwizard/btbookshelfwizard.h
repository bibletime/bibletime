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

#include "frontend/bookshelfwizard/btbookshelfworkspage.h"
#include "frontend/bookshelfwizard/btbookshelfwizardenums.h"
#include <QWizard>
#include <QStringList>

class BtBookshelfWorksPage;
class BtBookshelfLanguagesPage;
class BtBookshelfRemoveFinalPage;
class BtBookshelfSourcesPage;
class BtBookshelfTaskPage;
class BtBookshelfUpdatePage;
class BtBookshelfInstallFinalPage;
class BtModuleSet;

/**
* The Bookshelf Manager wizard.
*/
class BtBookshelfWizard: public QWizard {

        Q_OBJECT

    public:
        BtBookshelfWizard(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);

        QStringList selectedSources() const;
        QStringList selectedLanguages() const;
        BtModuleSet selectedWorks() const;
        WizardTaskType taskType() const;
        QString installPath() const;
        BtBookshelfLanguagesPage * languagesPage();

public slots:
        void accept();

    private:
        void retranslateUi();

        void loadWizardGeometry();

        void saveWizardGeometry() const;
        void saveSettings() const;

        BtBookshelfTaskPage *m_taskPage;
        BtBookshelfSourcesPage *m_sourcesPage;
        BtBookshelfLanguagesPage *m_languagesPage;
        BtBookshelfWorksPage *m_installWorksPage;
        BtBookshelfWorksPage *m_updateWorksPage;
        BtBookshelfWorksPage *m_removeWorksPage;
        BtBookshelfRemoveFinalPage *m_removeFinalPage;
        BtBookshelfInstallFinalPage *m_installFinalPage;
};

#endif
