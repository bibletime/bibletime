/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CSWORDBACKEND_H
#define CSWORDBACKEND_H

#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>
#include "backend/drivers/cswordmoduleinfo.h"

// Sword includes:
#include <swmgr.h>
#include <swbuf.h>
#include <swmodule.h>
#include <swversion.h>
#include <localemgr.h>
#include <utilstr.h>

namespace Rendering {
    class CEntryDisplay;
    class CChapterDisplay;
    class CBookDisplay;
}

/** The backend layer main class.
 * This is the implementation of CBackend for Sword. It's additionally derived from SWMgr
 * to provide functions of Sword.
  *
  * @short The backend implementation of Sword
  * @author The BibleTime team
  * @version $Id: cswordbackend.h,v 1.58 2007/03/14 21:32:47 joachim Exp $
  */
class CSwordBackend : public QObject, public sword::SWMgr {
    Q_OBJECT
    public:

        /** The reason for the sigSwordSetupChanged signal, i.e. why the module list has changed. */
        enum SetupChangedReason {
            AddedModules = 1,
            RemovedModules = 2,
            HidedModules = 4,
            PathChanged = 8,
            OtherChange = 16
        };

        /** Filter options. Filter options to
         * control the text display of modules. Uses int and not bool because not all
         * options have just two toggle values.
         */
        struct FilterOptions {
            int footnotes; /**< 0 for disabled, 1 for enabled */
            int strongNumbers; /**< 0 for disabled, 1 for enabled */
            int headings; /**< 0 for disabled, 1 for enabled */
            int morphTags; /**< 0 for disabled, 1 for enabled */
            int lemmas; /**< 0 for disabled, 1 for enabled */
            int hebrewPoints; /**< 0 for disabled, 1 for enabled */
            int hebrewCantillation; /**< 0 for disabled, 1 for enabled */
            int greekAccents; /**< 0 for disabled, 1 for enabled */
            int textualVariants; /**< Number n to enabled the n-th variant */
            int redLetterWords; /**< 0 for disabled, 1 for enabled */
            int scriptureReferences; /**< 0 for disabled, 1 for enabled */
            int morphSegmentation; /**< 0 for disabled, 1 for enabled */
        };

        /** Control the display of a text.
        */
        struct DisplayOptions {
            int lineBreaks;
            int verseNumbers;
        };

        /** The error codes which may be returned by the @ref Load() call.
        */
        enum LoadError { // the values exist to cast from the char return of SWMgr::Load
            NoSwordConfig = -1,
            NoError = 0,
            NoModules = 1
        };
        /**
        * The constructor of the Sword backend.
        * It creates the SWModule objects using SWMgr's methods, it adds the necessary
        * filters for the module format.
        */
        CSwordBackend();
        /**
        * The constructor of the Sword backend. This is actually used nowhere.
        * Notice that using augmentHome=false can mess up the system because it is true elsewhere.
        * @param path The path which is used to load modules
        * @param augmentHome True if the $HOME/.sword/ modules should be augmented with the other modules
        */
        CSwordBackend( const QString& path, const bool augmentHome = true );

        /**
        * The destrctor of this backend. This function shuts the modules down using @ref shutdownModules.
        */
        virtual ~CSwordBackend();

