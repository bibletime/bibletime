/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CBUTTONS_H
#define CBUTTONS_H

#include <QToolButton>

#include "backend/managers/cswordbackend.h"


class CSwordModuleInfo;
class QMenu;

/** This class manages the display options of the selected modules.
  * @author The BibleTime team
  */
class CDisplaySettingsButton: public QToolButton {
        Q_OBJECT

    public:
        CDisplaySettingsButton(QWidget *parent = 0);

        void setDisplayOptions(const CSwordBackend::DisplayOptions &displaySettings,
                               bool repopulate = true);
        void setFilterOptions(const CSwordBackend::FilterOptions &moduleSettings,
                              bool repopulate = true);
        void setModules(const QList<CSwordModuleInfo*> &modules);

    signals:
        void sigFilterOptionsChanged(CSwordBackend::FilterOptions filterOptions);
        void sigDisplayOptionsChanged(CSwordBackend::DisplayOptions displayOptions);
        void sigChanged(void);

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
        CSwordBackend::FilterOptions  m_filterOptions;
        CSwordBackend::DisplayOptions m_displayOptions;
        QList<CSwordModuleInfo*> m_modules;

        QMenu *m_popup;
        QAction *m_lineBreakAction;
        QAction *m_verseNumbersAction;
        QAction *m_headingsAction;
        QAction *m_redWordsAction;
        QAction *m_hebrewPointsAction;
        QAction *m_hebrewCantillationAction;
        QAction *m_greekAccentsAction;
        QAction *m_variantAction;
        QAction *m_scriptureReferencesAction;
        QAction *m_morphSegmentationAction;
};

#endif
