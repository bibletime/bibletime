/******************************************************************************
 *  rawfiles.h - code for class 'RawFiles'- a module that produces HTML HREFs
 *            pointing to actual text desired.  Uses standard
 *            files:    ot and nt using indexs ??.bks ??.cps ??.vss
 *
 * $Id: rawfiles.h 2599 2011-02-12 05:11:09Z scribe $
 *
 * Copyright 1998 CrossWire Bible Society (http://www.crosswire.org)
 *    CrossWire Bible Society
 *    P. O. Box 2528
 *    Tempe, AZ  85280-2528
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */

#ifndef RAWFILES_H
#define RAWFILES_H

#include <rawverse.h>
#include <swcom.h>

#include <defs.h>

SWORD_NAMESPACE_START

class SWDLLEXPORT RawFiles : public RawVerse, public SWCom {

    const char *getNextFilename();

public:


    RawFiles(const char *ipath, const char *iname = 0, const char *idesc = 0,
            SWDisplay *idisp = 0, SWTextEncoding encoding = ENC_UNKNOWN,
            SWTextDirection dir = DIRECTION_LTR, SWTextMarkup markup = FMT_UNKNOWN,
            const char *ilang = 0);
    virtual ~RawFiles();
    virtual SWBuf &getRawEntryBuf();

    // write interface ----------------------------
    /** Is the module writable? :)
    * @return yes or no
    */
    virtual bool isWritable();

    /** Creates a new module
    * @param path The first parameter is path of the new module
    * @return error
    */
    static char createModule(const char *);

    /** Modify the current module entry text
    * - only if module @ref isWritable
    * @return *this
    */
    virtual void setEntry(const char *inbuf, long len = -1);    // Modify current module entry

    /** Link the current module entry to another module entry
    * - only if module @ref isWritable
    * @return *this
    */
    virtual void linkEntry(const SWKey *linkKey);    // Link current module entry to other module entry

    /** Delete current module entry - only if module @ref isWritable
    *
    */
    virtual void deleteEntry();
    // end write interface ------------------------


    // OPERATORS -----------------------------------------------------------------

    SWMODULE_OPERATORS

};

SWORD_NAMESPACE_END
#endif
