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
    ISO 639-3 -------
    x-E-XXX form is deprecated and no modules in repositories use it.
    If you find a module with x-E-XXX language, update the module.

    Some of these codes are from http://www.iso639-3.sil.org/
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

    addLanguage({"aai"}, QT_TRANSLATE_NOOP("QObject", "Arifama-Miniafia"));
    addLanguage({"aak"}, QT_TRANSLATE_NOOP("QObject", "Ankave"));
    addLanguage({"aar", "aa"}, QT_TRANSLATE_NOOP("QObject", "Afar"));
    addLanguage({"aau"}, QT_TRANSLATE_NOOP("QObject", "Abau"));
    addLanguage({"aaz"}, QT_TRANSLATE_NOOP("QObject", "Amarasi"));
    addLanguage({"abk", "ab"}, QT_TRANSLATE_NOOP("QObject", "Abkhazian"));
    addLanguage({"abt"}, QT_TRANSLATE_NOOP("QObject", "Ambulas"));
    addLanguage({"abx"}, QT_TRANSLATE_NOOP("QObject", "Inabaknon"));
    addLanguage({"aby"}, QT_TRANSLATE_NOOP("QObject", "Aneme Wake"));
    addLanguage({"acf"}, QT_TRANSLATE_NOOP("QObject", "Saint Lucian Creole French"));
    addLanguage({"acr"}, QT_TRANSLATE_NOOP("QObject", "Achi"));
    addLanguage({"acu"}, QT_TRANSLATE_NOOP("QObject", "Achuar-Shiwiar"));
    addLanguage({"ady", "ady-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Adyghe"));
    addLanguage({"adz"}, QT_TRANSLATE_NOOP("QObject", "Adzera"));
    addLanguage({"aer"}, QT_TRANSLATE_NOOP("QObject", "Eastern Arrernte"));
    addLanguage({"aey"}, QT_TRANSLATE_NOOP("QObject", "Amele"));
    addLanguage({"afr", "af"}, QT_TRANSLATE_NOOP("QObject", "Afrikaans"));
    addLanguage({"agd"}, QT_TRANSLATE_NOOP("QObject", "Agarabi"));
    addLanguage({"agg"}, QT_TRANSLATE_NOOP("QObject", "Angor"));
    addLanguage({"agm"}, QT_TRANSLATE_NOOP("QObject", "Angaataha"));
    addLanguage({"agn"}, QT_TRANSLATE_NOOP("QObject", "Agutaynen"));
    addLanguage({"agr"}, QT_TRANSLATE_NOOP("QObject", "Aguaruna"));
    addLanguage({"agt"}, QT_TRANSLATE_NOOP("QObject", "Central Cagayan Agta"));
    addLanguage({"agu"}, QT_TRANSLATE_NOOP("QObject", "Aguacateco"));
    addLanguage({"agx"}, QT_TRANSLATE_NOOP("QObject", "Aghul"));
    addLanguage({"agx-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Aghul (Cyrillic script)"));
    addLanguage({"aia"}, QT_TRANSLATE_NOOP("QObject", "Arosi"));
    addLanguage({"aii"}, QT_TRANSLATE_NOOP("QObject", "Assyrian Neo-Aramaic"));
    addLanguage({"ake"}, QT_TRANSLATE_NOOP("QObject", "Akawaio"));
    addLanguage({"akh"}, QT_TRANSLATE_NOOP("QObject", "Angal Heneng"));
    addLanguage({"alp"}, QT_TRANSLATE_NOOP("QObject", "Alune"));
    addLanguage({"alq"}, QT_TRANSLATE_NOOP("QObject", "Algonquin"));
    addLanguage({"als"}, QT_TRANSLATE_NOOP("QObject", "Tosk Albanian"));
    addLanguage({"alt"}, QT_TRANSLATE_NOOP("QObject", "Southern Altai"));
    addLanguage({"alt-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Southern Altai (Cyrillic script)"));
    addLanguage({"aly"}, QT_TRANSLATE_NOOP("QObject", "Alyawarr"));
    addLanguage({"ame"}, QT_TRANSLATE_NOOP("QObject", "Yanesha'"));
    addLanguage({"amf"}, QT_TRANSLATE_NOOP("QObject", "Hamer-Banna"));
    addLanguage({"amh", "am"}, QT_TRANSLATE_NOOP("QObject", "Amharic"));
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
    addLanguage({"aom", "\u00d6mie"}, QT_TRANSLATE_NOOP("QObject", "\u00d6mie"));
    addLanguage({"aon"}, QT_TRANSLATE_NOOP("QObject", "Bumbita Arapesh"));
    addLanguage({"apb"}, QT_TRANSLATE_NOOP("QObject", "Sa'a"));
    addLanguage({"ape"}, QT_TRANSLATE_NOOP("QObject", "Bukiyip"));
    addLanguage({"apn"}, QT_TRANSLATE_NOOP("QObject", "Apinay\u00e9"));
    addLanguage({"apr"}, QT_TRANSLATE_NOOP("QObject", "Arop-Lokep"));
    addLanguage({"apu"}, QT_TRANSLATE_NOOP("QObject", "Apurin\u00e3"));
    addLanguage({"apw"}, QT_TRANSLATE_NOOP("QObject", "Western Apache"));
    addLanguage({"apy"}, QT_TRANSLATE_NOOP("QObject", "Apala\u00ed"));
    addLanguage({"apz"}, QT_TRANSLATE_NOOP("QObject", "Safeyoka"));
    addLanguage({"ara", "ar"}, QT_TRANSLATE_NOOP("QObject", "Arabic"));
    addLanguage({"arb"}, QT_TRANSLATE_NOOP("QObject", "Standard Arabic"));
    addLanguage({"are"}, QT_TRANSLATE_NOOP("QObject", "Western Arrarnta"));
    addLanguage({"arl"}, QT_TRANSLATE_NOOP("QObject", "Arabela"));
    addLanguage({"arn"}, QT_TRANSLATE_NOOP("QObject", "Mapudungun"));
    addLanguage({"arp"}, QT_TRANSLATE_NOOP("QObject", "Arapaho"));
    addLanguage({"asm", "as"}, QT_TRANSLATE_NOOP("QObject", "Assamese"));
    addLanguage({"aso"}, QT_TRANSLATE_NOOP("QObject", "Dano"));
    addLanguage({"ata"}, QT_TRANSLATE_NOOP("QObject", "Pele-Ata"));
    addLanguage({"atb"}, QT_TRANSLATE_NOOP("QObject", "Zaiwa"));
    addLanguage({"atd"}, QT_TRANSLATE_NOOP("QObject", "Manobo, Ata"));
    addLanguage({"atg"}, QT_TRANSLATE_NOOP("QObject", "Ivbie North-Okpela-Arhe"));
    addLanguage({"att"}, QT_TRANSLATE_NOOP("QObject", "Pamplona Atta"));
    addLanguage({"auc"}, QT_TRANSLATE_NOOP("QObject", "Waorani"));
    addLanguage({"aui"}, QT_TRANSLATE_NOOP("QObject", "Anuki"));
    addLanguage({"auy"}, QT_TRANSLATE_NOOP("QObject", "Awiyaana"));
    addLanguage({"ava", "av"}, QT_TRANSLATE_NOOP("QObject", "Avaric"));
    addLanguage({"av-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Avaric (Cyrillic script)"));
    addLanguage({"ave", "ae"}, QT_TRANSLATE_NOOP("QObject", "Avestan"));
    addLanguage({"avt"}, QT_TRANSLATE_NOOP("QObject", "Au"));
    addLanguage({"awb"}, QT_TRANSLATE_NOOP("QObject", "Awa (Papua New Guinea)"));
    addLanguage({"awk"}, QT_TRANSLATE_NOOP("QObject", "Awabakal"));
    addLanguage({"awx"}, QT_TRANSLATE_NOOP("QObject", "Awara"));
    addLanguage({"azb"}, QT_TRANSLATE_NOOP("QObject", "Azerbaijani, South"));
    addLanguage({"aze", "az"}, QT_TRANSLATE_NOOP("QObject", "Azerbaijani"));
    addLanguage({"azg"}, QT_TRANSLATE_NOOP("QObject", "San Pedro Amuzgos Amuzgo"));
    addLanguage({"azz"}, QT_TRANSLATE_NOOP("QObject", "Nahuatl, Highland Puebla"));

    addLanguage({"ba-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Bashkir (Cyrillic script)"));
    addLanguage({"bak", "ba"}, QT_TRANSLATE_NOOP("QObject", "Bashkir"));
    addLanguage({"bam", "bm"}, QT_TRANSLATE_NOOP("QObject", "Bambara"));
    addLanguage({"bao"}, QT_TRANSLATE_NOOP("QObject", "Waimaha"));
    addLanguage({"bar"}, QT_TRANSLATE_NOOP("QObject", "Bavarian"));
    addLanguage({"bba"}, QT_TRANSLATE_NOOP("QObject", "Baatonum"));
    addLanguage({"bbb"}, QT_TRANSLATE_NOOP("QObject", "Barai"));
    addLanguage({"bbr"}, QT_TRANSLATE_NOOP("QObject", "Girawa"));
    addLanguage({"bch"}, QT_TRANSLATE_NOOP("QObject", "Bariai"));
    addLanguage({"bco"}, QT_TRANSLATE_NOOP("QObject", "Kaluli"));
    addLanguage({"bdd"}, QT_TRANSLATE_NOOP("QObject", "Bunama"));
    addLanguage({"bea"}, QT_TRANSLATE_NOOP("QObject", "Beaver"));
    addLanguage({"bef"}, QT_TRANSLATE_NOOP("QObject", "Benabena"));
    addLanguage({"bel", "be"}, QT_TRANSLATE_NOOP("QObject", "Belarusian"));
    addLanguage({"beo"}, QT_TRANSLATE_NOOP("QObject", "Beami"));
    addLanguage({"beu"}, QT_TRANSLATE_NOOP("QObject", "Blagar"));
    addLanguage({"bgs"}, QT_TRANSLATE_NOOP("QObject", "Tagabawa"));
    addLanguage({"bgt"}, QT_TRANSLATE_NOOP("QObject", "Bughotu"));
    addLanguage({"bhg"}, QT_TRANSLATE_NOOP("QObject", "Binandere"));
    addLanguage({"bhl"}, QT_TRANSLATE_NOOP("QObject", "Bimin"));
    addLanguage({"big"}, QT_TRANSLATE_NOOP("QObject", "Biangai"));
    addLanguage({"bih", "bh"}, QT_TRANSLATE_NOOP("QObject", "Bihari"));
    addLanguage({"bis", "bi"}, QT_TRANSLATE_NOOP("QObject", "Bislama"));
    addLanguage({"bjp"}, QT_TRANSLATE_NOOP("QObject", "Fanamaket"));
    addLanguage({"bjr"}, QT_TRANSLATE_NOOP("QObject", "Binumarien"));
    addLanguage({"bjv"}, QT_TRANSLATE_NOOP("QObject", "Bedjond"));
    addLanguage({"bkd"}, QT_TRANSLATE_NOOP("QObject", "Binukid"));
    addLanguage({"bki"}, QT_TRANSLATE_NOOP("QObject", "Baki"));
    addLanguage({"bkq"}, QT_TRANSLATE_NOOP("QObject", "Bakair\u00ed"));
    addLanguage({"bkx"}, QT_TRANSLATE_NOOP("QObject", "Baikeno"));
    addLanguage({"bla"}, QT_TRANSLATE_NOOP("QObject", "Siksika"));
    addLanguage({"blw"}, QT_TRANSLATE_NOOP("QObject", "Balangao"));
    addLanguage({"blz"}, QT_TRANSLATE_NOOP("QObject", "Balantak"));
    addLanguage({"bmh"}, QT_TRANSLATE_NOOP("QObject", "Kein"));
    addLanguage({"bmk"}, QT_TRANSLATE_NOOP("QObject", "Ghayavi"));
    addLanguage({"bmr"}, QT_TRANSLATE_NOOP("QObject", "Muinane"));
    // New value for "bmu" is "Somba-Siawari"
    addLanguage({"bmu"}, QT_TRANSLATE_NOOP("QObject", "Burum-Mindik"));
    addLanguage({"bn"}, QT_TRANSLATE_NOOP("QObject", "Bengali"));
    addLanguage({"bnp"}, QT_TRANSLATE_NOOP("QObject", "Bola"));
    addLanguage({"boa"}, QT_TRANSLATE_NOOP("QObject", "Bora"));
    addLanguage({"bod", "bo"}, QT_TRANSLATE_NOOP("QObject", "Tibetan"));
    addLanguage({"boj"}, QT_TRANSLATE_NOOP("QObject", "Anjam"));
    addLanguage({"bon"}, QT_TRANSLATE_NOOP("QObject", "Bine"));
    addLanguage({"bos", "bs"}, QT_TRANSLATE_NOOP("QObject", "Bosnian"));
    addLanguage({"box"}, QT_TRANSLATE_NOOP("QObject", "Buamu"));
    addLanguage({"bpr"}, QT_TRANSLATE_NOOP("QObject", "Blaan, Koronadal"));
    addLanguage({"bps"}, QT_TRANSLATE_NOOP("QObject", "Blaan, Sarangani"));
    addLanguage({"bqc"}, QT_TRANSLATE_NOOP("QObject", "Boko (Benin)"));
    addLanguage({"bqp"}, QT_TRANSLATE_NOOP("QObject", "Busa"));
    addLanguage({"bre", "br"}, QT_TRANSLATE_NOOP("QObject", "Breton"));
    addLanguage({"bsn"}, QT_TRANSLATE_NOOP("QObject", "Barasana-Eduria"));
    addLanguage({"bsp"}, QT_TRANSLATE_NOOP("QObject", "Baga Sitemu"));
    addLanguage({"bss"}, QT_TRANSLATE_NOOP("QObject", "Akoose"));
    addLanguage({"bua"}, QT_TRANSLATE_NOOP("QObject", "Buriat"));
    addLanguage({"bua-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Buriat (Cyrillic script)"));
    addLanguage({"buk"}, QT_TRANSLATE_NOOP("QObject", "Bugawac"));
    addLanguage({"bul", "bg"}, QT_TRANSLATE_NOOP("QObject", "Bulgarian"));
    addLanguage({"bus"}, QT_TRANSLATE_NOOP("QObject", "Bokobaru"));
    addLanguage({"bvd"}, QT_TRANSLATE_NOOP("QObject", "Baeggu"));
    addLanguage({"bvr"}, QT_TRANSLATE_NOOP("QObject", "Burarra"));
    addLanguage({"bxh"}, QT_TRANSLATE_NOOP("QObject", "Buhutu"));
    addLanguage({"byr"}, QT_TRANSLATE_NOOP("QObject", "Baruya"));
    addLanguage({"byx"}, QT_TRANSLATE_NOOP("QObject", "Qaqet"));
    addLanguage({"bzd"}, QT_TRANSLATE_NOOP("QObject", "Bribri"));
    addLanguage({"bzh"}, QT_TRANSLATE_NOOP("QObject", "Mapos Buang"));
    addLanguage({"bzj"}, QT_TRANSLATE_NOOP("QObject", "English, Belize Kriol"));

    addLanguage({"caa"}, QT_TRANSLATE_NOOP("QObject", "Chort\u00ed"));
    addLanguage({"cab"}, QT_TRANSLATE_NOOP("QObject", "Garifuna"));
    addLanguage({"cac"}, QT_TRANSLATE_NOOP("QObject", "Chuj"));
    addLanguage({"caf"}, QT_TRANSLATE_NOOP("QObject", "Southern Carrier"));
    addLanguage({"cak"}, QT_TRANSLATE_NOOP("QObject", "Kaqchikel"));
    addLanguage({"cao"}, QT_TRANSLATE_NOOP("QObject", "Ch\u00e1cobo"));
    addLanguage({"cap"}, QT_TRANSLATE_NOOP("QObject", "Chipaya"));
    addLanguage({"car"}, QT_TRANSLATE_NOOP("QObject", "Galibi Carib"));
    addLanguage({"cat", "ca"}, QT_TRANSLATE_NOOP("QObject", "Catalan"));
    addLanguage({"cav"}, QT_TRANSLATE_NOOP("QObject", "Cavine\u00f1a"));
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
    addLanguage({"ceb"}, QT_TRANSLATE_NOOP("QObject", "Cebuano"));
    addLanguage({"cek"}, QT_TRANSLATE_NOOP("QObject", "Eastern Khumi Chin"));
    addLanguage({"ces", "cs"}, QT_TRANSLATE_NOOP("QObject", "Czech"));
    addLanguage({"cgc"}, QT_TRANSLATE_NOOP("QObject", "Kagayanen"));
    addLanguage({"cha", "ch"}, QT_TRANSLATE_NOOP("QObject", "Chamorro"));
    addLanguage({"chd"}, QT_TRANSLATE_NOOP("QObject", "Highland Oaxaca Chontal"));
    addLanguage({"che", "ce"}, QT_TRANSLATE_NOOP("QObject", "Chechen"));
    addLanguage({"ce-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Chechen (Cyrillic script)"));
    addLanguage({"chf"}, QT_TRANSLATE_NOOP("QObject", "Tabasco Chontal"));
    addLanguage({"chk"}, QT_TRANSLATE_NOOP("QObject", "Chuukese"));
    addLanguage({"chq"}, QT_TRANSLATE_NOOP("QObject", "Chinantec, Quiotepec"));
    addLanguage({"chr"}, QT_TRANSLATE_NOOP("QObject", "Cherokee"));
    addLanguage({"chv", "cv"}, QT_TRANSLATE_NOOP("QObject", "Chuvash"));
    addLanguage({"chz"}, QT_TRANSLATE_NOOP("QObject", "Chinantec, Ozumac\u00edn"));
    addLanguage({"cjo"}, QT_TRANSLATE_NOOP("QObject", "Ash\u00e9ninka Pajonal"));
    addLanguage({"cjs"}, QT_TRANSLATE_NOOP("QObject", "Shor"));
    addLanguage({"cjs-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Shor (Cyrillic script)"));
    addLanguage({"cjv"}, QT_TRANSLATE_NOOP("QObject", "Chuave"));
    addLanguage({"ckb"}, QT_TRANSLATE_NOOP("QObject", "Kurdish, Central"));
    addLanguage({"ckt"}, QT_TRANSLATE_NOOP("QObject", "Chukot"));
    addLanguage({"ckt-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Chukot (Cyrillic script)"));
    addLanguage({"ckw"}, QT_TRANSLATE_NOOP("QObject", "Western Cakchiquel"));
    addLanguage({"cle"}, QT_TRANSLATE_NOOP("QObject", "Chinantec, Lealao"));
    addLanguage({"clu"}, QT_TRANSLATE_NOOP("QObject", "Caluyanun"));
    addLanguage({"cme"}, QT_TRANSLATE_NOOP("QObject", "Cerma"));
    addLanguage({"cmn"}, QT_TRANSLATE_NOOP("QObject", "Chinese, Mandarin"));
    addLanguage({"cni"}, QT_TRANSLATE_NOOP("QObject", "Ash\u00e1ninka"));
    addLanguage({"cnl"}, QT_TRANSLATE_NOOP("QObject", "Chinantec, Lalana"));
    addLanguage({"cnt"}, QT_TRANSLATE_NOOP("QObject", "Chinantec, Tepetotutla"));
    addLanguage({"cof"}, QT_TRANSLATE_NOOP("QObject", "Colorado"));
    addLanguage({"con"}, QT_TRANSLATE_NOOP("QObject", "Cof\u00e1n"));
    addLanguage({"cop", "cop-sa"}, QT_TRANSLATE_NOOP("QObject", "Coptic"));
    addLanguage({"cor", "kw"}, QT_TRANSLATE_NOOP("QObject", "Cornish"));
    addLanguage({"cos", "co"}, QT_TRANSLATE_NOOP("QObject", "Corsican"));
    addLanguage({"cot"}, QT_TRANSLATE_NOOP("QObject", "Caquinte"));
    addLanguage({"cpa"}, QT_TRANSLATE_NOOP("QObject", "Chinantec, Palantla"));
    addLanguage({"cpb"}, QT_TRANSLATE_NOOP("QObject", "Ucayali-Yur\u00faa Ash\u00e9ninka"));
    addLanguage({"cpc"}, QT_TRANSLATE_NOOP("QObject", "Ajy\u00edninka Apurucayali"));
    addLanguage({"cpu"}, QT_TRANSLATE_NOOP("QObject", "Pichis Ash\u00e9ninka"));
    addLanguage({"cpy"}, QT_TRANSLATE_NOOP("QObject", "South Ucayali Ash\u00e9ninka"));
    addLanguage({"crh"}, QT_TRANSLATE_NOOP("QObject", "Crimean Tatar"));
    addLanguage({"crh-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Crimean Tatar (Cyrillic script)"));
    addLanguage({"crn"}, QT_TRANSLATE_NOOP("QObject", "El Nayar Cora"));
    addLanguage({"crx"}, QT_TRANSLATE_NOOP("QObject", "Carrier"));
    addLanguage({"cso"}, QT_TRANSLATE_NOOP("QObject", "Chinantec, Sochiapan"));
    addLanguage({"cta"}, QT_TRANSLATE_NOOP("QObject", "Tataltepec Chatino"));
    addLanguage({"cth"}, QT_TRANSLATE_NOOP("QObject", "Thaiphum Chin"));
    addLanguage({"cti"}, QT_TRANSLATE_NOOP("QObject", "Tila Chol"));
    addLanguage({"ctp"}, QT_TRANSLATE_NOOP("QObject", "Western Highland Chatino"));
    addLanguage({"ctu"}, QT_TRANSLATE_NOOP("QObject", "Chol"));
    addLanguage({"cu"}, QT_TRANSLATE_NOOP("QObject", "Church Slavic"));
    addLanguage({"cub"}, QT_TRANSLATE_NOOP("QObject", "Cubeo"));
    addLanguage({"cuc"}, QT_TRANSLATE_NOOP("QObject", "Chinantec, Usila"));
    addLanguage({"cui"}, QT_TRANSLATE_NOOP("QObject", "Cuiba"));
    addLanguage({"cuk"}, QT_TRANSLATE_NOOP("QObject", "San Blas Kuna"));
    addLanguage({"cut"}, QT_TRANSLATE_NOOP("QObject", "Teutila Cuicatec"));
    addLanguage({"cux"}, QT_TRANSLATE_NOOP("QObject", "Tepeuxila Cuicatec"));
    addLanguage({"cv-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Chuvash (Cyrillic script)"));
    addLanguage({"cwe"}, QT_TRANSLATE_NOOP("QObject", "Kwere"));
    addLanguage({"cya"}, QT_TRANSLATE_NOOP("QObject", "Nopala Chatino"));
    addLanguage({"cym", "cy"}, QT_TRANSLATE_NOOP("QObject", "Welsh"));

    addLanguage({"daa"}, QT_TRANSLATE_NOOP("QObject", "Dangal\u00e9at"));
    addLanguage({"dad"}, QT_TRANSLATE_NOOP("QObject", "Marik"));
    addLanguage({"dah"}, QT_TRANSLATE_NOOP("QObject", "Gwahatike"));
    addLanguage({"dan", "da"}, QT_TRANSLATE_NOOP("QObject", "Danish"));
    addLanguage({"dar"}, QT_TRANSLATE_NOOP("QObject", "Dargwa"));
    addLanguage({"dar-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Dargwa (Cyrillic script)"));
    addLanguage({"ded"}, QT_TRANSLATE_NOOP("QObject", "Dedua"));
    addLanguage({"deu", "de"}, QT_TRANSLATE_NOOP("QObject", "German"));
    addLanguage({"dgc"}, QT_TRANSLATE_NOOP("QObject", "Casiguran Dumagat Agta"));
    addLanguage({"dgr"}, QT_TRANSLATE_NOOP("QObject", "Dogrib"));
    addLanguage({"dgz"}, QT_TRANSLATE_NOOP("QObject", "Daga"));
    addLanguage({"dhg"}, QT_TRANSLATE_NOOP("QObject", "Dhangu-Dhangu"));
    addLanguage({"dif"}, QT_TRANSLATE_NOOP("QObject", "Dieri"));
    addLanguage({"dik"}, QT_TRANSLATE_NOOP("QObject", "Southwestern Dinka"));
    addLanguage({"dji"}, QT_TRANSLATE_NOOP("QObject", "Djinang"));
    // New value for "djk" is "Eastern Maroon Creole"
    addLanguage({"djk"}, QT_TRANSLATE_NOOP("QObject", "Aukan"));
    addLanguage({"djr"}, QT_TRANSLATE_NOOP("QObject", "Djambarrpuyngu"));
    addLanguage({"dng"}, QT_TRANSLATE_NOOP("QObject", "Dungan"));
    addLanguage({"dng-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Dungan (Cyrillic script)"));
    addLanguage({"dnj"}, QT_TRANSLATE_NOOP("QObject", "Dan"));
    addLanguage({"dob"}, QT_TRANSLATE_NOOP("QObject", "Dobu"));
    addLanguage({"dop"}, QT_TRANSLATE_NOOP("QObject", "Lukpa"));
    addLanguage({"dug"}, QT_TRANSLATE_NOOP("QObject", "Duruma"));
    addLanguage({"dwr"}, QT_TRANSLATE_NOOP("QObject", "Dawro"));
    addLanguage({"dww"}, QT_TRANSLATE_NOOP("QObject", "Dawawa"));
    addLanguage({"dwy"}, QT_TRANSLATE_NOOP("QObject", "Dhuwaya"));
    addLanguage({"dzo", "dz"}, QT_TRANSLATE_NOOP("QObject", "Dzongkha"));

    addLanguage({"ebk"}, QT_TRANSLATE_NOOP("QObject", "East Bontok"));
    addLanguage({"eko"}, QT_TRANSLATE_NOOP("QObject", "Koti"));
    addLanguage({"ell", "el", "gre"}, QT_TRANSLATE_NOOP("QObject", "Greek, Modern (1453-)"));
    addLanguage({"emi"}, QT_TRANSLATE_NOOP("QObject", "Mussau-Emira"));
    addLanguage({"emp"}, QT_TRANSLATE_NOOP("QObject", "Northern Ember\u00e1"));
    addLanguage({"en-US"}, QT_TRANSLATE_NOOP("QObject", "English, American"));
    addLanguage({"eng", "en"}, QT_TRANSLATE_NOOP("QObject", "English"));
    addLanguage({"enm"}, QT_TRANSLATE_NOOP("QObject", "English, Middle (1100-1500)"));
    addLanguage({"enq"}, QT_TRANSLATE_NOOP("QObject", "Enga"));
    addLanguage({"epo", "eo"}, QT_TRANSLATE_NOOP("QObject", "Esperanto"));
    addLanguage({"eri"}, QT_TRANSLATE_NOOP("QObject", "Ogea"));
    addLanguage({"ese"}, QT_TRANSLATE_NOOP("QObject", "Ese Ejja"));
    addLanguage({"esg"}, QT_TRANSLATE_NOOP("QObject", "Aheri Gondi"));
    addLanguage({"esk"}, QT_TRANSLATE_NOOP("QObject", "Northwest Alaska Inupiatun"));
    addLanguage({"est", "et"}, QT_TRANSLATE_NOOP("QObject", "Estonian"));
    addLanguage({"etr"}, QT_TRANSLATE_NOOP("QObject", "Edolo"));
    addLanguage({"eus", "eu"}, QT_TRANSLATE_NOOP("QObject", "Basque"));

    addLanguage({"faa"}, QT_TRANSLATE_NOOP("QObject", "Fasu"));
    addLanguage({"fai"}, QT_TRANSLATE_NOOP("QObject", "Faiwol"));
    addLanguage({"fao", "fo"}, QT_TRANSLATE_NOOP("QObject", "Faroese"));
    addLanguage({"far"}, QT_TRANSLATE_NOOP("QObject", "Fataleka"));
    addLanguage({"fas", "fa"}, QT_TRANSLATE_NOOP("QObject", "Persian"));
    addLanguage({"ffm"}, QT_TRANSLATE_NOOP("QObject", "Fulfulde, Maasina"));
    addLanguage({"fin", "fi"}, QT_TRANSLATE_NOOP("QObject", "Finnish"));
    addLanguage({"fij", "fj"}, QT_TRANSLATE_NOOP("QObject", "Fijian"));
    addLanguage({"for"}, QT_TRANSLATE_NOOP("QObject", "Fore"));
    addLanguage({"fra", "fr"}, QT_TRANSLATE_NOOP("QObject", "French"));
    addLanguage({"fry", "fy"}, QT_TRANSLATE_NOOP("QObject", "Western Frisian"));
    addLanguage({"fue"}, QT_TRANSLATE_NOOP("QObject", "Fulfulde, Borgu"));
    addLanguage({"fuf"}, QT_TRANSLATE_NOOP("QObject", "Pular"));
    addLanguage({"fuh"}, QT_TRANSLATE_NOOP("QObject", "Fulfulde, Western Niger"));

    addLanguage({"gag"}, QT_TRANSLATE_NOOP("QObject", "Gagauz"));
    addLanguage({"gag-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Gagauz (Cyrillic script)"));
    addLanguage({"gag-Latn"}, QT_TRANSLATE_NOOP("QObject", "Gagauz (Latin script)"));
    addLanguage({"gah"}, QT_TRANSLATE_NOOP("QObject", "Alekano"));
    addLanguage({"gai"}, QT_TRANSLATE_NOOP("QObject", "Borei"));
    addLanguage({"gam"}, QT_TRANSLATE_NOOP("QObject", "Kandawo"));
    addLanguage({"gaw"}, QT_TRANSLATE_NOOP("QObject", "Nobonob"));
    addLanguage({"gd"}, QT_TRANSLATE_NOOP("QObject", "Gaelic (Scots)"));
    // New value for "gdn" is "Umanakaina"
    addLanguage({"gdn"}, QT_TRANSLATE_NOOP("QObject", "Mo'da"));
    addLanguage({"gdr"}, QT_TRANSLATE_NOOP("QObject", "Wipi"));
    addLanguage({"geb"}, QT_TRANSLATE_NOOP("QObject", "Kire"));
    addLanguage({"gez"}, QT_TRANSLATE_NOOP("QObject", "Geez"));
    addLanguage({"gfk"}, QT_TRANSLATE_NOOP("QObject", "Patpatar"));
    addLanguage({"ghs"}, QT_TRANSLATE_NOOP("QObject", "Guhu-Samane"));
    addLanguage({"gia"}, QT_TRANSLATE_NOOP("QObject", "Kija"));
    addLanguage({"gle", "ga"}, QT_TRANSLATE_NOOP("QObject", "Irish"));
    addLanguage({"glk"}, QT_TRANSLATE_NOOP("QObject", "Gilaki"));
    addLanguage({"glg", "gl"}, QT_TRANSLATE_NOOP("QObject", "Galician"));
    addLanguage({"gmv"}, QT_TRANSLATE_NOOP("QObject", "Gamo"));
    addLanguage({"gng"}, QT_TRANSLATE_NOOP("QObject", "Ngangam"));
    addLanguage({"gnn"}, QT_TRANSLATE_NOOP("QObject", "Gumatj"));
    addLanguage({"gnw"}, QT_TRANSLATE_NOOP("QObject", "Western Bolivian Guaran\u00ed"));
    addLanguage({"gof"}, QT_TRANSLATE_NOOP("QObject", "Gofa"));
    addLanguage({"got"}, QT_TRANSLATE_NOOP("QObject", "Gothic"));
    addLanguage({"grc"}, QT_TRANSLATE_NOOP("QObject", "Greek, Ancient (to 1453)"));
    addLanguage({"grn", "gn"}, QT_TRANSLATE_NOOP("QObject", "Guarani"));
    addLanguage({"gub"}, QT_TRANSLATE_NOOP("QObject", "Guajaj\u00e1ra"));
    addLanguage({"gue"}, QT_TRANSLATE_NOOP("QObject", "Gurinji"));
    addLanguage({"guh"}, QT_TRANSLATE_NOOP("QObject", "Guahibo"));
    addLanguage({"gui"}, QT_TRANSLATE_NOOP("QObject", "Eastern Bolivian Guaran\u00ed"));
    addLanguage({"guj", "gu"}, QT_TRANSLATE_NOOP("QObject", "Gujarati"));
    addLanguage({"gul"}, QT_TRANSLATE_NOOP("QObject", "English, Sea Island Creole"));
    addLanguage({"gum"}, QT_TRANSLATE_NOOP("QObject", "Guambiano"));
    addLanguage({"gun"}, QT_TRANSLATE_NOOP("QObject", "Mby\u00e1 Guaran\u00ed"));
    addLanguage({"guo"}, QT_TRANSLATE_NOOP("QObject", "Guayabero"));
    addLanguage({"gup"}, QT_TRANSLATE_NOOP("QObject", "Gunwinggu"));
    addLanguage({"gv"}, QT_TRANSLATE_NOOP("QObject", "Manx"));
    addLanguage({"gvc"}, QT_TRANSLATE_NOOP("QObject", "Guanano"));
    addLanguage({"gvf"}, QT_TRANSLATE_NOOP("QObject", "Golin"));
    addLanguage({"gvn"}, QT_TRANSLATE_NOOP("QObject", "Kuku-Yalanji"));
    addLanguage({"gwi"}, QT_TRANSLATE_NOOP("QObject", "Gwichʼin"));
    addLanguage({"gym"}, QT_TRANSLATE_NOOP("QObject", "Ng\u00e4bere"));
    addLanguage({"gyr"}, QT_TRANSLATE_NOOP("QObject", "Guarayu"));

    addLanguage({"hans"}, QT_TRANSLATE_NOOP("QObject", "Chinese, Simplified"));
    addLanguage({"hant"}, QT_TRANSLATE_NOOP("QObject", "Chinese, Traditional"));
    addLanguage({"hat", "ht"}, QT_TRANSLATE_NOOP("QObject", "Haitian"));
    addLanguage({"hau"}, QT_TRANSLATE_NOOP("QObject", "Hausa"));
    addLanguage({"haw"}, QT_TRANSLATE_NOOP("QObject", "Hawaiian"));
    addLanguage({"hbo"}, QT_TRANSLATE_NOOP("QObject", "Hebrew, Ancient"));
    addLanguage({"hch"}, QT_TRANSLATE_NOOP("QObject", "Huichol"));
    addLanguage({"heb", "he"}, QT_TRANSLATE_NOOP("QObject", "Hebrew"));
    addLanguage({"heg"}, QT_TRANSLATE_NOOP("QObject", "Helong"));
    addLanguage({"her", "hz"}, QT_TRANSLATE_NOOP("QObject", "Herero"));
    addLanguage({"hin", "hi"}, QT_TRANSLATE_NOOP("QObject", "Hindi"));
    addLanguage({"hix"}, QT_TRANSLATE_NOOP("QObject", "Hixkary\u00e1na"));
    addLanguage({"hla"}, QT_TRANSLATE_NOOP("QObject", "Halia"));
    // New value for "hlt" is "Matu Chin"
    addLanguage({"hlt"}, QT_TRANSLATE_NOOP("QObject", "Nga La"));
    addLanguage({"hmo", "ho"}, QT_TRANSLATE_NOOP("QObject", "Hiri Motu"));
    addLanguage({"hns"}, QT_TRANSLATE_NOOP("QObject", "Caribbean Hindustani"));
    addLanguage({"hop"}, QT_TRANSLATE_NOOP("QObject", "Hopi"));
    addLanguage({"hot"}, QT_TRANSLATE_NOOP("QObject", "Hote"));
    addLanguage({"hrv", "hr"}, QT_TRANSLATE_NOOP("QObject", "Croatian"));
    addLanguage({"hto"}, QT_TRANSLATE_NOOP("QObject", "Huitoto, Minica"));
    addLanguage({"hub"}, QT_TRANSLATE_NOOP("QObject", "Huambisa"));
    addLanguage({"hui"}, QT_TRANSLATE_NOOP("QObject", "Huli"));
    addLanguage({"hun", "hu"}, QT_TRANSLATE_NOOP("QObject", "Hungarian"));
    addLanguage({"hus"}, QT_TRANSLATE_NOOP("QObject", "Huastec"));
    addLanguage({"huu"}, QT_TRANSLATE_NOOP("QObject", "Huitoto, Murui"));
    addLanguage({"huv"}, QT_TRANSLATE_NOOP("QObject", "Huave, San Mateo Del Mar"));
    addLanguage({"hvn"}, QT_TRANSLATE_NOOP("QObject", "Sabu"));
    addLanguage({"hwc"}, QT_TRANSLATE_NOOP("QObject", "English, Hawai'i Creole"));
    addLanguage({"hye", "hy"}, QT_TRANSLATE_NOOP("QObject", "Armenian"));

    addLanguage({"ian"}, QT_TRANSLATE_NOOP("QObject", "Iatmul"));
    addLanguage({"id"}, QT_TRANSLATE_NOOP("QObject", "Indonesian"));
    addLanguage({"ign"}, QT_TRANSLATE_NOOP("QObject", "Ignaciano"));
    addLanguage({"ikk"}, QT_TRANSLATE_NOOP("QObject", "Ika"));
    addLanguage({"iku", "iu"}, QT_TRANSLATE_NOOP("QObject", "Inuktitut"));
    addLanguage({"ikw"}, QT_TRANSLATE_NOOP("QObject", "Ikwere"));
    addLanguage({"ile", "ie"}, QT_TRANSLATE_NOOP("QObject", "Interlingue"));
    addLanguage({"imo"}, QT_TRANSLATE_NOOP("QObject", "Imbongu"));
    addLanguage({"ina", "ia"}, QT_TRANSLATE_NOOP("QObject", "Interlingua"));
    addLanguage({"inb"}, QT_TRANSLATE_NOOP("QObject", "Inga"));
    addLanguage({"ing"}, QT_TRANSLATE_NOOP("QObject", "Degexit'an"));
    addLanguage({"ing-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Degexit'an (Cyrillic script)"));
    addLanguage({"ino"}, QT_TRANSLATE_NOOP("QObject", "Inoke-Yate"));
    addLanguage({"iou"}, QT_TRANSLATE_NOOP("QObject", "Tuma-Irumu"));
    addLanguage({"ipi"}, QT_TRANSLATE_NOOP("QObject", "Ipili"));
    addLanguage({"ipk", "ik"}, QT_TRANSLATE_NOOP("QObject", "Inupiaq"));
    addLanguage({"iqw"}, QT_TRANSLATE_NOOP("QObject", "Ikwo"));
    addLanguage({"isl", "is"}, QT_TRANSLATE_NOOP("QObject", "Icelandic"));
    addLanguage({"ita", "it"}, QT_TRANSLATE_NOOP("QObject", "Italian"));
    addLanguage({"itz"}, QT_TRANSLATE_NOOP("QObject", "Itz\u00e1"));
    addLanguage({"iws"}, QT_TRANSLATE_NOOP("QObject", "Sepik Iwam"));
    addLanguage({"ixl"}, QT_TRANSLATE_NOOP("QObject", "Ixil, San Juan Cotzal"));
    addLanguage({"izz"}, QT_TRANSLATE_NOOP("QObject", "Izii"));

    // New value for "jac" is "Popti'"
    addLanguage({"jac"}, QT_TRANSLATE_NOOP("QObject", "Jacalteco, Eastern"));
    addLanguage({"jae"}, QT_TRANSLATE_NOOP("QObject", "Yabem"));
    addLanguage({"jao"}, QT_TRANSLATE_NOOP("QObject", "Yanyuwa"));
    addLanguage({"jic"}, QT_TRANSLATE_NOOP("QObject", "Tol"));
    addLanguage({"jiv"}, QT_TRANSLATE_NOOP("QObject", "Shuar"));
    addLanguage({"jpn", "ja"}, QT_TRANSLATE_NOOP("QObject", "Japanese"));
    addLanguage({"jvn"}, QT_TRANSLATE_NOOP("QObject", "Javanese, Caribbean"));

    addLanguage({"kaa"}, QT_TRANSLATE_NOOP("QObject", "Kara-Kalpak"));
    addLanguage({"kaa-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Kara-Kalpak (Cyrillic script)"));
    addLanguage({"kaa-Latn"}, QT_TRANSLATE_NOOP("QObject", "Kara-Kalpak (Latin script)"));
    addLanguage({"kal", "kl"}, QT_TRANSLATE_NOOP("QObject", "Kalaallisut"));
    addLanguage({"kan", "kn"}, QT_TRANSLATE_NOOP("QObject", "Kannada"));
    addLanguage({"kap"}, QT_TRANSLATE_NOOP("QObject", "Bezhta"));
    addLanguage({"kap-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Bezhta (Cyrillic script)"));
    addLanguage({"kaq"}, QT_TRANSLATE_NOOP("QObject", "Capanahua"));
    addLanguage({"kas", "ks"}, QT_TRANSLATE_NOOP("QObject", "Kashmiri"));
    addLanguage({"kat", "ka"}, QT_TRANSLATE_NOOP("QObject", "Georgian"));
    addLanguage({"kaz", "kk"}, QT_TRANSLATE_NOOP("QObject", "Kazakh"));
    addLanguage({"kbc"}, QT_TRANSLATE_NOOP("QObject", "Kadiw\u00e9u"));
    addLanguage({"kbd"}, QT_TRANSLATE_NOOP("QObject", "Kabardian"));
    addLanguage({"kbh"}, QT_TRANSLATE_NOOP("QObject", "Cams\u00e1"));
    addLanguage({"kbm"}, QT_TRANSLATE_NOOP("QObject", "Iwal"));
    addLanguage({"kbq"}, QT_TRANSLATE_NOOP("QObject", "Kamano"));
    addLanguage({"kdc"}, QT_TRANSLATE_NOOP("QObject", "Kutu"));
    addLanguage({"kde"}, QT_TRANSLATE_NOOP("QObject", "Makonde"));
    addLanguage({"kdl"}, QT_TRANSLATE_NOOP("QObject", "Tsikimba"));
    addLanguage({"kek"}, QT_TRANSLATE_NOOP("QObject", "Kekch\u00ed"));
    addLanguage({"ken"}, QT_TRANSLATE_NOOP("QObject", "Kenyang"));
    addLanguage({"kew"}, QT_TRANSLATE_NOOP("QObject", "West Kewa"));
    addLanguage({"kgf"}, QT_TRANSLATE_NOOP("QObject", "Kube"));
    addLanguage({"kgk"}, QT_TRANSLATE_NOOP("QObject", "Kaiw\u00e1"));
    addLanguage({"kgp"}, QT_TRANSLATE_NOOP("QObject", "Kaingang"));
    addLanguage({"khm", "km"}, QT_TRANSLATE_NOOP("QObject", "Khmer"));
    addLanguage({"khs"}, QT_TRANSLATE_NOOP("QObject", "Kasua"));
    addLanguage({"khz"}, QT_TRANSLATE_NOOP("QObject", "Keapara"));
    addLanguage({"kik", "ki"}, QT_TRANSLATE_NOOP("QObject", "Kikuyu"));
    addLanguage({"kin", "rw"}, QT_TRANSLATE_NOOP("QObject", "Kinyarwanda"));
    addLanguage({"kir", "ky"}, QT_TRANSLATE_NOOP("QObject", "Kirghiz"));
    addLanguage({"ky-Arab"}, QT_TRANSLATE_NOOP("QObject", "Kirghiz (Arabic script)"));
    addLanguage({"ky-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Kirghiz (Cyrillic script)"));
    addLanguage({"kje"}, QT_TRANSLATE_NOOP("QObject", "Kisar"));
    addLanguage({"kjh"}, QT_TRANSLATE_NOOP("QObject", "Khakas"));
    addLanguage({"kjh-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Khakas (Cyrillic script)"));
    addLanguage({"kjs"}, QT_TRANSLATE_NOOP("QObject", "East Kewa"));
    addLanguage({"kkc"}, QT_TRANSLATE_NOOP("QObject", "Odoodee"));
    addLanguage({"kkl"}, QT_TRANSLATE_NOOP("QObject", "Kosarek Yale"));
    addLanguage({"kky"}, QT_TRANSLATE_NOOP("QObject", "Guguyimidjir"));
    addLanguage({"klt"}, QT_TRANSLATE_NOOP("QObject", "Nukna"));
    addLanguage({"klv"}, QT_TRANSLATE_NOOP("QObject", "Maskelynes"));
    addLanguage({"kmg"}, QT_TRANSLATE_NOOP("QObject", "K\u00e2te"));
    addLanguage({"kmh"}, QT_TRANSLATE_NOOP("QObject", "Kalam"));
    addLanguage({"kmk"}, QT_TRANSLATE_NOOP("QObject", "Limos Kalinga"));
    addLanguage({"kmo"}, QT_TRANSLATE_NOOP("QObject", "Kwoma"));
    addLanguage({"kmr"}, QT_TRANSLATE_NOOP("QObject", "Kurdish, Northern"));
    addLanguage({"kmr-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Kurdish, Northern (Cyrillic script)"));
    addLanguage({"kmr-Latn"}, QT_TRANSLATE_NOOP("QObject", "Kurdish, Northern (Latin script)"));
    addLanguage({"kms"}, QT_TRANSLATE_NOOP("QObject", "Kamasau"));
    addLanguage({"kmu"}, QT_TRANSLATE_NOOP("QObject", "Kanite"));
    addLanguage({"kne"}, QT_TRANSLATE_NOOP("QObject", "Kankanaey"));
    addLanguage({"knf"}, QT_TRANSLATE_NOOP("QObject", "Mankanya"));
    addLanguage({"knj"}, QT_TRANSLATE_NOOP("QObject", "Western Kanjobal"));
    addLanguage({"knv"}, QT_TRANSLATE_NOOP("QObject", "Tabo"));
    addLanguage({"kom", "kv"}, QT_TRANSLATE_NOOP("QObject", "Komi"));
    addLanguage({"kor", "ko"}, QT_TRANSLATE_NOOP("QObject", "Korean"));
    addLanguage({"kos"}, QT_TRANSLATE_NOOP("QObject", "Kosraean"));
    addLanguage({"kpf"}, QT_TRANSLATE_NOOP("QObject", "Komba"));
    addLanguage({"kpg"}, QT_TRANSLATE_NOOP("QObject", "Kapingamarangi"));
    addLanguage({"kpj"}, QT_TRANSLATE_NOOP("QObject", "Karaj\u00e1"));
    addLanguage({"kpl"}, QT_TRANSLATE_NOOP("QObject", "Kpala"));
    addLanguage({"kpr"}, QT_TRANSLATE_NOOP("QObject", "Korafe-Yegha"));
    addLanguage({"kpw"}, QT_TRANSLATE_NOOP("QObject", "Kobon"));
    addLanguage({"kpx"}, QT_TRANSLATE_NOOP("QObject", "Mountain Koiali"));
    addLanguage({"kpy"}, QT_TRANSLATE_NOOP("QObject", "Koryak"));
    addLanguage({"kpy-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Koryak (Cyrillic script)"));
    addLanguage({"kqa"}, QT_TRANSLATE_NOOP("QObject", "Mum"));
    addLanguage({"kqc"}, QT_TRANSLATE_NOOP("QObject", "Doromu-Koki"));
    addLanguage({"kqf"}, QT_TRANSLATE_NOOP("QObject", "Kakabai"));
    addLanguage({"kql"}, QT_TRANSLATE_NOOP("QObject", "Kyenele"));
    addLanguage({"kqw"}, QT_TRANSLATE_NOOP("QObject", "Kandas"));
    addLanguage({"ksd"}, QT_TRANSLATE_NOOP("QObject", "Kuanua"));
    addLanguage({"ksj"}, QT_TRANSLATE_NOOP("QObject", "Uare"));
    addLanguage({"ksr"}, QT_TRANSLATE_NOOP("QObject", "Borong"));
    addLanguage({"ktm"}, QT_TRANSLATE_NOOP("QObject", "Kurti"));
    addLanguage({"kto"}, QT_TRANSLATE_NOOP("QObject", "Kuot"));
    addLanguage({"ktu"}, QT_TRANSLATE_NOOP("QObject", "Kituba (Democratic Republic of Congo)"));
    addLanguage({"kua", "kj"}, QT_TRANSLATE_NOOP("QObject", "Kuanyama"));
    addLanguage({"kud"}, QT_TRANSLATE_NOOP("QObject", "'Auhelawa"));
    addLanguage({"kue"}, QT_TRANSLATE_NOOP("QObject", "Kuman (Papua New Guinea)"));
    addLanguage({"kum"}, QT_TRANSLATE_NOOP("QObject", "Kumyk"));
    addLanguage({"kup"}, QT_TRANSLATE_NOOP("QObject", "Kunimaipa"));
    addLanguage({"kur", "ku"}, QT_TRANSLATE_NOOP("QObject", "Kurdish"));
    addLanguage({"kvg"}, QT_TRANSLATE_NOOP("QObject", "Kuni-Boazi"));
    addLanguage({"kvn"}, QT_TRANSLATE_NOOP("QObject", "Border Kuna"));
    addLanguage({"kwd"}, QT_TRANSLATE_NOOP("QObject", "Kwaio"));
    addLanguage({"kwf"}, QT_TRANSLATE_NOOP("QObject", "Kwara'ae"));
    addLanguage({"kwi"}, QT_TRANSLATE_NOOP("QObject", "Awa-Cuaiquer"));
    addLanguage({"kwj"}, QT_TRANSLATE_NOOP("QObject", "Kwanga"));
    addLanguage({"kyc"}, QT_TRANSLATE_NOOP("QObject", "Kyaka"));
    addLanguage({"kyf"}, QT_TRANSLATE_NOOP("QObject", "Kouya"));
    addLanguage({"kyg"}, QT_TRANSLATE_NOOP("QObject", "Keyagana"));
    addLanguage({"kyq"}, QT_TRANSLATE_NOOP("QObject", "Kenga"));
    addLanguage({"kyz"}, QT_TRANSLATE_NOOP("QObject", "Kayab\u00ed"));
    addLanguage({"kze"}, QT_TRANSLATE_NOOP("QObject", "Kosena"));

    addLanguage({"lac"}, QT_TRANSLATE_NOOP("QObject", "Lacandon"));
    addLanguage({"lao", "lo"}, QT_TRANSLATE_NOOP("QObject", "Lao"));
    addLanguage({"lat", "la"}, QT_TRANSLATE_NOOP("QObject", "Latin"));
    addLanguage({"lav", "lv"}, QT_TRANSLATE_NOOP("QObject", "Latvian"));
    addLanguage({"lbb"}, QT_TRANSLATE_NOOP("QObject", "Label"));
    addLanguage({"lbk"}, QT_TRANSLATE_NOOP("QObject", "Central Bontok"));
    addLanguage({"lcm"}, QT_TRANSLATE_NOOP("QObject", "Tungag"));
    addLanguage({"leu"}, QT_TRANSLATE_NOOP("QObject", "Kara (Papua New Guinea)"));
    addLanguage({"lex"}, QT_TRANSLATE_NOOP("QObject", "Luang"));
    addLanguage({"lez"}, QT_TRANSLATE_NOOP("QObject", "Lezghian"));
    addLanguage({"lez-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Lezghian (Cyrillic script)"));
    addLanguage({"lgl"}, QT_TRANSLATE_NOOP("QObject", "Wala"));
    addLanguage({"lid"}, QT_TRANSLATE_NOOP("QObject", "Nyindrou"));
    addLanguage({"lif"}, QT_TRANSLATE_NOOP("QObject", "Limbu"));
    addLanguage({"lin", "ln"}, QT_TRANSLATE_NOOP("QObject", "Ling\u00e0la"));
    addLanguage({"lit", "lt"}, QT_TRANSLATE_NOOP("QObject", "Lithuanian"));
    addLanguage({"llg"}, QT_TRANSLATE_NOOP("QObject", "Lole"));
    addLanguage({"lmo"}, QT_TRANSLATE_NOOP("QObject", "Lombard"));
    addLanguage({"ltz", "lb"}, QT_TRANSLATE_NOOP("QObject", "Luxembourgish"));
    addLanguage({"lww"}, QT_TRANSLATE_NOOP("QObject", "Lewo"));
    addLanguage({"lzh"}, QT_TRANSLATE_NOOP("QObject", "Chinese, Literary"));

    addLanguage({"maa"}, QT_TRANSLATE_NOOP("QObject", "Mazatec, San Jer\u00f3nimo Tec\u00f3atl"));
    addLanguage({"mah", "mh"}, QT_TRANSLATE_NOOP("QObject", "Marshallese"));
    addLanguage({"maj"}, QT_TRANSLATE_NOOP("QObject", "Mazatec, Jalapa De D\u00edaz"));
    addLanguage({"mal", "ml"}, QT_TRANSLATE_NOOP("QObject", "Malayalam"));
    addLanguage({"mam"}, QT_TRANSLATE_NOOP("QObject", "Mam"));
    addLanguage({"maq"}, QT_TRANSLATE_NOOP("QObject", "Mazatec, Chiquihuitl\u00e1n"));
    addLanguage({"mar", "mr"}, QT_TRANSLATE_NOOP("QObject", "Marathi"));
    addLanguage({"mau"}, QT_TRANSLATE_NOOP("QObject", "Mazatec, Huautla"));
    addLanguage({"mav"}, QT_TRANSLATE_NOOP("QObject", "Sater\u00e9-Maw\u00e9"));
    addLanguage({"maz"}, QT_TRANSLATE_NOOP("QObject", "Central Mazahua"));
    addLanguage({"mbb"}, QT_TRANSLATE_NOOP("QObject", "Manobo, Western Bukidnon"));
    addLanguage({"mbc"}, QT_TRANSLATE_NOOP("QObject", "Macushi"));
    addLanguage({"mbh"}, QT_TRANSLATE_NOOP("QObject", "Mangseng"));
    addLanguage({"mbj"}, QT_TRANSLATE_NOOP("QObject", "Nad\u00ebb"));
    addLanguage({"mbl"}, QT_TRANSLATE_NOOP("QObject", "Maxakal\u00ed"));
    addLanguage({"mbs"}, QT_TRANSLATE_NOOP("QObject", "Manobo, Sarangani"));
    addLanguage({"mbt"}, QT_TRANSLATE_NOOP("QObject", "Manobo, Matigsalug"));
    addLanguage({"mca"}, QT_TRANSLATE_NOOP("QObject", "Maca"));
    addLanguage({"mcb"}, QT_TRANSLATE_NOOP("QObject", "Machiguenga"));
    addLanguage({"mcd"}, QT_TRANSLATE_NOOP("QObject", "Sharanahua"));
    addLanguage({"mcf"}, QT_TRANSLATE_NOOP("QObject", "Mats\u00e9s"));
    addLanguage({"mco"}, QT_TRANSLATE_NOOP("QObject", "Mixe, Coatl\u00e1n"));
    addLanguage({"mcp"}, QT_TRANSLATE_NOOP("QObject", "Makaa"));
    addLanguage({"mcq"}, QT_TRANSLATE_NOOP("QObject", "Ese"));
    addLanguage({"mdy"}, QT_TRANSLATE_NOOP("QObject", "Male (Ethiopia)"));
    addLanguage({"med"}, QT_TRANSLATE_NOOP("QObject", "Melpa"));
    addLanguage({"mee"}, QT_TRANSLATE_NOOP("QObject", "Mengen"));
    addLanguage({"mek"}, QT_TRANSLATE_NOOP("QObject", "Mekeo"));
    addLanguage({"meq"}, QT_TRANSLATE_NOOP("QObject", "Merey"));
    addLanguage({"met"}, QT_TRANSLATE_NOOP("QObject", "Mato"));
    addLanguage({"meu"}, QT_TRANSLATE_NOOP("QObject", "Motu"));
    addLanguage({"mgc"}, QT_TRANSLATE_NOOP("QObject", "Morokodo"));
    addLanguage({"mgh"}, QT_TRANSLATE_NOOP("QObject", "Makhuwa-Meetto"));
    addLanguage({"mgw"}, QT_TRANSLATE_NOOP("QObject", "Matumbi"));
    addLanguage({"mhl"}, QT_TRANSLATE_NOOP("QObject", "Mauwake"));
    addLanguage({"mib"}, QT_TRANSLATE_NOOP("QObject", "Mixtec, Atatl\u00e1huca"));
    addLanguage({"mic"}, QT_TRANSLATE_NOOP("QObject", "Mi'kmaq"));
    addLanguage({"mie"}, QT_TRANSLATE_NOOP("QObject", "Mixtec, Ocotepec"));
    addLanguage({"mig"}, QT_TRANSLATE_NOOP("QObject", "Mixtec, San Miguel El Grande"));
    addLanguage({"mih"}, QT_TRANSLATE_NOOP("QObject", "Mixtec, Chayuco"));
    addLanguage({"mil"}, QT_TRANSLATE_NOOP("QObject", "Mixtec, Pe\u00f1oles"));
    addLanguage({"mio"}, QT_TRANSLATE_NOOP("QObject", "Mixtec, Pinotepa Nacional"));
    addLanguage({"mir"}, QT_TRANSLATE_NOOP("QObject", "Mixe, Isthmus"));
    addLanguage({"mit"}, QT_TRANSLATE_NOOP("QObject", "Mixtec, Southern Puebla"));
    addLanguage({"miz"}, QT_TRANSLATE_NOOP("QObject", "Mixtec, Coatzospan"));
    addLanguage({"mjc"}, QT_TRANSLATE_NOOP("QObject", "Mixtec, San Juan Colorado"));
    addLanguage({"mkd", "mk"}, QT_TRANSLATE_NOOP("QObject", "Macedonian"));
    addLanguage({"mkj"}, QT_TRANSLATE_NOOP("QObject", "Mokilese"));
    addLanguage({"mkl"}, QT_TRANSLATE_NOOP("QObject", "Mokole"));
    addLanguage({"mkn"}, QT_TRANSLATE_NOOP("QObject", "Kupang Malay"));
    addLanguage({"mks"}, QT_TRANSLATE_NOOP("QObject", "Mixtec, Silacayoapan"));
    addLanguage({"mlg", "mg"}, QT_TRANSLATE_NOOP("QObject", "Malagasy"));
    addLanguage({"mlh"}, QT_TRANSLATE_NOOP("QObject", "Mape"));
    addLanguage({"mlp"}, QT_TRANSLATE_NOOP("QObject", "Bargam"));
    addLanguage({"mlt", "mt"}, QT_TRANSLATE_NOOP("QObject", "Maltese"));
    addLanguage({"mmo"}, QT_TRANSLATE_NOOP("QObject", "Mangga Buang"));
    addLanguage({"mmx"}, QT_TRANSLATE_NOOP("QObject", "Madak"));
    addLanguage({"mna"}, QT_TRANSLATE_NOOP("QObject", "Mbula"));
    addLanguage({"mol", "mo"}, QT_TRANSLATE_NOOP("QObject", "Moldavian"));
    addLanguage({"mon", "mn"}, QT_TRANSLATE_NOOP("QObject", "Mongolian"));
    addLanguage({"mop"}, QT_TRANSLATE_NOOP("QObject", "Mop\u00e1n Maya"));
    addLanguage({"mos"}, QT_TRANSLATE_NOOP("QObject", "Mossi"));
    addLanguage({"mox"}, QT_TRANSLATE_NOOP("QObject", "Molima"));
    addLanguage({"mph"}, QT_TRANSLATE_NOOP("QObject", "Maung"));
    addLanguage({"mpj"}, QT_TRANSLATE_NOOP("QObject", "Martu Wangka"));
    addLanguage({"mpm"}, QT_TRANSLATE_NOOP("QObject", "Mixtec, Yosond\u00faa"));
    addLanguage({"mpp"}, QT_TRANSLATE_NOOP("QObject", "Migabac"));
    addLanguage({"mps"}, QT_TRANSLATE_NOOP("QObject", "Dadibi"));
    addLanguage({"mpt"}, QT_TRANSLATE_NOOP("QObject", "Mian"));
    addLanguage({"mpx"}, QT_TRANSLATE_NOOP("QObject", "Misima-Paneati"));
    addLanguage({"mqb"}, QT_TRANSLATE_NOOP("QObject", "Mbuko"));
    addLanguage({"mqj"}, QT_TRANSLATE_NOOP("QObject", "Mamasa"));
    addLanguage({"mri", "mi"}, QT_TRANSLATE_NOOP("QObject", "Maori"));
    addLanguage({"msa", "ms"}, QT_TRANSLATE_NOOP("QObject", "Malay"));
    addLanguage({"msb"}, QT_TRANSLATE_NOOP("QObject", "Masbatenyo"));
    addLanguage({"msc"}, QT_TRANSLATE_NOOP("QObject", "Sankaran Maninka"));
    addLanguage({"msk"}, QT_TRANSLATE_NOOP("QObject", "Mansaka"));
    addLanguage({"msm"}, QT_TRANSLATE_NOOP("QObject", "Manobo, Agusan"));
    addLanguage({"msy"}, QT_TRANSLATE_NOOP("QObject", "Aruamu"));
    addLanguage({"mti"}, QT_TRANSLATE_NOOP("QObject", "Maiwa (Papua New Guinea)"));
    addLanguage({"mto"}, QT_TRANSLATE_NOOP("QObject", "Mixe, Totontepec"));
    addLanguage({"mul"}, QT_TRANSLATE_NOOP("QObject", "(Multiple languages)"));
    addLanguage({"mux"}, QT_TRANSLATE_NOOP("QObject", "Bo-Ung"));
    addLanguage({"muy"}, QT_TRANSLATE_NOOP("QObject", "Muyang"));
    addLanguage({"mva"}, QT_TRANSLATE_NOOP("QObject", "Manam"));
    // Code "mvc" is deprecated - merged with "mam"
    addLanguage({"mvc"}, QT_TRANSLATE_NOOP("QObject", "Central Mam"));
    // Code "mvj" is deprecated - merged with "mam"
    addLanguage({"mvj"}, QT_TRANSLATE_NOOP("QObject", "Mam, Todos Santos Cuchumat\u00e1n"));
    addLanguage({"mvn"}, QT_TRANSLATE_NOOP("QObject", "Minaveha"));
    addLanguage({"mwc"}, QT_TRANSLATE_NOOP("QObject", "Are"));
    addLanguage({"mwe"}, QT_TRANSLATE_NOOP("QObject", "Mwera (Chimwera)"));
    addLanguage({"mwp"}, QT_TRANSLATE_NOOP("QObject", "Kala Lagaw Ya"));
    addLanguage({"mxb"}, QT_TRANSLATE_NOOP("QObject", "Mixtec, Tezoatl\u00e1n"));
    addLanguage({"mxp"}, QT_TRANSLATE_NOOP("QObject", "Mixe, Tlahuitoltepec"));
    addLanguage({"mxq"}, QT_TRANSLATE_NOOP("QObject", "Mixe, Juquila"));
    addLanguage({"mxt"}, QT_TRANSLATE_NOOP("QObject", "Mixtec, Jamiltepec"));
    addLanguage({"mya", "my"}, QT_TRANSLATE_NOOP("QObject", "Burmese"));
    addLanguage({"myk"}, QT_TRANSLATE_NOOP("QObject", "Mamara Senoufo"));
    addLanguage({"myu"}, QT_TRANSLATE_NOOP("QObject", "Munduruk\u00fa"));
    addLanguage({"myw"}, QT_TRANSLATE_NOOP("QObject", "Muyuw"));
    addLanguage({"myy"}, QT_TRANSLATE_NOOP("QObject", "Macuna"));
    addLanguage({"mzz"}, QT_TRANSLATE_NOOP("QObject", "Maiadomu"));

    addLanguage({"nab"}, QT_TRANSLATE_NOOP("QObject", "Southern Nambiku\u00e1ra"));
    addLanguage({"naf"}, QT_TRANSLATE_NOOP("QObject", "Nabak"));
    addLanguage({"nak"}, QT_TRANSLATE_NOOP("QObject", "Nakanai"));
    addLanguage({"nas"}, QT_TRANSLATE_NOOP("QObject", "Naasioi"));
    addLanguage({"nau", "na"}, QT_TRANSLATE_NOOP("QObject", "Nauru"));
    addLanguage({"nav", "nv"}, QT_TRANSLATE_NOOP("QObject", "Navajo"));
    addLanguage({"nay"}, QT_TRANSLATE_NOOP("QObject", "Narrinyeri"));
    addLanguage({"nbl", "nr"}, QT_TRANSLATE_NOOP("QObject", "Ndebele, South"));
    addLanguage({"nbq"}, QT_TRANSLATE_NOOP("QObject", "Nggem"));
    addLanguage({"nca"}, QT_TRANSLATE_NOOP("QObject", "Iyo"));
    addLanguage({"nch"}, QT_TRANSLATE_NOOP("QObject", "Nahuatl, Central Huasteca"));
    addLanguage({"ncj"}, QT_TRANSLATE_NOOP("QObject", "Nahuatl, Northern Puebla"));
    addLanguage({"ncl"}, QT_TRANSLATE_NOOP("QObject", "Nahuatl, Michoac\u00e1n"));
    addLanguage({"ncq"}, QT_TRANSLATE_NOOP("QObject", "Northern Katang"));
    addLanguage({"ncu"}, QT_TRANSLATE_NOOP("QObject", "Chumburung"));
    addLanguage({"nde", "nd"}, QT_TRANSLATE_NOOP("QObject", "Ndebele, North"));
    addLanguage({"ndj"}, QT_TRANSLATE_NOOP("QObject", "Ndamba"));
    addLanguage({"ndo", "ng"}, QT_TRANSLATE_NOOP("QObject", "Ndonga"));
    addLanguage({"nds"}, QT_TRANSLATE_NOOP("QObject", "Low German"));
    addLanguage({"nep", "ne"}, QT_TRANSLATE_NOOP("QObject", "Nepali"));
    addLanguage({"nfa"}, QT_TRANSLATE_NOOP("QObject", "Dhao"));
    addLanguage({"ngp"}, QT_TRANSLATE_NOOP("QObject", "Ngulu"));
    addLanguage({"ngu"}, QT_TRANSLATE_NOOP("QObject", "Nahuatl, Guerrero"));
    addLanguage({"nhe"}, QT_TRANSLATE_NOOP("QObject", "Nahuatl, Eastern Huasteca"));
    addLanguage({"nhg"}, QT_TRANSLATE_NOOP("QObject", "Nahuatl, Tetelcingo"));
    addLanguage({"nhi"}, QT_TRANSLATE_NOOP("QObject", "Nahuatl, Zacatl\u00e1n-Ahuacatl\u00e1n-Tepetzintla"));
    addLanguage({"nho"}, QT_TRANSLATE_NOOP("QObject", "Takuu"));
    addLanguage({"nhr"}, QT_TRANSLATE_NOOP("QObject", "Naro"));
    addLanguage({"nhu"}, QT_TRANSLATE_NOOP("QObject", "Noone"));
    addLanguage({"nhw"}, QT_TRANSLATE_NOOP("QObject", "Nahuatl, Western Huasteca"));
    addLanguage({"nhy"}, QT_TRANSLATE_NOOP("QObject", "Nahuatl, Northern Oaxaca"));
    addLanguage({"nif"}, QT_TRANSLATE_NOOP("QObject", "Nek"));
    addLanguage({"nii"}, QT_TRANSLATE_NOOP("QObject", "Nii"));
    addLanguage({"nin"}, QT_TRANSLATE_NOOP("QObject", "Ninzo"));
    addLanguage({"nko"}, QT_TRANSLATE_NOOP("QObject", "Nkonya"));
    addLanguage({"nld", "nl"}, QT_TRANSLATE_NOOP("QObject", "Dutch"));
    addLanguage({"nlg"}, QT_TRANSLATE_NOOP("QObject", "Gela"));
    addLanguage({"nmw"}, QT_TRANSLATE_NOOP("QObject", "Nimoa"));
    addLanguage({"nna"}, QT_TRANSLATE_NOOP("QObject", "Nyangumarta"));
    addLanguage({"nno", "nn"}, QT_TRANSLATE_NOOP("QObject", "Norwegian Nynorsk"));
    addLanguage({"nnq"}, QT_TRANSLATE_NOOP("QObject", "Ngindo"));
    addLanguage({"noa"}, QT_TRANSLATE_NOOP("QObject", "Woun Meu"));
    addLanguage({"nob", "nb"}, QT_TRANSLATE_NOOP("QObject", "Norwegian Bokm\u00e5l"));
    addLanguage({"nog"}, QT_TRANSLATE_NOOP("QObject", "Nogai"));
    addLanguage({"nog-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Nogai (Cyrillic script)"));
    addLanguage({"nop"}, QT_TRANSLATE_NOOP("QObject", "Numanggang"));
    addLanguage({"nor", "no"}, QT_TRANSLATE_NOOP("QObject", "Norwegian"));
    addLanguage({"not"}, QT_TRANSLATE_NOOP("QObject", "Nomatsiguenga"));
    addLanguage({"nou"}, QT_TRANSLATE_NOOP("QObject", "Ewage-Notu"));
    addLanguage({"npi"}, QT_TRANSLATE_NOOP("QObject", "Nepali"));
    addLanguage({"npl"}, QT_TRANSLATE_NOOP("QObject", "Nahuatl, Southeastern Puebla"));
    addLanguage({"nsn"}, QT_TRANSLATE_NOOP("QObject", "Nehan"));
    addLanguage({"nss"}, QT_TRANSLATE_NOOP("QObject", "Nali"));
    addLanguage({"ntj"}, QT_TRANSLATE_NOOP("QObject", "Ngaanyatjarra"));
    addLanguage({"ntp"}, QT_TRANSLATE_NOOP("QObject", "Tepehuan, Northern"));
    addLanguage({"ntu"}, QT_TRANSLATE_NOOP("QObject", "Nat\u00fcgu"));
    addLanguage({"nuy"}, QT_TRANSLATE_NOOP("QObject", "Nunggubuyu"));
    addLanguage({"nvm"}, QT_TRANSLATE_NOOP("QObject", "Namiae"));
    addLanguage({"nwi"}, QT_TRANSLATE_NOOP("QObject", "Southwest Tanna"));
    addLanguage({"nya", "ny"}, QT_TRANSLATE_NOOP("QObject", "Nyanja"));
    addLanguage({"nys"}, QT_TRANSLATE_NOOP("QObject", "Nyungar"));
    addLanguage({"nyu"}, QT_TRANSLATE_NOOP("QObject", "Nyungwe"));

    addLanguage({"obo"}, QT_TRANSLATE_NOOP("QObject", "Monobo, Obo"));
    addLanguage({"oci", "oc"}, QT_TRANSLATE_NOOP("QObject", "Occitan (post 1500)"));
    addLanguage({"okv"}, QT_TRANSLATE_NOOP("QObject", "Orokaiva"));
    addLanguage({"omw"}, QT_TRANSLATE_NOOP("QObject", "South Tairora"));
    addLanguage({"ong"}, QT_TRANSLATE_NOOP("QObject", "Olo"));
    addLanguage({"ons"}, QT_TRANSLATE_NOOP("QObject", "Ono"));
    addLanguage({"ood"}, QT_TRANSLATE_NOOP("QObject", "Tohono O'odham"));
    addLanguage({"opm"}, QT_TRANSLATE_NOOP("QObject", "Oksapmin"));
    addLanguage({"ori", "or"}, QT_TRANSLATE_NOOP("QObject", "Oriya (Macrolanguage)"));
    addLanguage({"orm", "om"}, QT_TRANSLATE_NOOP("QObject", "Oromo"));
    addLanguage({"ory"}, QT_TRANSLATE_NOOP("QObject", "Oriya"));
    addLanguage({"oss", "os"}, QT_TRANSLATE_NOOP("QObject", "Ossetian"));
    addLanguage({"ote"}, QT_TRANSLATE_NOOP("QObject", "Otomi, Mezquital"));
    addLanguage({"otm"}, QT_TRANSLATE_NOOP("QObject", "Otomi, Eastern Highland"));
    addLanguage({"otn"}, QT_TRANSLATE_NOOP("QObject", "Otomi, Tenango"));
    addLanguage({"ots"}, QT_TRANSLATE_NOOP("QObject", "Otomi, Estado de M\u00e9xico"));
    addLanguage({"otq"}, QT_TRANSLATE_NOOP("QObject", "Otomi, Quer\u00e9taro"));

    addLanguage({"pab"}, QT_TRANSLATE_NOOP("QObject", "Parec\u00eds"));
    addLanguage({"pad"}, QT_TRANSLATE_NOOP("QObject", "Paumar\u00ed"));
    addLanguage({"pah"}, QT_TRANSLATE_NOOP("QObject", "Tenharim"));
    addLanguage({"pan", "pa"}, QT_TRANSLATE_NOOP("QObject", "Panjabi"));
    addLanguage({"pao"}, QT_TRANSLATE_NOOP("QObject", "Northern Paiute"));
    addLanguage({"pap"}, QT_TRANSLATE_NOOP("QObject", "Papiamento"));
    // New value of "pes" is "Iranian Persian"
    addLanguage({"pes"}, QT_TRANSLATE_NOOP("QObject", "Western Farsi"));
    addLanguage({"pib"}, QT_TRANSLATE_NOOP("QObject", "Yine"));
    addLanguage({"pio"}, QT_TRANSLATE_NOOP("QObject", "Piapoco"));
    addLanguage({"pir"}, QT_TRANSLATE_NOOP("QObject", "Piratapuyo"));
    addLanguage({"piu"}, QT_TRANSLATE_NOOP("QObject", "Pintupi-Luritja"));
    addLanguage({"pjt"}, QT_TRANSLATE_NOOP("QObject", "Pitjantjatjara"));
    addLanguage({"pli", "pi"}, QT_TRANSLATE_NOOP("QObject", "Pali"));
    addLanguage({"pls"}, QT_TRANSLATE_NOOP("QObject", "San Marcos Tlalcoyalco Popoloca"));
    addLanguage({"plu"}, QT_TRANSLATE_NOOP("QObject", "Palik\u00far"));
    addLanguage({"pma"}, QT_TRANSLATE_NOOP("QObject", "Paama"));
    addLanguage({"poe"}, QT_TRANSLATE_NOOP("QObject", "San Juan Atzingo Popoloca"));
    addLanguage({"poh"}, QT_TRANSLATE_NOOP("QObject", "Poqomchi'"));
    addLanguage({"poi"}, QT_TRANSLATE_NOOP("QObject", "Highland Popoluca"));
    addLanguage({"pol", "pl"}, QT_TRANSLATE_NOOP("QObject", "Polish"));
    addLanguage({"pon"}, QT_TRANSLATE_NOOP("QObject", "Pohnpeian"));
    addLanguage({"por", "pt"}, QT_TRANSLATE_NOOP("QObject", "Portuguese"));
    addLanguage({"pt-BR"}, QT_TRANSLATE_NOOP("QObject", "Brazilian Portuguese"));
    addLanguage({"pot"}, QT_TRANSLATE_NOOP("QObject", "Potawatomi"));
    addLanguage({"poy"}, QT_TRANSLATE_NOOP("QObject", "Pogolo"));
    addLanguage({"ppk"}, QT_TRANSLATE_NOOP("QObject", "Uma"));
    addLanguage({"ppo"}, QT_TRANSLATE_NOOP("QObject", "Folopa"));
    addLanguage({"prf"}, QT_TRANSLATE_NOOP("QObject", "Paranan"));
    addLanguage({"pri"}, QT_TRANSLATE_NOOP("QObject", "Paic\u00ee"));
    addLanguage({"prs"}, QT_TRANSLATE_NOOP("QObject", "Persian (Dari)"));
    addLanguage({"ptp"}, QT_TRANSLATE_NOOP("QObject", "Patep"));
    addLanguage({"ptu"}, QT_TRANSLATE_NOOP("QObject", "Bambam"));
    addLanguage({"pus", "ps"}, QT_TRANSLATE_NOOP("QObject", "Pushto"));
    addLanguage({"pwg"}, QT_TRANSLATE_NOOP("QObject", "Gapapaiwa"));

    addLanguage({"qub"}, QT_TRANSLATE_NOOP("QObject", "Quechua, Huallaga Hu\u00e1nuco"));
    addLanguage({"quc"}, QT_TRANSLATE_NOOP("QObject", "K'iche'"));
    addLanguage({"que", "qu"}, QT_TRANSLATE_NOOP("QObject", "Quechua"));
    addLanguage({"quf"}, QT_TRANSLATE_NOOP("QObject", "Quechua, Lambayeque"));
    addLanguage({"quh"}, QT_TRANSLATE_NOOP("QObject", "Quechua, South Bolivian"));
    addLanguage({"qul"}, QT_TRANSLATE_NOOP("QObject", "Quechua, North Bolivian"));
    addLanguage({"qup"}, QT_TRANSLATE_NOOP("QObject", "Quechua, Southern Pastaza"));
    // Code "qut" is deprecated - merged with "quc"
    addLanguage({"qut"}, QT_TRANSLATE_NOOP("QObject", "Quich\u00e9, West Central"));
    addLanguage({"qvc"}, QT_TRANSLATE_NOOP("QObject", "Quechua, Cajamarca"));
    addLanguage({"qve"}, QT_TRANSLATE_NOOP("QObject", "Quechua, Eastern Apur\u00edmac"));
    addLanguage({"qvh"}, QT_TRANSLATE_NOOP("QObject", "Quechua, Huamal\u00edes-Dos de Mayo Hu\u00e1nuco"));
    addLanguage({"qvm"}, QT_TRANSLATE_NOOP("QObject", "Quechua, Margos-Yarowilca-Lauricocha"));
    addLanguage({"qvn"}, QT_TRANSLATE_NOOP("QObject", "Quechua, North Jun\u00edn"));
    addLanguage({"qvs"}, QT_TRANSLATE_NOOP("QObject", "Quechua, San Mart\u00edn"));
    addLanguage({"qvw"}, QT_TRANSLATE_NOOP("QObject", "Quechua, Huaylla Wanca"));
    addLanguage({"qvz"}, QT_TRANSLATE_NOOP("QObject", "Quechua, Northern Pastaza"));
    addLanguage({"qwh"}, QT_TRANSLATE_NOOP("QObject", "Quechua, Huaylas Ancash"));
    addLanguage({"qxh"}, QT_TRANSLATE_NOOP("QObject", "Quechua, Panao Hu\u00e1nuco"));
    addLanguage({"qxn"}, QT_TRANSLATE_NOOP("QObject", "Quechua, Northern Conchucos Ancash"));
    addLanguage({"qxo"}, QT_TRANSLATE_NOOP("QObject", "Quechua, Southern Conchucos Ancash"));

    addLanguage({"rai"}, QT_TRANSLATE_NOOP("QObject", "Ramoaaina"));
    addLanguage({"rgu"}, QT_TRANSLATE_NOOP("QObject", "Ringgou"));
    addLanguage({"rkb"}, QT_TRANSLATE_NOOP("QObject", "Rikbaktsa"));
    addLanguage({"rmc"}, QT_TRANSLATE_NOOP("QObject", "Romani, Carpathian"));
    addLanguage({"rmq"}, QT_TRANSLATE_NOOP("QObject", "Cal\u00f3"));
    addLanguage({"rmy"}, QT_TRANSLATE_NOOP("QObject", "Romani, Vlax"));
    addLanguage({"roh", "rm"}, QT_TRANSLATE_NOOP("QObject", "Romansh"));
    addLanguage({"ron", "ro"}, QT_TRANSLATE_NOOP("QObject", "Romanian"));
    addLanguage({"roo"}, QT_TRANSLATE_NOOP("QObject", "Rotokas"));
    addLanguage({"rop"}, QT_TRANSLATE_NOOP("QObject", "Kriol"));
    addLanguage({"row"}, QT_TRANSLATE_NOOP("QObject", "Dela-Oenale"));
    addLanguage({"rro"}, QT_TRANSLATE_NOOP("QObject", "Waima"));
    addLanguage({"ruf"}, QT_TRANSLATE_NOOP("QObject", "Luguru"));
    addLanguage({"rug"}, QT_TRANSLATE_NOOP("QObject", "Roviana"));
    addLanguage({"run", "rn"}, QT_TRANSLATE_NOOP("QObject", "Rundi"));
    addLanguage({"rus", "ru"}, QT_TRANSLATE_NOOP("QObject", "Russian"));
    addLanguage({"rwo"}, QT_TRANSLATE_NOOP("QObject", "Rawa"));

    addLanguage({"sab"}, QT_TRANSLATE_NOOP("QObject", "Buglere"));
    addLanguage({"sag", "sg"}, QT_TRANSLATE_NOOP("QObject", "Sango"));
    addLanguage({"sah"}, QT_TRANSLATE_NOOP("QObject", "Yakut"));
    addLanguage({"sah-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Yakut (Cyrillic script)"));
    addLanguage({"san","sa"}, QT_TRANSLATE_NOOP("QObject", "Sanskrit"));
    addLanguage({"sbe"}, QT_TRANSLATE_NOOP("QObject", "Saliba"));
    addLanguage({"sco"}, QT_TRANSLATE_NOOP("QObject", "Scots"));
    addLanguage({"seh"}, QT_TRANSLATE_NOOP("QObject", "Sena"));
    addLanguage({"sey"}, QT_TRANSLATE_NOOP("QObject", "Secoya"));
    addLanguage({"sgb"}, QT_TRANSLATE_NOOP("QObject", "Mag-antsi Ayta"));
    addLanguage({"sgz"}, QT_TRANSLATE_NOOP("QObject", "Sursurunga"));
    addLanguage({"shj"}, QT_TRANSLATE_NOOP("QObject", "Shatt"));
    addLanguage({"shp"}, QT_TRANSLATE_NOOP("QObject", "Shipibo-Conibo"));
    addLanguage({"sim"}, QT_TRANSLATE_NOOP("QObject", "Mende (Papua New Guinea)"));
    addLanguage({"sin", "si"}, QT_TRANSLATE_NOOP("QObject", "Sinhala"));
    addLanguage({"sja"}, QT_TRANSLATE_NOOP("QObject", "Epena"));
    addLanguage({"slk", "sk"}, QT_TRANSLATE_NOOP("QObject", "Slovak"));
    addLanguage({"sll"}, QT_TRANSLATE_NOOP("QObject", "Salt-Yui"));
    addLanguage({"slv", "sl"}, QT_TRANSLATE_NOOP("QObject", "Slovenian"));
    addLanguage({"sme", "se"}, QT_TRANSLATE_NOOP("QObject", "Northern Sami"));
    addLanguage({"smk"}, QT_TRANSLATE_NOOP("QObject", "Bolinao"));
    addLanguage({"sml"}, QT_TRANSLATE_NOOP("QObject", "Central Sama"));
    addLanguage({"smo", "sm"}, QT_TRANSLATE_NOOP("QObject", "Samoan"));
    addLanguage({"sna", "sn"}, QT_TRANSLATE_NOOP("QObject", "Shona"));
    addLanguage({"snc"}, QT_TRANSLATE_NOOP("QObject", "Sinaugoro"));
    addLanguage({"snd", "sd"}, QT_TRANSLATE_NOOP("QObject", "Sindhi"));
    addLanguage({"snn"}, QT_TRANSLATE_NOOP("QObject", "Siona"));
    addLanguage({"snp"}, QT_TRANSLATE_NOOP("QObject", "Siane"));
    addLanguage({"sny"}, QT_TRANSLATE_NOOP("QObject", "Saniyo-Hiyewe"));
    addLanguage({"som", "so"}, QT_TRANSLATE_NOOP("QObject", "Somali"));
    addLanguage({"soq"}, QT_TRANSLATE_NOOP("QObject", "Kanasi"));
    addLanguage({"sot", "st"}, QT_TRANSLATE_NOOP("QObject", "Southern Sotho"));
    addLanguage({"soy"}, QT_TRANSLATE_NOOP("QObject", "Miyobe"));
    addLanguage({"spa", "es"}, QT_TRANSLATE_NOOP("QObject", "Spanish"));
    addLanguage({"spl"}, QT_TRANSLATE_NOOP("QObject", "Selepet"));
    // New value for "spm" is "Akukem"
    addLanguage({"spm"}, QT_TRANSLATE_NOOP("QObject", "Sepen"));
    addLanguage({"spp"}, QT_TRANSLATE_NOOP("QObject", "Supyire Senoufo"));
    addLanguage({"sps"}, QT_TRANSLATE_NOOP("QObject", "Saposa"));
    addLanguage({"spy"}, QT_TRANSLATE_NOOP("QObject", "Sabaot"));
    addLanguage({"sqi", "sq"}, QT_TRANSLATE_NOOP("QObject", "Albanian"));
    addLanguage({"srd", "sc"}, QT_TRANSLATE_NOOP("QObject", "Sardinian"));
    addLanguage({"sri"}, QT_TRANSLATE_NOOP("QObject", "Siriano"));
    addLanguage({"srm"}, QT_TRANSLATE_NOOP("QObject", "Saramaccan"));
    addLanguage({"srn"}, QT_TRANSLATE_NOOP("QObject", "Sranan Tongo"));
    addLanguage({"srp", "sr"}, QT_TRANSLATE_NOOP("QObject", "Serbian"));
    addLanguage({"srq"}, QT_TRANSLATE_NOOP("QObject", "Sirion\u00f3"));
    addLanguage({"ssd"}, QT_TRANSLATE_NOOP("QObject", "Siroi"));
    addLanguage({"ssg"}, QT_TRANSLATE_NOOP("QObject", "Seimat"));
    addLanguage({"ssw", "ss"}, QT_TRANSLATE_NOOP("QObject", "Swati"));
    addLanguage({"ssx"}, QT_TRANSLATE_NOOP("QObject", "Samberigi"));
    addLanguage({"stp"}, QT_TRANSLATE_NOOP("QObject", "Tepehuan, Southeastern"));
    addLanguage({"sua"}, QT_TRANSLATE_NOOP("QObject", "Sulka"));
    addLanguage({"sue"}, QT_TRANSLATE_NOOP("QObject", "Suena"));
    addLanguage({"sun", "su"}, QT_TRANSLATE_NOOP("QObject", "Sundanese"));
    addLanguage({"sus"}, QT_TRANSLATE_NOOP("QObject", "Susu"));
    addLanguage({"suz"}, QT_TRANSLATE_NOOP("QObject", "Sunwar"));
    addLanguage({"swa"}, QT_TRANSLATE_NOOP("QObject", "Swahili (Marcolanguage)"));
    addLanguage({"swe", "sv"}, QT_TRANSLATE_NOOP("QObject", "Swedish"));
    addLanguage({"swh", "sw"}, QT_TRANSLATE_NOOP("QObject", "Swahili"));
    addLanguage({"swp"}, QT_TRANSLATE_NOOP("QObject", "Suau"));
    addLanguage({"sxb"}, QT_TRANSLATE_NOOP("QObject", "Suba"));
    addLanguage({"syr"}, QT_TRANSLATE_NOOP("QObject", "Syriac"));

    addLanguage({"tab"}, QT_TRANSLATE_NOOP("QObject", "Tabassaran"));
    addLanguage({"tac"}, QT_TRANSLATE_NOOP("QObject", "Lowland Tarahumara"));
    addLanguage({"tah", "ty"}, QT_TRANSLATE_NOOP("QObject", "Tahitian"));
    addLanguage({"taj"}, QT_TRANSLATE_NOOP("QObject", "Eastern Tamang"));
    addLanguage({"tam", "ta"}, QT_TRANSLATE_NOOP("QObject", "Tamil"));
    addLanguage({"tat", "tt"}, QT_TRANSLATE_NOOP("QObject", "Tatar"));
    addLanguage({"tav"}, QT_TRANSLATE_NOOP("QObject", "Tatuyo"));
    addLanguage({"taw"}, QT_TRANSLATE_NOOP("QObject", "Tai"));
    addLanguage({"tbc"}, QT_TRANSLATE_NOOP("QObject", "Takia"));
    addLanguage({"tbg"}, QT_TRANSLATE_NOOP("QObject", "North Tairora"));
    addLanguage({"tbl"}, QT_TRANSLATE_NOOP("QObject", "Tboli"));
    addLanguage({"tbo"}, QT_TRANSLATE_NOOP("QObject", "Tawala"));
    addLanguage({"tbz"}, QT_TRANSLATE_NOOP("QObject", "Ditammari"));
    addLanguage({"tca"}, QT_TRANSLATE_NOOP("QObject", "Ticuna"));
    addLanguage({"tcs"}, QT_TRANSLATE_NOOP("QObject", "Torres Strait Creole"));
    addLanguage({"tee"}, QT_TRANSLATE_NOOP("QObject", "Huehuetla Tepehua"));
    addLanguage({"tel", "te"}, QT_TRANSLATE_NOOP("QObject", "Telugu"));
    addLanguage({"ter"}, QT_TRANSLATE_NOOP("QObject", "Tereno"));
    addLanguage({"tet"}, QT_TRANSLATE_NOOP("QObject", "Tetum"));
    addLanguage({"tew"}, QT_TRANSLATE_NOOP("QObject", "Tewa (USA)"));
    addLanguage({"tfr"}, QT_TRANSLATE_NOOP("QObject", "Teribe"));
    addLanguage({"tgk", "tg"}, QT_TRANSLATE_NOOP("QObject", "Tajik"));
    addLanguage({"tgl", "tl"}, QT_TRANSLATE_NOOP("QObject", "Tagalog"));
    addLanguage({"tgo"}, QT_TRANSLATE_NOOP("QObject", "Sudest"));
    addLanguage({"tgp"}, QT_TRANSLATE_NOOP("QObject", "Tangoa"));
    addLanguage({"tha", "th"}, QT_TRANSLATE_NOOP("QObject", "Thai"));
    // New value for "thd" is "Kuuk Thaayorre", "Thayore"
    addLanguage({"thd"}, QT_TRANSLATE_NOOP("QObject", "Thayore"));
    addLanguage({"tif"}, QT_TRANSLATE_NOOP("QObject", "Tifal"));
    addLanguage({"tim"}, QT_TRANSLATE_NOOP("QObject", "Timbe"));
    addLanguage({"tiy"}, QT_TRANSLATE_NOOP("QObject", "Tiruray"));
    addLanguage({"tke"}, QT_TRANSLATE_NOOP("QObject", "Takwane"));
    addLanguage({"tk-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Turkmen (Cyrillic script)"));
    addLanguage({"tk-Latn"}, QT_TRANSLATE_NOOP("QObject", "Turkmen (Latin script)"));
    addLanguage({"tkr"}, QT_TRANSLATE_NOOP("QObject", "Tsakhur"));
    addLanguage({"tkr-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Tsakhur (Cyrillic script)"));
    addLanguage({"tku"}, QT_TRANSLATE_NOOP("QObject", "Totonac, Upper Necaxa"));
    addLanguage({"tlf"}, QT_TRANSLATE_NOOP("QObject", "Telefol"));
    addLanguage({"tlh"}, QT_TRANSLATE_NOOP("QObject", "Klingon"));
    addLanguage({"tna"}, QT_TRANSLATE_NOOP("QObject", "Tacana"));
    addLanguage({"tnc"}, QT_TRANSLATE_NOOP("QObject", "Tanimuca-Retuar\u00e3"));
    addLanguage({"tnk"}, QT_TRANSLATE_NOOP("QObject", "Kwamera"));
    addLanguage({"tnn"}, QT_TRANSLATE_NOOP("QObject", "North Tanna"));
    addLanguage({"tnp"}, QT_TRANSLATE_NOOP("QObject", "Whitesands"));
    addLanguage({"toc"}, QT_TRANSLATE_NOOP("QObject", "Totonac, Coyutla"));
    addLanguage({"tod"}, QT_TRANSLATE_NOOP("QObject", "Toma"));
    addLanguage({"toj"}, QT_TRANSLATE_NOOP("QObject", "Tojolabal"));
    addLanguage({"too"}, QT_TRANSLATE_NOOP("QObject", "Totonac, Xicotepec De Ju\u00e1rez"));
    addLanguage({"ton", "to"}, QT_TRANSLATE_NOOP("QObject", "Tonga (Tonga Islands)"));
    addLanguage({"top"}, QT_TRANSLATE_NOOP("QObject", "Totonac, Papantla"));
    addLanguage({"tos"}, QT_TRANSLATE_NOOP("QObject", "Totonac, Highland"));
    addLanguage({"tpa"}, QT_TRANSLATE_NOOP("QObject", "Taupota"));
    addLanguage({"tpi"}, QT_TRANSLATE_NOOP("QObject", "Tok Pisin"));
    addLanguage({"tpt"}, QT_TRANSLATE_NOOP("QObject", "Tlachichilco Tepehua"));
    addLanguage({"tpz"}, QT_TRANSLATE_NOOP("QObject", "Tinputz"));
    addLanguage({"trc"}, QT_TRANSLATE_NOOP("QObject", "Copala Triqui"));
    addLanguage({"tsg"}, QT_TRANSLATE_NOOP("QObject", "Tausug"));
    addLanguage({"tsn", "tn"}, QT_TRANSLATE_NOOP("QObject", "Tswana"));
    addLanguage({"tso", "ts"}, QT_TRANSLATE_NOOP("QObject", "Tsonga"));
    addLanguage({"tsw"}, QT_TRANSLATE_NOOP("QObject", "Tsishingini"));
    addLanguage({"tt-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Tatar (Cyrillic script)"));
    addLanguage({"ttc"}, QT_TRANSLATE_NOOP("QObject", "Tektiteko"));
    addLanguage({"tte"}, QT_TRANSLATE_NOOP("QObject", "Bwanabwana"));
    addLanguage({"tuc"}, QT_TRANSLATE_NOOP("QObject", "Mutu"));
    addLanguage({"tue"}, QT_TRANSLATE_NOOP("QObject", "Tuyuca"));
    addLanguage({"tuf"}, QT_TRANSLATE_NOOP("QObject", "Central Tunebo"));
    addLanguage({"tuk", "tk"}, QT_TRANSLATE_NOOP("QObject", "Turkmen"));
    addLanguage({"tuo"}, QT_TRANSLATE_NOOP("QObject", "Tucano"));
    addLanguage({"tur", "tr"}, QT_TRANSLATE_NOOP("QObject", "Turkish"));
    addLanguage({"tvk"}, QT_TRANSLATE_NOOP("QObject", "Southeast Ambrym"));
    addLanguage({"twi", "tw"}, QT_TRANSLATE_NOOP("QObject", "Twi"));
    addLanguage({"txq"}, QT_TRANSLATE_NOOP("QObject", "Tii"));
    addLanguage({"txu"}, QT_TRANSLATE_NOOP("QObject", "Kayap\u00f3"));
    addLanguage({"tyv"}, QT_TRANSLATE_NOOP("QObject", "Tuvinian"));
    addLanguage({"tyv-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Tuvinian (Cyrillic script)"));
    addLanguage({"tzj"}, QT_TRANSLATE_NOOP("QObject", "Tz'utujil"));
    addLanguage({"tzo"}, QT_TRANSLATE_NOOP("QObject", "Tzotzil"));
    // Code "tzz" is deprecated - merged with "tzo"
    addLanguage({"tzz"}, QT_TRANSLATE_NOOP("QObject", "Tzotzil, Zinacant\u00e1n"));

    addLanguage({"ubr"}, QT_TRANSLATE_NOOP("QObject", "Ubir"));
    addLanguage({"ubu"}, QT_TRANSLATE_NOOP("QObject", "Umbu-Ungu"));
    addLanguage({"udu"}, QT_TRANSLATE_NOOP("QObject", "Uduk"));
    addLanguage({"uig", "ug"}, QT_TRANSLATE_NOOP("QObject", "Uyghurche‎"));
    addLanguage({"ukr", "uk"}, QT_TRANSLATE_NOOP("QObject", "Ukrainian"));
    addLanguage({"uli"}, QT_TRANSLATE_NOOP("QObject", "Ulithian"));
    // New value for "ulk" is "Meriam", "Meriam Mir"
    addLanguage({"ulk"}, QT_TRANSLATE_NOOP("QObject", "Meriam"));
    addLanguage({"upv"}, QT_TRANSLATE_NOOP("QObject", "Uripiv-Wala-Rano-Atchin"));
    addLanguage({"ura"}, QT_TRANSLATE_NOOP("QObject", "Urarina"));
    addLanguage({"urb"}, QT_TRANSLATE_NOOP("QObject", "Urub\u00fa-Kaapor"));
    addLanguage({"urd", "ur"}, QT_TRANSLATE_NOOP("QObject", "Urdu"));
    addLanguage({"ur-Deva"}, QT_TRANSLATE_NOOP("QObject", "Urdu (Deva script)"));
    addLanguage({"urt"}, QT_TRANSLATE_NOOP("QObject", "Urat"));
    addLanguage({"urw"}, QT_TRANSLATE_NOOP("QObject", "Sop"));
    addLanguage({"ury"}, QT_TRANSLATE_NOOP("QObject", "Orya"));
    addLanguage({"usa"}, QT_TRANSLATE_NOOP("QObject", "Usarufa"));
    addLanguage({"usp"}, QT_TRANSLATE_NOOP("QObject", "Uspanteco"));
    addLanguage({"uvl"}, QT_TRANSLATE_NOOP("QObject", "Lote"));
    addLanguage({"uzb", "uz"}, QT_TRANSLATE_NOOP("QObject", "Uzbek"));
    addLanguage({"uz-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Uzbek (Cyrillic script)"));
    addLanguage({"uz-Latn"}, QT_TRANSLATE_NOOP("QObject", "Uzbek (Latin script)"));

    addLanguage({"vie", "vi"}, QT_TRANSLATE_NOOP("QObject", "Vietnamese"));
    addLanguage({"vid"}, QT_TRANSLATE_NOOP("QObject", "Vidunda"));
    addLanguage({"viv"}, QT_TRANSLATE_NOOP("QObject", "Iduna"));
    addLanguage({"vls"}, QT_TRANSLATE_NOOP("QObject", "Vlaams"));
    addLanguage({"vmi"}, QT_TRANSLATE_NOOP("QObject", "Miwa"));
    addLanguage({"vol", "vo"}, QT_TRANSLATE_NOOP("QObject", "Volap\u00fck"));

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
    addLanguage({"wol", "wo"}, QT_TRANSLATE_NOOP("QObject", "Wolof"));
    addLanguage({"wos"}, QT_TRANSLATE_NOOP("QObject", "Hanga Hundi"));
    addLanguage({"wrk"}, QT_TRANSLATE_NOOP("QObject", "Garrwa"));
    addLanguage({"wro"}, QT_TRANSLATE_NOOP("QObject", "Worrorra"));
    addLanguage({"wrs"}, QT_TRANSLATE_NOOP("QObject", "Waris"));
    addLanguage({"wsk"}, QT_TRANSLATE_NOOP("QObject", "Waskia"));
    addLanguage({"wuv"}, QT_TRANSLATE_NOOP("QObject", "Wuvulu-Aua"));

    addLanguage({"xal"}, QT_TRANSLATE_NOOP("QObject", "Kalmyk"));
    addLanguage({"xal-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Kalmyk (Cyrillic script)"));
    addLanguage({"xav"}, QT_TRANSLATE_NOOP("QObject", "Xav\u00e1nte"));
    addLanguage({"xbi"}, QT_TRANSLATE_NOOP("QObject", "Kombio"));
    addLanguage({"xed"}, QT_TRANSLATE_NOOP("QObject", "Hdi"));
    addLanguage({"xho", "xh"}, QT_TRANSLATE_NOOP("QObject", "Xhosa"));
    addLanguage({"xla"}, QT_TRANSLATE_NOOP("QObject", "Kamula"));
    addLanguage({"xnn"}, QT_TRANSLATE_NOOP("QObject", "Northern Kankanay"));
    addLanguage({"xon"}, QT_TRANSLATE_NOOP("QObject", "Konkomba"));
    addLanguage({"xsi"}, QT_TRANSLATE_NOOP("QObject", "Sio"));
    addLanguage({"xtd"}, QT_TRANSLATE_NOOP("QObject", "Mixtec, Diuxi-Tilantongo"));
    addLanguage({"xtm"}, QT_TRANSLATE_NOOP("QObject", "Mixtec, Magdalena Pe\u00f1asco"));

    addLanguage({"yaa"}, QT_TRANSLATE_NOOP("QObject", "Yaminahua"));
    addLanguage({"yad"}, QT_TRANSLATE_NOOP("QObject", "Yagua"));
    addLanguage({"yaf"}, QT_TRANSLATE_NOOP("QObject", "Yaka (Democratic Republic of Congo)"));
    addLanguage({"yal"}, QT_TRANSLATE_NOOP("QObject", "Yalunka"));
    addLanguage({"yap"}, QT_TRANSLATE_NOOP("QObject", "Yapese"));
    addLanguage({"yaq"}, QT_TRANSLATE_NOOP("QObject", "Yaqui"));
    addLanguage({"yby"}, QT_TRANSLATE_NOOP("QObject", "Yaweyuha"));
    addLanguage({"ycn"}, QT_TRANSLATE_NOOP("QObject", "Yucuna"));
    addLanguage({"yid", "yi"}, QT_TRANSLATE_NOOP("QObject", "Yiddish"));
    addLanguage({"yij"}, QT_TRANSLATE_NOOP("QObject", "Yindjibarndi"));
    addLanguage({"yka"}, QT_TRANSLATE_NOOP("QObject", "Yakan"));
    addLanguage({"yle"}, QT_TRANSLATE_NOOP("QObject", "Yele"));
    addLanguage({"yml"}, QT_TRANSLATE_NOOP("QObject", "Iamalele"));
    addLanguage({"yor", "yo"}, QT_TRANSLATE_NOOP("QObject", "Yoruba"));
    addLanguage({"yre"}, QT_TRANSLATE_NOOP("QObject", "Yaour\u00e9"));
    addLanguage({"yrk"}, QT_TRANSLATE_NOOP("QObject", "Nenets"));
    addLanguage({"yrk-Cyrl"}, QT_TRANSLATE_NOOP("QObject", "Nenets (Cyrillic script)"));
    addLanguage({"yss"}, QT_TRANSLATE_NOOP("QObject", "Yessan-Mayo"));
    addLanguage({"yuj"}, QT_TRANSLATE_NOOP("QObject", "Karkar-Yuri"));
    addLanguage({"yut"}, QT_TRANSLATE_NOOP("QObject", "Yopno"));
    addLanguage({"yuw"}, QT_TRANSLATE_NOOP("QObject", "Yau (Morobe Province)"));
    addLanguage({"yva"}, QT_TRANSLATE_NOOP("QObject", "Yawa"));

    addLanguage({"zaa"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Sierra de Ju\u00e1rez"));
    addLanguage({"zab"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, San Juan Guelav\u00eda"));
    addLanguage({"zac"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Ocotl\u00e1n"));
    addLanguage({"zad"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Cajonos"));
    addLanguage({"zai"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Isthmus"));
    addLanguage({"zaj"}, QT_TRANSLATE_NOOP("QObject", "Zaramo"));
    addLanguage({"zam"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Miahuatl\u00e1n"));
    addLanguage({"zao"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Ozolotepec"));
    addLanguage({"zar"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Rinc\u00f3n"));
    addLanguage({"zas"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Santo Domingo Albarradas"));
    addLanguage({"zat"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Tabaa"));
    addLanguage({"zav"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Yatzachi"));
    addLanguage({"zaw"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Mitla"));
    addLanguage({"zca"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Coatecas Altas"));
    addLanguage({"zha", "za"}, QT_TRANSLATE_NOOP("QObject", "Zhuang"));
    addLanguage({"zho", "zh"}, QT_TRANSLATE_NOOP("QObject", "Chinese"));
    addLanguage({"zh-Hans"}, QT_TRANSLATE_NOOP("QObject", "Chinese, Simplified"));
    addLanguage({"zh-Hant"}, QT_TRANSLATE_NOOP("QObject", "Chinese, Traditional"));
    addLanguage({"zia"}, QT_TRANSLATE_NOOP("QObject", "Zia"));
    addLanguage({"ziw"}, QT_TRANSLATE_NOOP("QObject", "Zigula"));
    addLanguage({"zlm"}, QT_TRANSLATE_NOOP("QObject", "Malay"));
    addLanguage({"zos"}, QT_TRANSLATE_NOOP("QObject", "Francisco Le\u00f3n Zoque"));
    addLanguage({"zpc"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Choapan"));
    addLanguage({"zpi"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Santa Mar\u00eda Quiegolani"));
    addLanguage({"zpl"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Lachix\u00edo"));
    addLanguage({"zpm"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Mixtepec"));
    addLanguage({"zpo"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Amatl\u00e1n"));
    addLanguage({"zpq"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Zoogocho"));
    addLanguage({"zpu"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Yal\u00e1lag"));
    addLanguage({"zpv"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Chichicapan"));
    addLanguage({"zpz"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Texmelucan"));
    addLanguage({"zsr"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Southern Rincon"));
    addLanguage({"ztq"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Quioquitani-Quier\u00ed"));
    addLanguage({"zty"}, QT_TRANSLATE_NOOP("QObject", "Zapotec, Yatee"));
    addLanguage({"zul", "zu"}, QT_TRANSLATE_NOOP("QObject", "Zulu"));
    addLanguage({"zxx"}, QT_TRANSLATE_NOOP("QObject", "No linguistic content"));
    // New value for "zyp" is "Zyphe Chin"
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
