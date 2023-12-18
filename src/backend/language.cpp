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

#include "language.h"

#include <libintl.h>
#include <QDir>
#include <QFile>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QLocale>
#include <QObject>
#include <QtGlobal>
#include <QIODevice>
#include <QStringLiteral>
#include <utility>
#include "../util/btassert.h"
#include "managers/btlocalemgr.h"

// Sword includes:
#include <swlocale.h>

namespace {

struct LanguageMap: QHash<QString, std::shared_ptr<Language const>> {
    LanguageMap();
    void addLanguage(QStringList abbrevs, QString englishName);

    const char * m_localeDir;
};

LanguageMap::LanguageMap() {
    // Developers: It's easy to get a list of used language codes from all modules:
    // Refresh all sources; go to .sword/InstallMgr/; run:
    // grep -R -hs Lang= *|cut -c 6-|sort|uniq
    // Don't remove unused languages from the source code unless you know it won't be used
    // anymore.in any module ever.

    /*:
    The string "Names of languages" doesn't actually need translation.
    It is put here to help translators notice this help text.
     -------
    The names of the languages should follow the conventions of your
    language. You can write the names with a capital first letter even if your language
    uses non-capitalized language names (they look better with capital
    first letter when they are listed).
     -------
    To find the names of all languages from internet try searching for
    "names of languages in language_x" but in your own language, e.g.
    "kielten nimet suomeksi" in Finnish or "names of languages in english"
    in English.
     -------
    You can find the language codes and names by googling for the standards
    mentioned below.
     -------
    Preference order for locale codes are:
      -------
    ISO 639-1 -------
    ISO 639-2 -------
    ISO 639-3
      -------
    x-E-XXX form is deprecated and no modules in repositories use it.
    If you find a module with x-E-XXX language, update the module.
    */
    QObject::tr("Names of languages", "No need to translate - see the longer comment (If there is no longer comment, it doesn't work yet :)) ------ ");

    // Initialize gettext with correct domain and code set
    m_localeDir = "/usr/share/iso-codes/json/";
    bindtextdomain("iso_639-3", m_localeDir);
    bind_textdomain_codeset("iso_639-3", "UTF-8");
    textdomain("iso_639-3");

    // Load the iso_683-3.json file in and populate the language information
    QFile iso6393(QString::fromLocal8Bit(m_localeDir) + "iso_639-3.json");

    if (iso6393.open(QIODevice::ReadOnly)) {
        QByteArray data = iso6393.readAll();
        QJsonDocument iso6393Doc(QJsonDocument::fromJson(data));

        if (iso6393Doc.isNull()) {
            qWarning("Failed to create JSON document for iso_639-3.");
        }

        if (!iso6393Doc.isObject()) {
            qWarning("JSON is not an object for iso_639-3.");
        }

        QJsonObject iso6393Obj = iso6393Doc.object();

        if (iso6393Obj.isEmpty()) {
            qWarning("JSON object for iso_639-3 is empty.");
        }

        QJsonArray iso6393Data = iso6393Obj["639-3"].toArray();

        QJsonArray::const_iterator i;

        for (auto v: iso6393Data) {
            QJsonObject entry = (v).toObject();
            if (entry.contains("alpha_2")) {
                if (entry.contains("inverted_name")) {
                    addLanguage({entry["alpha_2"].toString(), entry["alpha_3"].toString()}, gettext(entry["inverted_name"].toString().toStdString().data()));
                } else {
                    addLanguage({entry["alpha_2"].toString(), entry["alpha_3"].toString()}, gettext(entry["name"].toString().toStdString().data()));
                }
            } else {
                if (entry.contains("inverted_name")) {
                    addLanguage({entry["alpha_3"].toString()}, gettext(entry["inverted_name"].toString().toStdString().data()));
                } else {
                    addLanguage({entry["alpha_3"].toString()}, gettext(entry["name"].toString().toStdString().data()));
                }
            }
        }
    } else {
        qWarning() << "Could not open iso_639-3.json file(" << iso6393.fileName() << ")";
    }
    // Other codes not set in iso_639-3
    // If you load the bookshelf manager there are some codes that are referenced in the various repositories that are
    // not valid ios_693-3 codes. For these we have to provide manual translations.
    addLanguage({"abq-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Abaza (Cyrillic script)"));
    addLanguage({"cop-sa"}, QT_TRANSLATE_NOOP("QObject", "Coptic"));
    addLanguage({"dng-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Dungan (Cyrillic script)"));
    addLanguage({"gag-Latn"}, QT_TRANSLATE_NOOP("QObject", "Gagauz (Latin script)"));
    addLanguage({"ing-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Degexit'an (Cyrillic script)"));
    addLanguage({"lbe-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Lak (Cyrllic script)"));
    addLanguage({"ncq"}, QT_TRANSLATE_NOOP("QObject", "Katang, Northern"));
    addLanguage({"nog-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Nogai (Cyrillic script)"));
    addLanguage({"oss-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Ossetian (Cyrillic script)"));
    addLanguage({"tyv-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Tuvinian (Cyrillic script)"));
    addLanguage({"xnj"}, QT_TRANSLATE_NOOP("QObject", "Ngoni"));
    addLanguage({"zh-Hans"}, QT_TRANSLATE_NOOP("QObject", "Chinese, Simplified"));
    addLanguage({"zh-Hant"}, QT_TRANSLATE_NOOP("QObject", "Chinese, Traditional"));
}

void LanguageMap::addLanguage(QStringList abbrevs, QString englishName)
{
    auto language =
            std::make_shared<Language>(std::move(abbrevs),
                                       std::move(englishName));
    for (auto const & abbrev : language->abbrevs()) {
        BT_ASSERT(!contains(abbrev));
        insert(abbrev, language);
    }
}
} // anonymous namespace

