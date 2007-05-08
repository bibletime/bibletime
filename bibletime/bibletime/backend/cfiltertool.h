/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CFILTERTOOL_H
#define CFILTERTOOL_H

#include "util/cpointers.h"

#include "swmodule.h"
#include "swkey.h"
#include "swbuf.h"

/**CFilterTool includes some functions to use filters from Sword without multiple virtual inheritance.
  *@author The BibleTime team
  */

// class CFilterTool : public CPointers  {
// public:
//  CFilterTool();
//  virtual ~CFilterTool();
//
// protected: // Protected methods
//   /**
//  * updates the filter specific settings like colors
//  * the variables are used in the inherited filters
//  * in HandleToken()
//  */
//   void updateSettings();
//   /**
//  * This filter converts the RWP #Gen 1:1|
//  * style bible references to HTML
//  */
//   char ProcessRWPRefs(sword::SWBuf & buf, sword::SWModule* const module);
//   /**
//   * parses the verse reference ref and returns it.
//   */
//   const sword::SWBuf thmlRefEnd();
//   /**
//   * Parses the verse reference ref and returns it.
//   */
//   const sword::SWBuf parseRef(const sword::SWBuf ref, sword::SWModule * const , sword::SWKey * const, const char* lang="en", const bool insertFullRef = true);
//   /**
//   * Parses the verse reference ref and returns it.
//   */
//   const sword::SWBuf parseThMLRef(const sword::SWBuf& ref, const char* mod = 0);
//   /**
//   * Parses the verse reference ref and returns it.
//   */
//   const sword::SWBuf parseSimpleRef(const sword::SWBuf& ref, const char* lang = 0);
//
// private:
//   QString m_standardBible;
// };

#endif
