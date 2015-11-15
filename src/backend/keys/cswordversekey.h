/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CSWORDVERSEKEY_H
#define CSWORDVERSEKEY_H

#include "cswordkey.h"

#include <QString>

// Sword includes:
#include <versekey.h>


class CSwordModuleInfo;

/**
 * The CSwordKey implementation for verse based modules (Bibles and Commentaries)
 *
 * This class is the implementation of CKey for verse based modules like
 * Bibles and commentaries.
 * This class provides the special functions to work with the verse based modules.
 *
 * Useful functions are
 * @see NextBook()
 * @see PreviousBook()
 * @see NextChapter()
 * @see PreviousChapter()
 * @see NextVerse()
 * @see PreviousVerse().
 *
 * Call the constructor only with valid verse based modules, otherwise this key
 * will be invalid and the application will probably crash.
 *
 * @version $Id: cswordversekey.h,v 1.26 2006/02/25 11:38:15 joachim Exp $
 * @short CSwordKey implementation for Sword's VerseKey.
 * @author The BibleTime team
 */

class CSwordVerseKey : public CSwordKey, public sword::VerseKey {

    public:
        enum JumpType {
            UseBook,
            UseChapter,
            UseVerse
        };

        /**
          Constructs a versekey with the current module position and setups
          the m_module members.
        */
        CSwordVerseKey(const CSwordModuleInfo *module);

        CSwordVerseKey(const CSwordVerseKey &copy);

        /**
         * Constructs a CSwordVerseKey using the given module at the position given
         * by the versekey.
         *
         * \param[in] k Position to use.
         * \param[in] module Module to use.
         */
        CSwordVerseKey(const sword::VerseKey *k,
                       const CSwordModuleInfo *module);

        /**
          Reimplementation of CSwordKey::copy().
        */
        virtual CSwordKey* copy() const override;

        /**
        * Set/get the key. If the parameter is not set (means equal to QString::null)
        * the used key is returned. Otherwise the key is set and the new on ei returned.
        */
        virtual QString key() const override;

        /**
          Reimplemented from CSwordKey::setKey(const QString &key).
        */
        virtual bool setKey(const QString &key) override;

        /**
          Reimplemented from CSwordKey::setKey(const char *key).
        */
        virtual bool setKey(const char *key) override;

        /**
        * Jumps to the next entry of the given type
        */
        bool next( const JumpType type );
        /**
        * Jumps to the previous entry of the given type
        */
        bool previous ( const JumpType type );
        /**
        * This functions returns the current book as localised text, not as book numer.
        *
        * Use "char Book()" to retrieve the book number of the current book.
        * @return The name of the current book
        */
        QString book(const QString& newBook = QString::null);

        /**
          Sets the module for this key.
        */
        virtual void setModule(const CSwordModuleInfo *newModule) override;

    protected:
        /**
         * Returns the raw key appropriate for use directly with Sword.
         */
        virtual const char * rawKey() const override;

    private:
        /** Disable assignment operator    */
        CSwordVerseKey& operator= (const CSwordVerseKey&);
        /** Disable from base class to prevent compiler warnings */
        inline virtual CSwordVerseKey& operator= (const sword::VerseKey&) override {
            return (*this);
        };
};

#endif
