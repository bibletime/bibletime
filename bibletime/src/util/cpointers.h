/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CPOINTERS_H
#define CPOINTERS_H

#include "backend/managers/clanguagemgr.h"


class CSwordBackend;
class CLanguageMgr;

namespace InfoDisplay {
class CInfoDisplay;
}

/**
  Holds the pointers to important classes like modules, backend etc.
*/
namespace CPointers {

struct PointerCache {
    PointerCache() {
        backend = 0;
        langMgr = 0;
        infoDisplay = 0;
    };

    CSwordBackend *backend;
    CLanguageMgr *langMgr;
    InfoDisplay::CInfoDisplay *infoDisplay;
};

extern PointerCache m_pointerCache;

/**
  Sets the backend.
  \param backend Pointer to the new application-wide Sword backend.
*/
void setBackend(CSwordBackend* const backend);

/**
  Sets the info display.
  \param iDisplay The pointer to the new info display.
*/
void setInfoDisplay(InfoDisplay::CInfoDisplay* const iDisplay);

/**
  Delete the backend. Should be called by BibleTimeApp, because the backend
  should be deleted as late as possible.
*/
void deleteBackend();

/**
  Delete the printer. Should be called by BibleTimeApp, because the printer
  should be deleted as late as possible.
*/
void deletePrinter();

/**
  Delete the language manager. Should be called by BibleTimeApp, because the
  language manager should be deleted as late as possible.
*/
void deleteLanguageMgr();

/**
  Delete the display template manager. Should be called by BibleTimeApp, because
  the template manager should be deleted as late as possible.
*/
void deleteDisplayTemplateMgr();

/**
  \returns a pointer to the backend.
*/
inline CSwordBackend* backend() {
    return m_pointerCache.backend;
}

/**
  \returns a pointer to the language manager.
*/
inline CLanguageMgr* languageMgr() {
    if (!m_pointerCache.langMgr) {
        m_pointerCache.langMgr = new CLanguageMgr();
    }
    return m_pointerCache.langMgr;
}

/**
  \returns a pointer to the info display.
*/
inline InfoDisplay::CInfoDisplay* infoDisplay() {
    return m_pointerCache.infoDisplay;
}

}; // namespace CPointers


#endif
