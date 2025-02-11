/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2025 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

enum WizardPage: int {
    taskPage,
    sourcesProgressPage,
    sourcesPage,
    languagesPage,
    installWorksPage,
    updateWorksPage,
    removeWorksPage,
    removeFinalPage,
    installFinalPage
};

enum WizardTaskType {
    installWorks,
    updateWorks,
    removeWorks
};
