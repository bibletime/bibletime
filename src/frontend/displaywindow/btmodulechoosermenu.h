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

#include "../btmenuview.h"

#include <QFlags>
#include <QString>
#include <QStringList>
#include "../../backend/drivers/cswordmoduleinfo.h"


class QAction;

class BtModuleChooserMenu final: public BtMenuView {

    Q_OBJECT

public: /* Types: */

    enum Flag {
        DisableNonBiblesOnFirstButton,
        DisableSelectedModule,
        AddNoneButton
    };
    Q_DECLARE_FLAGS(Flags, Flag)

public: /* Methods: */

    BtModuleChooserMenu(QString const & title,
                        CSwordModuleInfo::ModuleType moduleType,
                        Flags flags,
                        QWidget * parent = nullptr);

    void update(QStringList newModulesToUse,
                QString newSelectedModule,
                int newButtonIndexIndex,
                int newLeftLikeModules);

    CSwordModuleInfo::ModuleType moduleType() const noexcept
    { return m_moduleType; }

    QString const & selectedModule() const noexcept { return m_selectedModule; }
    void setSelectedModule(QString newSelectedModule) noexcept;

    int buttonIndex() const noexcept { return m_buttonIndex; }

signals:

    void sigModuleChosen(CSwordModuleInfo * module);

private: /* Methods: */

    void preBuildMenu(QActionGroup * actionGroup) final override;

    QAction * newAction(QMenu * parentMenu,
                        QModelIndex const & itemIndex) final override;

public: /* Fields: */

    CSwordModuleInfo::ModuleType const m_moduleType;
    void * const m_sortedModel;
    Flags const m_flags;
    QAction * m_noneAction;
    QStringList m_newModulesToUse;
    QString m_selectedModule;
    int m_buttonIndex;
    int m_leftLikeModules;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(BtModuleChooserMenu::Flags)
