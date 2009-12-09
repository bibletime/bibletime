/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CPOINTERS_H
#define CPOINTERS_H

#include "backend/managers/clanguagemgr.h"


class CSwordBackend;
class CLanguageMgr;
class CDisplayTemplateMgr;

namespace InfoDisplay {
class CInfoDisplay;
}

/** Holds the pointers to important classes like modules, backend etc.
*/
class CPointers {
    protected:
        friend class BibleTime; //BibleTime may initialize this object
        friend class BibleTimeApp; //BibleTimeApp may initialize this object
        friend int main(int argc, char* argv[]); //main may set the printer

        //Empty virtuaual destructor
        virtual ~CPointers() {}

        /** Set the backend.
        * @param backend Pointer to the new application-wide Sword backend
        */
        static void setBackend(CSwordBackend* const backend);
        /** Set the info display.
        * @param iDisplay The pointer to the new info display.
        */
        static void setInfoDisplay(InfoDisplay::CInfoDisplay* const iDisplay);

        /** Delete the backend. Should be called by BibleTimeApp,
        * because the backend should be deleted as late as possible.
        */
        static void deleteBackend();
        /** Delete the printer. Should be called by BibleTimeApp,
        * because the printer should be deleted as late as possible.
        */
        static void deletePrinter();
        /** Delete the language manager. Should be called by BibleTimeApp,
        * because the language manager should be deleted as late as possible.
        */
        static void deleteLanguageMgr();
        /** Delete the display template manager. Should be called by BibleTimeApp,
        * because the template manager should be deleted as late as possible.
        */
        static void deleteDisplayTemplateMgr();

    public: // Public methods
        /** Returns a pointer to the backend
        * @return The backend pointer.
        */
        inline static CSwordBackend* backend();
        /** Returns a pointer to the language manager
        * @return The language manager
        */
        inline static CLanguageMgr* languageMgr();
        /** Returns a pointer to the info display.
        * @return The backend pointer.
        */
        inline static InfoDisplay::CInfoDisplay* infoDisplay();
        /** Returns a pointer to the application's display template manager
        * @return The backend pointer.
        */
        static CDisplayTemplateMgr* displayTemplateManager();

        struct PointerCache {
            PointerCache() {
                backend = 0;
                langMgr = 0;
                infoDisplay = 0;
                displayTemplateMgr = 0;
            };

            CSwordBackend* backend;
            CLanguageMgr*  langMgr;
            InfoDisplay::CInfoDisplay*  infoDisplay;
            CDisplayTemplateMgr* displayTemplateMgr;
        };
};

extern CPointers::PointerCache m_pointerCache;

/** Returns a pointer to the backend ... */
inline CSwordBackend* CPointers::backend() {
    return m_pointerCache.backend;
}

/** Returns a pointer to the backend ... */
inline CLanguageMgr* CPointers::languageMgr() {
    if (!m_pointerCache.langMgr) {
        m_pointerCache.langMgr = new CLanguageMgr();
    }
    return m_pointerCache.langMgr;
}

/** Returns a pointer to the printer object. */
inline InfoDisplay::CInfoDisplay* CPointers::infoDisplay() {
    return m_pointerCache.infoDisplay;
}


#endif
