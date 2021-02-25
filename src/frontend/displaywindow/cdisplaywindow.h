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

#ifndef CDISPLAYWINDOW_H
#define CDISPLAYWINDOW_H

#include <QMainWindow>

#include <QAction>
#include <QStringList>
#include "../../backend/btglobal.h"
#include "../../backend/managers/cswordbackend.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "btactioncollection.h"


class BtConfigCore;
class BtToolBarPopupAction;
class BtModelViewReadDisplay;
class CSwordLDKey;
class BtDisplaySettingsButton;
class CKeyChooser;
class CMDIArea;
class BtModuleChooserBar;
class CSwordModuleInfo;
class QMenu;
class QToolBar;
class BTHistory;
class BibleTime;
class CLexiconReadWindow;

/** \brief The base class for all display windows of BibleTime. */
class CDisplayWindow : public QMainWindow {
    Q_OBJECT
    friend class CLexiconReadWindow;
public:

    virtual CSwordModuleInfo::ModuleType moduleType() const;

    /** Insert the keyboard accelerators of this window into the given actioncollection.*/
    static void insertKeyboardActions( BtActionCollection* const a );

    /** Returns pointer to the mdi area object.*/
    CMDIArea * mdi() const { return m_mdi; }

    /** Returns the correct window caption.*/
    QString windowCaption();

    /** Returns the used modules as a pointer list.*/
    BtConstModuleList modules() const;

    /** Returns the used modules as a string list. */
    QStringList const & getModuleList() const { return m_modules; }

    /**
       \brief Stores the settings of this window to configuration.
       \param[in] windowConf The locked configuration group.
    */
    virtual void storeProfileSettings(BtConfigCore & windowConf) const;

    /**
       \brief Loads the settings of this window from configuration.
       \param[in] windowConf The locked configuration group.
    */
    virtual void applyProfileSettings(BtConfigCore const & windowConf);

    /** Returns the display options used by this display window. */
    DisplayOptions const & displayOptions() const { return m_displayOptions; }

    /** Returns the filter options used by this window. */
    FilterOptions const & filterOptions() const { return m_filterOptions; }

    /** Returns true if the widget is ready for use. */
    bool isReady() const { return m_isReady; }

    /** Returns history for this window */
    BTHistory* history();

    /** Returns the keychooser widget of this display window. */
    CKeyChooser * keyChooser() const { return m_keyChooser; }

    /** Returns the key of this display window. */
    CSwordKey * key() const {
        BT_ASSERT(m_swordKey);
        return m_swordKey;
    }

    /**
        * Initialize the window. Call this method from the outside,
        * because calling this in the constructor is not possible!
        */
    bool init();

    /** Sets and inits the properties of the main navigation toolbar.*/
    void setMainToolBar( QToolBar* bar );

    /** Sets and inits the properties of the tool buttons toolbar.*/
    void setButtonsToolBar( QToolBar* bar );

    /** Returns the main navigation toolbar. */
    QToolBar * mainToolBar() const { return m_mainToolBar; }

    /** Returns the tool buttons toolbar. */
    QToolBar * buttonsToolBar() const { return m_buttonsToolBar; }

    /** Sets the display settings button.*/
    void setDisplaySettingsButton( BtDisplaySettingsButton* button );

    /** Returns the display widget used by this implementation of CDisplayWindow. */
    BtModelViewReadDisplay * displayWidget() const {
        BT_ASSERT(m_displayWidget);
        return m_displayWidget;
    }

    /** Sets the display widget used by this display window.*/
    void setDisplayWidget(BtModelViewReadDisplay * newDisplay);

    /**
        * Returns whether syncs to the active window are allowed at this time for this display window
        * @return boolean value whether sync is allowed
        */
    virtual bool syncAllowed() const { return false; }

    /**
        * Return pointer to the BibleTime main window
        */
    BibleTime* btMainWindow();
    /**
        * Called when this window is activated
        */
    void windowActivated();

    BtActionCollection * actionCollection() const { return m_actionCollection; }

    bool hasSelectedText();

    /** Updates the status of the popup menu entries. */
    virtual void copyDisplayedText();

    int getSelectedColumn() const;

    int getFirstSelectedIndex() const;

    int getLastSelectedIndex() const;

Q_SIGNALS:
    /** The module list was set because backend was reloaded.*/
    void sigModuleListSet(QStringList modules);
    /** A module was added to this window.*/
    void sigModuleAdded(int index, QString module);
    void sigModuleReplaced(int index, QString newModule);
    void sigModuleRemoved(int index);
    /** The module list of window changed but backend list didn't.*/
    void sigModuleListChanged();

    /**
          Signal emitted when display options are changed.
        */
    void sigDisplayOptionsChanged(const DisplayOptions &displayOptions);

