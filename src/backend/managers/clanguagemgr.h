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
#include <QString>
#include <QStringList>


namespace CLanguageMgr {

class Language;

/**
   \param[in] input The potentially invalid BCP 47 string from Sword to fix.
   \returns a string (hopefully) more conformant to BCP 47
 */
QString fixSwordBcp47(QString input);

/**
   \param[in] abbrev the language abbreviation in BCP 47 format.
   \returns a pointer to the language of the given abbreviation.
*/
std::shared_ptr<Language const> languageForAbbrev(QString const & abbrev);


/** \brief A language descriptor for CLanguageMgr. */
class Language {

    friend std::shared_ptr<Language const> languageForAbbrev(QString const &);

public: /* Methods: */

    Language(Language &&) = delete;
    Language(Language const &) = delete;
    Language(QStringList abbrevs, QString englishName);

    virtual ~Language();

    Language & operator=(Language &&) = delete;
    Language & operator=(Language const &) = delete;

    /** \returns the first abbreviation of this language. */
    QString const & abbrev() const { return m_abbrevs.first(); }

    /** \returns the abbreviations of this language. */
    QStringList const & abbrevs() const { return m_abbrevs; }

    /** \returns the translated name of this language. */
    virtual QString translatedName() const = 0;

    /** \returns the english name of this language. */
    QString const & englishName() const noexcept { return m_englishName; }

private: /* Fields: */

    QStringList const m_abbrevs;
    QString const m_englishName;

}; /* class Language */

} /* namespace CLanguageMgr */
