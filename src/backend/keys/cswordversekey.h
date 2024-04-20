/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include "cswordkey.h"

#include <QPointer>
#include <QString>
#include "../btsignal.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#endif
#include <versekey.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#pragma GCC diagnostic pop


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

class CSwordVerseKey final : public CSwordKey {

    public: // types:
        enum JumpType {
            UseBook,
            UseChapter,
            UseVerse
        };

    public: // methods:

        #define BibleTime_CSwordVerseKey_DEFINE_COMP(op) \
            friend bool operator op(CSwordVerseKey const & lhs, \
                                    CSwordVerseKey const & rhs) \
            { \
                return std::tuple(lhs.testament(), lhs.book(), lhs.chapter(), \
                                  lhs.verse(), lhs.suffix()) op \
                       std::tuple(rhs.testament(), rhs.book(), rhs.chapter(), \
                                  rhs.verse(), rhs.suffix()); \
            }
        #if __cpp_impl_three_way_comparison >= 201907L
        BibleTime_CSwordVerseKey_DEFINE_COMP(<=>)
        #else
        BibleTime_CSwordVerseKey_DEFINE_COMP(<)
        BibleTime_CSwordVerseKey_DEFINE_COMP(<=)
        BibleTime_CSwordVerseKey_DEFINE_COMP(==)
        BibleTime_CSwordVerseKey_DEFINE_COMP(!=)
        BibleTime_CSwordVerseKey_DEFINE_COMP(>=)
        BibleTime_CSwordVerseKey_DEFINE_COMP(>)
        #endif
        #undef BibleTime_CSwordVerseKey_DEFINE_COMP

        CSwordVerseKey & operator=(CSwordVerseKey const &) = delete;

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

        ~CSwordVerseKey() override;

        sword::SWKey const & asSwordKey() const noexcept final override;

        CSwordVerseKey * copy() const final override;

        QString key() const final override;
        QString normalizedKey() const final override;

        bool setKey(const QString &key) final override;

        bool setKey(const char *key) final override;

        /**
        * Jumps to the next entry of the given type
        */
        bool next( const JumpType type = JumpType::UseVerse );
        /**
        * Jumps to the previous entry of the given type
        */
        bool previous ( const JumpType type = JumpType::UseVerse );
        /**
        * This functions returns the current book as localised text, not as book numer.
        *
        * Use "char Book()" to retrieve the book number of the current book.
        * @return The name of the current book
        */
        QString bookName() const;

        void setBookName(QString const & newBookName)
        { m_key.setBookName(newBookName.toUtf8().constData()); }

        void setModule(const CSwordModuleInfo *newModule) final override;

        CSwordVerseKey lowerBound() const;
        void setLowerBound(CSwordVerseKey const & bound);

        CSwordVerseKey upperBound() const;
        void setUpperBound(CSwordVerseKey const & bound);

        QString shortText() const
        { return QString::fromUtf8(m_key.getShortText()); }

        void setLocale(char const * const locale) { m_key.setLocale(locale); }
        bool isBoundSet() const { return m_key.isBoundSet(); }
        void setIntros(bool v) { m_key.setIntros(v); }
        char testament() const { return m_key.getTestament(); }
        void setTestament(char v) { m_key.setTestament(v); }

        /** Return book number within a testament */
        char book() const { return m_key.getBook(); }

        void setBook(char v) { m_key.setBook(v); }
        int chapter() const { return m_key.getChapter(); }
        void setChapter(int v) { m_key.setChapter(v); }
        int verse() const { return m_key.getVerse(); }
        void setVerse(int v) { m_key.setVerse(v); }
        char suffix() const { return m_key.getSuffix(); }
        void setSuffix(char v) { m_key.setSuffix(v); }
        long index() const { return m_key.getIndex(); }
        void setIndex(long v) { m_key.setIndex(v); }
        void positionToTop() { m_key.setPosition(sword::TOP); }

        /** Return book number within entire bible */
        char bibleBook() const;

        QString versification() const {
            return m_key.getVersificationSystem();
        }
        void setVersification(const char * name) {
            m_key.setVersificationSystem(name);
        }

        const BtSignal * afterChangedSignaller();

        /** This is called after a key change to emit a signal. */
        void emitAfterChanged();

    protected:

        const char * rawKey() const final override;

    private: // fields:

        sword::VerseKey m_key;
        QPointer<BtSignal> m_afterChangedSignaller;

};
