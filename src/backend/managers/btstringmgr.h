/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTSTRINGMGR_H
#define BTSTRINGMGR_H

#include <QString>

/* Sword includes: */
#include <stringmgr.h>


/**
  Unicode string manager implementation.

  A Qt-based sword::StringMgr is better than the default one in Sword, in case
  Sword is not compiled against ICU regarding this. However, we have no good
  means to check this, so let's use this class as default. This is currently
  done in BibleTime::initBackends() as follows:
    sword::StringMgr::setSystemStringMgr(new BtStringMgr());
*/
class BtStringMgr : public sword::StringMgr {

    public:
        /** Converts the param to an upper case Utf8 string
        * @param The text encoded in utf8 which should be turned into an upper case string
        */
        virtual char *upperUTF8(char *text, unsigned int max = 0) const;

        /** Converts the param to an uppercase latin1 string
        * @param The text encoded in latin1 which should be turned into an upper case string
        */
        virtual char *upperLatin1(char *text, unsigned int max = 0) const;

    protected:
        /** Enable Unicode support.
        * Reimplementation to show unicode support.
        */
        virtual bool supportsUnicode() const;

        /** CODE TAKEN FROM KDELIBS 3.2, which is licensed under the LGPL 2.
        *
        * This code was taken from KStringHandler, which is part of the KDE libraries.
        *
        * This function checks whether a string is utf8 or not.
        * It was taken from kdelibs so we do not depend on KDE 3.2.
        */
        bool isUtf8(const char *buf) const;
};

#endif /* BTSTRINGMGR_H */
