/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTDISPLAYSETTINGSBUTTON_H
#define BTDISPLAYSETTINGSBUTTON_H

#include <QToolButton>

#include "backend/btglobal.h"
#include "backend/managers/cswordbackend.h"


class CSwordModuleInfo;
class QMenu;

/** This class manages the display options of the selected modules.
  * @author The BibleTime team
  */
class BtDisplaySettingsButton: public QToolButton {
        Q_OBJECT

    public:
        BtDisplaySettingsButton(QWidget *parent = nullptr);

    public slots:
        void setDisplayOptions(const DisplayOptions &displaySettings,
                               bool repopulate = true);
        void setFilterOptions(const FilterOptions &moduleSettings,
                              bool repopulate = true);

        void setModules(const BtConstModuleList &modules);

    signals:
        void sigFilterOptionsChanged(FilterOptions filterOptions);
        void sigDisplayOptionsChanged(DisplayOptions displayOptions);
        void sigModulesChanged(const BtConstModuleList &modules);
        void sigChanged();

    protected slots:
        void slotOptionToggled(QAction *action);

    protected:
        void initMenu();
        void retranslateUi();
        void retranslateToolTip();
        void repopulateMenu();

        bool isOptionAvailable(const CSwordModuleInfo::FilterTypes option);
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

#endif
