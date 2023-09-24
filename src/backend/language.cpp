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

#include <QHash>
#include <QLocale>
#include <QObject>
#include <QtGlobal>
#include <utility>
#include "../util/btassert.h"
#include "managers/btlocalemgr.h"

// Sword includes:
#include <swlocale.h>


namespace {

struct LanguageMap: QHash<QString, std::shared_ptr<Language const>> {
    LanguageMap();
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

    auto const addLanguage =
        [this](QStringList abbrevs, QString englishName) {
            auto language =
                    std::make_shared<Language>(std::move(abbrevs),
                                               std::move(englishName));
            for (auto const & abbrev : language->abbrevs()) {
                BT_ASSERT(!contains(abbrev));
                insert(abbrev, language);
            }
        };

    //  addLanguage({QStringLiteral("aa")}, QT_TRANSLATE_NOOP("QObject", "Afar"));
    //  addLanguage({QStringLiteral("ab")}, QT_TRANSLATE_NOOP("QObject", "Abkhazian"));
    //  addLanguage({QStringLiteral("ae")}, QT_TRANSLATE_NOOP("QObject", "Avestan"));
    addLanguage({QStringLiteral("af")}, QT_TRANSLATE_NOOP("QObject", "Afrikaans"));
    //  addLanguage({QStringLiteral("am")}, QT_TRANSLATE_NOOP("QObject", "Amharic"));
    addLanguage({QStringLiteral("amu")}, QT_TRANSLATE_NOOP("QObject", "Amuzgo, Guerrero"));
    addLanguage({QStringLiteral("ang")}, QT_TRANSLATE_NOOP("QObject", "English, Old (ca.450-1100)"));
    addLanguage({QStringLiteral("ar")}, QT_TRANSLATE_NOOP("QObject", "Arabic"));
    //  addLanguage({QStringLiteral("as")}, QT_TRANSLATE_NOOP("QObject", "Assamese"));
    addLanguage({QStringLiteral("az")}, QT_TRANSLATE_NOOP("QObject", "Azerbaijani"));
    addLanguage({QStringLiteral("azb")}, QT_TRANSLATE_NOOP("QObject", "Azerbaijani, South"));
    //  addLanguage({QStringLiteral("ba")}, QT_TRANSLATE_NOOP("QObject", "Bashkir"));
    addLanguage({QStringLiteral("bar")}, QT_TRANSLATE_NOOP("QObject", "Bavarian"));
    addLanguage({QStringLiteral("be")}, QT_TRANSLATE_NOOP("QObject", "Belarusian"));
    addLanguage({QStringLiteral("bg")}, QT_TRANSLATE_NOOP("QObject", "Bulgarian"));
    //  addLanguage({QStringLiteral("bh")}, QT_TRANSLATE_NOOP("QObject", "Bihari"));
    //  addLanguage({QStringLiteral("bi")}, QT_TRANSLATE_NOOP("QObject", "Bislama"));
    //  addLanguage({QStringLiteral("bn")}, QT_TRANSLATE_NOOP("QObject", "Bengali"));
    //  addLanguage({QStringLiteral("bo")}, QT_TRANSLATE_NOOP("QObject", "Tibetan"));
    addLanguage({QStringLiteral("br")}, QT_TRANSLATE_NOOP("QObject", "Breton"));
    addLanguage({QStringLiteral("bs")}, QT_TRANSLATE_NOOP("QObject", "Bosnian"));
    addLanguage({QStringLiteral("ca")}, QT_TRANSLATE_NOOP("QObject", "Catalan"));
    //  addLanguage({QStringLiteral("ce")}, QT_TRANSLATE_NOOP("QObject", "Chechen"));
    addLanguage({QStringLiteral("cco")}, QT_TRANSLATE_NOOP("QObject", "Chinantec, Comaltepec"));
    addLanguage({QStringLiteral("ceb")}, QT_TRANSLATE_NOOP("QObject", "Cebuano"));
    addLanguage({QStringLiteral("ch")}, QT_TRANSLATE_NOOP("QObject", "Chamorro"));
    addLanguage({QStringLiteral("chd")}, QT_TRANSLATE_NOOP("QObject", "Chontal, Highland Oaxaca"));
    addLanguage({QStringLiteral("chq")}, QT_TRANSLATE_NOOP("QObject", "Chinantec, Quiotepec"));
    addLanguage({QStringLiteral("chz")}, QT_TRANSLATE_NOOP("QObject", "Chinantec, Ozumac\u00edn"));
    //  addLanguage({QStringLiteral("co")}, QT_TRANSLATE_NOOP("QObject", "Corsican"));
    addLanguage({QStringLiteral("ckw")}, QT_TRANSLATE_NOOP("QObject", "Cakchiquel, Western"));
    addLanguage({QStringLiteral("cnl")}, QT_TRANSLATE_NOOP("QObject", "Chinantec, Lalana"));
    addLanguage({QStringLiteral("cnt")}, QT_TRANSLATE_NOOP("QObject", "Chinantec, Tepetotutla"));
    addLanguage({QStringLiteral("cop")}, QT_TRANSLATE_NOOP("QObject", "Coptic"));
    addLanguage({QStringLiteral("cs")}, QT_TRANSLATE_NOOP("QObject", "Czech"));
    addLanguage({QStringLiteral("cso")}, QT_TRANSLATE_NOOP("QObject", "Chinantec, Sochiapan"));
    addLanguage({QStringLiteral("cti")}, QT_TRANSLATE_NOOP("QObject", "Chol, Tila"));
    addLanguage({QStringLiteral("ctp")}, QT_TRANSLATE_NOOP("QObject", "Chatino, Western Highland"));
    addLanguage({QStringLiteral("cu")}, QT_TRANSLATE_NOOP("QObject", "Church Slavic"));
    //  addLanguage({QStringLiteral("cv")}, QT_TRANSLATE_NOOP("QObject", "Chuvash"));
    addLanguage({QStringLiteral("cy")}, QT_TRANSLATE_NOOP("QObject", "Welsh"));
    addLanguage({QStringLiteral("da")}, QT_TRANSLATE_NOOP("QObject", "Danish"));
    addLanguage({QStringLiteral("de")}, QT_TRANSLATE_NOOP("QObject", "German"));
    addLanguage({QStringLiteral("dug")}, QT_TRANSLATE_NOOP("QObject", "Duruma"));
    //  addLanguage({QStringLiteral("dz")}, QT_TRANSLATE_NOOP("QObject", "Dzongkha"));
    addLanguage({QStringLiteral("el"), QStringLiteral("gre"), QStringLiteral("ell")}, QT_TRANSLATE_NOOP("QObject", "Greek, Modern (1453-)"));
    addLanguage({QStringLiteral("en")}, QT_TRANSLATE_NOOP("QObject", "English"));
    addLanguage({QStringLiteral("en-US")}, QT_TRANSLATE_NOOP("QObject", "American English"));
    addLanguage({QStringLiteral("enm")}, QT_TRANSLATE_NOOP("QObject", "English, Middle (1100-1500)"));
    addLanguage({QStringLiteral("eo")}, QT_TRANSLATE_NOOP("QObject", "Esperanto"));
    addLanguage({QStringLiteral("es")}, QT_TRANSLATE_NOOP("QObject", "Spanish"));
    addLanguage({QStringLiteral("et")}, QT_TRANSLATE_NOOP("QObject", "Estonian"));
    addLanguage({QStringLiteral("eu")}, QT_TRANSLATE_NOOP("QObject", "Basque"));
    addLanguage({QStringLiteral("fa")}, QT_TRANSLATE_NOOP("QObject", "Persian"));
    addLanguage({QStringLiteral("fi")}, QT_TRANSLATE_NOOP("QObject", "Finnish"));
    //  addLanguage({QStringLiteral("fj")}, QT_TRANSLATE_NOOP("QObject", "Fijian"));
    //  addLanguage({QStringLiteral("fo")}, QT_TRANSLATE_NOOP("QObject", "Faroese"));
    addLanguage({QStringLiteral("fr")}, QT_TRANSLATE_NOOP("QObject", "French"));
    addLanguage({QStringLiteral("fy")}, QT_TRANSLATE_NOOP("QObject", "Frisian"));
    addLanguage({QStringLiteral("ga")}, QT_TRANSLATE_NOOP("QObject", "Irish"));
    addLanguage({QStringLiteral("gd")}, QT_TRANSLATE_NOOP("QObject", "Gaelic (Scots)"));
    addLanguage({QStringLiteral("gez")}, QT_TRANSLATE_NOOP("QObject", "Geez"));
    //  addLanguage({QStringLiteral("gl")}, QT_TRANSLATE_NOOP("QObject", "Gallegan"));
    //  addLanguage({QStringLiteral("gn")}, QT_TRANSLATE_NOOP("QObject", "Guarani"));
    //  addLanguage({QStringLiteral("gn")}, QT_TRANSLATE_NOOP("QObject", "Gujarati"));
    addLanguage({QStringLiteral("got")}, QT_TRANSLATE_NOOP("QObject", "Gothic"));
    addLanguage({QStringLiteral("gv")}, QT_TRANSLATE_NOOP("QObject", "Manx"));
    addLanguage({QStringLiteral("grc")}, QT_TRANSLATE_NOOP("QObject", "Greek, Ancient (to 1453)"));
    addLanguage({QStringLiteral("hau")}, QT_TRANSLATE_NOOP("QObject", "Hausa"));
    addLanguage({QStringLiteral("haw")}, QT_TRANSLATE_NOOP("QObject", "Hawaiian"));
    addLanguage({QStringLiteral("hbo")}, QT_TRANSLATE_NOOP("QObject", "Hebrew, Ancient"));
    addLanguage({QStringLiteral("he")}, QT_TRANSLATE_NOOP("QObject", "Hebrew"));
    addLanguage({QStringLiteral("hi")}, QT_TRANSLATE_NOOP("QObject", "Hindi"));
    //  addLanguage({QStringLiteral("ho")}, QT_TRANSLATE_NOOP("QObject", "Hiri Motu"));
    addLanguage({QStringLiteral("hr")}, QT_TRANSLATE_NOOP("QObject", "Croatian"));
    addLanguage({QStringLiteral("ht")}, QT_TRANSLATE_NOOP("QObject", "Haitian Creole"));
    addLanguage({QStringLiteral("hu")}, QT_TRANSLATE_NOOP("QObject", "Hungarian"));
    addLanguage({QStringLiteral("huv")}, QT_TRANSLATE_NOOP("QObject", "Huave, San Mateo Del Mar"));
    addLanguage({QStringLiteral("hy")}, QT_TRANSLATE_NOOP("QObject", "Armenian"));
    //  addLanguage({QStringLiteral("hz")}, QT_TRANSLATE_NOOP("QObject", "Herero"));
    //  addLanguage({QStringLiteral("ia")}, QT_TRANSLATE_NOOP("QObject", "Interlingua"));
    addLanguage({QStringLiteral("id")}, QT_TRANSLATE_NOOP("QObject", "Indonesian"));
    //  addLanguage({QStringLiteral("ie")}, QT_TRANSLATE_NOOP("QObject", "Interlingue"));
    //  addLanguage({QStringLiteral("ik")}, QT_TRANSLATE_NOOP("QObject", "Inupiaq"));
    addLanguage({QStringLiteral("is")}, QT_TRANSLATE_NOOP("QObject", "Icelandic"));
    addLanguage({QStringLiteral("it")}, QT_TRANSLATE_NOOP("QObject", "Italian"));
    addLanguage({QStringLiteral("itz")}, QT_TRANSLATE_NOOP("QObject", "Itz\u00e1"));
    addLanguage({QStringLiteral("ixl")}, QT_TRANSLATE_NOOP("QObject", "Ixil, San Juan Cotzal"));
    //  addLanguage({QStringLiteral("iu")}, QT_TRANSLATE_NOOP("QObject", "Inuktitut"));
    addLanguage({QStringLiteral("ja")}, QT_TRANSLATE_NOOP("QObject", "Japanese"));
    addLanguage({QStringLiteral("jac")}, QT_TRANSLATE_NOOP("QObject", "Jacalteco, Eastern"));
    addLanguage({QStringLiteral("jvn")}, QT_TRANSLATE_NOOP("QObject", "Javanese, Caribbean"));
    addLanguage({QStringLiteral("ka")}, QT_TRANSLATE_NOOP("QObject", "Georgian"));
    addLanguage({QStringLiteral("kek")}, QT_TRANSLATE_NOOP("QObject", "Kekchi"));
    //  addLanguage({QStringLiteral("ki")}, QT_TRANSLATE_NOOP("QObject", "Kikuyu"));
    //  addLanguage({QStringLiteral("kj")}, QT_TRANSLATE_NOOP("QObject", "Kuanyama"));
    //  addLanguage({QStringLiteral("kk")}, QT_TRANSLATE_NOOP("QObject", "Kazakh"));
    //  addLanguage({QStringLiteral("kl")}, QT_TRANSLATE_NOOP("QObject", "Kalaallisut"));
    //  addLanguage({QStringLiteral("km")}, QT_TRANSLATE_NOOP("QObject", "Khmer"));
    //  addLanguage({QStringLiteral("kn")}, QT_TRANSLATE_NOOP("QObject", "Kannada"));
    addLanguage({QStringLiteral("ko")}, QT_TRANSLATE_NOOP("QObject", "Korean"));
    //  addLanguage({QStringLiteral("ks")}, QT_TRANSLATE_NOOP("QObject", "Kashmiri"));
    addLanguage({QStringLiteral("ku")}, QT_TRANSLATE_NOOP("QObject", "Kurdish"));
    //  addLanguage({QStringLiteral("kv")}, QT_TRANSLATE_NOOP("QObject", "Komi"));
    //  addLanguage({QStringLiteral("kw")}, QT_TRANSLATE_NOOP("QObject", "Cornish"));
    addLanguage({QStringLiteral("ky")}, QT_TRANSLATE_NOOP("QObject", "Kirghiz"));
    addLanguage({QStringLiteral("la")}, QT_TRANSLATE_NOOP("QObject", "Latin"));
    addLanguage({QStringLiteral("lac")}, QT_TRANSLATE_NOOP("QObject", "Lacandon"));
    //  addLanguage({QStringLiteral("lb")}, QT_TRANSLATE_NOOP("QObject", "Letzeburgesch"));
    addLanguage({QStringLiteral("lmo")}, QT_TRANSLATE_NOOP("QObject", "Lombard"));
    //  addLanguage({QStringLiteral("ln")}, QT_TRANSLATE_NOOP("QObject", "Lingala"));
    //  addLanguage({QStringLiteral("lo")}, QT_TRANSLATE_NOOP("QObject", "Lao"));
    addLanguage({QStringLiteral("lt")}, QT_TRANSLATE_NOOP("QObject", "Lithuanian"));
    addLanguage({QStringLiteral("lv")}, QT_TRANSLATE_NOOP("QObject", "Latvian"));
    addLanguage({QStringLiteral("mg")}, QT_TRANSLATE_NOOP("QObject", "Malagasy"));
    //  addLanguage({QStringLiteral("mh")}, QT_TRANSLATE_NOOP("QObject", "Marshall"));
    addLanguage({QStringLiteral("mi")}, QT_TRANSLATE_NOOP("QObject", "Maori"));
    addLanguage({QStringLiteral("mir")}, QT_TRANSLATE_NOOP("QObject", "Mixe, Isthmus"));
    addLanguage({QStringLiteral("miz")}, QT_TRANSLATE_NOOP("QObject", "Mixtec, Coatzospan"));
    addLanguage({QStringLiteral("mk")}, QT_TRANSLATE_NOOP("QObject", "Macedonian"));
    addLanguage({QStringLiteral("mks")}, QT_TRANSLATE_NOOP("QObject", "Mixtec, Silacayoapan"));
    //  addLanguage({QStringLiteral("ml")}, QT_TRANSLATE_NOOP("QObject", "Malayalam"));
    //  addLanguage({QStringLiteral("mn")}, QT_TRANSLATE_NOOP("QObject", "Mongolian"));
    //  addLanguage({QStringLiteral("mo")}, QT_TRANSLATE_NOOP("QObject", "Moldavian"));
    addLanguage({QStringLiteral("mos")}, QT_TRANSLATE_NOOP("QObject", "More"));
    //  addLanguage({QStringLiteral("mr")}, QT_TRANSLATE_NOOP("QObject", "Marathi"));
    addLanguage({QStringLiteral("ms")}, QT_TRANSLATE_NOOP("QObject", "Malay"));
    addLanguage({QStringLiteral("mt")}, QT_TRANSLATE_NOOP("QObject", "Maltese"));
    addLanguage({QStringLiteral("mul")}, QT_TRANSLATE_NOOP("QObject", "(Multiple languages)"));
    addLanguage({QStringLiteral("mvc")}, QT_TRANSLATE_NOOP("QObject", "Mam, Central"));
    addLanguage({QStringLiteral("mvj")}, QT_TRANSLATE_NOOP("QObject", "Mam, Todos Santos Cuchumat\u00e1n"));
    addLanguage({QStringLiteral("mxq")}, QT_TRANSLATE_NOOP("QObject", "Mixe, Juquila"));
    addLanguage({QStringLiteral("mxt")}, QT_TRANSLATE_NOOP("QObject", "Mixtec, Jamiltepec"));
    addLanguage({QStringLiteral("my")}, QT_TRANSLATE_NOOP("QObject", "Burmese"));
    //  addLanguage({QStringLiteral("na")}, QT_TRANSLATE_NOOP("QObject", "Nauru"));
    addLanguage({QStringLiteral("nb")}, QT_TRANSLATE_NOOP("QObject", "Norwegian Bokm\u00e5l"));
    addLanguage({QStringLiteral("ncl")}, QT_TRANSLATE_NOOP("QObject", "Nahuatl, Michoac\u00e1n"));
    //  addLanguage({QStringLiteral("nd")}, QT_TRANSLATE_NOOP("QObject", "Ndebele, North"));
    addLanguage({QStringLiteral("nds")}, QT_TRANSLATE_NOOP("QObject", "Low German; Low Saxon"));
    addLanguage({QStringLiteral("ne")}, QT_TRANSLATE_NOOP("QObject", "Nepali"));
    addLanguage({QStringLiteral("ngu")}, QT_TRANSLATE_NOOP("QObject", "Nahuatl, Guerrero"));
    addLanguage({QStringLiteral("nhy")}, QT_TRANSLATE_NOOP("QObject", "Nahuatl, Northern Oaxaca"));
    //  addLanguage({QStringLiteral("ng")}, QT_TRANSLATE_NOOP("QObject", "Ndonga"));
    addLanguage({QStringLiteral("nl")}, QT_TRANSLATE_NOOP("QObject", "Dutch"));
    addLanguage({QStringLiteral("nn")}, QT_TRANSLATE_NOOP("QObject", "Norwegian Nynorsk"));
    addLanguage({QStringLiteral("no")}, QT_TRANSLATE_NOOP("QObject", "Norwegian"));
    //  addLanguage({QStringLiteral("nr")}, QT_TRANSLATE_NOOP("QObject", "Ndebele, South"));
    //  addLanguage({QStringLiteral("nv")}, QT_TRANSLATE_NOOP("QObject", "Navajo"));
    //  addLanguage({QStringLiteral("ny")}, QT_TRANSLATE_NOOP("QObject", "Chichewa; Nyanja"));
    //  addLanguage({QStringLiteral("oc")}, QT_TRANSLATE_NOOP("QObject", "Occitan (post 1500); Provençal"));
    //  addLanguage({QStringLiteral("om")}, QT_TRANSLATE_NOOP("QObject", "Oromo"));
    //  addLanguage({QStringLiteral("or")}, QT_TRANSLATE_NOOP("QObject", "Oriya"));
    //  addLanguage({QStringLiteral("os")}, QT_TRANSLATE_NOOP("QObject", "Ossetian; Ossetic"));
    addLanguage({QStringLiteral("otq")}, QT_TRANSLATE_NOOP("QObject", "Otomi, Quer\u00e9taro"));
    //  addLanguage({QStringLiteral("pa")}, QT_TRANSLATE_NOOP("QObject", "Panjabi"));
    addLanguage({QStringLiteral("pap")}, QT_TRANSLATE_NOOP("QObject", "Papiamento"));
    //  addLanguage({QStringLiteral("pi")}, QT_TRANSLATE_NOOP("QObject", "Pali"));
    addLanguage({QStringLiteral("pl")}, QT_TRANSLATE_NOOP("QObject", "Polish"));
    addLanguage({QStringLiteral("pot")}, QT_TRANSLATE_NOOP("QObject", "Potawatomi"));
    addLanguage({QStringLiteral("ppk")}, QT_TRANSLATE_NOOP("QObject", "Uma"));
    addLanguage({QStringLiteral("prs")}, QT_TRANSLATE_NOOP("QObject", "Persian (Dari)"));
    //  addLanguage({QStringLiteral("ps")}, QT_TRANSLATE_NOOP("QObject", "Pushto"));
    addLanguage({QStringLiteral("pt")}, QT_TRANSLATE_NOOP("QObject", "Portuguese"));
    addLanguage({QStringLiteral("pt-BR")}, QT_TRANSLATE_NOOP("QObject", "Brazilian Portuguese"));
    //  addLanguage({QStringLiteral("qu")}, QT_TRANSLATE_NOOP("QObject", "Quechua"));
    addLanguage({QStringLiteral("qut")}, QT_TRANSLATE_NOOP("QObject", "Quich\u00e9, West Central"));
    //  addLanguage({QStringLiteral("rm")}, QT_TRANSLATE_NOOP("QObject", "Raeto-Romance"));
    //  addLanguage({QStringLiteral("rn")}, QT_TRANSLATE_NOOP("QObject", "Rundi"));
    addLanguage({QStringLiteral("ro")}, QT_TRANSLATE_NOOP("QObject", "Romanian"));
    addLanguage({QStringLiteral("ru")}, QT_TRANSLATE_NOOP("QObject", "Russian"));
    //  addLanguage({QStringLiteral("rw")}, QT_TRANSLATE_NOOP("QObject", "Kinyarwanda"));
    //  addLanguage({QStringLiteral("sa")}, QT_TRANSLATE_NOOP("QObject", "Sanskrit"));
    //  addLanguage({QStringLiteral("sc")}, QT_TRANSLATE_NOOP("QObject", "Sardinian"));
    addLanguage({QStringLiteral("sco")}, QT_TRANSLATE_NOOP("QObject", "Scots"));
    //  addLanguage({QStringLiteral("sd")}, QT_TRANSLATE_NOOP("QObject", "Sindhi"));
    //  addLanguage({QStringLiteral("se")}, QT_TRANSLATE_NOOP("QObject", "Northern Sami"));
    //  addLanguage({QStringLiteral("sg")}, QT_TRANSLATE_NOOP("QObject", "Sango"));
    //  addLanguage({QStringLiteral("si")}, QT_TRANSLATE_NOOP("QObject", "Sinhalese"));
    addLanguage({QStringLiteral("sk")}, QT_TRANSLATE_NOOP("QObject", "Slovak"));
    addLanguage({QStringLiteral("sl")}, QT_TRANSLATE_NOOP("QObject", "Slovenian"));
    //  addLanguage({QStringLiteral("sm")}, QT_TRANSLATE_NOOP("QObject", "Samoan"));
    //  addLanguage({QStringLiteral("sn")}, QT_TRANSLATE_NOOP("QObject", "Shona"));
    addLanguage({QStringLiteral("so")}, QT_TRANSLATE_NOOP("QObject", "Somali"));
    addLanguage({QStringLiteral("sq")}, QT_TRANSLATE_NOOP("QObject", "Albanian"));
    //  addLanguage({QStringLiteral("sr")}, QT_TRANSLATE_NOOP("QObject", "Serbian"));
    addLanguage({QStringLiteral("srn")}, QT_TRANSLATE_NOOP("QObject", "Sranan"));
    //  addLanguage({QStringLiteral("ss")}, QT_TRANSLATE_NOOP("QObject", "Swati"));
    //  addLanguage({QStringLiteral("st")}, QT_TRANSLATE_NOOP("QObject", "Sotho, Southern"));
    //  addLanguage({QStringLiteral("su")}, QT_TRANSLATE_NOOP("QObject", "Sundanese"));
    addLanguage({QStringLiteral("sv")}, QT_TRANSLATE_NOOP("QObject", "Swedish"));
    addLanguage({QStringLiteral("sw")}, QT_TRANSLATE_NOOP("QObject", "Swahili"));
    addLanguage({QStringLiteral("syr")}, QT_TRANSLATE_NOOP("QObject", "Syriac"));
    addLanguage({QStringLiteral("ta")}, QT_TRANSLATE_NOOP("QObject", "Tamil"));
    //  addLanguage({QStringLiteral("te")}, QT_TRANSLATE_NOOP("QObject", "Telugu"));
    //  addLanguage({QStringLiteral("tg")}, QT_TRANSLATE_NOOP("QObject", "Tajik"));
    addLanguage({QStringLiteral("th")}, QT_TRANSLATE_NOOP("QObject", "Thai"));
    //  addLanguage({QStringLiteral("tk")}, QT_TRANSLATE_NOOP("QObject", "Turkmen"));
    addLanguage({QStringLiteral("tl")}, QT_TRANSLATE_NOOP("QObject", "Tagalog"));
    addLanguage({QStringLiteral("tlh")}, QT_TRANSLATE_NOOP("QObject", "Klingon"));
    addLanguage({QStringLiteral("tn")}, QT_TRANSLATE_NOOP("QObject", "Tswana"));
    addLanguage({QStringLiteral("tr")}, QT_TRANSLATE_NOOP("QObject", "Turkish"));
    //  addLanguage({QStringLiteral("ts")}, QT_TRANSLATE_NOOP("QObject", "Tsonga"));
    //  addLanguage({QStringLiteral("tt")}, QT_TRANSLATE_NOOP("QObject", "Tatar"));
    addLanguage({QStringLiteral("ttc")}, QT_TRANSLATE_NOOP("QObject", "Tektiteko"));
    //  addLanguage({QStringLiteral("tw")}, QT_TRANSLATE_NOOP("QObject", "Twi"));
    addLanguage({QStringLiteral("ty")}, QT_TRANSLATE_NOOP("QObject", "Tahitian"));
    addLanguage({QStringLiteral("tzz")}, QT_TRANSLATE_NOOP("QObject", "Tzotzil, Zinacant\u00e1n"));
    //  addLanguage({QStringLiteral("ug")}, QT_TRANSLATE_NOOP("QObject", "Uighur"));
    addLanguage({QStringLiteral("uk")}, QT_TRANSLATE_NOOP("QObject", "Ukrainian"));
    //  addLanguage({QStringLiteral("ur")}, QT_TRANSLATE_NOOP("QObject", "Urdu"));
    addLanguage({QStringLiteral("ury")}, QT_TRANSLATE_NOOP("QObject", "Orya"));
    addLanguage({QStringLiteral("usp")}, QT_TRANSLATE_NOOP("QObject", "Uspanteco"));
    //  addLanguage({QStringLiteral("uz")}, QT_TRANSLATE_NOOP("QObject", "Uzbek"));
    addLanguage({QStringLiteral("vi")}, QT_TRANSLATE_NOOP("QObject", "Vietnamese"));
    //  addLanguage({QStringLiteral("vo")}, QT_TRANSLATE_NOOP("QObject", "Volapük"));
    //  addLanguage({QStringLiteral("wo")}, QT_TRANSLATE_NOOP("QObject", "Wolof"));
    addLanguage({QStringLiteral("xh")}, QT_TRANSLATE_NOOP("QObject", "Xhosa"));
    addLanguage({QStringLiteral("xtd")}, QT_TRANSLATE_NOOP("QObject", "Mixtec, Diuxi-Tilantongo"));
    addLanguage({QStringLiteral("yi")}, QT_TRANSLATE_NOOP("QObject", "Yiddish"));
    addLanguage({QStringLiteral("yo")}, QT_TRANSLATE_NOOP("QObject", "Yoruba"));
    //  addLanguage({QStringLiteral("za")}, QT_TRANSLATE_NOOP("QObject", "Zhuang"));
    addLanguage({QStringLiteral("zab")}, QT_TRANSLATE_NOOP("QObject", "Zapotec, San Juan Guelav\u00eda"));
    addLanguage({QStringLiteral("zaw")}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Mitla"));
    addLanguage({QStringLiteral("zh")}, QT_TRANSLATE_NOOP("QObject", "Chinese"));
    addLanguage({QStringLiteral("zpo")}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Amatl\u00e1n"));
    addLanguage({QStringLiteral("zpq")}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Zoogocho"));
    addLanguage({QStringLiteral("zpu")}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Yal\u00e1lag"));
    addLanguage({QStringLiteral("zpv")}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Chichicapan"));
    addLanguage({QStringLiteral("zsr")}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Southern Rincon"));
    addLanguage({QStringLiteral("ztq")}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Quioquitani-Quier\u00ed"));
    addLanguage({QStringLiteral("zty")}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Yatee"));
    addLanguage({QStringLiteral("zu")}, QT_TRANSLATE_NOOP("QObject", "Zulu"));
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