Language::Language(QStringList abbrevs, QString englishName)
    : m_abbrevs(std::move(abbrevs))
    , m_englishName(std::move(englishName))
{
    BT_ASSERT(!m_abbrevs.isEmpty());
    BT_ASSERT(!m_abbrevs.contains(QString()));
    BT_ASSERT(!m_englishName.isEmpty());
}

Language::~Language() = default;

QString Language::translatedName() const
{ return QObject::tr(englishName().toUtf8()); }

std::shared_ptr<Language const> Language::fromAbbrev(QString const & abbrev) {
    BT_ASSERT(!abbrev.contains('_')); // Weak check for certain BCP 47 bugs

    static LanguageMap languageMap;

    BT_ASSERT(languageMap.contains(QStringLiteral("en")));
    static auto const defaultLanguage = *languageMap.find(QStringLiteral("en"));

    if (abbrev.isEmpty())
        return defaultLanguage;

    auto it(languageMap.find(abbrev));
    if (it != languageMap.constEnd())
        return *it;

    struct SwordLanguage: Language {

        SwordLanguage(QStringList abbrevs, QString abbrev)
            : Language(
                std::move(abbrevs),
                [](QString abbrev) {
                    if (auto * const locale = BtLocaleMgr::localeTranslator()) {
                        auto const abbrevEn = abbrev + QStringLiteral(".en");
                        auto englishName(
                                    QString::fromUtf8(
                                        locale->translate(
                                            abbrevEn.toUtf8().constData())));
                        if (!englishName.isEmpty() && englishName != abbrevEn)
                            return englishName;
                    }
                    return abbrev;
                }(std::move(abbrev)))
        {}

        QString translatedName() const override {
            if (auto * const locale = BtLocaleMgr::localeTranslator()) {
                QStringList tryTranslateNames;
                {
                    auto localeName = QLocale().name();
                    while (!localeName.isEmpty()
                           && localeName != QStringLiteral("en"))
                    {
                        tryTranslateNames.append(abbrev() + '.' + localeName);
                        if (localeName == abbrev())
                            tryTranslateNames.append(localeName);
                        while (localeName.back().isLetterOrNumber()) {
                            localeName.chop(1);
                            if (localeName.isEmpty())
                                break;
                        }
                        while (!localeName.isEmpty()
                               && !localeName.back().isLetterOrNumber())
                            localeName.chop(1);
                    }
                }
                for (auto const & translateName : tryTranslateNames) {
                    auto trName =
                            QString::fromUtf8(
                                locale->translate(
                                    translateName.toUtf8().constData()));
                    if (trName != translateName)
                        return trName;
                }
            }
            return Language::translatedName();
        }

    }; // struct SwordLanguage

    auto newLang = std::make_shared<SwordLanguage>(QStringList{abbrev}, abbrev);
    languageMap.insert(abbrev, newLang);
    return newLang;
}
