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

#include <QMainWindow>

#include <memory>
#include <QAction>
#include <QStringList>
#include "../../backend/btglobal.h"
#include "../../backend/drivers/btmodulelist.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
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

    /** Insert the keyboard accelerators of this window into the given
        actioncollection.*/
    static void insertKeyboardActions( BtActionCollection* const a );

    /** Returns pointer to the mdi area object.*/
    CMDIArea * mdi() const { return m_mdi; }

    void updateWindowTitle();

    CSwordModuleInfo const * firstModule() const noexcept
    { return m_modules.first(); }

    BtModuleList const & modules() const noexcept { return m_modules; }

    BtConstModuleList constModules() const
    { return BtConstModuleList(m_modules.begin(), m_modules.end()); }

    /** \returns the used modules as a string list. */
    QStringList const & moduleNames() const noexcept
    { return m_moduleNames; }

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

    /** Returns the keychooser widget of this display window. */
    CKeyChooser * keyChooser() const { return m_keyChooser; }

    /** Returns the key of this display window. */
    CSwordKey * key() const {
        BT_ASSERT(m_swordKey);
        return m_swordKey.get();
    }

    /** Returns the main navigation toolbar. */
    QToolBar * mainToolBar() const noexcept { return m_mainToolBar; }

    /** Returns the tool buttons toolbar. */
    QToolBar * buttonsToolBar() const noexcept { return m_buttonsToolBar; }

    /** Sets the display settings button.*/
    void setDisplaySettingsButton( BtDisplaySettingsButton* button );

    /** Returns the display widget used by this implementation of
        CDisplayWindow. */
    BtModelViewReadDisplay * displayWidget() const {
        BT_ASSERT(m_displayWidget);
        return m_displayWidget;
    }

    /** \returns whether syncs to the active window are allowed for this display
                 window. */
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

    CSwordKey* getMouseClickedKey() const;

    /* set new reference in this module, or open standard bible */
    void setBibleReference(const QString& reference);

Q_SIGNALS:
    /** A module was added to this window.*/
    void sigModuleAdded(int index, QString module);
    void sigModuleReplaced(int index, QString newModule);
    void sigModuleRemoved(int index);
    /** The module list of window changed but backend list didn't.*/
    void sigModuleListChanged(BtModuleList newList);

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
    void slotAddModule(int index, CSwordModuleInfo * module);
    void slotReplaceModule(int index, CSwordModuleInfo * newModule);
    void slotRemoveModule(int index);

    /** Lookup the key in the chosen modules.*/
    void lookupKey(QString const & key);

    /** Refresh the settings of this window.*/
    virtual void reload();

protected:

    friend class CBibleReadWindow;

    CDisplayWindow(BtModuleList const & modules,
                   bool const addTextHeaderToolbar,
                   CMDIArea * const parent);
    ~CDisplayWindow() override;

    /** Returns history for this window */
    BTHistory * history() const noexcept { return m_history; }

    /**
        * Initialize the window. Call this method from the outside,
        * because calling this in the constructor is not possible!
        */
    bool init();

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

    /** Lookup the given key.*/
    virtual void lookupSwordKey(CSwordKey *);

    /** Initializes the signal / slot connections of this display window.*/
    virtual void initConnections();

    /** Initialize the toolbars.*/
    virtual void initToolbars();

    virtual QMenu * newDisplayWidgetPopupMenu();

    /** Called to add actions to mainWindow toolbars */
    virtual void setupMainWindowToolBars();

    void clearMainWindowToolBars();

protected Q_SLOTS:

    virtual void modulesChanged();

    /** Lookup the current key. Used to refresh the display. This also needs to
        be called after programmatically changing filter/display options. */
    void lookup();

    void printAll();

    void printAnchorWithText();

private: // methods:

    template <typename Name, typename ... Args>
    QAction & initAction(Name && name, Args && ... args) {
        QAction & a = m_actionCollection->action(std::forward<Name>(name));
        BT_CONNECT(&a, &QAction::triggered, std::forward<Args>(args)...);
        return a;
    }

protected: // fields:

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
#ifdef BUILD_TEXT_TO_SPEECH
        QAction * speakSelectedText;
#endif
    } m_actions;

private:
    BtActionCollection * const m_actionCollection;
    CMDIArea * const m_mdi;

    BtModuleList m_modules;

    // Cache names of modules in case the backend invalidates the pointers in
    // m_modules. This must be kept in sync with m_modules.
    QStringList m_moduleNames;

    FilterOptions m_filterOptions;
    DisplayOptions m_displayOptions;

    CKeyChooser * m_keyChooser = nullptr;
    std::unique_ptr<CSwordKey> const m_swordKey;
    bool m_isInitialized = false; ///< Whether init() has been called
    QToolBar * m_mainToolBar;
    BtModuleChooserBar* m_moduleChooserBar;
    QToolBar * m_buttonsToolBar;
    QToolBar * m_headerBar = nullptr;
    BtModelViewReadDisplay * m_displayWidget;
    BTHistory * const m_history;
};
