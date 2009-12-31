/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CDISPLAYWINDOW_H
#define CDISPLAYWINDOW_H

#include <QMainWindow>
#include "util/cpointers.h"

#include <QStringList>
#include "backend/managers/cswordbackend.h"
#include "frontend/profile/cprofilewindow.h"


class BtActionCollection;
class CDisplay;
class BtDisplaySettingsButton;
class CKeyChooser;
class CMDIArea;
//class CModuleChooserBar;
class BtModuleChooserBar;
class CReadWindow;
class CSwordModuleInfo;
class CWriteWindow;
class QCloseEvent;
class QMenu;
class QToolBar;

/** The base class for all display windows of BibleTime.
  * @author The BibleTime team
  */
class CDisplayWindow : public QMainWindow, public CPointers {
        Q_OBJECT
    public:
        enum WriteWindowType {
            HTMLWindow = 1,
            PlainTextWindow = 2
        };

        /** Insert the keyboard accelerators of this window into the given actioncollection.*/
        static void insertKeyboardActions( BtActionCollection* const a );

        inline CMDIArea *mdi() const {
            return m_mdi;
        }

        /** Returns the correct window caption.*/
        const QString windowCaption();

        /** Returns the used modules as a pointer list.*/
        QList<CSwordModuleInfo*> modules();

        /** Returns the used modules as a string list. */
        inline const QStringList &getModuleList() const {
            return m_modules;
        }

        /** Store the settings of this window in the given CProfileWindow object.*/
        virtual void storeProfileSettings( Profile::CProfileWindow* profileWindow ) = 0;

        /** Store the settings of this window in the given profile window.*/
        virtual void applyProfileSettings( Profile::CProfileWindow* profileWindow ) = 0;

        /** Returns the display options used by this display window. */
        inline const CSwordBackend::DisplayOptions &displayOptions() const {
            return m_displayOptions;
        }

        /** Returns the filter options used by this window. */
        inline const CSwordBackend::FilterOptions &filterOptions() const {
            return m_filterOptions;
        }

        /** Set the ready status. */
        void setReady(bool ready);

        /** Returns true if the widget is ready for use. */
        inline bool isReady() const {
            return m_isReady;
        }

        /** Returns true if the window may be closed.*/
        virtual bool queryClose();

        /** Returns the keychooser widget of this display window. */
        inline CKeyChooser *keyChooser() const {
            return m_keyChooser;
        }

        /** Sets the new sword key.*/
        void setKey( CSwordKey* key );

        /** Returns the key of this display window. */
        inline CSwordKey *key() const {
            Q_ASSERT(m_swordKey != 0);
            return m_swordKey;
        }

        /**
        * Initialize the window. Call this method from the outside,
        * because calling this in the constructor is not possible!
        */
        virtual bool init();

        /** Sets and inits the properties of the main navigation toolbar.*/
        void setMainToolBar( QToolBar* bar );

        /** Sets and inits the properties of the tool buttons toolbar.*/
        void setButtonsToolBar( QToolBar* bar );

        /** Returns the main navigation toolbar. */
        inline QToolBar *mainToolBar() const {
            return m_mainToolBar;
        }

        /** Returns the tool buttons toolbar. */
        inline QToolBar *buttonsToolBar() const {
            return m_buttonsToolBar;
        }

        /** Initialize the toolbars.*/
        virtual void initToolbars() = 0;

        /** Returns the display settings button. */
        inline BtDisplaySettingsButton *displaySettingsButton() const {
            return m_displaySettingsButton;
        }

        /** Sets the display settings button.*/
        void setDisplaySettingsButton( BtDisplaySettingsButton* button );

        virtual void setupPopupMenu() = 0;

        /** Returns the display widget used by this implementation of CDisplayWindow. */
        virtual inline CDisplay *displayWidget() const {
            Q_ASSERT(m_displayWidget != 0);
            return m_displayWidget;
        }

        /** Sets the display widget used by this display window.*/
        virtual void setDisplayWidget( CDisplay* newDisplay );

        /**
        * Returns whether syncs to the active window are allowed at this time for this display window
        * @return boolean value whether sync is allowed
        */
        virtual bool syncAllowed() const {
            return false;
        };

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
        void slotShowHeader(bool show);

    protected:
        friend class CMDIArea;
        friend class CBibleReadWindow;

        CDisplayWindow(QList<CSwordModuleInfo*> modules, CMDIArea* parent);
        virtual ~CDisplayWindow();

        /** Returns the display options used by this display window. */
        inline CSwordBackend::DisplayOptions &displayOptions() {
            return m_displayOptions;
        }

        /** Returns the filter options used by this window. */
        inline CSwordBackend::FilterOptions &filterOptions() {
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

        virtual void closeEvent(QCloseEvent* e);

    protected slots:
        /** Sets the new filter options of this window.*/
        void setFilterOptions(const CSwordBackend::FilterOptions &filterOptions);

        /** Sets the new display options for this window.*/
        void setDisplayOptions(const CSwordBackend::DisplayOptions &displayOptions);

        virtual void modulesChanged();

        /** Lookup the current key. Used to refresh the display.*/
        void lookup();

        virtual void updatePopupMenu();

        void slotSearchInModules();

        void printAll();

        void printAnchorWithText();

        void setFocusKeyChooser();

    private:
        BtActionCollection* m_actionCollection;
        CMDIArea* m_mdi;

        //we may only cache the module names bacause after a backend relaod the pointers are invalid!
        QStringList m_modules;

        CSwordBackend::FilterOptions m_filterOptions;
        CSwordBackend::DisplayOptions m_displayOptions;

        BtDisplaySettingsButton* m_displaySettingsButton;
        CKeyChooser* m_keyChooser;
        CSwordKey* m_swordKey;
        bool m_isReady;
        BtModuleChooserBar* m_moduleChooserBar;
        QToolBar* m_mainToolBar;
        QToolBar* m_buttonsToolBar;
        QToolBar* m_headerBar;
        QMenu* m_popupMenu;
        CDisplay* m_displayWidget;
};

#endif
