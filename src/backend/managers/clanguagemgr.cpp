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

#include "clanguagemgr.h"

#include <QLocale>
#include "../../util/btassert.h"
#include "../drivers/cswordmoduleinfo.h"
#include "btlocalemgr.h"
#include "cswordbackend.h"

// Sword includes:
#include <swlocale.h>


namespace {

using LanguageList = QList<std::shared_ptr<CLanguageMgr::Language const>>;
using LangMap = QHash<QString, std::shared_ptr<CLanguageMgr::Language const>>;

struct LanguageInfo {

    LanguageInfo();

    LanguageList m_langList;
    LangMap m_langMap;
    LangMap m_abbrLangMap;
};

static_assert(std::is_nothrow_move_assignable_v<LanguageInfo>, "");
static_assert(std::is_nothrow_move_constructible_v<LanguageInfo>, "");

LanguageInfo::LanguageInfo() {
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

    auto const addLanguage =
        [this](QString abbrev,
               QString englishName,
               QStringList altAbbrevs = {})
        {
            struct BibleTimeLanguage: CLanguageMgr::Language {

            // Methods:

                BibleTimeLanguage(QString abbrev,
                                  QString englishName,
                                  QStringList altAbbrevs)
                    : Language(std::move(abbrev), std::move(altAbbrevs))
                    , m_englishName(std::move(englishName))
                {}

                QString translatedName() const override
                { return QObject::tr(m_englishName.toUtf8()); }

                QString const & englishName() const override
                { return m_englishName; }

            // Fields:

                QString const m_englishName;

            }; // struct BibleTimeLanguage
            auto language =
                    std::make_shared<BibleTimeLanguage>(std::move(abbrev),
                                                        std::move(englishName),
                                                        std::move(altAbbrevs));
            m_langMap.insert(language->abbrev(), language);
            m_langList.append(std::move(language));
        };

    //  addLanguage("aa", QT_TRANSLATE_NOOP("QObject", "Afar"));
    //  addLanguage("ab", QT_TRANSLATE_NOOP("QObject", "Abkhazian"));
    //  addLanguage("ae", QT_TRANSLATE_NOOP("QObject", "Avestan"));
    addLanguage("af", QT_TRANSLATE_NOOP("QObject", "Afrikaans"));
    //  addLanguage("am", QT_TRANSLATE_NOOP("QObject", "Amharic"));
    addLanguage("amu", QT_TRANSLATE_NOOP("QObject", "Amuzgo, Guerrero"));
    addLanguage("ang", QT_TRANSLATE_NOOP("QObject", "English, Old (ca.450-1100)"));
    addLanguage("ar", QT_TRANSLATE_NOOP("QObject", "Arabic"));
    //  addLanguage("as", QT_TRANSLATE_NOOP("QObject", "Assamese"));
    addLanguage("az", QT_TRANSLATE_NOOP("QObject", "Azerbaijani"));
    addLanguage("azb", QT_TRANSLATE_NOOP("QObject", "Azerbaijani, South"));
    //  addLanguage("ba", QT_TRANSLATE_NOOP("QObject", "Bashkir"));
    addLanguage("bar", QT_TRANSLATE_NOOP("QObject", "Bavarian"));
    addLanguage("be", QT_TRANSLATE_NOOP("QObject", "Belarusian"));
    addLanguage("bg", QT_TRANSLATE_NOOP("QObject", "Bulgarian"));
    //  addLanguage("bh", QT_TRANSLATE_NOOP("QObject", "Bihari"));
    //  addLanguage("bi", QT_TRANSLATE_NOOP("QObject", "Bislama"));
    //  addLanguage("bn", QT_TRANSLATE_NOOP("QObject", "Bengali"));
    //  addLanguage("bo", QT_TRANSLATE_NOOP("QObject", "Tibetan"));
    addLanguage("br", QT_TRANSLATE_NOOP("QObject", "Breton"));
    addLanguage("bs", QT_TRANSLATE_NOOP("QObject", "Bosnian"));
    addLanguage("ca", QT_TRANSLATE_NOOP("QObject", "Catalan"));
    //  addLanguage("ce", QT_TRANSLATE_NOOP("QObject", "Chechen"));
    addLanguage("cco", QT_TRANSLATE_NOOP("QObject", "Chinantec, Comaltepec"));
    addLanguage("ceb", QT_TRANSLATE_NOOP("QObject", "Cebuano"));
    addLanguage("ch", QT_TRANSLATE_NOOP("QObject", "Chamorro"));
    addLanguage("chd", QT_TRANSLATE_NOOP("QObject", "Chontal, Highland Oaxaca"));
    addLanguage("chq", QT_TRANSLATE_NOOP("QObject", "Chinantec, Quiotepec"));
    addLanguage("chz", QT_TRANSLATE_NOOP("QObject", "Chinantec, Ozumac\u00edn"));
    //  addLanguage("co", QT_TRANSLATE_NOOP("QObject", "Corsican"));
    addLanguage("ckw", QT_TRANSLATE_NOOP("QObject", "Cakchiquel, Western"));
    addLanguage("cnl", QT_TRANSLATE_NOOP("QObject", "Chinantec, Lalana"));
    addLanguage("cnt", QT_TRANSLATE_NOOP("QObject", "Chinantec, Tepetotutla"));
    addLanguage("cop", QT_TRANSLATE_NOOP("QObject", "Coptic"));
    addLanguage("cs", QT_TRANSLATE_NOOP("QObject", "Czech"));
    addLanguage("cso", QT_TRANSLATE_NOOP("QObject", "Chinantec, Sochiapan"));
    addLanguage("cti", QT_TRANSLATE_NOOP("QObject", "Chol, Tila"));
    addLanguage("ctp", QT_TRANSLATE_NOOP("QObject", "Chatino, Western Highland"));
    addLanguage("cu", QT_TRANSLATE_NOOP("QObject", "Church Slavic"));
    //  addLanguage("cv", QT_TRANSLATE_NOOP("QObject", "Chuvash"));
    addLanguage("cy", QT_TRANSLATE_NOOP("QObject", "Welsh"));
    addLanguage("da", QT_TRANSLATE_NOOP("QObject", "Danish"));
    addLanguage("de", QT_TRANSLATE_NOOP("QObject", "German"));
    addLanguage("dug", QT_TRANSLATE_NOOP("QObject", "Duruma"));
    //  addLanguage("dz", QT_TRANSLATE_NOOP("QObject", "Dzongkha"));
    addLanguage("el", QT_TRANSLATE_NOOP("QObject", "Greek, Modern (1453-)"), {"gre", "ell"});
    addLanguage("en", QT_TRANSLATE_NOOP("QObject", "English"));
    addLanguage("en_US", QT_TRANSLATE_NOOP("QObject", "American English"));
    addLanguage("enm", QT_TRANSLATE_NOOP("QObject", "English, Middle (1100-1500)"));
    addLanguage("eo", QT_TRANSLATE_NOOP("QObject", "Esperanto"));
    addLanguage("es", QT_TRANSLATE_NOOP("QObject", "Spanish"));
    addLanguage("et", QT_TRANSLATE_NOOP("QObject", "Estonian"));
    addLanguage("eu", QT_TRANSLATE_NOOP("QObject", "Basque"));
    addLanguage("fa", QT_TRANSLATE_NOOP("QObject", "Persian"));
    addLanguage("fi", QT_TRANSLATE_NOOP("QObject", "Finnish"));
    //  addLanguage("fj", QT_TRANSLATE_NOOP("QObject", "Fijian"));
    //  addLanguage("fo", QT_TRANSLATE_NOOP("QObject", "Faroese"));
    addLanguage("fr", QT_TRANSLATE_NOOP("QObject", "French"));
    addLanguage("fy", QT_TRANSLATE_NOOP("QObject", "Frisian"));
    addLanguage("ga", QT_TRANSLATE_NOOP("QObject", "Irish"));
    addLanguage("gd", QT_TRANSLATE_NOOP("QObject", "Gaelic (Scots)"));
    addLanguage("gez", QT_TRANSLATE_NOOP("QObject", "Geez"));
    //  addLanguage("gl", QT_TRANSLATE_NOOP("QObject", "Gallegan"));
    //  addLanguage("gn", QT_TRANSLATE_NOOP("QObject", "Guarani"));
    //  addLanguage("gn", QT_TRANSLATE_NOOP("QObject", "Gujarati"));
    addLanguage("got", QT_TRANSLATE_NOOP("QObject", "Gothic"));
    addLanguage("gv", QT_TRANSLATE_NOOP("QObject", "Manx"));
    addLanguage("grc", QT_TRANSLATE_NOOP("QObject", "Greek, Ancient (to 1453)"));
    addLanguage("hau", QT_TRANSLATE_NOOP("QObject", "Hausa"));
    addLanguage("haw", QT_TRANSLATE_NOOP("QObject", "Hawaiian"));
    addLanguage("hbo", QT_TRANSLATE_NOOP("QObject", "Hebrew, Ancient"));
    addLanguage("he", QT_TRANSLATE_NOOP("QObject", "Hebrew"));
    addLanguage("hi", QT_TRANSLATE_NOOP("QObject", "Hindi"));
    //  addLanguage("ho", QT_TRANSLATE_NOOP("QObject", "Hiri Motu"));
    addLanguage("hr", QT_TRANSLATE_NOOP("QObject", "Croatian"));
    addLanguage("ht", QT_TRANSLATE_NOOP("QObject", "Haitian Creole"));
    addLanguage("hu", QT_TRANSLATE_NOOP("QObject", "Hungarian"));
    addLanguage("huv", QT_TRANSLATE_NOOP("QObject", "Huave, San Mateo Del Mar"));
    addLanguage("hy", QT_TRANSLATE_NOOP("QObject", "Armenian"));
    //  addLanguage("hz", QT_TRANSLATE_NOOP("QObject", "Herero"));
    //  addLanguage("ia", QT_TRANSLATE_NOOP("QObject", "Interlingua"));
    addLanguage("id", QT_TRANSLATE_NOOP("QObject", "Indonesian"));
    //  addLanguage("ie", QT_TRANSLATE_NOOP("QObject", "Interlingue"));
    //  addLanguage("ik", QT_TRANSLATE_NOOP("QObject", "Inupiaq"));
    addLanguage("is", QT_TRANSLATE_NOOP("QObject", "Icelandic"));
    addLanguage("it", QT_TRANSLATE_NOOP("QObject", "Italian"));
    addLanguage("itz", QT_TRANSLATE_NOOP("QObject", "Itz\u00e1"));
    addLanguage("ixl", QT_TRANSLATE_NOOP("QObject", "Ixil, San Juan Cotzal"));
    //  addLanguage("iu", QT_TRANSLATE_NOOP("QObject", "Inuktitut"));
    addLanguage("ja", QT_TRANSLATE_NOOP("QObject", "Japanese"));
    addLanguage("jac", QT_TRANSLATE_NOOP("QObject", "Jacalteco, Eastern"));
    addLanguage("jvn", QT_TRANSLATE_NOOP("QObject", "Javanese, Caribbean"));
    addLanguage("ka", QT_TRANSLATE_NOOP("QObject", "Georgian"));
    addLanguage("kek", QT_TRANSLATE_NOOP("QObject", "Kekchi"));
    //  addLanguage("ki", QT_TRANSLATE_NOOP("QObject", "Kikuyu"));
    //  addLanguage("kj", QT_TRANSLATE_NOOP("QObject", "Kuanyama"));
    //  addLanguage("kk", QT_TRANSLATE_NOOP("QObject", "Kazakh"));
    //  addLanguage("kl", QT_TRANSLATE_NOOP("QObject", "Kalaallisut"));
    //  addLanguage("km", QT_TRANSLATE_NOOP("QObject", "Khmer"));
    //  addLanguage("kn", QT_TRANSLATE_NOOP("QObject", "Kannada"));
    addLanguage("ko", QT_TRANSLATE_NOOP("QObject", "Korean"));
    //  addLanguage("ks", QT_TRANSLATE_NOOP("QObject", "Kashmiri"));
    addLanguage("ku", QT_TRANSLATE_NOOP("QObject", "Kurdish"));
    //  addLanguage("kv", QT_TRANSLATE_NOOP("QObject", "Komi"));
    //  addLanguage("kw", QT_TRANSLATE_NOOP("QObject", "Cornish"));
    addLanguage("ky", QT_TRANSLATE_NOOP("QObject", "Kirghiz"));
    addLanguage("la", QT_TRANSLATE_NOOP("QObject", "Latin"));
    addLanguage("lac", QT_TRANSLATE_NOOP("QObject", "Lacandon"));
    //  addLanguage("lb", QT_TRANSLATE_NOOP("QObject", "Letzeburgesch"));
    addLanguage("lmo", QT_TRANSLATE_NOOP("QObject", "Lombard"));
    //  addLanguage("ln", QT_TRANSLATE_NOOP("QObject", "Lingala"));
    //  addLanguage("lo", QT_TRANSLATE_NOOP("QObject", "Lao"));
    addLanguage("lt", QT_TRANSLATE_NOOP("QObject", "Lithuanian"));
    addLanguage("lv", QT_TRANSLATE_NOOP("QObject", "Latvian"));
    addLanguage("mg", QT_TRANSLATE_NOOP("QObject", "Malagasy"));
    //  addLanguage("mh", QT_TRANSLATE_NOOP("QObject", "Marshall"));
    addLanguage("mi", QT_TRANSLATE_NOOP("QObject", "Maori"));
    addLanguage("mir", QT_TRANSLATE_NOOP("QObject", "Mixe, Isthmus"));
    addLanguage("miz", QT_TRANSLATE_NOOP("QObject", "Mixtec, Coatzospan"));
    addLanguage("mk", QT_TRANSLATE_NOOP("QObject", "Macedonian"));
    addLanguage("mks", QT_TRANSLATE_NOOP("QObject", "Mixtec, Silacayoapan"));
    //  addLanguage("ml", QT_TRANSLATE_NOOP("QObject", "Malayalam"));
    //  addLanguage("mn", QT_TRANSLATE_NOOP("QObject", "Mongolian"));
    //  addLanguage("mo", QT_TRANSLATE_NOOP("QObject", "Moldavian"));
    addLanguage("mos", QT_TRANSLATE_NOOP("QObject", "More"));
    //  addLanguage("mr", QT_TRANSLATE_NOOP("QObject", "Marathi"));
    addLanguage("ms", QT_TRANSLATE_NOOP("QObject", "Malay"));
    addLanguage("mt", QT_TRANSLATE_NOOP("QObject", "Maltese"));
    addLanguage("mul", QT_TRANSLATE_NOOP("QObject", "(Multiple languages)"));
    addLanguage("mvc", QT_TRANSLATE_NOOP("QObject", "Mam, Central"));
    addLanguage("mvj", QT_TRANSLATE_NOOP("QObject", "Mam, Todos Santos Cuchumat\u00e1n"));
    addLanguage("mxq", QT_TRANSLATE_NOOP("QObject", "Mixe, Juquila"));
    addLanguage("mxt", QT_TRANSLATE_NOOP("QObject", "Mixtec, Jamiltepec"));
    addLanguage("my", QT_TRANSLATE_NOOP("QObject", "Burmese"));
    //  addLanguage("na", QT_TRANSLATE_NOOP("QObject", "Nauru"));
    addLanguage("nb", QT_TRANSLATE_NOOP("QObject", "Norwegian Bokm\u00e5l"));
    addLanguage("ncl", QT_TRANSLATE_NOOP("QObject", "Nahuatl, Michoac\u00e1n"));
    //  addLanguage("nd", QT_TRANSLATE_NOOP("QObject", "Ndebele, North"));
    addLanguage("nds", QT_TRANSLATE_NOOP("QObject", "Low German; Low Saxon"));
    addLanguage("ne", QT_TRANSLATE_NOOP("QObject", "Nepali"));
    addLanguage("ngu", QT_TRANSLATE_NOOP("QObject", "Nahuatl, Guerrero"));
    addLanguage("nhy", QT_TRANSLATE_NOOP("QObject", "Nahuatl, Northern Oaxaca"));
    //  addLanguage("ng", QT_TRANSLATE_NOOP("QObject", "Ndonga"));
    addLanguage("nl", QT_TRANSLATE_NOOP("QObject", "Dutch"));
    addLanguage("nn", QT_TRANSLATE_NOOP("QObject", "Norwegian Nynorsk"));
    addLanguage("no", QT_TRANSLATE_NOOP("QObject", "Norwegian"));
    //  addLanguage("nr", QT_TRANSLATE_NOOP("QObject", "Ndebele, South"));
    //  addLanguage("nv", QT_TRANSLATE_NOOP("QObject", "Navajo"));
    //  addLanguage("ny", QT_TRANSLATE_NOOP("QObject", "Chichewa; Nyanja"));
    //  addLanguage("oc", QT_TRANSLATE_NOOP("QObject", "Occitan (post 1500); Provençal"));
    //  addLanguage("om", QT_TRANSLATE_NOOP("QObject", "Oromo"));
    //  addLanguage("or", QT_TRANSLATE_NOOP("QObject", "Oriya"));
    //  addLanguage("os", QT_TRANSLATE_NOOP("QObject", "Ossetian; Ossetic"));
    addLanguage("otq", QT_TRANSLATE_NOOP("QObject", "Otomi, Quer\u00e9taro"));
    //  addLanguage("pa", QT_TRANSLATE_NOOP("QObject", "Panjabi"));
    addLanguage("pap", QT_TRANSLATE_NOOP("QObject", "Papiamento"));
    //  addLanguage("pi", QT_TRANSLATE_NOOP("QObject", "Pali"));
    addLanguage("ppk", QT_TRANSLATE_NOOP("QObject", "Uma"));
    addLanguage("pl", QT_TRANSLATE_NOOP("QObject", "Polish"));
    addLanguage("pot", QT_TRANSLATE_NOOP("QObject", "Potawatomi"));
    addLanguage("ppk", QT_TRANSLATE_NOOP("QObject", "Uma"));
    addLanguage("prs", QT_TRANSLATE_NOOP("QObject", "Persian (Dari)"));
    //  addLanguage("ps", QT_TRANSLATE_NOOP("QObject", "Pushto"));
    addLanguage("pt", QT_TRANSLATE_NOOP("QObject", "Portuguese"));
    addLanguage("pt_BR", QT_TRANSLATE_NOOP("QObject", "Brazilian Portuguese"));//added by ourself
    //  addLanguage("qu", QT_TRANSLATE_NOOP("QObject", "Quechua"));
    addLanguage("qut", QT_TRANSLATE_NOOP("QObject", "Quich\u00e9, West Central"));
    //  addLanguage("rm", QT_TRANSLATE_NOOP("QObject", "Raeto-Romance"));
    //  addLanguage("rn", QT_TRANSLATE_NOOP("QObject", "Rundi"));
    addLanguage("ro", QT_TRANSLATE_NOOP("QObject", "Romanian"));
    addLanguage("ru", QT_TRANSLATE_NOOP("QObject", "Russian"));
    //  addLanguage("rw", QT_TRANSLATE_NOOP("QObject", "Kinyarwanda"));
    //  addLanguage("sa", QT_TRANSLATE_NOOP("QObject", "Sanskrit"));
    //  addLanguage("sc", QT_TRANSLATE_NOOP("QObject", "Sardinian"));
    addLanguage("sco", QT_TRANSLATE_NOOP("QObject", "Scots"));
    //  addLanguage("sd", QT_TRANSLATE_NOOP("QObject", "Sindhi"));
    //  addLanguage("se", QT_TRANSLATE_NOOP("QObject", "Northern Sami"));
    //  addLanguage("sg", QT_TRANSLATE_NOOP("QObject", "Sango"));
    //  addLanguage("si", QT_TRANSLATE_NOOP("QObject", "Sinhalese"));
    addLanguage("sk", QT_TRANSLATE_NOOP("QObject", "Slovak"));
    addLanguage("sl", QT_TRANSLATE_NOOP("QObject", "Slovenian"));
    //  addLanguage("sm", QT_TRANSLATE_NOOP("QObject", "Samoan"));
    //  addLanguage("sn", QT_TRANSLATE_NOOP("QObject", "Shona"));
    addLanguage("so", QT_TRANSLATE_NOOP("QObject", "Somali"));
    addLanguage("sq", QT_TRANSLATE_NOOP("QObject", "Albanian"));
    //  addLanguage("sr", QT_TRANSLATE_NOOP("QObject", "Serbian"));
    addLanguage("srn", QT_TRANSLATE_NOOP("QObject", "Sranan"));
    //  addLanguage("ss", QT_TRANSLATE_NOOP("QObject", "Swati"));
    //  addLanguage("st", QT_TRANSLATE_NOOP("QObject", "Sotho, Southern"));
    //  addLanguage("su", QT_TRANSLATE_NOOP("QObject", "Sundanese"));
    addLanguage("sv", QT_TRANSLATE_NOOP("QObject", "Swedish"));
    addLanguage("sw", QT_TRANSLATE_NOOP("QObject", "Swahili"));
    addLanguage("syr", QT_TRANSLATE_NOOP("QObject", "Syriac"));
    addLanguage("ta", QT_TRANSLATE_NOOP("QObject", "Tamil"));
    //  addLanguage("te", QT_TRANSLATE_NOOP("QObject", "Telugu"));
    //  addLanguage("tg", QT_TRANSLATE_NOOP("QObject", "Tajik"));
    addLanguage("th", QT_TRANSLATE_NOOP("QObject", "Thai"));
    //  addLanguage("tk", QT_TRANSLATE_NOOP("QObject", "Turkmen"));
    addLanguage("tl", QT_TRANSLATE_NOOP("QObject", "Tagalog"));
    addLanguage("tlh", QT_TRANSLATE_NOOP("QObject", "Klingon"));
    addLanguage("tn", QT_TRANSLATE_NOOP("QObject", "Tswana"));
    addLanguage("tr", QT_TRANSLATE_NOOP("QObject", "Turkish"));
    //  addLanguage("ts", QT_TRANSLATE_NOOP("QObject", "Tsonga"));
    //  addLanguage("tt", QT_TRANSLATE_NOOP("QObject", "Tatar"));
    addLanguage("ttc", QT_TRANSLATE_NOOP("QObject", "Tektiteko"));
    //  addLanguage("tw", QT_TRANSLATE_NOOP("QObject", "Twi"));
    addLanguage("ty", QT_TRANSLATE_NOOP("QObject", "Tahitian"));
    addLanguage("tzz", QT_TRANSLATE_NOOP("QObject", "Tzotzil, Zinacant\u00e1n"));
    //  addLanguage("ug", QT_TRANSLATE_NOOP("QObject", "Uighur"));
    addLanguage("uk", QT_TRANSLATE_NOOP("QObject", "Ukrainian"));
    //  addLanguage("ur", QT_TRANSLATE_NOOP("QObject", "Urdu"));
    addLanguage("ury", QT_TRANSLATE_NOOP("QObject", "Orya"));
    addLanguage("usp", QT_TRANSLATE_NOOP("QObject", "Uspanteco"));
    //  addLanguage("uz", QT_TRANSLATE_NOOP("QObject", "Uzbek"));
    addLanguage("vi", QT_TRANSLATE_NOOP("QObject", "Vietnamese"));
    //  addLanguage("vo", QT_TRANSLATE_NOOP("QObject", "Volapük"));
    //  addLanguage("wo", QT_TRANSLATE_NOOP("QObject", "Wolof"));
    addLanguage("xh", QT_TRANSLATE_NOOP("QObject", "Xhosa"));
    addLanguage("xtd", QT_TRANSLATE_NOOP("QObject", "Mixtec, Diuxi-Tilantongo"));
    addLanguage("yi", QT_TRANSLATE_NOOP("QObject", "Yiddish"));
    addLanguage("yo", QT_TRANSLATE_NOOP("QObject", "Yoruba"));
    //  addLanguage("za", QT_TRANSLATE_NOOP("QObject", "Zhuang"));
    addLanguage("zab", QT_TRANSLATE_NOOP("QObject", "Zapotec, San Juan Guelav\u00eda"));
    addLanguage("zaw", QT_TRANSLATE_NOOP("QObject", "Zapotec, Mitla"));
    addLanguage("zh", QT_TRANSLATE_NOOP("QObject", "Chinese"));
    addLanguage("zpo", QT_TRANSLATE_NOOP("QObject", "Zapotec, Amatl\u00e1n"));
    addLanguage("zpq", QT_TRANSLATE_NOOP("QObject", "Zapotec, Zoogocho"));
    addLanguage("zpu", QT_TRANSLATE_NOOP("QObject", "Zapotec, Yal\u00e1lag"));
    addLanguage("zpv", QT_TRANSLATE_NOOP("QObject", "Zapotec, Chichicapan"));
    addLanguage("zsr", QT_TRANSLATE_NOOP("QObject", "Zapotec, Southern Rincon"));
    addLanguage("ztq", QT_TRANSLATE_NOOP("QObject", "Zapotec, Quioquitani-Quier\u00ed"));
    addLanguage("zty", QT_TRANSLATE_NOOP("QObject", "Zapotec, Yatee"));
    addLanguage("zu", QT_TRANSLATE_NOOP("QObject", "Zulu"));
}

} // anonymous namespace

