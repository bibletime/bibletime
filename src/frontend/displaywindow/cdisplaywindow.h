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

#ifndef CDISPLAYWINDOW_H
#define CDISPLAYWINDOW_H

#include <QMainWindow>

#include <QAction>
#include <QStringList>
#include "backend/btglobal.h"
#include "backend/managers/cswordbackend.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "util/btassert.h"
#include "util/btconnect.h"


class CDisplay;
class BtDisplaySettingsButton;
class CKeyChooser;
class CMDIArea;
class BtModuleChooserBar;
class CSwordModuleInfo;
class QMenu;
class QToolBar;
class BTHistory;
class BibleTime;

/** The base class for all display windows of BibleTime.
  *
  * Inherits QMainWindow.
  *
  * Inherited by CReadWindow and CWriteWindow.
  *
  * @author The BibleTime team
  */
class CDisplayWindow : public QMainWindow {
    Q_OBJECT
public:

    /** Insert the keyboard accelerators of this window into the given actioncollection.*/
    static void insertKeyboardActions( BtActionCollection* const a );

    /** Returns pointer to the mdi area object.*/
    inline CMDIArea *mdi() const {
        return m_mdi;
    }

    /** Returns the correct window caption.*/
    const QString windowCaption();

    /** Returns the used modules as a pointer list.*/
    const BtConstModuleList modules() const;

    /** Returns the used modules as a string list. */
    inline const QStringList &getModuleList() const {
        return m_modules;
    }

    /** Store the settings of this window in the given CProfileWindow object.*/
    virtual void storeProfileSettings(QString const & windowGroup) const;

    /** Load the settings the given CProfileWindow object into this window.*/
    virtual void applyProfileSettings(const QString & windowGroup);

    /** Returns the display options used by this display window. */
    inline const DisplayOptions &displayOptions() const {
        return m_displayOptions;
    }

    /** Returns the filter options used by this window. */
    inline const FilterOptions &filterOptions() const {
        return m_filterOptions;
    }

    /** Returns true if the widget is ready for use. */
    inline bool isReady() const {
        return m_isReady;
    }

    /** Returns true if the window may be closed.*/
    virtual bool queryClose();

    /** Returns history for this window */
    BTHistory* history();

    /** Returns the keychooser widget of this display window. */
    inline CKeyChooser *keyChooser() const {
        return m_keyChooser;
    }

    /** Sets the new sword key.*/
    void setKey( CSwordKey* key );

    /** Returns the key of this display window. */
    inline CSwordKey *key() const {
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

    /** Sets and inits the properties of the format toolbar.*/
    void setFormatToolBar( QToolBar* bar );

    /** Returns the main navigation toolbar. */
    inline QToolBar *mainToolBar() const {
        return m_mainToolBar;
    }

    /** Returns the tool buttons toolbar. */
    inline QToolBar *buttonsToolBar() const {
        return m_buttonsToolBar;
    }

    /** Returns the format toolbar. */
    inline QToolBar *formatToolBar() const {
        return m_formatToolBar;
    }

    /** Initialize the toolbars.*/
    virtual void initToolbars() = 0;

    /** Sets the display settings button.*/
    void setDisplaySettingsButton( BtDisplaySettingsButton* button );

    virtual void setupPopupMenu() = 0;

    /** Returns the display widget used by this implementation of CDisplayWindow. */
    virtual inline CDisplay *displayWidget() const {
        BT_ASSERT(m_displayWidget);
        return m_displayWidget;
    }

    /** Sets the display widget used by this display window.*/
    virtual void setDisplayWidget( CDisplay* newDisplay );

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

    inline BtActionCollection *actionCollection() const {
        return m_actionCollection;
    }

signals:
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

public slots:
    /** Receives a signal telling that a module should be added.*/
    void slotAddModule(int index, QString module);
    void slotReplaceModule(int index, QString newModule);
    void slotRemoveModule(int index);

    /**
        * Lookup the specified key in the given module. If the module is not chosen withing
        * this display window create a new displaywindow with the right module in it.
        */
    virtual void lookupModKey( const QString& module, const QString& key );

    /** Lookup the key in the chosen modules.*/
    virtual void lookupKey( const QString& key );

    /** Refresh the settings of this window.*/
    virtual void reload(CSwordBackend::SetupChangedReason reason);

    void slotShowNavigator(bool show);
    void slotShowToolButtons(bool show);
    void slotShowModuleChooser(bool show);
    void slotShowFormatToolBar(bool show);
    void slotShowHeader(bool show);

protected:

    friend class CBibleReadWindow;

    CDisplayWindow(const QList<CSwordModuleInfo *> & modules, CMDIArea * parent);
    ~CDisplayWindow() override;

    /**
          \returns the display options used by this display window.
        */
    inline DisplayOptions &displayOptions() {
        return m_displayOptions;
    }

    /**
          \returns the filter options used by this window.
        */
    inline FilterOptions &filterOptions() {
        return m_filterOptions;
    }

    /** Initializes the internel keyboard actions.*/
    virtual void initActions();

    /** Sets the keychooser widget for this display window.*/
    void setKeyChooser( CKeyChooser* ck );

    /** Returns the module chooser bar. */
    inline BtModuleChooserBar *moduleChooserBar() const {
        return m_moduleChooserBar;
    }

    /** Lookup the given key.*/
    virtual void lookupSwordKey( CSwordKey* ) = 0;

    /** Sets the module chooser bar.*/
    void setModuleChooserBar( BtModuleChooserBar* bar );

    void setHeaderBar(QToolBar* header);

    inline QToolBar *headerBar() const {
        return m_headerBar;
    }

    /** Sets the modules. */
    void setModules( const QList<CSwordModuleInfo*>& modules );

    /** Initializes the signal / slot connections of this display window.*/
    virtual void initConnections() = 0;

    /** Initialize the view of this display window.*/
    virtual void initView() = 0;

    /** Returns the installed RMB popup menu.*/
    QMenu* popup();

    /** Called to add actions to mainWindow toolbars */
    virtual void setupMainWindowToolBars() = 0;

    void closeEvent(QCloseEvent* e) override;

    void setToolBarsHidden();
    void clearMainWindowToolBars();

protected slots:
    /**
          Sets the new filter options of this window.
        */
    void setFilterOptions(const FilterOptions &filterOptions);

    /**
          Sets the new display options for this window.
        */
    void setDisplayOptions(const DisplayOptions &displayOptions);

    virtual void modulesChanged();

    /**
          Lookup the current key. Used to refresh the display. This also needs to be called
          after programmatically changing filter/display options.
        */
    void lookup();

    virtual void updatePopupMenu();

    void slotSearchInModules();

    void printAll();

    void printAnchorWithText();

    void setFocusKeyChooser();

    virtual void pageDown();

    virtual void pageUp();

private: /* Methods: */

    template <typename Name, typename ... Args>
    inline QAction & initAction(Name && name, Args && ... args) {
        QAction & a = m_actionCollection->action(std::forward<Name>(name));
        BT_CONNECT(&a, &QAction::triggered, std::forward<Args>(args)...);
        return a;
    }

    template <typename ... Args>
    inline void initAddAction(Args && ... args)
    { addAction(&initAction(std::forward<Args>(args)...)); }

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
    QToolBar* m_formatToolBar;
    QToolBar* m_headerBar;
    QMenu* m_popupMenu;
    CDisplay* m_displayWidget;
    BTHistory* m_history;
};

#endif