        /**
        * This function returns the list of available modules managed by this backend.
        * You have to call initModules() first;
        *
        * @return The list of modules managed by this backend
        */
        inline virtual QList<CSwordModuleInfo*>& moduleList();
        /**
        * Initializes the Sword modules.
        *
        * @return True if the initializiation was succesful, otherwise return false.
        */
        virtual CSwordBackend::LoadError initModules(SetupChangedReason reason);
        /**
        * This function deinitializes the modules and deletes them.
        *
        * @return True if it was succesful, otherwise return false
        */
        virtual bool shutdownModules();
        /**
        * Sets the given options enabled or disabled depending on the second parameter.
        *
        * @param type This is the type this function should set enabled or disabled
        * @param enable If this is true the option will be enabled, otherwise it will be disabled.
        */
        void setOption( const CSwordModuleInfo::FilterTypes type, const int state );
        /** */
        void setFilterOptions( const CSwordBackend::FilterOptions options );
        /**
        * Sets the language for the international booknames of Sword.
        * @param langName The abbreviation string which should be used for the Sword backend
        */
        const QString booknameLanguage( const QString& langName = QString::null );
        /**
        * This function searches for a module with the specified description
        * @param description The description of the desired module
        * @return pointer to the desired module; null if no module has the specified description
        */
        virtual CSwordModuleInfo* findModuleByDescription(const QString& description);
        /**
        * This function searches for a module with the specified description
        * @param description The description of the desired module
        * @return pointer to the desired module; null if no module has the specified description
        */
        const QString findModuleNameByDescription(const QString& description);
        /**
        * This function searches for a module with the specified name
        * @param name The name of the desired module
        * @return Pointer to the desired module; null if no module has the specified name
        */
        CSwordModuleInfo* findModuleByName(const QString& name);
        /**
        * This function searches for a module with the specified sword module as module() object!
        * @param swmodule to a Sword module
        * @return pointer to the desired module; null if no module has the specified name
        */
        CSwordModuleInfo* findSwordModuleByPointer(const sword::SWModule* const swmodule);
        /**
        * This function searches for a module which is the same as the passed module.
        * @param module The module which should be used for searching the new one. May be child of a different backend.
        * @return Pointer to the desired module; null if no module has the specified name
        */
        CSwordModuleInfo* findModuleByPointer(const CSwordModuleInfo* const module);
        /**
        * @return Our global config object which contains the configs of all modules merged together.
        */
        inline sword::SWConfig* getConfig() const;
        /**
        * Tries to find the config object for the module. The second paramter will be the found config.
        * @return True if the config was found, false if not. If false is returned the moduleConfig object is in undefined/unknwon state.
        */
        bool moduleConfig(const QString& module, sword::SWConfig& moduleConfig );
        /**
        * Returns the text used for the option given as parameter.
        * @param The paramter enum
        * @return The name of the option given by the parameter
        */
        static const QString optionName( const CSwordModuleInfo::FilterTypes option );
        /**
        * Returns the text used for the option given as parameter.
        */
        static const QString configOptionName( const CSwordModuleInfo::FilterTypes option );
        /**
        * Returns the translated name of the option given as parameter.
        * @param The translated option name
        */
        static const QString translatedOptionName(const CSwordModuleInfo::FilterTypes option );
        /**
        * Returns the version of the Sword library.
        * @return The version used by this backend
        */
        inline virtual const sword::SWVersion Version();
        /**
        * Reload all Sword modules.
        */
        void reloadModules(SetupChangedReason reason);

        /**
        * Takes off the given modules from the list and returns them.
        * User must take care of the deletion of the returned CSwordModuleInfo pointers.
        */
        QList<CSwordModuleInfo*> takeModulesFromList(QStringList names);

        /** Sword prefix list.
        * @return A list of all known Sword prefix dirs
        */
        const QStringList swordDirList();

        /** Emits the sigSwordSetupChanged signal.
        * This can be called directly from outside if there is no need to reload the backend.
        */
        void notifyChange(SetupChangedReason reason);

    signals:
        void sigSwordSetupChanged(CSwordBackend::SetupChangedReason reason);

    protected:
        /**
        * Adds a render filter to the module.
        * This is used to apply our own render filters to our modules instead of the sword filters
        */
        virtual void AddRenderFilters(sword::SWModule *module, sword::ConfigEntMap &section);
        /**
        * Overrides Sword filters which appear to be buggy.
        */
        virtual void filterInit();

    private:
        // Filters
        struct Filters {
            sword::SWFilter* gbf;
            sword::SWFilter* plain;
            sword::SWFilter* thml;
            sword::SWFilter* osis;
        }	m_filters;

        struct Displays {
            Rendering::CChapterDisplay* chapter;
            Rendering::CEntryDisplay* entry;
            Rendering::CBookDisplay* book;
        }	m_displays;

        QList<CSwordModuleInfo*> m_moduleList;
        QMap<QString, QString> m_moduleDescriptionMap;
};

/**Returns The list of modules managed by this backend*/
inline QList<CSwordModuleInfo*>& CSwordBackend::moduleList() {
    return m_moduleList;
}

/** Returns our local config object to store the cipher keys etc. locally for each user. The values of the config are merged with the global config. */
inline sword::SWConfig* CSwordBackend::getConfig() const {
    return config;
}

/** Returns the version of the Sword library. */
inline const sword::SWVersion CSwordBackend::Version() {
    return sword::SWVersion::currentVersion;
}

#endif
