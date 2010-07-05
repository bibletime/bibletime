/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "util/cpointers.h"

#include "frontend/cprinter.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "backend/managers/cswordbackend.h"


CPointers::PointerCache CPointers::m_pointerCache;

void CPointers::setBackend(CSwordBackend * const backend) {
    Q_ASSERT( m_pointerCache.backend == 0);
    CPointers::deleteBackend();
    m_pointerCache.backend = backend;
}

void CPointers::setInfoDisplay(InfoDisplay::CInfoDisplay * const infoDisplay) {
    Q_ASSERT( m_pointerCache.infoDisplay == 0);
    m_pointerCache.infoDisplay = infoDisplay;
}

void CPointers::deleteBackend() {
    delete m_pointerCache.backend;
    m_pointerCache.backend = 0;
}

void CPointers::deleteLanguageMgr() {
    delete m_pointerCache.langMgr;
    m_pointerCache.langMgr = 0;
}
