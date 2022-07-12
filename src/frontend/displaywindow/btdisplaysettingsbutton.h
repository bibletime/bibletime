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

#include <QToolButton>

#include <QObject>
#include <QString>
#include "../../backend/btglobal.h"
#include "../../backend/drivers/btmodulelist.h"
#include "../../backend/drivers/cswordmoduleinfo.h"


class QAction;
class QMenu;
class QWidget;

/** This class manages the display options of the selected modules.
  * @author The BibleTime team
  */
class BtDisplaySettingsButton: public QToolButton {
        Q_OBJECT

    public:
        BtDisplaySettingsButton(QWidget *parent = nullptr);

        void setDisplayOptionsNoRepopulate(
                DisplayOptions const & displaySettings);
        void setFilterOptionsNoRepopulate(FilterOptions const & moduleSettings);

    public Q_SLOTS:
        void setDisplayOptions(DisplayOptions const & displaySettings);
        void setFilterOptions(FilterOptions const & moduleSettings);

        void setModules(const BtConstModuleList &modules);

    Q_SIGNALS:
        void sigFilterOptionsChanged(FilterOptions filterOptions);
        void sigDisplayOptionsChanged(DisplayOptions displayOptions);
        void sigModulesChanged(const BtConstModuleList &modules);
        void sigChanged();

    protected:
        void initMenu();
        void retranslateUi();
        void retranslateToolTip();
        void repopulateMenu();

        bool isOptionAvailable(CSwordModuleInfo::FilterOption const & option);
        void addMenuEntry(QAction *action, bool checked);

    private:
        FilterOptions  m_filterOptions;
        DisplayOptions m_displayOptions;
        BtConstModuleList m_modules;

        QMenu *m_popup;
        QAction *m_verseNumbersAction;
        QAction *m_headingsAction;
        QAction *m_redWordsAction;
        QAction *m_hebrewPointsAction;
        QAction *m_hebrewCantillationAction;
        QAction *m_greekAccentsAction;
        QAction *m_variantAction;
        QAction *m_scriptureReferencesAction;
        QAction *m_footnotesAction;
        QAction *m_morphSegmentationAction;
};
