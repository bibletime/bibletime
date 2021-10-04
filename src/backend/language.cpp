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

#include <QByteArray>
#include <QCharRef>
#include <QHash>
#include <QList>
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

    //  addLanguage({"aa"}, QT_TRANSLATE_NOOP("QObject", "Afar"));
    addLanguage({"aai"}, QT_TRANSLATE_NOOP("QObject", "Arifama-Miniafia"));
    addLanguage({"aak"}, QT_TRANSLATE_NOOP("QObject", "Ankave"));
    addLanguage({"aau"}, QT_TRANSLATE_NOOP("QObject", "Abau"));
    addLanguage({"aaz"}, QT_TRANSLATE_NOOP("QObject", "Amarasi"));
    //  addLanguage({"ab"}, QT_TRANSLATE_NOOP("QObject", "Abkhazian"));
    addLanguage({"abt"}, QT_TRANSLATE_NOOP("QObject", "Ambulas"));
    addLanguage({"abx"}, QT_TRANSLATE_NOOP("QObject", "Inabaknon"));
    addLanguage({"aby"}, QT_TRANSLATE_NOOP("QObject", "Aneme Wake"));
    addLanguage({"acf"}, QT_TRANSLATE_NOOP("QObject", "Saint Lucian Creole French"));
    addLanguage({"acr"}, QT_TRANSLATE_NOOP("QObject", "Achi"));
    addLanguage({"acu"}, QT_TRANSLATE_NOOP("QObject", "Achuar-Shiwiar"));
    addLanguage({"ady-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Adyghe"));
    addLanguage({"adz"}, QT_TRANSLATE_NOOP("QObject", "Adzera"));
    //  addLanguage({"ae"}, QT_TRANSLATE_NOOP("QObject", "Avestan"));
    addLanguage({"aer"}, QT_TRANSLATE_NOOP("QObject", "Eastern Arrernte"));
    addLanguage({"aey"}, QT_TRANSLATE_NOOP("QObject", "Amele"));
    addLanguage({"af"}, QT_TRANSLATE_NOOP("QObject", "Afrikaans"));
    addLanguage({"agd"}, QT_TRANSLATE_NOOP("QObject", "Agarabi"));
    addLanguage({"agg"}, QT_TRANSLATE_NOOP("QObject", "Angor"));
    addLanguage({"agm"}, QT_TRANSLATE_NOOP("QObject", "Angaataha"));
    addLanguage({"agn"}, QT_TRANSLATE_NOOP("QObject", "Agutaynen"));
    addLanguage({"agr"}, QT_TRANSLATE_NOOP("QObject", "Aguaruna"));
    addLanguage({"agt"}, QT_TRANSLATE_NOOP("QObject", "Central Cagayan Agta"));
    addLanguage({"agu"}, QT_TRANSLATE_NOOP("QObject", "Aguacateco"));
    addLanguage({"agx-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Aghul"));
    addLanguage({"aia"}, QT_TRANSLATE_NOOP("QObject", "Arosi"));
    addLanguage({"aii"}, QT_TRANSLATE_NOOP("QObject", "Assyrian Neo-Aramaic"));
    addLanguage({"ake"}, QT_TRANSLATE_NOOP("QObject", "Akawaio"));
    addLanguage({"akh"}, QT_TRANSLATE_NOOP("QObject", "Angal Heneng"));
    addLanguage({"alp"}, QT_TRANSLATE_NOOP("QObject", "Alune"));
    addLanguage({"alq"}, QT_TRANSLATE_NOOP("QObject", "Algonquin"));
    addLanguage({"als"}, QT_TRANSLATE_NOOP("QObject", "Tosk Albanian"));
    addLanguage({"alt-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Southern Altai"));
    addLanguage({"aly"}, QT_TRANSLATE_NOOP("QObject", "Alyawarr"));
    addLanguage({"am"}, QT_TRANSLATE_NOOP("QObject", "Amharic"));
    addLanguage({"ame"}, QT_TRANSLATE_NOOP("QObject", "Yanesha'"));
    addLanguage({"amf"}, QT_TRANSLATE_NOOP("QObject", "Hamer-Banna"));
    addLanguage({"amk"}, QT_TRANSLATE_NOOP("QObject", "Ambai"));
    addLanguage({"amm"}, QT_TRANSLATE_NOOP("QObject", "Ama (Papua New Guinea)"));
    addLanguage({"amn"}, QT_TRANSLATE_NOOP("QObject", "Amanab"));
    addLanguage({"amp"}, QT_TRANSLATE_NOOP("QObject", "Alamblak"));
    addLanguage({"amr"}, QT_TRANSLATE_NOOP("QObject", "Amarakaeri"));
    addLanguage({"amu"}, QT_TRANSLATE_NOOP("QObject", "Amuzgo, Guerrero"));
    addLanguage({"amx"}, QT_TRANSLATE_NOOP("QObject", "Anmatyerre"));
    addLanguage({"ang"}, QT_TRANSLATE_NOOP("QObject", "English, Old (ca.450-1100)"));
    addLanguage({"anh"}, QT_TRANSLATE_NOOP("QObject", "Nend"));
    addLanguage({"anv"}, QT_TRANSLATE_NOOP("QObject", "Denya"));
    addLanguage({"aoi"}, QT_TRANSLATE_NOOP("QObject", "Anindilyakwa"));
    addLanguage({"aoj"}, QT_TRANSLATE_NOOP("QObject", "Mufian"));
    addLanguage({"aom"}, QT_TRANSLATE_NOOP("QObject", "Ömie"));
    addLanguage({"aon"}, QT_TRANSLATE_NOOP("QObject", "Bumbita Arapesh"));
    addLanguage({"apb"}, QT_TRANSLATE_NOOP("QObject", "Sa'a"));
    addLanguage({"ape"}, QT_TRANSLATE_NOOP("QObject", "Bukiyip"));
    addLanguage({"apn"}, QT_TRANSLATE_NOOP("QObject", "Apinayé"));
    addLanguage({"apr"}, QT_TRANSLATE_NOOP("QObject", "Arop-Lokep"));
    addLanguage({"apu"}, QT_TRANSLATE_NOOP("QObject", "Apurinã"));
    addLanguage({"apw"}, QT_TRANSLATE_NOOP("QObject", "Western Apache"));
    addLanguage({"apy"}, QT_TRANSLATE_NOOP("QObject", "Apalaí"));
    addLanguage({"apz"}, QT_TRANSLATE_NOOP("QObject", "Safeyoka"));
    addLanguage({"ar"}, QT_TRANSLATE_NOOP("QObject", "Arabic"));
    addLanguage({"arb"}, QT_TRANSLATE_NOOP("QObject", "Arabic"));
    addLanguage({"are"}, QT_TRANSLATE_NOOP("QObject", "Arabic"));
    addLanguage({"arl"}, QT_TRANSLATE_NOOP("QObject", "Standard Arabic"));
    addLanguage({"arn"}, QT_TRANSLATE_NOOP("QObject", "Mapudungun"));
    addLanguage({"arp"}, QT_TRANSLATE_NOOP("QObject", "Arapaho"));
    addLanguage({"as"}, QT_TRANSLATE_NOOP("QObject", "Assamese"));
    addLanguage({"aso"}, QT_TRANSLATE_NOOP("QObject", "Dano"));
    addLanguage({"ata"}, QT_TRANSLATE_NOOP("QObject", "Pele-Ata"));
    addLanguage({"atb"}, QT_TRANSLATE_NOOP("QObject", "Zaiwa"));
    addLanguage({"atd"}, QT_TRANSLATE_NOOP("QObject", "Ata Manobo"));
    addLanguage({"atg"}, QT_TRANSLATE_NOOP("QObject", "Ivbie North-Okpela-Arhe"));
    addLanguage({"att"}, QT_TRANSLATE_NOOP("QObject", "Pamplona Atta"));
    addLanguage({"auc"}, QT_TRANSLATE_NOOP("QObject", "Waorani"));
    addLanguage({"aui"}, QT_TRANSLATE_NOOP("QObject", "Anuki"));
    addLanguage({"auy"}, QT_TRANSLATE_NOOP("QObject", "Awiyaana"));
    addLanguage({"av-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Avaric (Cyrillic script)"));
    addLanguage({"avt"}, QT_TRANSLATE_NOOP("QObject", "Au"));
    addLanguage({"awb"}, QT_TRANSLATE_NOOP("QObject", "Awa (Papua New Guinea)"));
    addLanguage({"awk"}, QT_TRANSLATE_NOOP("QObject", "Awabakal"));
    addLanguage({"awx"}, QT_TRANSLATE_NOOP("QObject", "Awara"));
    addLanguage({"az"}, QT_TRANSLATE_NOOP("QObject", "Azerbaijani"));
    addLanguage({"azb"}, QT_TRANSLATE_NOOP("QObject", "Azerbaijani, South"));
    addLanguage({"azg"}, QT_TRANSLATE_NOOP("QObject", "San Pedro Amuzgos Amuzgo"));
    addLanguage({"azz"}, QT_TRANSLATE_NOOP("QObject", "Highland Puebla Nahuatl"));

    //  addLanguage({"ba"}, QT_TRANSLATE_NOOP("QObject", "Bashkir"));
    addLanguage({"ba-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Bashkir (Cyrillic script)"));
    addLanguage({"bam"}, QT_TRANSLATE_NOOP("QObject", "Waimaha"));
    addLanguage({"bao"}, QT_TRANSLATE_NOOP("QObject", "Bambara"));
    addLanguage({"bar"}, QT_TRANSLATE_NOOP("QObject", "Bavarian"));
    addLanguage({"bba"}, QT_TRANSLATE_NOOP("QObject", "Baatonum"));
    addLanguage({"bbb"}, QT_TRANSLATE_NOOP("QObject", "Barai"));
    addLanguage({"bbr"}, QT_TRANSLATE_NOOP("QObject", "Girawa"));
    addLanguage({"bch"}, QT_TRANSLATE_NOOP("QObject", "Bariai"));
    addLanguage({"bco"}, QT_TRANSLATE_NOOP("QObject", "Kaluli"));
    addLanguage({"bdd"}, QT_TRANSLATE_NOOP("QObject", "Bunama"));
    addLanguage({"be"}, QT_TRANSLATE_NOOP("QObject", "Belarusian"));
    addLanguage({"bea"}, QT_TRANSLATE_NOOP("QObject", "Beaver"));
    addLanguage({"bef"}, QT_TRANSLATE_NOOP("QObject", "Benabena"));
    addLanguage({"beo"}, QT_TRANSLATE_NOOP("QObject", "Beami"));
    addLanguage({"beu"}, QT_TRANSLATE_NOOP("QObject", "Blagar"));
    addLanguage({"bg"}, QT_TRANSLATE_NOOP("QObject", "Bulgarian"));
    addLanguage({"bgs"}, QT_TRANSLATE_NOOP("QObject", "Tagabawa"));
    addLanguage({"bgt"}, QT_TRANSLATE_NOOP("QObject", "Bughotu"));
    //  addLanguage({"bh"}, QT_TRANSLATE_NOOP("QObject", "Bihari"));
    addLanguage({"bhg"}, QT_TRANSLATE_NOOP("QObject", "Binandere"));
    addLanguage({"bhl"}, QT_TRANSLATE_NOOP("QObject", "Bimin"));
    //  addLanguage({"bi"}, QT_TRANSLATE_NOOP("QObject", "Bislama"));
    addLanguage({"big"}, QT_TRANSLATE_NOOP("QObject", "Biangai"));
    addLanguage({"bjp"}, QT_TRANSLATE_NOOP("QObject", "Tanga"));
    addLanguage({"bjr"}, QT_TRANSLATE_NOOP("QObject", "Binumarien"));
    addLanguage({"bjv"}, QT_TRANSLATE_NOOP("QObject", "Bedjond"));
    addLanguage({"bkd"}, QT_TRANSLATE_NOOP("QObject", "Binukid"));
    addLanguage({"bki"}, QT_TRANSLATE_NOOP("QObject", "Baki"));
    addLanguage({"bkq"}, QT_TRANSLATE_NOOP("QObject", "Bakairí"));
    addLanguage({"bkx"}, QT_TRANSLATE_NOOP("QObject", "Baikeno"));
    addLanguage({"bla"}, QT_TRANSLATE_NOOP("QObject", "Siksika"));
    addLanguage({"blw"}, QT_TRANSLATE_NOOP("QObject", "Balangao"));
    addLanguage({"blz"}, QT_TRANSLATE_NOOP("QObject", "Balantak"));
    addLanguage({"bm"}, QT_TRANSLATE_NOOP("QObject", "Bambara"));
    addLanguage({"bmh"}, QT_TRANSLATE_NOOP("QObject", "Kein"));
    addLanguage({"bmk"}, QT_TRANSLATE_NOOP("QObject", "Ghayavi"));
    addLanguage({"bmr"}, QT_TRANSLATE_NOOP("QObject", "Muinane"));
    addLanguage({"bmu"}, QT_TRANSLATE_NOOP("QObject", "Burum-Mindik"));
    addLanguage({"bn"}, QT_TRANSLATE_NOOP("QObject", "Bengali"));
    addLanguage({"bnp"}, QT_TRANSLATE_NOOP("QObject", "Bola"));
    //  addLanguage({"bo"}, QT_TRANSLATE_NOOP("QObject", "Tibetan"));
    addLanguage({"boa"}, QT_TRANSLATE_NOOP("QObject", "Bora"));
    addLanguage({"boj"}, QT_TRANSLATE_NOOP("QObject", "Anjam"));
    addLanguage({"bon"}, QT_TRANSLATE_NOOP("QObject", "Bine"));
    addLanguage({"box"}, QT_TRANSLATE_NOOP("QObject", "Buamu"));
    addLanguage({"bpr"}, QT_TRANSLATE_NOOP("QObject", "Koronadal Blaan"));
    addLanguage({"bps"}, QT_TRANSLATE_NOOP("QObject", "Sarangani Blaan"));
    addLanguage({"bqc"}, QT_TRANSLATE_NOOP("QObject", "Boko (Benin)"));
    addLanguage({"bqp"}, QT_TRANSLATE_NOOP("QObject", "Busa"));
    addLanguage({"br"}, QT_TRANSLATE_NOOP("QObject", "Breton"));
    addLanguage({"bs"}, QT_TRANSLATE_NOOP("QObject", "Bosnian"));
    addLanguage({"bsn"}, QT_TRANSLATE_NOOP("QObject", "Barasana-Eduria"));
    addLanguage({"bsp"}, QT_TRANSLATE_NOOP("QObject", "Baga Sitemu"));
    addLanguage({"bss"}, QT_TRANSLATE_NOOP("QObject", "Akoose"));
    addLanguage({"bua-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Buriat (Cyrillic script)"));
    addLanguage({"buk"}, QT_TRANSLATE_NOOP("QObject", "Bugawac"));
    addLanguage({"bus"}, QT_TRANSLATE_NOOP("QObject", "Bokobaru"));
    addLanguage({"bvd"}, QT_TRANSLATE_NOOP("QObject", "Baeggu"));
    addLanguage({"bvr"}, QT_TRANSLATE_NOOP("QObject", "Burarra"));
    addLanguage({"bxh"}, QT_TRANSLATE_NOOP("QObject", "Buhutu"));
    addLanguage({"byr"}, QT_TRANSLATE_NOOP("QObject", "Baruya"));
    addLanguage({"byx"}, QT_TRANSLATE_NOOP("QObject", "Qaqet"));
    addLanguage({"bzd"}, QT_TRANSLATE_NOOP("QObject", "Bribri"));
    addLanguage({"bzh"}, QT_TRANSLATE_NOOP("QObject", "Mapos Buang"));
    addLanguage({"bzj"}, QT_TRANSLATE_NOOP("QObject", "Belize Kriol English"));

    addLanguage({"ca"}, QT_TRANSLATE_NOOP("QObject", "Catalan"));
    addLanguage({"caa"}, QT_TRANSLATE_NOOP("QObject", "Chortí"));
    addLanguage({"cab"}, QT_TRANSLATE_NOOP("QObject", "Garifuna"));
    addLanguage({"cac"}, QT_TRANSLATE_NOOP("QObject", "Chuj"));
    addLanguage({"caf"}, QT_TRANSLATE_NOOP("QObject", "Southern Carrier"));
    addLanguage({"cak"}, QT_TRANSLATE_NOOP("QObject", "Kaqchikel"));
    addLanguage({"cao"}, QT_TRANSLATE_NOOP("QObject", "Chácobo"));
    addLanguage({"cap"}, QT_TRANSLATE_NOOP("QObject", "Chipaya"));
    addLanguage({"car"}, QT_TRANSLATE_NOOP("QObject", "Galibi Carib"));
    addLanguage({"cav"}, QT_TRANSLATE_NOOP("QObject", "Cavineña"));
    addLanguage({"cax"}, QT_TRANSLATE_NOOP("QObject", "Chiquitano"));
    addLanguage({"cbc"}, QT_TRANSLATE_NOOP("QObject", "Carapana"));
    addLanguage({"cbi"}, QT_TRANSLATE_NOOP("QObject", "Chachi"));
    addLanguage({"cbk"}, QT_TRANSLATE_NOOP("QObject", "Chavacano"));
    addLanguage({"cbr"}, QT_TRANSLATE_NOOP("QObject", "Cashibo-Cacataibo"));
    addLanguage({"cbs"}, QT_TRANSLATE_NOOP("QObject", "Cashinahua"));
    addLanguage({"cbt"}, QT_TRANSLATE_NOOP("QObject", "Chayahuita"));
    addLanguage({"cbu"}, QT_TRANSLATE_NOOP("QObject", "Candoshi-Shapra"));
    addLanguage({"cbv"}, QT_TRANSLATE_NOOP("QObject", "Cacua"));
    addLanguage({"cco"}, QT_TRANSLATE_NOOP("QObject", "Chinantec, Comaltepec"));
    // addLanguage({"ce"}, QT_TRANSLATE_NOOP("QObject", "Chechen"));
    addLanguage({"ce-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Chechen (Cyrillic script)"));
    addLanguage({"ceb"}, QT_TRANSLATE_NOOP("QObject", "Cebuano"));
    addLanguage({"cgc"}, QT_TRANSLATE_NOOP("QObject", "Kagayanen"));
    addLanguage({"ch"}, QT_TRANSLATE_NOOP("QObject", "Chamorro"));
    addLanguage({"chd"}, QT_TRANSLATE_NOOP("QObject", "Chontal, Highland Oaxaca"));
    addLanguage({"chf"}, QT_TRANSLATE_NOOP("QObject", "Tabasco Chontal"));
    addLanguage({"chk"}, QT_TRANSLATE_NOOP("QObject", "Chuukese"));
    addLanguage({"chq"}, QT_TRANSLATE_NOOP("QObject", "Chinantec, Quiotepec"));
    addLanguage({"chr"}, QT_TRANSLATE_NOOP("QObject", "Cherokee"));
    addLanguage({"chz"}, QT_TRANSLATE_NOOP("QObject", "Chinantec, Ozumac\u00edn"));
    addLanguage({"cjo"}, QT_TRANSLATE_NOOP("QObject", "Ashéninka Pajonal"));
    addLanguage({"cjs-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Shor (Cyrillic script)"));
    addLanguage({"cjv"}, QT_TRANSLATE_NOOP("QObject", "Chuave"));
    addLanguage({"ckb"}, QT_TRANSLATE_NOOP("QObject", "Central Kurdish"));
    addLanguage({"ckw"}, QT_TRANSLATE_NOOP("QObject", "Cakchiquel, Western"));
    addLanguage({"ckt-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Chukot (Cyrillic script)"));
    addLanguage({"cle"}, QT_TRANSLATE_NOOP("QObject", "Lealao Chinantec"));
    addLanguage({"clu"}, QT_TRANSLATE_NOOP("QObject", "Caluyanun"));
    addLanguage({"cme"}, QT_TRANSLATE_NOOP("QObject", "Cerma"));
    addLanguage({"cmn"}, QT_TRANSLATE_NOOP("QObject", "Mandarin Chinese"));
    addLanguage({"cni"}, QT_TRANSLATE_NOOP("QObject", "Asháninka"));
    addLanguage({"cnl"}, QT_TRANSLATE_NOOP("QObject", "Chinantec, Lalana"));
    addLanguage({"cnt"}, QT_TRANSLATE_NOOP("QObject", "Chinantec, Tepetotutla"));
    // addLanguage({"co"}, QT_TRANSLATE_NOOP("QObject", "Corsican"));
    addLanguage({"cof"}, QT_TRANSLATE_NOOP("QObject", "Colorado"));
    addLanguage({"con"}, QT_TRANSLATE_NOOP("QObject", "Cofán"));
    addLanguage({"cop"}, QT_TRANSLATE_NOOP("QObject", "Coptic"));
    addLanguage({"cot"}, QT_TRANSLATE_NOOP("QObject", "Caquinte"));
    addLanguage({"cpa"}, QT_TRANSLATE_NOOP("QObject", "Palantla Chinantec"));
    addLanguage({"cpb"}, QT_TRANSLATE_NOOP("QObject", "Ucayali-Yurúa Ashéninka"));
    addLanguage({"cpc"}, QT_TRANSLATE_NOOP("QObject", "Ajyíninka Apurucayali"));
    addLanguage({"cpu"}, QT_TRANSLATE_NOOP("QObject", "Pichis Ashéninka"));
    addLanguage({"cpy"}, QT_TRANSLATE_NOOP("QObject", "South Ucayali Ashéninka"));
    addLanguage({"crh-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Crimean Tatar (Cyrillic script)"));
    addLanguage({"crn"}, QT_TRANSLATE_NOOP("QObject", "El Nayar Cora"));
    addLanguage({"crx"}, QT_TRANSLATE_NOOP("QObject", "Carrier"));
    addLanguage({"cs"}, QT_TRANSLATE_NOOP("QObject", "Czech"));
    addLanguage({"cso"}, QT_TRANSLATE_NOOP("QObject", "Chinantec, Sochiapan"));
    addLanguage({"cta"}, QT_TRANSLATE_NOOP("QObject", "Tataltepec Chatino"));
    addLanguage({"cti"}, QT_TRANSLATE_NOOP("QObject", "Chol, Tila"));
    addLanguage({"ctp"}, QT_TRANSLATE_NOOP("QObject", "Chatino, Western Highland"));
    addLanguage({"ctu"}, QT_TRANSLATE_NOOP("QObject", "Chol"));
    addLanguage({"cu"}, QT_TRANSLATE_NOOP("QObject", "Church Slavic"));
    addLanguage({"cub"}, QT_TRANSLATE_NOOP("QObject", "Cubeo"));
    addLanguage({"cuc"}, QT_TRANSLATE_NOOP("QObject", "Usila Chinantec"));
    addLanguage({"cui"}, QT_TRANSLATE_NOOP("QObject", "Cuiba"));
    addLanguage({"cuk"}, QT_TRANSLATE_NOOP("QObject", "San Blas Kuna"));
    addLanguage({"cut"}, QT_TRANSLATE_NOOP("QObject", "Teutila Cuicatec"));
    addLanguage({"cux"}, QT_TRANSLATE_NOOP("QObject", "Tepeuxila Cuicatec"));
    // addLanguage({"cv"}, QT_TRANSLATE_NOOP("QObject", "Chuvash"));
    addLanguage({"cv-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Chuvash (Cyrillic script)"));
    addLanguage({"cwe"}, QT_TRANSLATE_NOOP("QObject", "Kwere"));
    addLanguage({"cy"}, QT_TRANSLATE_NOOP("QObject", "Welsh"));
    addLanguage({"cya"}, QT_TRANSLATE_NOOP("QObject", "Nopala Chatino"));

    addLanguage({"da"}, QT_TRANSLATE_NOOP("QObject", "Danish"));
    addLanguage({"de"}, QT_TRANSLATE_NOOP("QObject", "German"));
    addLanguage({"dug"}, QT_TRANSLATE_NOOP("QObject", "Duruma"));
    addLanguage({"daa"}, QT_TRANSLATE_NOOP("QObject", "Dangaléat"));
    addLanguage({"dad"}, QT_TRANSLATE_NOOP("QObject", "Marik"));
    addLanguage({"dah"}, QT_TRANSLATE_NOOP("QObject", "Gwahatike"));
    addLanguage({"dar-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Dargwa (Cyrillic script)"));
    addLanguage({"ded"}, QT_TRANSLATE_NOOP("QObject", "Dedua"));
    addLanguage({"dgc"}, QT_TRANSLATE_NOOP("QObject", "Casiguran Dumagat Agta"));
    addLanguage({"dgr"}, QT_TRANSLATE_NOOP("QObject", "Dogrib"));
    addLanguage({"dgz"}, QT_TRANSLATE_NOOP("QObject", "Daga"));
    addLanguage({"dhg"}, QT_TRANSLATE_NOOP("QObject", "Dhangu"));
    addLanguage({"dif"}, QT_TRANSLATE_NOOP("QObject", "Dieri"));
    addLanguage({"dik"}, QT_TRANSLATE_NOOP("QObject", "Southwestern Dinka"));
    addLanguage({"dji"}, QT_TRANSLATE_NOOP("QObject", "Djinang"));
    addLanguage({"djk"}, QT_TRANSLATE_NOOP("QObject", "Aukan"));
    addLanguage({"djr"}, QT_TRANSLATE_NOOP("QObject", "Djambarrpuyngu"));
    addLanguage({"dng"}, QT_TRANSLATE_NOOP("QObject", "Dungan"));
    addLanguage({"dob"}, QT_TRANSLATE_NOOP("QObject", "Dobu"));
    addLanguage({"dop"}, QT_TRANSLATE_NOOP("QObject", "Lukpa"));
    addLanguage({"dwr"}, QT_TRANSLATE_NOOP("QObject", "Dawro"));
    addLanguage({"dww"}, QT_TRANSLATE_NOOP("QObject", "Dawawa"));
    addLanguage({"dwy"}, QT_TRANSLATE_NOOP("QObject", "Dhuwaya"));
    // addLanguage({"dz"}, QT_TRANSLATE_NOOP("QObject", "Dzongkha"));

    addLanguage({"el"}, QT_TRANSLATE_NOOP("QObject", "Greek, Modern (1453-)"));
    addLanguage({"en"}, QT_TRANSLATE_NOOP("QObject", "English"));
    addLanguage({"en_US"}, QT_TRANSLATE_NOOP("QObject", "American English"));
    addLanguage({"enm"}, QT_TRANSLATE_NOOP("QObject", "English, Middle (1100-1500)"));
    addLanguage({"eo"}, QT_TRANSLATE_NOOP("QObject", "Esperanto"));
    addLanguage({"es"}, QT_TRANSLATE_NOOP("QObject", "Spanish"));
    addLanguage({"et"}, QT_TRANSLATE_NOOP("QObject", "Estonian"));
    addLanguage({"eu"}, QT_TRANSLATE_NOOP("QObject", "Basque"));
    addLanguage({"ebk"}, QT_TRANSLATE_NOOP("QObject", "East Bontoc"));
    addLanguage({"eko"}, QT_TRANSLATE_NOOP("QObject", "Koti"));
    addLanguage({"emi"}, QT_TRANSLATE_NOOP("QObject", "Mussau-Emira"));
    addLanguage({"emp"}, QT_TRANSLATE_NOOP("QObject", "Northern Emberá"));
    addLanguage({"enq"}, QT_TRANSLATE_NOOP("QObject", "Enga"));
    addLanguage({"eri"}, QT_TRANSLATE_NOOP("QObject", "Ogea"));
    addLanguage({"ese"}, QT_TRANSLATE_NOOP("QObject", "Ese Ejja"));
    addLanguage({"esk"}, QT_TRANSLATE_NOOP("QObject", "Northwest Alaska Inupiatun"));
    addLanguage({"etr"}, QT_TRANSLATE_NOOP("QObject", "Edolo"));

    addLanguage({"fa"}, QT_TRANSLATE_NOOP("QObject", "Persian"));
    addLanguage({"faa"}, QT_TRANSLATE_NOOP("QObject", "Fasu"));
    addLanguage({"fai"}, QT_TRANSLATE_NOOP("QObject", "Faiwol"));
    addLanguage({"far"}, QT_TRANSLATE_NOOP("QObject", "Fataleka"));
    addLanguage({"ffm"}, QT_TRANSLATE_NOOP("QObject", "Maasina Fulfulde"));
    addLanguage({"fi"}, QT_TRANSLATE_NOOP("QObject", "Finnish"));
    //  addLanguage({"fj"}, QT_TRANSLATE_NOOP("QObject", "Fijian"));
    //  addLanguage({"fo"}, QT_TRANSLATE_NOOP("QObject", "Faroese"));
    addLanguage({"for"}, QT_TRANSLATE_NOOP("QObject", "Fore"));
    addLanguage({"fr"}, QT_TRANSLATE_NOOP("QObject", "French"));
    addLanguage({"fue"}, QT_TRANSLATE_NOOP("QObject", "Borgu Fulfulde"));
    addLanguage({"fuf"}, QT_TRANSLATE_NOOP("QObject", "Pular"));
    addLanguage({"fuh"}, QT_TRANSLATE_NOOP("QObject", "Western Niger Fulfulde"));
    addLanguage({"fy"}, QT_TRANSLATE_NOOP("QObject", "Frisian"));

    addLanguage({"ga"}, QT_TRANSLATE_NOOP("QObject", "Irish"));
    addLanguage({"gag-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Gagauz (Cyrillic script)"));
    addLanguage({"gag-Latn"}, QT_TRANSLATE_NOOP("QObject", "Gagauz (Cyrillic script)"));
    addLanguage({"gah"}, QT_TRANSLATE_NOOP("QObject", "Alekano"));
    addLanguage({"gai"}, QT_TRANSLATE_NOOP("QObject", "Borei"));
    addLanguage({"gam"}, QT_TRANSLATE_NOOP("QObject", "Kandawo"));
    addLanguage({"gaw"}, QT_TRANSLATE_NOOP("QObject", "Nobonob"));
    addLanguage({"gd"}, QT_TRANSLATE_NOOP("QObject", "Gaelic (Scots)"));
    addLanguage({"gdn"}, QT_TRANSLATE_NOOP("QObject", "Mo'da"));
    addLanguage({"gdr"}, QT_TRANSLATE_NOOP("QObject", "Wipi"));
    addLanguage({"geb"}, QT_TRANSLATE_NOOP("QObject", "Kire"));
    addLanguage({"gez"}, QT_TRANSLATE_NOOP("QObject", "Geez"));
    addLanguage({"gfk"}, QT_TRANSLATE_NOOP("QObject", "Patpatar"));
    addLanguage({"ghs"}, QT_TRANSLATE_NOOP("QObject", "Guhu-Samane"));
    addLanguage({"gia"}, QT_TRANSLATE_NOOP("QObject", "Kitja"));
    //  addLanguage({"gl"}, QT_TRANSLATE_NOOP("QObject", "Gallegan"));
    addLanguage({"glk"}, QT_TRANSLATE_NOOP("QObject", "Gilaki"));
    addLanguage({"gmv"}, QT_TRANSLATE_NOOP("QObject", "Gamo"));
    //  addLanguage({"gn"}, QT_TRANSLATE_NOOP("QObject", "Guarani"));
    //  addLanguage({"gn"}, QT_TRANSLATE_NOOP("QObject", "Gujarati"));
    addLanguage({"gng"}, QT_TRANSLATE_NOOP("QObject", "Gamo"));
    addLanguage({"gnn"}, QT_TRANSLATE_NOOP("QObject", "Gumatj"));
    addLanguage({"gnw"}, QT_TRANSLATE_NOOP("QObject", "Western Bolivian Guaraní"));
    addLanguage({"gof"}, QT_TRANSLATE_NOOP("QObject", "Gofa"));
    addLanguage({"got"}, QT_TRANSLATE_NOOP("QObject", "Gothic"));
    addLanguage({"grc"}, QT_TRANSLATE_NOOP("QObject", "Greek, Ancient (to 1453)"));
    addLanguage({"gu"}, QT_TRANSLATE_NOOP("QObject", "Gujarati"));
    addLanguage({"gub"}, QT_TRANSLATE_NOOP("QObject", "Guajajára"));
    addLanguage({"gue"}, QT_TRANSLATE_NOOP("QObject", "Gurinji"));
    addLanguage({"guh"}, QT_TRANSLATE_NOOP("QObject", "Guahibo"));
    addLanguage({"gui"}, QT_TRANSLATE_NOOP("QObject", "Eastern Bolivian Guaraní"));
    addLanguage({"gul"}, QT_TRANSLATE_NOOP("QObject", "Sea Island Creole English"));
    addLanguage({"gum"}, QT_TRANSLATE_NOOP("QObject", "Guambiano"));
    addLanguage({"gun"}, QT_TRANSLATE_NOOP("QObject", "Mbyá Guaraní"));
    addLanguage({"guo"}, QT_TRANSLATE_NOOP("QObject", "Guayabero"));
    addLanguage({"gup"}, QT_TRANSLATE_NOOP("QObject", "Gunwinggu"));
    addLanguage({"gv"}, QT_TRANSLATE_NOOP("QObject", "Manx"));
    addLanguage({"gvc"}, QT_TRANSLATE_NOOP("QObject", "Guanano"));
    addLanguage({"gvf"}, QT_TRANSLATE_NOOP("QObject", "Golin"));
    addLanguage({"gvn"}, QT_TRANSLATE_NOOP("QObject", "Kuku-Yalanji"));
    addLanguage({"gwi"}, QT_TRANSLATE_NOOP("QObject", "Gwichʼin"));
    addLanguage({"gym"}, QT_TRANSLATE_NOOP("QObject", "Ngäbere"));
    addLanguage({"gyr"}, QT_TRANSLATE_NOOP("QObject", "Guarayu"));

    addLanguage({"hau"}, QT_TRANSLATE_NOOP("QObject", "Hausa"));
    addLanguage({"hans"}, QT_TRANSLATE_NOOP("QObject", "Simplified Chinese"));
    addLanguage({"hant"}, QT_TRANSLATE_NOOP("QObject", "Traditional Chinese"));
    addLanguage({"haw"}, QT_TRANSLATE_NOOP("QObject", "Hawaiian"));
    addLanguage({"hbo"}, QT_TRANSLATE_NOOP("QObject", "Ancient Hebrew"));
    addLanguage({"hch"}, QT_TRANSLATE_NOOP("QObject", "Huichol"));
    addLanguage({"he"}, QT_TRANSLATE_NOOP("QObject", "Hebrew"));
    addLanguage({"heg"}, QT_TRANSLATE_NOOP("QObject", "Helong"));
    addLanguage({"hi"}, QT_TRANSLATE_NOOP("QObject", "Hindi"));
    addLanguage({"hix"}, QT_TRANSLATE_NOOP("QObject", "Hixkaryána"));
    addLanguage({"hla"}, QT_TRANSLATE_NOOP("QObject", "Halia"));
    addLanguage({"hlt"}, QT_TRANSLATE_NOOP("QObject", "Nga La"));
    addLanguage({"hns"}, QT_TRANSLATE_NOOP("QObject", "Caribbean Hindustani"));
    addLanguage({"ho"}, QT_TRANSLATE_NOOP("QObject", "Hiri Motu"));
    addLanguage({"hop"}, QT_TRANSLATE_NOOP("QObject", "Hopi"));
    addLanguage({"hot"}, QT_TRANSLATE_NOOP("QObject", "Hote"));
    addLanguage({"hr"}, QT_TRANSLATE_NOOP("QObject", "Croatian"));
    addLanguage({"ht"}, QT_TRANSLATE_NOOP("QObject", "Haitian Creole"));
    addLanguage({"hto"}, QT_TRANSLATE_NOOP("QObject", "Minica Huitoto"));
    addLanguage({"hu"}, QT_TRANSLATE_NOOP("QObject", "Hungarian"));
    addLanguage({"hub"}, QT_TRANSLATE_NOOP("QObject", "Huambisa"));
    addLanguage({"hui"}, QT_TRANSLATE_NOOP("QObject", "Huli"));
    addLanguage({"hus"}, QT_TRANSLATE_NOOP("QObject", "Huastec"));
    addLanguage({"huu"}, QT_TRANSLATE_NOOP("QObject", "Murui Huitoto"));
    addLanguage({"huv"}, QT_TRANSLATE_NOOP("QObject", "Huave, San Mateo Del Mar"));
    addLanguage({"hvn"}, QT_TRANSLATE_NOOP("QObject", "Sabu"));
    addLanguage({"hwc"}, QT_TRANSLATE_NOOP("QObject", "Hawai'i Creole English"));
    addLanguage({"hy"}, QT_TRANSLATE_NOOP("QObject", "Armenian"));
    //  addLanguage({"hz"}, QT_TRANSLATE_NOOP("QObject", "Herero"));

    //  addLanguage({"ia"}, QT_TRANSLATE_NOOP("QObject", "Interlingua"));
    addLanguage({"ian"}, QT_TRANSLATE_NOOP("QObject", "Iatmul"));
    addLanguage({"id"}, QT_TRANSLATE_NOOP("QObject", "Indonesian"));
    //  addLanguage({"ie"}, QT_TRANSLATE_NOOP("QObject", "Interlingue"));
    addLanguage({"ign"}, QT_TRANSLATE_NOOP("QObject", "Ignaciano"));
    //  addLanguage({"ik"}, QT_TRANSLATE_NOOP("QObject", "Inupiaq"));
    addLanguage({"ikk"}, QT_TRANSLATE_NOOP("QObject", "Ika"));
    addLanguage({"ikw"}, QT_TRANSLATE_NOOP("QObject", "Ikwere"));
    addLanguage({"is"}, QT_TRANSLATE_NOOP("QObject", "Icelandic"));
    addLanguage({"it"}, QT_TRANSLATE_NOOP("QObject", "Italian"));
    addLanguage({"itz"}, QT_TRANSLATE_NOOP("QObject", "Itz\u00e1"));
    addLanguage({"ixl"}, QT_TRANSLATE_NOOP("QObject", "Ixil, San Juan Cotzal"));
    addLanguage({"imo"}, QT_TRANSLATE_NOOP("QObject", "Imbongu"));
    addLanguage({"inb"}, QT_TRANSLATE_NOOP("QObject", "Inga"));
    addLanguage({"ino"}, QT_TRANSLATE_NOOP("QObject", "Inoke-Yate"));
    addLanguage({"iou"}, QT_TRANSLATE_NOOP("QObject", "Tuma-Irumu"));
    addLanguage({"ipi"}, QT_TRANSLATE_NOOP("QObject", "Ipili"));
    // addLanguage({"iu"}, QT_TRANSLATE_NOOP("QObject", "Inuktitut"));
    addLanguage({"iws"}, QT_TRANSLATE_NOOP("QObject", "Sepik Iwam"));

    addLanguage({"ja"}, QT_TRANSLATE_NOOP("QObject", "Japanese"));
    addLanguage({"jac"}, QT_TRANSLATE_NOOP("QObject", "Jacalteco, Eastern"));
    addLanguage({"jvn"}, QT_TRANSLATE_NOOP("QObject", "Javanese, Caribbean"));
    addLanguage({"jae"}, QT_TRANSLATE_NOOP("QObject", "Yabem"));
    addLanguage({"jao"}, QT_TRANSLATE_NOOP("QObject", "Yanyuwa"));
    addLanguage({"jic"}, QT_TRANSLATE_NOOP("QObject", "Tol"));
    addLanguage({"jiv"}, QT_TRANSLATE_NOOP("QObject", "Shuar"));

    addLanguage({"ka"}, QT_TRANSLATE_NOOP("QObject", "Georgian"));
    addLanguage({"kaa-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Kara-Kalpak (Cyrillic script)"));
    // addLanguage({"kaa-Latn"}, QT_TRANSLATE_NOOP("QObject", ""));
    addLanguage({"kap-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Kara-Kalpak (Latin script)"));
    addLanguage({"kaq"}, QT_TRANSLATE_NOOP("QObject", "Capanahua"));
    addLanguage({"kbc"}, QT_TRANSLATE_NOOP("QObject", "Kadiwéu"));
    addLanguage({"kbd"}, QT_TRANSLATE_NOOP("QObject", "Kabardian"));
    addLanguage({"kbh"}, QT_TRANSLATE_NOOP("QObject", "Camsá"));
    addLanguage({"kbm"}, QT_TRANSLATE_NOOP("QObject", "Iwal"));
    addLanguage({"kbq"}, QT_TRANSLATE_NOOP("QObject", "Kamano"));
    addLanguage({"kdc"}, QT_TRANSLATE_NOOP("QObject", "Kutu"));
    addLanguage({"kde"}, QT_TRANSLATE_NOOP("QObject", "Makonde"));
    addLanguage({"kdl"}, QT_TRANSLATE_NOOP("QObject", "Tsikimba"));
    addLanguage({"kek"}, QT_TRANSLATE_NOOP("QObject", "Kekch\u00ed"));
    addLanguage({"ken"}, QT_TRANSLATE_NOOP("QObject", "Kenyang"));
    addLanguage({"kew"}, QT_TRANSLATE_NOOP("QObject", "West Kewa"));
    addLanguage({"kgf"}, QT_TRANSLATE_NOOP("QObject", "Kube"));
    addLanguage({"kgk"}, QT_TRANSLATE_NOOP("QObject", "Kaiwá"));
    addLanguage({"kgp"}, QT_TRANSLATE_NOOP("QObject", "Kaingang"));
    addLanguage({"khs"}, QT_TRANSLATE_NOOP("QObject", "Kasua"));
    addLanguage({"khz"}, QT_TRANSLATE_NOOP("QObject", "Keapara"));
    //  addLanguage({"ki"}, QT_TRANSLATE_NOOP("QObject", "Kikuyu"));
    //  addLanguage({"kj"}, QT_TRANSLATE_NOOP("QObject", "Kuanyama"));
    addLanguage({"kje"}, QT_TRANSLATE_NOOP("QObject", "Kisar"));
    // addLanguage({"kjh"}, QT_TRANSLATE_NOOP("QObject", ""));
    addLanguage({"kjh-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Khakas"));
    addLanguage({"kjs"}, QT_TRANSLATE_NOOP("QObject", "Khakas"));
    addLanguage({"kk"}, QT_TRANSLATE_NOOP("QObject", "Kazakh"));
    addLanguage({"kkc"}, QT_TRANSLATE_NOOP("QObject", "Odoodee"));
    addLanguage({"kkl"}, QT_TRANSLATE_NOOP("QObject", "Kosarek Yale"));
    addLanguage({"kky"}, QT_TRANSLATE_NOOP("QObject", "Guguyimidjir"));
    //  addLanguage({"kl"}, QT_TRANSLATE_NOOP("QObject", "Kalaallisut"));
    addLanguage({"klt"}, QT_TRANSLATE_NOOP("QObject", "Nukna"));
    addLanguage({"klv"}, QT_TRANSLATE_NOOP("QObject", "Maskelynes"));
    addLanguage({"km"}, QT_TRANSLATE_NOOP("QObject", "Central Khmer"));
    addLanguage({"kmg"}, QT_TRANSLATE_NOOP("QObject", "Kâte"));
    addLanguage({"kmh"}, QT_TRANSLATE_NOOP("QObject", "Kalam"));
    addLanguage({"kmk"}, QT_TRANSLATE_NOOP("QObject", "Limos Kalinga"));
    addLanguage({"kmo"}, QT_TRANSLATE_NOOP("QObject", "Kwoma"));
    // addLanguage({"kmr-Cyrl"}, QT_TRANSLATE_NOOP("QObject", ""));
    addLanguage({"kmr-Latn"}, QT_TRANSLATE_NOOP("QObject", "Northern Kurdish (Cyrillic script)"));
    addLanguage({"kms"}, QT_TRANSLATE_NOOP("QObject", "Kamasau"));
    addLanguage({"kmu"}, QT_TRANSLATE_NOOP("QObject", "Kanite"));
    addLanguage({"kn"}, QT_TRANSLATE_NOOP("QObject", "Kannada"));
    addLanguage({"kne"}, QT_TRANSLATE_NOOP("QObject", "Kankanaey"));
    addLanguage({"knf"}, QT_TRANSLATE_NOOP("QObject", "Mankanya"));
    addLanguage({"knj"}, QT_TRANSLATE_NOOP("QObject", "Western Kanjobal"));
    addLanguage({"knv"}, QT_TRANSLATE_NOOP("QObject", "Tabo"));
    addLanguage({"ko"}, QT_TRANSLATE_NOOP("QObject", "Korean"));
    addLanguage({"kos"}, QT_TRANSLATE_NOOP("QObject", "Kosraean"));
    addLanguage({"kpf"}, QT_TRANSLATE_NOOP("QObject", "Komba"));
    addLanguage({"kpg"}, QT_TRANSLATE_NOOP("QObject", "Kapingamarangi"));
    addLanguage({"kpj"}, QT_TRANSLATE_NOOP("QObject", "Karajá"));
    addLanguage({"kpl"}, QT_TRANSLATE_NOOP("QObject", "Kpala"));
    addLanguage({"kpr"}, QT_TRANSLATE_NOOP("QObject", "Korafe-Yegha"));
    addLanguage({"kpw"}, QT_TRANSLATE_NOOP("QObject", "Kobon"));
    addLanguage({"kpx"}, QT_TRANSLATE_NOOP("QObject", "Mountain Koiali"));
    addLanguage({"kpy-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Koryak (Cyrillic script)"));
    addLanguage({"kqa"}, QT_TRANSLATE_NOOP("QObject", "Mum"));
    addLanguage({"kqc"}, QT_TRANSLATE_NOOP("QObject", "Doromu-Koki"));
    addLanguage({"kqf"}, QT_TRANSLATE_NOOP("QObject", "Kakabai"));
    addLanguage({"kql"}, QT_TRANSLATE_NOOP("QObject", "Kyenele"));
    addLanguage({"kqw"}, QT_TRANSLATE_NOOP("QObject", "Kandas"));
    // addLanguage({"ks"}, QT_TRANSLATE_NOOP("QObject", "Kashmiri"));
    addLanguage({"ksd"}, QT_TRANSLATE_NOOP("QObject", "Kuanua"));
    addLanguage({"ksj"}, QT_TRANSLATE_NOOP("QObject", "Uare"));
    addLanguage({"ksr"}, QT_TRANSLATE_NOOP("QObject", "Borong"));
    addLanguage({"ktm"}, QT_TRANSLATE_NOOP("QObject", "Kurti"));
    addLanguage({"kto"}, QT_TRANSLATE_NOOP("QObject", "Kuot"));
    addLanguage({"ktu"}, QT_TRANSLATE_NOOP("QObject", "Kituba (Democratic Republic of Congo)"));
    addLanguage({"ku"}, QT_TRANSLATE_NOOP("QObject", "Kurdish"));
    addLanguage({"kud"}, QT_TRANSLATE_NOOP("QObject", "'Auhelawa"));
    addLanguage({"kue"}, QT_TRANSLATE_NOOP("QObject", "Kuman"));
    addLanguage({"kum"}, QT_TRANSLATE_NOOP("QObject", "Kumyk"));
    addLanguage({"kup"}, QT_TRANSLATE_NOOP("QObject", "Kunimaipa"));
    //  addLanguage({"kv"}, QT_TRANSLATE_NOOP("QObject", "Komi"));
    addLanguage({"kvg"}, QT_TRANSLATE_NOOP("QObject", "Kuni-Boazi"));
    addLanguage({"kvn"}, QT_TRANSLATE_NOOP("QObject", "Border Kuna"));
    //  addLanguage({"kw"}, QT_TRANSLATE_NOOP("QObject", "Cornish"));
    addLanguage({"kwd"}, QT_TRANSLATE_NOOP("QObject", "Kwaio"));
    addLanguage({"kwf"}, QT_TRANSLATE_NOOP("QObject", "Kwara'ae"));
    addLanguage({"kwi"}, QT_TRANSLATE_NOOP("QObject", "Awa-Cuaiquer"));
    addLanguage({"kwj"}, QT_TRANSLATE_NOOP("QObject", "Kwanga"));
    addLanguage({"ky"}, QT_TRANSLATE_NOOP("QObject", "Kirghiz"));
    addLanguage({"ky-Arab"}, QT_TRANSLATE_NOOP("QObject", "Kirghiz (Arabic script)"));
    addLanguage({"kyc"}, QT_TRANSLATE_NOOP("QObject", "Kyaka"));
    addLanguage({"ky-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Kirghiz (Cyrillic script)"));
    addLanguage({"kyf"}, QT_TRANSLATE_NOOP("QObject", "Kouya"));
    addLanguage({"kyg"}, QT_TRANSLATE_NOOP("QObject", "Keyagana"));
    addLanguage({"kyq"}, QT_TRANSLATE_NOOP("QObject", "Kenga"));
    addLanguage({"kyz"}, QT_TRANSLATE_NOOP("QObject", "Kayabí"));
    addLanguage({"kze"}, QT_TRANSLATE_NOOP("QObject", "Kosena"));

    addLanguage({"la"}, QT_TRANSLATE_NOOP("QObject", "Latin"));
    addLanguage({"lac"}, QT_TRANSLATE_NOOP("QObject", "Lacandon"));
    // addLanguage({"lb"}, QT_TRANSLATE_NOOP("QObject", "Letzeburgesch"));
    addLanguage({"lbb"}, QT_TRANSLATE_NOOP("QObject", "Label"));
    addLanguage({"lbk"}, QT_TRANSLATE_NOOP("QObject", "Central Bontoc"));
    addLanguage({"lcm"}, QT_TRANSLATE_NOOP("QObject", "Tungag"));
    addLanguage({"leu"}, QT_TRANSLATE_NOOP("QObject", "Kara (Papua New Guinea)"));
    addLanguage({"lex"}, QT_TRANSLATE_NOOP("QObject", "Luang"));
    addLanguage({"lez"}, QT_TRANSLATE_NOOP("QObject", "Lezghian"));
    addLanguage({"lez-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Lezghian (Cyrillic script)"));
    addLanguage({"lgl"}, QT_TRANSLATE_NOOP("QObject", "Wala"));
    addLanguage({"lid"}, QT_TRANSLATE_NOOP("QObject", "Nyindrou"));
    addLanguage({"lif"}, QT_TRANSLATE_NOOP("QObject", "Limbu"));
    addLanguage({"lin"}, QT_TRANSLATE_NOOP("QObject", "Lingàla"));
    addLanguage({"llg"}, QT_TRANSLATE_NOOP("QObject", "Lole"));
    addLanguage({"lmo"}, QT_TRANSLATE_NOOP("QObject", "Lombard"));
    addLanguage({"ln"}, QT_TRANSLATE_NOOP("QObject", "Lingàla"));
    // addLanguage({"lo"}, QT_TRANSLATE_NOOP("QObject", "Lao"));
    addLanguage({"lt"}, QT_TRANSLATE_NOOP("QObject", "Lithuanian"));
    addLanguage({"lv"}, QT_TRANSLATE_NOOP("QObject", "Latvian"));
    addLanguage({"lww"}, QT_TRANSLATE_NOOP("QObject", "Lewo"));
    addLanguage({"lzh"}, QT_TRANSLATE_NOOP("QObject", "Literary Chinese"));

    addLanguage({"maa"}, QT_TRANSLATE_NOOP("QObject", "San Jerónimo Tecóatl Mazatec"));
    addLanguage({"maj"}, QT_TRANSLATE_NOOP("QObject", "Jalapa De Díaz Mazatec"));
    addLanguage({"mal"}, QT_TRANSLATE_NOOP("QObject", "Malayalam"));
    addLanguage({"mam"}, QT_TRANSLATE_NOOP("QObject", "Mam"));
    addLanguage({"maq"}, QT_TRANSLATE_NOOP("QObject", "Chiquihuitlán Mazatec"));
    addLanguage({"mau"}, QT_TRANSLATE_NOOP("QObject", "Huautla Mazatec"));
    addLanguage({"mav"}, QT_TRANSLATE_NOOP("QObject", "Sateré-Mawé"));
    addLanguage({"maz"}, QT_TRANSLATE_NOOP("QObject", "Central Mazahua"));
    addLanguage({"mbb"}, QT_TRANSLATE_NOOP("QObject", "Western Bukidnon Manobo"));
    addLanguage({"mbc"}, QT_TRANSLATE_NOOP("QObject", "Macushi"));
    addLanguage({"mbh"}, QT_TRANSLATE_NOOP("QObject", "Mangseng"));
    addLanguage({"mbj"}, QT_TRANSLATE_NOOP("QObject", "Nadëb"));
    addLanguage({"mbl"}, QT_TRANSLATE_NOOP("QObject", "Maxakalí"));
    addLanguage({"mbs"}, QT_TRANSLATE_NOOP("QObject", "Sarangani Manobo"));
    addLanguage({"mbt"}, QT_TRANSLATE_NOOP("QObject", "Matigsalug Manobo"));
    addLanguage({"mca"}, QT_TRANSLATE_NOOP("QObject", "Maca"));
    addLanguage({"mcb"}, QT_TRANSLATE_NOOP("QObject", "Machiguenga"));
    addLanguage({"mcd"}, QT_TRANSLATE_NOOP("QObject", "Sharanahua"));
    addLanguage({"mcf"}, QT_TRANSLATE_NOOP("QObject", "Matsés"));
    addLanguage({"mco"}, QT_TRANSLATE_NOOP("QObject", "Coatlán Mixe"));
    addLanguage({"mcp"}, QT_TRANSLATE_NOOP("QObject", "Makaa"));
    addLanguage({"mcq"}, QT_TRANSLATE_NOOP("QObject", "Ese"));
    addLanguage({"mdy"}, QT_TRANSLATE_NOOP("QObject", "Male (Ethiopia)"));
    addLanguage({"med"}, QT_TRANSLATE_NOOP("QObject", "Melpa"));
    addLanguage({"mee"}, QT_TRANSLATE_NOOP("QObject", "Mengen"));
    addLanguage({"mek"}, QT_TRANSLATE_NOOP("QObject", "Mekeo"));
    addLanguage({"meq"}, QT_TRANSLATE_NOOP("QObject", "Merey"));
    addLanguage({"met"}, QT_TRANSLATE_NOOP("QObject", "Mato"));
    addLanguage({"meu"}, QT_TRANSLATE_NOOP("QObject", "Motu"));
    addLanguage({"mg"}, QT_TRANSLATE_NOOP("QObject", "Malagasy"));
    addLanguage({"mgc"}, QT_TRANSLATE_NOOP("QObject", "Morokodo"));
    addLanguage({"mgh"}, QT_TRANSLATE_NOOP("QObject", "Makhuwa-Meetto"));
    addLanguage({"mgw"}, QT_TRANSLATE_NOOP("QObject", "Matumbi"));
    // addLanguage({"mh"}, QT_TRANSLATE_NOOP("QObject", "Marshall"));
    addLanguage({"mhl"}, QT_TRANSLATE_NOOP("QObject", "Mauwake"));
    addLanguage({"mi"}, QT_TRANSLATE_NOOP("QObject", "Maori"));
    addLanguage({"mib"}, QT_TRANSLATE_NOOP("QObject", "Atatláhuca Mixtec"));
    addLanguage({"mic"}, QT_TRANSLATE_NOOP("QObject", "Mi'kmaq"));
    addLanguage({"mie"}, QT_TRANSLATE_NOOP("QObject", "Ocotepec Mixtec"));
    addLanguage({"mig"}, QT_TRANSLATE_NOOP("QObject", "San Miguel El Grande Mixtec"));
    addLanguage({"mih"}, QT_TRANSLATE_NOOP("QObject", "Chayuco Mixtec"));
    addLanguage({"mil"}, QT_TRANSLATE_NOOP("QObject", "Peñoles Mixtec"));
    addLanguage({"mio"}, QT_TRANSLATE_NOOP("QObject", "Pinotepa Nacional Mixtec"));
    addLanguage({"mir"}, QT_TRANSLATE_NOOP("QObject", "Mixe, Isthmus"));
    addLanguage({"mit"}, QT_TRANSLATE_NOOP("QObject", "Southern Puebla Mixtec"));
    addLanguage({"miz"}, QT_TRANSLATE_NOOP("QObject", "Mixtec, Coatzospan"));
    addLanguage({"mjc"}, QT_TRANSLATE_NOOP("QObject", "San Juan Colorado Mixtec"));
    addLanguage({"mk"}, QT_TRANSLATE_NOOP("QObject", "Macedonian"));
    addLanguage({"mkj"}, QT_TRANSLATE_NOOP("QObject", "Mokilese"));
    addLanguage({"mkl"}, QT_TRANSLATE_NOOP("QObject", "Mokole"));
    addLanguage({"mkn"}, QT_TRANSLATE_NOOP("QObject", "Kupang Malay"));
    addLanguage({"mks"}, QT_TRANSLATE_NOOP("QObject", "Mixtec, Silacayoapan"));
    addLanguage({"ml"}, QT_TRANSLATE_NOOP("QObject", "Malayalam"));
    addLanguage({"mlh"}, QT_TRANSLATE_NOOP("QObject", "Mape"));
    addLanguage({"mlp"}, QT_TRANSLATE_NOOP("QObject", "Bargam"));
    addLanguage({"mmo"}, QT_TRANSLATE_NOOP("QObject", "Mangga Buang"));
    addLanguage({"mmx"}, QT_TRANSLATE_NOOP("QObject", "Madak"));
    addLanguage({"mn"}, QT_TRANSLATE_NOOP("QObject", "Mongolian"));
    addLanguage({"mna"}, QT_TRANSLATE_NOOP("QObject", "Mbula"));
    //  addLanguage({"mo"}, QT_TRANSLATE_NOOP("QObject", "Moldavian"));
    addLanguage({"mop"}, QT_TRANSLATE_NOOP("QObject", "Mopán Maya"));
    addLanguage({"mos"}, QT_TRANSLATE_NOOP("QObject", "More"));
    addLanguage({"mox"}, QT_TRANSLATE_NOOP("QObject", "Molima"));
    addLanguage({"mph"}, QT_TRANSLATE_NOOP("QObject", "Maung"));
    addLanguage({"mpj"}, QT_TRANSLATE_NOOP("QObject", "Martu Wangka"));
    addLanguage({"mpm"}, QT_TRANSLATE_NOOP("QObject", "Yosondúa Mixtec"));
    addLanguage({"mpp"}, QT_TRANSLATE_NOOP("QObject", "Migabac"));
    addLanguage({"mps"}, QT_TRANSLATE_NOOP("QObject", "Dadibi"));
    addLanguage({"mpt"}, QT_TRANSLATE_NOOP("QObject", "Mian"));
    addLanguage({"mpx"}, QT_TRANSLATE_NOOP("QObject", "Misima-Paneati"));
    addLanguage({"mqb"}, QT_TRANSLATE_NOOP("QObject", "Mbuko"));
    addLanguage({"mqj"}, QT_TRANSLATE_NOOP("QObject", "Mamasa"));
    addLanguage({"mr"}, QT_TRANSLATE_NOOP("QObject", "Marathi"));
    addLanguage({"ms"}, QT_TRANSLATE_NOOP("QObject", "Malay"));
    addLanguage({"msb"}, QT_TRANSLATE_NOOP("QObject", "Masbatenyo"));
    addLanguage({"msc"}, QT_TRANSLATE_NOOP("QObject", "Sankaran Maninka"));
    addLanguage({"msk"}, QT_TRANSLATE_NOOP("QObject", "Mansaka"));
    addLanguage({"msm"}, QT_TRANSLATE_NOOP("QObject", "Agusan Manobo"));
    addLanguage({"msy"}, QT_TRANSLATE_NOOP("QObject", "Aruamu"));
    addLanguage({"mt"}, QT_TRANSLATE_NOOP("QObject", "Maltese"));
    addLanguage({"mti"}, QT_TRANSLATE_NOOP("QObject", "Maiwa (Papua New Guinea)"));
    addLanguage({"mto"}, QT_TRANSLATE_NOOP("QObject", "Totontepec Mixe"));
    addLanguage({"mul"}, QT_TRANSLATE_NOOP("QObject", "(Multiple languages)"));
    addLanguage({"mux"}, QT_TRANSLATE_NOOP("QObject", "Bo-Ung"));
    addLanguage({"muy"}, QT_TRANSLATE_NOOP("QObject", "Muyang"));
    addLanguage({"mva"}, QT_TRANSLATE_NOOP("QObject", "Manam"));
    addLanguage({"mvc"}, QT_TRANSLATE_NOOP("QObject", "Mam, Central"));
    addLanguage({"mvj"}, QT_TRANSLATE_NOOP("QObject", "Mam, Todos Santos Cuchumat\u00e1n"));
    addLanguage({"mvn"}, QT_TRANSLATE_NOOP("QObject", "Minaveha"));
    addLanguage({"mwc"}, QT_TRANSLATE_NOOP("QObject", "Are"));
    addLanguage({"mwe"}, QT_TRANSLATE_NOOP("QObject", "Mwera (Chimwera)"));
    addLanguage({"mwp"}, QT_TRANSLATE_NOOP("QObject", "Kala Lagaw Ya"));
    addLanguage({"mxb"}, QT_TRANSLATE_NOOP("QObject", "Tezoatlán Mixtec"));
    addLanguage({"mxp"}, QT_TRANSLATE_NOOP("QObject", "Tlahuitoltepec Mixe"));
    addLanguage({"mxq"}, QT_TRANSLATE_NOOP("QObject", "Mixe, Juquila"));
    addLanguage({"mxt"}, QT_TRANSLATE_NOOP("QObject", "Mixtec, Jamiltepec"));
    addLanguage({"my"}, QT_TRANSLATE_NOOP("QObject", "Burmese"));
    addLanguage({"myk"}, QT_TRANSLATE_NOOP("QObject", "Mamara Senoufo"));
    addLanguage({"myu"}, QT_TRANSLATE_NOOP("QObject", "Mundurukú"));
    addLanguage({"myw"}, QT_TRANSLATE_NOOP("QObject", "Muyuw"));
    addLanguage({"myy"}, QT_TRANSLATE_NOOP("QObject", "Macuna"));
    addLanguage({"mzz"}, QT_TRANSLATE_NOOP("QObject", "Maiadomu"));

    // addLanguage({"na"}, QT_TRANSLATE_NOOP("QObject", "Nauru"));
    addLanguage({"nab"}, QT_TRANSLATE_NOOP("QObject", "Southern Nambikuára"));
    addLanguage({"naf"}, QT_TRANSLATE_NOOP("QObject", "Nabak"));
    addLanguage({"nak"}, QT_TRANSLATE_NOOP("QObject", "Nakanai"));
    addLanguage({"nas"}, QT_TRANSLATE_NOOP("QObject", "Naasioi"));
    addLanguage({"nay"}, QT_TRANSLATE_NOOP("QObject", "Narrinyeri"));
    addLanguage({"nb"}, QT_TRANSLATE_NOOP("QObject", "Norwegian Bokm\u00e5l"));
    addLanguage({"nbq"}, QT_TRANSLATE_NOOP("QObject", "Nggem"));
    addLanguage({"nca"}, QT_TRANSLATE_NOOP("QObject", "Iyo"));
    addLanguage({"nch"}, QT_TRANSLATE_NOOP("QObject", "Central Huasteca Nahuatl"));
    addLanguage({"ncj"}, QT_TRANSLATE_NOOP("QObject", "Northern Puebla Nahuatl"));
    addLanguage({"ncl"}, QT_TRANSLATE_NOOP("QObject", "Nahuatl, Michoac\u00e1n"));
    addLanguage({"ncu"}, QT_TRANSLATE_NOOP("QObject", "Chumburung"));
    addLanguage({"nd"}, QT_TRANSLATE_NOOP("QObject", "Sindebele"));
    addLanguage({"ndj"}, QT_TRANSLATE_NOOP("QObject", "Ndamba"));
    addLanguage({"nds"}, QT_TRANSLATE_NOOP("QObject", "Low German; Low Saxon"));
    addLanguage({"ne"}, QT_TRANSLATE_NOOP("QObject", "Nepali"));
    addLanguage({"nfa"}, QT_TRANSLATE_NOOP("QObject", "Dhao"));
    // addLanguage({"ng"}, QT_TRANSLATE_NOOP("QObject", "Ndonga"));
    addLanguage({"ngp"}, QT_TRANSLATE_NOOP("QObject", "Ngulu"));
    addLanguage({"ngu"}, QT_TRANSLATE_NOOP("QObject", "Nahuatl, Guerrero"));
    addLanguage({"nhe"}, QT_TRANSLATE_NOOP("QObject", "Eastern Huasteca Nahuatl"));
    addLanguage({"nhg"}, QT_TRANSLATE_NOOP("QObject", "Tetelcingo Nahuatl"));
    addLanguage({"nhi"}, QT_TRANSLATE_NOOP("QObject", "Zacatlán-Ahuacatlán-Tepetzintla Nahuatl"));
    addLanguage({"nho"}, QT_TRANSLATE_NOOP("QObject", "Takuu"));
    addLanguage({"nhr"}, QT_TRANSLATE_NOOP("QObject", "Naro"));
    addLanguage({"nhu"}, QT_TRANSLATE_NOOP("QObject", "Noone"));
    addLanguage({"nhw"}, QT_TRANSLATE_NOOP("QObject", "Western Huasteca Nahuatl"));
    addLanguage({"nhy"}, QT_TRANSLATE_NOOP("QObject", "Nahuatl, Northern Oaxaca"));
    addLanguage({"nif"}, QT_TRANSLATE_NOOP("QObject", "Nek"));
    addLanguage({"nii"}, QT_TRANSLATE_NOOP("QObject", "Nii"));
    addLanguage({"nin"}, QT_TRANSLATE_NOOP("QObject", "Ninzo"));
    addLanguage({"nko"}, QT_TRANSLATE_NOOP("QObject", "Nkonya"));
    addLanguage({"nl"}, QT_TRANSLATE_NOOP("QObject", "Dutch"));
    addLanguage({"nlg"}, QT_TRANSLATE_NOOP("QObject", "Gela"));
    addLanguage({"nmw"}, QT_TRANSLATE_NOOP("QObject", "Nimoa"));
    addLanguage({"nn"}, QT_TRANSLATE_NOOP("QObject", "Norwegian Nynorsk"));
    addLanguage({"nna"}, QT_TRANSLATE_NOOP("QObject", "Nyangumarta"));
    addLanguage({"nnq"}, QT_TRANSLATE_NOOP("QObject", "Ngindo"));
    addLanguage({"no"}, QT_TRANSLATE_NOOP("QObject", "Norwegian"));
    addLanguage({"noa"}, QT_TRANSLATE_NOOP("QObject", "Woun Meu"));
    addLanguage({"nog-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Nogai (Cyrillic script)"));
    addLanguage({"nop"}, QT_TRANSLATE_NOOP("QObject", "Numanggang"));
    addLanguage({"not"}, QT_TRANSLATE_NOOP("QObject", "Nomatsiguenga"));
    addLanguage({"nou"}, QT_TRANSLATE_NOOP("QObject", "Ewage-Notu"));
    addLanguage({"npi"}, QT_TRANSLATE_NOOP("QObject", "Nepali (individual language)"));
    addLanguage({"npl"}, QT_TRANSLATE_NOOP("QObject", "Southeastern Puebla Nahuatl"));
    //  addLanguage({"nr"}, QT_TRANSLATE_NOOP("QObject", "Ndebele, South"));
    addLanguage({"nsn"}, QT_TRANSLATE_NOOP("QObject", "Nehan"));
    addLanguage({"nss"}, QT_TRANSLATE_NOOP("QObject", "Nali"));
    addLanguage({"ntj"}, QT_TRANSLATE_NOOP("QObject", "Ngaanyatjarra"));
    addLanguage({"ntp"}, QT_TRANSLATE_NOOP("QObject", "Northern Tepehuan"));
    addLanguage({"ntu"}, QT_TRANSLATE_NOOP("QObject", "Natügu"));
    addLanguage({"nuy"}, QT_TRANSLATE_NOOP("QObject", "Nunggubuyu"));
    //  addLanguage({"nv"}, QT_TRANSLATE_NOOP("QObject", "Navajo"));
    addLanguage({"nvm"}, QT_TRANSLATE_NOOP("QObject", "Namiae"));
    addLanguage({"nwi"}, QT_TRANSLATE_NOOP("QObject", "Southwest Tanna"));
    //  addLanguage({"ny"}, QT_TRANSLATE_NOOP("QObject", "Chichewa; Nyanja"));
    addLanguage({"nys"}, QT_TRANSLATE_NOOP("QObject", "Nyunga"));
    addLanguage({"nyu"}, QT_TRANSLATE_NOOP("QObject", "Nyungwe"));

    addLanguage({"obo"}, QT_TRANSLATE_NOOP("QObject", "Obo Manobo"));
    //  addLanguage({"oc"}, QT_TRANSLATE_NOOP("QObject", "Occitan (post 1500); Provençal"));
    addLanguage({"okv"}, QT_TRANSLATE_NOOP("QObject", "Orokaiva"));
    //  addLanguage({"om"}, QT_TRANSLATE_NOOP("QObject", "Oromo"));
    addLanguage({"omw"}, QT_TRANSLATE_NOOP("QObject", "South Tairora"));
    addLanguage({"ong"}, QT_TRANSLATE_NOOP("QObject", "Olo"));
    addLanguage({"ons"}, QT_TRANSLATE_NOOP("QObject", "Ono"));
    addLanguage({"ood"}, QT_TRANSLATE_NOOP("QObject", "Tohono O'odham"));
    addLanguage({"opm"}, QT_TRANSLATE_NOOP("QObject", "Oksapmin"));
    //  addLanguage({"or"}, QT_TRANSLATE_NOOP("QObject", "Oriya"));
    addLanguage({"ory"}, QT_TRANSLATE_NOOP("QObject", "Oriya (individual language)"));
    addLanguage({"os"}, QT_TRANSLATE_NOOP("QObject", "Ossetian"));
    addLanguage({"ote"}, QT_TRANSLATE_NOOP("QObject", "Mezquital Otomi"));
    addLanguage({"otm"}, QT_TRANSLATE_NOOP("QObject", "Eastern Highland Otomi"));
    addLanguage({"otn"}, QT_TRANSLATE_NOOP("QObject", "Tenango Otomi"));
    addLanguage({"ots"}, QT_TRANSLATE_NOOP("QObject", "Estado de México Otomi"));
    addLanguage({"otq"}, QT_TRANSLATE_NOOP("QObject", "Otomi, Quer\u00e9taro"));

    addLanguage({"pa"}, QT_TRANSLATE_NOOP("QObject", "Panjabi"));
    addLanguage({"pab"}, QT_TRANSLATE_NOOP("QObject", "Parecís"));
    addLanguage({"pad"}, QT_TRANSLATE_NOOP("QObject", "Paumarí"));
    addLanguage({"pah"}, QT_TRANSLATE_NOOP("QObject", "Tenharim"));
    addLanguage({"pao"}, QT_TRANSLATE_NOOP("QObject", "Northern Paiute"));
    addLanguage({"pap"}, QT_TRANSLATE_NOOP("QObject", "Papiamento"));
    addLanguage({"pes"}, QT_TRANSLATE_NOOP("QObject", "Western Farsi"));
    // addLanguage({"pi"}, QT_TRANSLATE_NOOP("QObject", "Pali"));
    addLanguage({"pib"}, QT_TRANSLATE_NOOP("QObject", "Yine"));
    addLanguage({"pio"}, QT_TRANSLATE_NOOP("QObject", "Piapoco"));
    addLanguage({"pir"}, QT_TRANSLATE_NOOP("QObject", "Piratapuyo"));
    addLanguage({"piu"}, QT_TRANSLATE_NOOP("QObject", "Pintupi-Luritja"));
    addLanguage({"pjt"}, QT_TRANSLATE_NOOP("QObject", "Pitjantjatjara"));
    addLanguage({"pl"}, QT_TRANSLATE_NOOP("QObject", "Polish"));
    addLanguage({"pls"}, QT_TRANSLATE_NOOP("QObject", "San Marcos Tlalcoyalco Popoloca"));
    addLanguage({"plu"}, QT_TRANSLATE_NOOP("QObject", "Palikúr"));
    addLanguage({"pma"}, QT_TRANSLATE_NOOP("QObject", "Paama"));
    addLanguage({"poe"}, QT_TRANSLATE_NOOP("QObject", "San Juan Atzingo Popoloca"));
    addLanguage({"poh"}, QT_TRANSLATE_NOOP("QObject", "Poqomchi'"));
    addLanguage({"poi"}, QT_TRANSLATE_NOOP("QObject", "Highland Popoluca"));
    addLanguage({"pon"}, QT_TRANSLATE_NOOP("QObject", "Pohnpeian"));
    addLanguage({"pot"}, QT_TRANSLATE_NOOP("QObject", "Potawatomi"));
    addLanguage({"poy"}, QT_TRANSLATE_NOOP("QObject", "Pogolo"));
    addLanguage({"ppk"}, QT_TRANSLATE_NOOP("QObject", "Uma"));
    addLanguage({"ppo"}, QT_TRANSLATE_NOOP("QObject", "Folopa"));
    addLanguage({"prf"}, QT_TRANSLATE_NOOP("QObject", "Paranan"));
    addLanguage({"pri"}, QT_TRANSLATE_NOOP("QObject", "Paicî"));
    addLanguage({"prs"}, QT_TRANSLATE_NOOP("QObject", "Persian (Dari)"));
    //  addLanguage({"ps"}, QT_TRANSLATE_NOOP("QObject", "Pushto"));
    addLanguage({"pt"}, QT_TRANSLATE_NOOP("QObject", "Portuguese"));
    addLanguage({"pt-BR"}, QT_TRANSLATE_NOOP("QObject", "Brazilian Portuguese"));
    addLanguage({"ptp"}, QT_TRANSLATE_NOOP("QObject", "Patep"));
    addLanguage({"ptu"}, QT_TRANSLATE_NOOP("QObject", "Bambam"));
    addLanguage({"pwg"}, QT_TRANSLATE_NOOP("QObject", "Gapapaiwa"));

    // addLanguage({"qu"}, QT_TRANSLATE_NOOP("QObject", "Quechua"));
    addLanguage({"qut"}, QT_TRANSLATE_NOOP("QObject", "Quich\u00e9, West Central"));
    addLanguage({"qub"}, QT_TRANSLATE_NOOP("QObject", "Huallaga Huánuco Quechua"));
    addLanguage({"quc"}, QT_TRANSLATE_NOOP("QObject", "K'iche'"));
    addLanguage({"quf"}, QT_TRANSLATE_NOOP("QObject", "Lambayeque Quechua"));
    addLanguage({"quh"}, QT_TRANSLATE_NOOP("QObject", "South Bolivian Quechua"));
    addLanguage({"qul"}, QT_TRANSLATE_NOOP("QObject", "North Bolivian Quechua"));
    addLanguage({"qup"}, QT_TRANSLATE_NOOP("QObject", "Southern Pastaza Quechua"));
    addLanguage({"qvc"}, QT_TRANSLATE_NOOP("QObject", "Cajamarca Quechua"));
    addLanguage({"qve"}, QT_TRANSLATE_NOOP("QObject", "Eastern Apurímac Quechua"));
    addLanguage({"qvh"}, QT_TRANSLATE_NOOP("QObject", "Huamalíes-Dos de Mayo Huánuco Quechua"));
    addLanguage({"qvm"}, QT_TRANSLATE_NOOP("QObject", "Margos-Yarowilca-Lauricocha Quechua"));
    addLanguage({"qvn"}, QT_TRANSLATE_NOOP("QObject", "North Junín Quechua"));
    addLanguage({"qvs"}, QT_TRANSLATE_NOOP("QObject", "San Martín Quechua"));
    addLanguage({"qvw"}, QT_TRANSLATE_NOOP("QObject", "Huaylla Wanca Quechua"));
    addLanguage({"qvz"}, QT_TRANSLATE_NOOP("QObject", "Northern Pastaza Quichua"));
    addLanguage({"qwh"}, QT_TRANSLATE_NOOP("QObject", "Huaylas Ancash Quechua"));
    addLanguage({"qxh"}, QT_TRANSLATE_NOOP("QObject", "Panao Huánuco Quechua"));
    addLanguage({"qxn"}, QT_TRANSLATE_NOOP("QObject", "Northern Conchucos Ancash Quechua"));
    addLanguage({"qxo"}, QT_TRANSLATE_NOOP("QObject", "Southern Conchucos Ancash Quechua"));

    addLanguage({"rai"}, QT_TRANSLATE_NOOP("QObject", "Ramoaaina"));
    addLanguage({"rgu"}, QT_TRANSLATE_NOOP("QObject", "Ringgou"));
    addLanguage({"rkb"}, QT_TRANSLATE_NOOP("QObject", "Rikbaktsa"));
    //  addLanguage({"rm"}, QT_TRANSLATE_NOOP("QObject", "Raeto-Romance"));
    addLanguage({"rmc"}, QT_TRANSLATE_NOOP("QObject", "Carpathian Romani"));
    addLanguage({"rmq"}, QT_TRANSLATE_NOOP("QObject", "Caló"));
    addLanguage({"rmy"}, QT_TRANSLATE_NOOP("QObject", "Vlax Romani"));
    //  addLanguage({"rn"}, QT_TRANSLATE_NOOP("QObject", "Rundi"));
    addLanguage({"ro"}, QT_TRANSLATE_NOOP("QObject", "Romanian"));
    addLanguage({"roo"}, QT_TRANSLATE_NOOP("QObject", "Rotokas"));
    addLanguage({"rop"}, QT_TRANSLATE_NOOP("QObject", "Kriol"));
    addLanguage({"row"}, QT_TRANSLATE_NOOP("QObject", "Dela-Oenale"));
    addLanguage({"rro"}, QT_TRANSLATE_NOOP("QObject", "Waima"));
    addLanguage({"ru"}, QT_TRANSLATE_NOOP("QObject", "Russian"));
    addLanguage({"ruf"}, QT_TRANSLATE_NOOP("QObject", "Luguru"));
    addLanguage({"rug"}, QT_TRANSLATE_NOOP("QObject", "Roviana"));
    //  addLanguage({"rw"}, QT_TRANSLATE_NOOP("QObject", "Kinyarwanda"));
    addLanguage({"rwo"}, QT_TRANSLATE_NOOP("QObject", "Rawa"));

    addLanguage({"sa"}, QT_TRANSLATE_NOOP("QObject", "Sanskrit"));
    addLanguage({"sab"}, QT_TRANSLATE_NOOP("QObject", "Buglere"));
    addLanguage({"sah-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Yakut (Cyrillic script)"));
    addLanguage({"sbe"}, QT_TRANSLATE_NOOP("QObject", "Saliba"));
    //  addLanguage({"sc"}, QT_TRANSLATE_NOOP("QObject", "Sardinian"));
    addLanguage({"sco"}, QT_TRANSLATE_NOOP("QObject", "Scots"));
    //  addLanguage({"sd"}, QT_TRANSLATE_NOOP("QObject", "Sindhi"));
    //  addLanguage({"se"}, QT_TRANSLATE_NOOP("QObject", "Northern Sami"));
    addLanguage({"seh"}, QT_TRANSLATE_NOOP("QObject", "Sena"));
    addLanguage({"sey"}, QT_TRANSLATE_NOOP("QObject", "Secoya"));
    //  addLanguage({"sg"}, QT_TRANSLATE_NOOP("QObject", "Sango"));
    addLanguage({"sgb"}, QT_TRANSLATE_NOOP("QObject", "Mag-Anchi Ayta"));
    addLanguage({"sgz"}, QT_TRANSLATE_NOOP("QObject", "Sursurunga"));
    addLanguage({"shj"}, QT_TRANSLATE_NOOP("QObject", "Shatt"));
    addLanguage({"shp"}, QT_TRANSLATE_NOOP("QObject", "Shipibo-Conibo"));
    //  addLanguage({"si"}, QT_TRANSLATE_NOOP("QObject", "Sinhalese"));
    addLanguage({"sim"}, QT_TRANSLATE_NOOP("QObject", "Mende (Papua New Guinea)"));
    addLanguage({"sja"}, QT_TRANSLATE_NOOP("QObject", "Epena"));
    addLanguage({"sk"}, QT_TRANSLATE_NOOP("QObject", "Slovak"));
    addLanguage({"sl"}, QT_TRANSLATE_NOOP("QObject", "Slovenian"));
    addLanguage({"sll"}, QT_TRANSLATE_NOOP("QObject", "Salt-Yui"));
    //  addLanguage({"sm"}, QT_TRANSLATE_NOOP("QObject", "Samoan"));
    addLanguage({"smk"}, QT_TRANSLATE_NOOP("QObject", "Bolinao"));
    addLanguage({"sml"}, QT_TRANSLATE_NOOP("QObject", "Central Sama"));
    addLanguage({"sn"}, QT_TRANSLATE_NOOP("QObject", "Shishona"));
    addLanguage({"snc"}, QT_TRANSLATE_NOOP("QObject", "Sinaugoro"));
    addLanguage({"snn"}, QT_TRANSLATE_NOOP("QObject", "Siona"));
    addLanguage({"snp"}, QT_TRANSLATE_NOOP("QObject", "Siane"));
    addLanguage({"sny"}, QT_TRANSLATE_NOOP("QObject", "Saniyo-Hiyewe"));
    addLanguage({"so"}, QT_TRANSLATE_NOOP("QObject", "Somali"));
    addLanguage({"soq"}, QT_TRANSLATE_NOOP("QObject", "Kanasi"));
    addLanguage({"soy"}, QT_TRANSLATE_NOOP("QObject", "Miyobe"));
    addLanguage({"spl"}, QT_TRANSLATE_NOOP("QObject", "Selepet"));
    addLanguage({"spm"}, QT_TRANSLATE_NOOP("QObject", "Sepen"));
    addLanguage({"spp"}, QT_TRANSLATE_NOOP("QObject", "Supyire Senoufo"));
    addLanguage({"sps"}, QT_TRANSLATE_NOOP("QObject", "Saposa"));
    addLanguage({"spy"}, QT_TRANSLATE_NOOP("QObject", "Sabaot"));
    addLanguage({"sq"}, QT_TRANSLATE_NOOP("QObject", "Albanian"));
    addLanguage({"sr"}, QT_TRANSLATE_NOOP("QObject", "Srpski"));
    addLanguage({"sri"}, QT_TRANSLATE_NOOP("QObject", "Siriano"));
    addLanguage({"srm"}, QT_TRANSLATE_NOOP("QObject", "Saramaccan"));
    addLanguage({"srn"}, QT_TRANSLATE_NOOP("QObject", "Sranan"));
    addLanguage({"srq"}, QT_TRANSLATE_NOOP("QObject", "Sirionó"));
    //  addLanguage({"ss"}, QT_TRANSLATE_NOOP("QObject", "Swati"));
    addLanguage({"ssd"}, QT_TRANSLATE_NOOP("QObject", "Siroi"));
    addLanguage({"ssg"}, QT_TRANSLATE_NOOP("QObject", "Seimat"));
    addLanguage({"ssx"}, QT_TRANSLATE_NOOP("QObject", "Samberigi"));
    //  addLanguage({"st"}, QT_TRANSLATE_NOOP("QObject", "Sotho, Southern"));
    addLanguage({"stp"}, QT_TRANSLATE_NOOP("QObject", "Southeastern Tepehuan"));
    //  addLanguage({"su"}, QT_TRANSLATE_NOOP("QObject", "Sundanese"));
    addLanguage({"sua"}, QT_TRANSLATE_NOOP("QObject", "Sulka"));
    addLanguage({"sue"}, QT_TRANSLATE_NOOP("QObject", "Suena"));
    addLanguage({"sus"}, QT_TRANSLATE_NOOP("QObject", "Susu"));
    addLanguage({"suz"}, QT_TRANSLATE_NOOP("QObject", "Sunwar"));
    addLanguage({"sv"}, QT_TRANSLATE_NOOP("QObject", "Swedish"));
    addLanguage({"sw"}, QT_TRANSLATE_NOOP("QObject", "Swahili"));
    addLanguage({"swh"}, QT_TRANSLATE_NOOP("QObject", "Swahili"));
    addLanguage({"swp"}, QT_TRANSLATE_NOOP("QObject", "Suau"));
    addLanguage({"sxb"}, QT_TRANSLATE_NOOP("QObject", "Suba"));
    addLanguage({"syr"}, QT_TRANSLATE_NOOP("QObject", "Syriac"));

    addLanguage({"ta"}, QT_TRANSLATE_NOOP("QObject", "Tamil"));
    addLanguage({"tab"}, QT_TRANSLATE_NOOP("QObject", "Tabassaran"));
    addLanguage({"tac"}, QT_TRANSLATE_NOOP("QObject", "Lowland Tarahumara"));
    addLanguage({"taj"}, QT_TRANSLATE_NOOP("QObject", "Eastern Tamang"));
    addLanguage({"tav"}, QT_TRANSLATE_NOOP("QObject", "Tatuyo"));
    addLanguage({"taw"}, QT_TRANSLATE_NOOP("QObject", "Tai"));
    addLanguage({"tbc"}, QT_TRANSLATE_NOOP("QObject", "Takia"));
    addLanguage({"tbg"}, QT_TRANSLATE_NOOP("QObject", "North Tairora"));
    addLanguage({"tbl"}, QT_TRANSLATE_NOOP("QObject", "Tboli"));
    addLanguage({"tbo"}, QT_TRANSLATE_NOOP("QObject", "Tawala"));
    addLanguage({"tbz"}, QT_TRANSLATE_NOOP("QObject", "Ditammari"));
    addLanguage({"tca"}, QT_TRANSLATE_NOOP("QObject", "Ticuna"));
    addLanguage({"tcs"}, QT_TRANSLATE_NOOP("QObject", "Torres Strait Creole"));
    addLanguage({"te"}, QT_TRANSLATE_NOOP("QObject", "Telugu"));
    addLanguage({"tee"}, QT_TRANSLATE_NOOP("QObject", "Huehuetla Tepehua"));
    addLanguage({"ter"}, QT_TRANSLATE_NOOP("QObject", "Tereno"));
    addLanguage({"tet"}, QT_TRANSLATE_NOOP("QObject", "Tetum"));
    addLanguage({"tew"}, QT_TRANSLATE_NOOP("QObject", "Tewa (USA)"));
    addLanguage({"tfr"}, QT_TRANSLATE_NOOP("QObject", "Teribe"));
    addLanguage({"tg"}, QT_TRANSLATE_NOOP("QObject", "Tajik"));
    addLanguage({"tgo"}, QT_TRANSLATE_NOOP("QObject", "Sudest"));
    addLanguage({"tgp"}, QT_TRANSLATE_NOOP("QObject", "Tangoa"));
    addLanguage({"th"}, QT_TRANSLATE_NOOP("QObject", "Thai"));
    addLanguage({"thd"}, QT_TRANSLATE_NOOP("QObject", "Thayore"));
    addLanguage({"tif"}, QT_TRANSLATE_NOOP("QObject", "Tifal"));
    addLanguage({"tim"}, QT_TRANSLATE_NOOP("QObject", "Timbe"));
    addLanguage({"tiy"}, QT_TRANSLATE_NOOP("QObject", "Tiruray"));
    // addLanguage({"tk"}, QT_TRANSLATE_NOOP("QObject", "Turkmen"));
    addLanguage({"tk-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Turkmen (Cyrillic script)"));
    // addLanguage({"tke"}, QT_TRANSLATE_NOOP("QObject", ""));
    addLanguage({"tk-Latn"}, QT_TRANSLATE_NOOP("QObject", "Turkmen (Latin script)"));
    addLanguage({"tkr-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Tsakhur (Cyrillic script)"));
    addLanguage({"tku"}, QT_TRANSLATE_NOOP("QObject", "Upper Necaxa Totonac"));
    addLanguage({"tl"}, QT_TRANSLATE_NOOP("QObject", "Tagalog"));
    addLanguage({"tlf"}, QT_TRANSLATE_NOOP("QObject", "Telefol"));
    addLanguage({"tlh"}, QT_TRANSLATE_NOOP("QObject", "Klingon"));
    addLanguage({"tn"}, QT_TRANSLATE_NOOP("QObject", "Tswana"));
    addLanguage({"tna"}, QT_TRANSLATE_NOOP("QObject", "Tacana"));
    addLanguage({"tnc"}, QT_TRANSLATE_NOOP("QObject", "Tanimuca-Retuarã"));
    addLanguage({"tnk"}, QT_TRANSLATE_NOOP("QObject", "Kwamera"));
    addLanguage({"tnn"}, QT_TRANSLATE_NOOP("QObject", "North Tanna"));
    addLanguage({"tnp"}, QT_TRANSLATE_NOOP("QObject", "Whitesands"));
    addLanguage({"to"}, QT_TRANSLATE_NOOP("QObject", "Tonga (Tonga Islands)"));
    addLanguage({"toc"}, QT_TRANSLATE_NOOP("QObject", "Coyutla Totonac"));
    addLanguage({"tod"}, QT_TRANSLATE_NOOP("QObject", "Toma"));
    addLanguage({"toj"}, QT_TRANSLATE_NOOP("QObject", "Tojolabal"));
    addLanguage({"too"}, QT_TRANSLATE_NOOP("QObject", "Xicotepec De Juárez Totonac"));
    addLanguage({"top"}, QT_TRANSLATE_NOOP("QObject", "Papantla Totonac"));
    addLanguage({"tos"}, QT_TRANSLATE_NOOP("QObject", "Highland Totonac"));
    addLanguage({"tpa"}, QT_TRANSLATE_NOOP("QObject", "Taupota"));
    addLanguage({"tpi"}, QT_TRANSLATE_NOOP("QObject", "Tok Pisin"));
    addLanguage({"tpt"}, QT_TRANSLATE_NOOP("QObject", "Tlachichilco Tepehua"));
    addLanguage({"tpz"}, QT_TRANSLATE_NOOP("QObject", "Tinputz"));
    addLanguage({"tr"}, QT_TRANSLATE_NOOP("QObject", "Turkish"));
    addLanguage({"trc"}, QT_TRANSLATE_NOOP("QObject", "Copala Triqui"));
    //  addLanguage({"ts"}, QT_TRANSLATE_NOOP("QObject", "Tsonga"));
    addLanguage({"tsg"}, QT_TRANSLATE_NOOP("QObject", "Tausug"));
    addLanguage({"tsw"}, QT_TRANSLATE_NOOP("QObject", "Tsishingini"));
    //  addLanguage({"tt"}, QT_TRANSLATE_NOOP("QObject", "Tatar"));
    addLanguage({"tt-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Tatar (Cyrillic script)"));
    addLanguage({"ttc"}, QT_TRANSLATE_NOOP("QObject", "Tektiteko"));
    addLanguage({"tte"}, QT_TRANSLATE_NOOP("QObject", "Bwanabwana"));
    addLanguage({"tuc"}, QT_TRANSLATE_NOOP("QObject", "Mutu"));
    addLanguage({"tue"}, QT_TRANSLATE_NOOP("QObject", "Tuyuca"));
    addLanguage({"tuf"}, QT_TRANSLATE_NOOP("QObject", "Central Tunebo"));
    addLanguage({"tuo"}, QT_TRANSLATE_NOOP("QObject", "Tucano"));
    addLanguage({"tvk"}, QT_TRANSLATE_NOOP("QObject", "Southeast Ambrym"));
    // addLanguage({"tw"}, QT_TRANSLATE_NOOP("QObject", "Twi"));
    addLanguage({"txq"}, QT_TRANSLATE_NOOP("QObject", "Tii"));
    addLanguage({"txu"}, QT_TRANSLATE_NOOP("QObject", "Kayapó"));
    addLanguage({"ty"}, QT_TRANSLATE_NOOP("QObject", "Tahitian"));
    addLanguage({"tyv"}, QT_TRANSLATE_NOOP("QObject", "Tuvinian"));
    addLanguage({"tzj"}, QT_TRANSLATE_NOOP("QObject", "Tz'utujil"));
    addLanguage({"tzo"}, QT_TRANSLATE_NOOP("QObject", "Tzotzil"));
    addLanguage({"tzz"}, QT_TRANSLATE_NOOP("QObject", "Tzotzil, Zinacant\u00e1n"));

    addLanguage({"uk"}, QT_TRANSLATE_NOOP("QObject", "Ukrainian"));
    addLanguage({"ury"}, QT_TRANSLATE_NOOP("QObject", "Orya"));
    addLanguage({"usp"}, QT_TRANSLATE_NOOP("QObject", "Uspanteco"));
    addLanguage({"ubr"}, QT_TRANSLATE_NOOP("QObject", "Ubir"));
    addLanguage({"ubu"}, QT_TRANSLATE_NOOP("QObject", "Umbu-Ungu"));
    addLanguage({"udu"}, QT_TRANSLATE_NOOP("QObject", "Uduk"));
    addLanguage({"ug"}, QT_TRANSLATE_NOOP("QObject", "Uyghurche‎"));
    addLanguage({"uli"}, QT_TRANSLATE_NOOP("QObject", "Ulithian"));
    addLanguage({"ulk"}, QT_TRANSLATE_NOOP("QObject", "Meriam"));
    addLanguage({"upv"}, QT_TRANSLATE_NOOP("QObject", "Uripiv-Wala-Rano-Atchin"));
    addLanguage({"ur"}, QT_TRANSLATE_NOOP("QObject", "Urdu"));
    // addLanguage({"ura"}, QT_TRANSLATE_NOOP("QObject", ""));
    // addLanguage({"urb"}, QT_TRANSLATE_NOOP("QObject", ""));
    addLanguage({"ur-Deva"}, QT_TRANSLATE_NOOP("QObject", "Urdu (Deva script)"));
    addLanguage({"urt"}, QT_TRANSLATE_NOOP("QObject", "Urat"));
    addLanguage({"urw"}, QT_TRANSLATE_NOOP("QObject", "Sop"));
    addLanguage({"usa"}, QT_TRANSLATE_NOOP("QObject", "Usarufa"));
    addLanguage({"uvl"}, QT_TRANSLATE_NOOP("QObject", "Lote"));
    // addLanguage({"uz"}, QT_TRANSLATE_NOOP("QObject", "Uzbek"));
    addLanguage({"uz-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Uzbek (Cyrillic script)"));
    addLanguage({"uz-Latn"}, QT_TRANSLATE_NOOP("QObject", "Uzbek (Latin script)"));

    addLanguage({"vi"}, QT_TRANSLATE_NOOP("QObject", "Vietnamese"));
    addLanguage({"vid"}, QT_TRANSLATE_NOOP("QObject", "Vidunda"));
    addLanguage({"viv"}, QT_TRANSLATE_NOOP("QObject", "Iduna"));
    addLanguage({"vls"}, QT_TRANSLATE_NOOP("QObject", "Vlaams"));
    addLanguage({"vmy"}, QT_TRANSLATE_NOOP("QObject", "Miwa"));
    //  addLanguage({"vo"}, QT_TRANSLATE_NOOP("QObject", "Volapük"));

    addLanguage({"waj"}, QT_TRANSLATE_NOOP("QObject", "Waffa"));
    addLanguage({"wal"}, QT_TRANSLATE_NOOP("QObject", "Wolaytta"));
    addLanguage({"wap"}, QT_TRANSLATE_NOOP("QObject", "Wapishana"));
    addLanguage({"wat"}, QT_TRANSLATE_NOOP("QObject", "Kaninuwa"));
    addLanguage({"wbp"}, QT_TRANSLATE_NOOP("QObject", "Warlpiri"));
    addLanguage({"wed"}, QT_TRANSLATE_NOOP("QObject", "Wedau"));
    addLanguage({"wer"}, QT_TRANSLATE_NOOP("QObject", "Weri"));
    addLanguage({"wim"}, QT_TRANSLATE_NOOP("QObject", "Wik-Mungkan"));
    addLanguage({"wiu"}, QT_TRANSLATE_NOOP("QObject", "Wiru"));
    addLanguage({"wmt"}, QT_TRANSLATE_NOOP("QObject", "Walmajarri"));
    addLanguage({"wmw"}, QT_TRANSLATE_NOOP("QObject", "Mwani"));
    addLanguage({"wnc"}, QT_TRANSLATE_NOOP("QObject", "Wantoat"));
    addLanguage({"wnu"}, QT_TRANSLATE_NOOP("QObject", "Usan"));
    //  addLanguage({"wo"}, QT_TRANSLATE_NOOP("QObject", "Wolof"));
    addLanguage({"wos"}, QT_TRANSLATE_NOOP("QObject", "Hanga Hundi"));
    addLanguage({"wrk"}, QT_TRANSLATE_NOOP("QObject", "Garrwa"));
    addLanguage({"wro"}, QT_TRANSLATE_NOOP("QObject", "Worrorra"));
    addLanguage({"wrs"}, QT_TRANSLATE_NOOP("QObject", "Waris"));
    addLanguage({"wsk"}, QT_TRANSLATE_NOOP("QObject", "Waskia"));
    addLanguage({"wuv"}, QT_TRANSLATE_NOOP("QObject", "Wuvulu-Aua"));

    addLanguage({"xh"}, QT_TRANSLATE_NOOP("QObject", "Xhosa"));
    addLanguage({"xtd"}, QT_TRANSLATE_NOOP("QObject", "Mixtec, Diuxi-Tilantongo"));
    addLanguage({"xal"}, QT_TRANSLATE_NOOP("QObject", "Kalmyk"));
    addLanguage({"xal-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Kalmyk (Cyrillic script)"));
    addLanguage({"xav"}, QT_TRANSLATE_NOOP("QObject", "Xavánte"));
    addLanguage({"xbi"}, QT_TRANSLATE_NOOP("QObject", "Kombio"));
    addLanguage({"xed"}, QT_TRANSLATE_NOOP("QObject", "Hdi"));
    addLanguage({"xla"}, QT_TRANSLATE_NOOP("QObject", "Kamula"));
    addLanguage({"xnn"}, QT_TRANSLATE_NOOP("QObject", "Northern Kankanay"));
    addLanguage({"xon"}, QT_TRANSLATE_NOOP("QObject", "Konkomba"));
    addLanguage({"xsi"}, QT_TRANSLATE_NOOP("QObject", "Sio"));
    addLanguage({"xtm"}, QT_TRANSLATE_NOOP("QObject", "Magdalena Peñasco Mixtec"));

    addLanguage({"yaa"}, QT_TRANSLATE_NOOP("QObject", "Yaminahua"));
    addLanguage({"yad"}, QT_TRANSLATE_NOOP("QObject", "Yagua"));
    addLanguage({"yaf"}, QT_TRANSLATE_NOOP("QObject", "Yaka (Democratic Republic of Congo)"));
    addLanguage({"yal"}, QT_TRANSLATE_NOOP("QObject", "Yalunka"));
    addLanguage({"yap"}, QT_TRANSLATE_NOOP("QObject", "Yapese"));
    addLanguage({"yaq"}, QT_TRANSLATE_NOOP("QObject", "Yaqui"));
    addLanguage({"yby"}, QT_TRANSLATE_NOOP("QObject", "Yaweyuha"));
    addLanguage({"ycn"}, QT_TRANSLATE_NOOP("QObject", "Yucuna"));
    addLanguage({"yi"}, QT_TRANSLATE_NOOP("QObject", "Yiddish"));
    addLanguage({"yij"}, QT_TRANSLATE_NOOP("QObject", "Yindjibarndi"));
    addLanguage({"yka"}, QT_TRANSLATE_NOOP("QObject", "Yakan"));
    addLanguage({"yle"}, QT_TRANSLATE_NOOP("QObject", "Yele"));
    addLanguage({"yml"}, QT_TRANSLATE_NOOP("QObject", "Iamalele"));
    addLanguage({"yo"}, QT_TRANSLATE_NOOP("QObject", "Yoruba"));
    addLanguage({"yre"}, QT_TRANSLATE_NOOP("QObject", "Yaouré"));
    addLanguage({"yrk-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Nenets (Cyrillic script)"));
    addLanguage({"yss"}, QT_TRANSLATE_NOOP("QObject", "Yessan-Mayo"));
    addLanguage({"yuj"}, QT_TRANSLATE_NOOP("QObject", "Karkar-Yuri"));
    addLanguage({"yut"}, QT_TRANSLATE_NOOP("QObject", "Yopno"));
    addLanguage({"yuw"}, QT_TRANSLATE_NOOP("QObject", "Yau (Morobe Province)"));
    addLanguage({"yva"}, QT_TRANSLATE_NOOP("QObject", "Yawa"));

    // addLanguage({"za"}, QT_TRANSLATE_NOOP("QObject", "Zhuang"));
    addLanguage({"zaa"}, QT_TRANSLATE_NOOP("QObject", "Sierra de Juárez Zapotec"));
    addLanguage({"zab"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, San Juan Guelav\u00eda"));
    addLanguage({"zac"}, QT_TRANSLATE_NOOP("QObject", "Ocotlán Zapotec"));
    addLanguage({"zad"}, QT_TRANSLATE_NOOP("QObject", "Cajonos Zapotec"));
    addLanguage({"zai"}, QT_TRANSLATE_NOOP("QObject", "Isthmus Zapotec"));
    addLanguage({"zaj"}, QT_TRANSLATE_NOOP("QObject", "Zaramo"));
    addLanguage({"zam"}, QT_TRANSLATE_NOOP("QObject", "Miahuatlán Zapotec"));
    addLanguage({"zao"}, QT_TRANSLATE_NOOP("QObject", "Ozolotepec Zapotec"));
    addLanguage({"zar"}, QT_TRANSLATE_NOOP("QObject", "Rincón Zapotec"));
    addLanguage({"zas"}, QT_TRANSLATE_NOOP("QObject", "Santo Domingo Albarradas Zapotec"));
    addLanguage({"zat"}, QT_TRANSLATE_NOOP("QObject", "Tabaa Zapotec"));
    addLanguage({"zav"}, QT_TRANSLATE_NOOP("QObject", "Yatzachi Zapotec"));
    addLanguage({"zaw"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Mitla"));
    addLanguage({"zca"}, QT_TRANSLATE_NOOP("QObject", "Coatecas Altas Zapotec"));
    addLanguage({"zh"}, QT_TRANSLATE_NOOP("QObject", "Chinese"));
    addLanguage({"zia"}, QT_TRANSLATE_NOOP("QObject", "Zia"));
    addLanguage({"ziw"}, QT_TRANSLATE_NOOP("QObject", "Zigula"));
    addLanguage({"zlm"}, QT_TRANSLATE_NOOP("QObject", "Malay (individual language)"));
    addLanguage({"zos"}, QT_TRANSLATE_NOOP("QObject", "Francisco León Zoque"));
    addLanguage({"zpc"}, QT_TRANSLATE_NOOP("QObject", "Choapan Zapotec"));
    addLanguage({"zpi"}, QT_TRANSLATE_NOOP("QObject", "Santa María Quiegolani Zapotec"));
    addLanguage({"zpl"}, QT_TRANSLATE_NOOP("QObject", "Lachixío Zapotec"));
    addLanguage({"zpm"}, QT_TRANSLATE_NOOP("QObject", "Mixtepec Zapotec"));
    addLanguage({"zpo"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Amatl\u00e1n"));
    addLanguage({"zpq"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Zoogocho"));
    addLanguage({"zpu"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Yal\u00e1lag"));
    addLanguage({"zpv"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Chichicapan"));
    addLanguage({"zpz"}, QT_TRANSLATE_NOOP("QObject", "Texmelucan Zapotec"));
    addLanguage({"zsr"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Southern Rincon"));
    addLanguage({"ztq"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Quioquitani-Quier\u00ed"));
    addLanguage({"zty"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Yatee"));
    addLanguage({"zu"}, QT_TRANSLATE_NOOP("QObject", "Zulu"));
    addLanguage({"zyp"}, QT_TRANSLATE_NOOP("QObject", "Zyphe"));
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

    BT_ASSERT(languageMap.contains("en"));
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
                        auto const abbrevEn = abbrev + ".en";
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
                    while (!localeName.isEmpty() && localeName != "en") {
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
