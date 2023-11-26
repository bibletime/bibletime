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
#include <iostream>

#include <QHash>
#include <QLocale>
#include <QObject>
#include <QtGlobal>
#include <QIODevice>
#include <QStringLiteral>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>
#include <utility>
#include "../util/btassert.h"
#include "managers/btlocalemgr.h"

// Sword includes:
#include <swlocale.h>

// iso-codes includes
#include <libintl.h>


namespace {

QString locale = "";


struct LanguageMap: QHash<QString, std::shared_ptr<Language const>> {
    LanguageMap();
    void addLanguage(QStringList abbrevs, QString englishName);
    void InitGetText(void);
    void LoadISOCodes(void);

    const char *localeDir;
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

    InitGetText();
    LoadISOCodes();

    // Other codes not set in iso_639-3
    addLanguage({"en-US"}, QT_TRANSLATE_NOOP("QObject", "English, American"));
    addLanguage({"pt-BR"}, QT_TRANSLATE_NOOP("QObject", "Portuguese, Brazilian"));

    addLanguage({"abq-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Abaza (Cyrillic script)"));
    addLanguage({"ady-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Adyghe (Cyrillic script)"));
    addLanguage({"agx-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Aghul (Cyrillic script)"));
    addLanguage({"alt-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Southern Altai (Cyrillic script)"));
    addLanguage({"av-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Avaric (Cyrillic script)"));
    addLanguage({"ba-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Bashkir (Cyrillic script)"));
    addLanguage({"bua-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Buriat (Cyrillic script)"));
    addLanguage({"ce-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Chechen (Cyrillic script)"));
    addLanguage({"cjs-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Shor (Cyrillic script)"));
    addLanguage({"ckt-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Chukot (Cyrillic script)"));
    addLanguage({"cop-sa"}, gettext("Coptic"));
    addLanguage({"crh-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Crimean Tatar (Cyrillic script)"));
    addLanguage({"cv-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Chuvash (Cyrillic script)"));
    addLanguage({"dar-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Dargwa (Cyrillic script)"));
    addLanguage({"dng-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Dungan (Cyrillic script)"));
    addLanguage({"gag-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Gagauz (Cyrillic script)"));
    addLanguage({"gag-Latn"}, QT_TRANSLATE_NOOP("QObject", "Gagauz (Latin script)"));
    addLanguage({"ing-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Degexit'an (Cyrillic script)"));
    addLanguage({"kaa-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Kara-Kalpak (Cyrillic script)"));
    addLanguage({"kaa-Latn"}, QT_TRANSLATE_NOOP("QObject", "Kara-Kalpak (Latin script)"));
    addLanguage({"kap-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Bezhta (Cyrillic script)"));
    addLanguage({"kjh-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Khakas (Cyrillic script)"));
    addLanguage({"kmr-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Kurdish, Northern (Cyrillic script)"));
    addLanguage({"kmr-Latn"}, QT_TRANSLATE_NOOP("QObject", "Kurdish, Northern (Latin script)"));
    addLanguage({"kpy-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Koryak (Cyrillic script)"));
    addLanguage({"ky-Arab"}, QT_TRANSLATE_NOOP("QObject", "Kirghiz (Arabic script)"));
    addLanguage({"ky-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Kirghiz (Cyrillic script)"));
    addLanguage({"lez-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Lezghian (Cyrillic script)"));
    // This next one is not in the iso_codes table - see https://iso639-3.sil.org/code/ncq
    addLanguage({"ncq"}, QT_TRANSLATE_NOOP("QObject", "Katang, Northern"));
    addLanguage({"nog-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Nogai (Cyrillic script)"));
    addLanguage({"oss-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Ossetian (Cyrillic script)"));
    addLanguage({"sah-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Yakut (Cyrillic script)"));
    addLanguage({"tk-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Turkmen (Cyrillic script)"));
    addLanguage({"tk-Latn"}, QT_TRANSLATE_NOOP("QObject", "Turkmen (Latin script)"));
    addLanguage({"tkr-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Tsakhur (Cyrillic script)"));
    addLanguage({"tt-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Tatar (Cyrillic script)"));
    addLanguage({"tyv-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Tuvinian (Cyrillic script)"));
    addLanguage({"uz-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Uzbek (Cyrillic script)"));
    addLanguage({"uz-Latn"}, QT_TRANSLATE_NOOP("QObject", "Uzbek (Latin script)"));
    addLanguage({"xal-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Kalmyk (Cyrillic script)"));
    addLanguage({"xnj"}, QT_TRANSLATE_NOOP("QObject", "Ngoni"));
    addLanguage({"yrk-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Nenets (Cyrillic script)"));
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

void LanguageMap::InitGetText(void)
{
#if !defined(__APPLE__)
    localeDir = "/usr/share/iso-codes/json/";
#else
    char *iso_codes_version = ISO_CODES_INFO.VERSION
    localeDir = sprintf("/usr/local/Cellar/iso-codes/%s/share/iso-codes/json/", iso_codes_version);
#endif

    bindtextdomain("iso_639-3", localeDir);
    bind_textdomain_codeset("iso_639-3", "UTF-8");
    textdomain("iso_639-3");
}

void LanguageMap::LoadISOCodes(void)
{
    QFile iso6393(QString::fromLocal8Bit(localeDir) + "iso_639-3.json");

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

        for (i = iso6393Data.constBegin(); i != iso6393Data.constEnd(); ++i) {
            QJsonObject entry = (*i).toObject();
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
{
    return QObject::tr(englishName().toUtf8());
}

std::shared_ptr<Language const> Language::fromAbbrev(QString const & abbrev) {
    BT_ASSERT(!abbrev.contains('_')); // Weak check for certain BCP 47 bugs

    static LanguageMap languageMap;

    static auto const defaultLanguage = *languageMap.find("en");
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