/****************************************************/
/******************** CLanguageMgr ******************/
/****************************************************/

CLanguageMgr::Language::~Language() = default;

CLanguageMgr *CLanguageMgr::m_instance = nullptr;

void CLanguageMgr::destroyInstance() {
    delete m_instance;
    m_instance = nullptr;
}

CLanguageMgr *CLanguageMgr::instance() {
    if (m_instance == nullptr) {
        m_instance = new CLanguageMgr();
    }

    return m_instance;
}

CLanguageMgr::CLanguageMgr() {
}

CLanguageMgr::~CLanguageMgr() = default;

std::shared_ptr<CLanguageMgr::Language const>
CLanguageMgr::languageForAbbrev(QString const & abbrev) {
    static LanguageInfo info;

    auto it(info.m_langMap.find(abbrev));
    if (it != info.m_langMap.constEnd()) return *it; //Language is already here

    //try to search in the alternative abbrevs
    for (auto const & lang : info.m_langList)
        if (lang->alternativeAbbrevs().contains(abbrev))
            return lang;
    Q_ASSERT(abbrev != "en");

    if (auto const it = info.m_abbrLangMap.find(abbrev);
        it != info.m_abbrLangMap.end())
        return it.value();

    std::shared_ptr<Language const> newLang;
    if (auto * const locale = BtLocaleMgr::localeTranslator()) {
        // Attempt to retrieve english name:
        auto englishName = abbrev;
        {
            auto const abbrevEn = abbrev + ".en";
            auto newEnglishName(
                        QString::fromUtf8(
                            locale->translate(abbrevEn.toUtf8().constData())));
            if (newEnglishName != abbrevEn)
                englishName = std::move(newEnglishName);
        }

        struct SwordLanguage: Language {

        // Methods:

            SwordLanguage(QString abbrev, QString englishName)
                : Language(std::move(abbrev))
                , m_englishName(std::move(englishName))
            {}

            QString translatedName() const override {
                if (auto * const locale = BtLocaleMgr::localeTranslator()) {
                    QStringList tryTranslateNames;
                    {
                        auto localeName = QLocale().name();
                        while (!localeName.isEmpty() && localeName != "en") {
                            tryTranslateNames.append(
                                        m_abbrev + '.' + localeName);
                            if (localeName == m_abbrev)
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
                return m_englishName;
            }

            QString const & englishName() const override
            { return m_englishName; }

        // Fields:

            QString const m_englishName;

        }; // struct SwordLanguage

        newLang = std::make_shared<SwordLanguage>(abbrev,
                                                  std::move(englishName));
    } else {
        struct WeirdLanguage: Language {
            WeirdLanguage(QString abbrev) : Language(std::move(abbrev)) {}
            QString translatedName() const override { return abbrev(); }
            QString const & englishName() const override { return abbrev(); }
        }; // struct WeirdLanguage
        newLang = std::make_shared<WeirdLanguage>(abbrev);
    }
    info.m_abbrLangMap.insert(abbrev, newLang);

    return newLang;
}