    /**
          Signal emitted when display options are changed.
        */
    void sigFilterOptionsChanged(const FilterOptions &filterOptions);

    /**  signal for change of modules */
    void sigModulesChanged(const BtConstModuleList &modules);

    /**  signal for sword key change */
    void sigKeyChanged(CSwordKey* key);

public Q_SLOTS:
    /** Receives a signal telling that a module should be added.*/
    void slotAddModule(int index, QString module);
    void slotReplaceModule(int index, QString newModule);
    void slotRemoveModule(int index);

    /** Lookup the key in the chosen modules.*/
    void lookupKey(QString const & key);

    /** Refresh the settings of this window.*/
    virtual void reload(CSwordBackend::SetupChangedReason reason);

protected:

    friend class CBibleReadWindow;

    CDisplayWindow(const QList<CSwordModuleInfo *> & modules, CMDIArea * parent);
    ~CDisplayWindow() override;

    void setDisplayOptions(DisplayOptions const & v) { m_displayOptions = v; }

    void setFilterOptions(FilterOptions const & v) { m_filterOptions = v; }

    template <typename ... Args>
    QAction & initAddAction(Args && ... args) {
        QAction & action = initAction(std::forward<Args>(args)...);
        addAction(&action);
        return action;
    }

    /** Initializes the internel keyboard actions.*/
    virtual void initActions();

    /** Sets the keychooser widget for this display window.*/
    void setKeyChooser( CKeyChooser* ck );

    /** Returns the module chooser bar. */
    BtModuleChooserBar * moduleChooserBar() const { return m_moduleChooserBar; }

    /** Lookup the given key.*/
    virtual void lookupSwordKey(CSwordKey *);

    /** Sets the module chooser bar.*/
    void setModuleChooserBar( BtModuleChooserBar* bar );

    QToolBar * headerBar() const { return m_headerBar; }

    /** Initializes the signal / slot connections of this display window.*/
    virtual void initConnections();

    /** Initialize the view of this display window.*/
    virtual void initView();

    /** Initialize the toolbars.*/
    virtual void initToolbars();

    virtual void setupPopupMenu();

    /** Update the status of the popup menu entries. */
    virtual void updatePopupMenu();

    /** Returns the installed RMB popup menu.*/
    QMenu* popup();

    /** Called to add actions to mainWindow toolbars */
    virtual void setupMainWindowToolBars();

    void setToolBarsHidden();
    void clearMainWindowToolBars();

protected Q_SLOTS:

    virtual void modulesChanged();

    /**
          Lookup the current key. Used to refresh the display. This also needs to be called
          after programmatically changing filter/display options.
        */
    void lookup();

    void printAll();

    void printAnchorWithText();

    /**
        * This function saves the entry as html using the CExportMgr class.
        */
    void saveAsHTML();
    /**
        * This function saves the entry as plain text using the CExportMgr class.
        */
    void saveAsPlain();
    void saveRawHTML();

private: /* Methods: */

    template <typename Name, typename ... Args>
    QAction & initAction(Name && name, Args && ... args) {
        QAction & a = m_actionCollection->action(std::forward<Name>(name));
        BT_CONNECT(&a, &QAction::triggered, std::forward<Args>(args)...);
        return a;
    }

private Q_SLOTS:

    /** Opens the search dialog with the strong info of the last clicked word.*/
    void openSearchStrongsDialog();

protected: /* Fields: */

    struct ActionsStruct {
        BtToolBarPopupAction * backInHistory;
        BtToolBarPopupAction * forwardInHistory;
        QAction * findText;
        QAction * findStrongs;
        QMenu * copyMenu;
        struct {
            QAction * byReferences;
            QAction * reference;
            QAction * entry;
            QAction * selectedText;
        } copy;
        QMenu * saveMenu;
        struct {
            QAction * reference;
            QAction * entryAsPlain;
            QAction * entryAsHTML;
        } save;
        QMenu * printMenu;
        struct {
            QAction * reference;
            QAction * entry;
        } print;
    } m_actions;

private:
    BtActionCollection* m_actionCollection;
    CMDIArea* m_mdi;

    //we may only cache the module names bacause after a backend reload the pointers are invalid!
    QStringList m_modules;

    FilterOptions m_filterOptions;
    DisplayOptions m_displayOptions;

    CKeyChooser* m_keyChooser;
    CSwordKey* m_swordKey;
    bool m_isReady;
    BtModuleChooserBar* m_moduleChooserBar;
    QToolBar* m_mainToolBar;
    QToolBar* m_buttonsToolBar;
    QToolBar* m_headerBar;
    QMenu* m_popupMenu;
    BtModelViewReadDisplay * m_displayWidget = nullptr;
    BTHistory* m_history;
};

#endif
