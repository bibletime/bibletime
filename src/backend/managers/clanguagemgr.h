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

#include <memory>
#include <QHash>
#include <QList>
#include <QString>
#include <QStringList>
#include <set>
#include <utility>


/**
  \brief Manages the languages and provides functions to work with them.
  \note This is a singleton.
*/
class CLanguageMgr {

public: /* Types: */

    /** \brief A language descriptor for CLanguageMgr. */
    class Language {

        friend class CLanguageMgr;

    public: /* Methods: */

        /** \returns the abbreviation of the this language. */
        QString const & abbrev() const {
            if (m_abbrev.isEmpty() && m_altAbbrevs.count())
                /* No standard abbrev but alternative ones */
                return m_altAbbrevs.first();
            return m_abbrev;
        }

        /** \returns the translated name of this language. */
        virtual QString translatedName() const = 0;

        /** \returns the english name of this language. */
        virtual QString const & englishName() const = 0;

        /** \returns a list of alternative abbreviations for this language. */
        QStringList const & alternativeAbbrevs() const
        { return m_altAbbrevs; }

        /** \returns whether this language object is valid, i.e. has an
                     abbreviation and an english name. */
        bool isValid() const
        { return (!m_abbrev.isEmpty() && !englishName().isEmpty()); }

        virtual ~Language();

    protected: /* Methods: */

        Language(Language &&) = delete;
        Language(Language const &) = delete;
        Language & operator=(Language &&) = delete;
        Language & operator=(Language const &) = delete;

        Language(QString abbrev, QStringList altAbbrevs = {})
            : m_abbrev(std::move(abbrev))
            , m_altAbbrevs(std::move(altAbbrevs))
        {}

    private: /* Fields: */

        QString const m_abbrev;
        QStringList const m_altAbbrevs;

    }; /* class Language */

    /** \returns the singleton instance, creating it if one does not exist. */
    static CLanguageMgr * instance();

    /** Destroys the singleton instance, if one exists. */
    static void destroyInstance();

    CLanguageMgr();

    virtual ~CLanguageMgr();

    /**
       \param abbrev the language abbreviation
       \returns a pointer to the language of the given abbreviation.
    */
    static std::shared_ptr<CLanguageMgr::Language const>
    languageForAbbrev(QString const & abbrev);

private: /* Fields: */

    static CLanguageMgr * m_instance;

}; /* class CLanguageMgr */
