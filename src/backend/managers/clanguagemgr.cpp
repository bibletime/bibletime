/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "clanguagemgr.h"

#include "../drivers/cswordmoduleinfo.h"
#include "cswordbackend.h"


/****************************************************/
/******************** CLanguageMgr ******************/
/****************************************************/

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

CLanguageMgr::CLanguageMgr()
    : m_defaultLanguage("", "", QString())
    , m_langMap()
{
    m_availableModulesCache.moduleCount = 0;
    init();
}

CLanguageMgr::~CLanguageMgr() {
    qDeleteAll(m_cleanupLangPtrs);
    m_cleanupLangPtrs.clear();
    qDeleteAll(m_langList);
    m_langList.clear();
}

const CLanguageMgr::LangMap& CLanguageMgr::availableLanguages() {
    QList<CSwordModuleInfo*> const & mods = CSwordBackend::instance()->moduleList();

    // Do we have to refill the cached map?
    if (m_availableModulesCache.moduleCount != mods.count()) {
        m_availableModulesCache.availableLanguages.clear();
        m_availableModulesCache.moduleCount = mods.count();

        //collect the languages abbrevs of all modules
        QStringList abbrevs;

        Q_FOREACH(const CSwordModuleInfo * const mod,  mods) {
            auto & m = mod->module();
            if (!abbrevs.contains(m.getLanguage()))
                abbrevs.append(m.getLanguage());
        }

        //now create a map of available langs
        Q_FOREACH(QString const & abbrev, abbrevs) {
            const Language* const lang = languageForAbbrev(abbrev);

            if (lang->isValid()) {
                m_availableModulesCache.availableLanguages.insert( abbrev, lang );
            }
            else { //invalid lang used by a module, create a new language using the abbrev
                Language* newLang = new Language(abbrev, abbrev, abbrev);
                m_cleanupLangPtrs.append(newLang);
                m_availableModulesCache.availableLanguages.insert( abbrev, newLang );
            }
        }
    }
    return m_availableModulesCache.availableLanguages;
}

const CLanguageMgr::Language* CLanguageMgr::languageForAbbrev( const QString& abbrev ) const {
    LangMapIterator it = m_langMap.find(abbrev);
    if (it != m_langMap.constEnd()) return *it; //Language is already here

    //try to search in the alternative abbrevs
    Q_FOREACH(const Language * const lang, m_langList)
        if (lang->alternativeAbbrevs().contains(abbrev))
            return lang;

    // Invalid lang used by a modules, create a new language using the abbrev
    Language* newLang = new Language(abbrev, abbrev, abbrev); //return a language which holds the valid abbrev
    m_cleanupLangPtrs.append(newLang);

    return newLang;
}

const CLanguageMgr::Language* CLanguageMgr::languageForTranslatedName( const QString& name ) const {
    Q_FOREACH(const Language * const lang, m_langList)
        if (lang->translatedName() == name)
            return lang;
    return &m_defaultLanguage; //invalid language
}

void CLanguageMgr::init() {

    //if we've already inserted all items we do not proceed
    if (m_langMap.count() > 0) return;

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
    //  m_langList.append( new Language("aa", "Afar", QObject::tr("Afar")) );
    //  m_langList.append( new Language("ab", "Abkhazian", QObject::tr("Abkhazian")) );
    //  m_langList.append( new Language("ae", "Avestan", QObject::tr("Avestan")) );
     //: Language name aai
    m_langList.append( new Language("aai", "Arifama-Miniafia", QObject::tr("Arifama-Miniafia")) );
     //: Language name aak
    m_langList.append( new Language("aak", "Ankave", QObject::tr("Ankave")) );
     //: Language name aak
    m_langList.append( new Language("aau", "Abau", QObject::tr("Abau")) );
      //: Language name aaz
     m_langList.append( new Language("aaz", "Amarasi", QObject::tr("Amarasi")) );
   //: Language name abt
                m_langList.append( new Language("abt", "Ambulas", QObject::tr("Ambulas")) );
    //: Language name abx
                m_langList.append( new Language("abx", "Inabaknon", QObject::tr("Inabaknon")) );
    //: Language name aby
                m_langList.append( new Language("aby", "Aneme Wake", QObject::tr("Aneme Wake")) );  
   //: Language name acf
                m_langList.append( new Language("acf", "Saint Lucian Creole French", QObject::tr("Saint Lucian Creole French")) );
                   //: Language name acr
                m_langList.append( new Language("acr", "Achí", QObject::tr("Achi")) );
                   //: Language name acu
                m_langList.append( new Language("acu", "Achuar-Shiwiar", QObject::tr("Achuar-Shiwiar")) );
      //: Language name ady-Cyrl
                m_langList.append( new Language("ady-Cyrl", "Адыгaбзэ", QObject::tr("Adyghe")) );
     //: Language name adz
                m_langList.append( new Language("adz", "Adzera", QObject::tr("Adzera")) );         
    //: Language name aer
                m_langList.append( new Language("aer", "Eastern Arrernte", QObject::tr("Eastern Arrernte")) );     
    //: Language name aey
                m_langList.append( new Language("aey", "Amele", QObject::tr("Amele")) ); 
    //: Language name af
    m_langList.append( new Language("af", "Afrikaans", QObject::tr("Afrikaans")) );
    //: Language name agd
    m_langList.append( new Language("agd", "Agarabi", QObject::tr("Agarabi")) );
    //: Language name agg
    m_langList.append( new Language("agg", "Angor", QObject::tr("Angor")) );
    //: Language name agm
    m_langList.append( new Language("agm", "Angaataha", QObject::tr("Angaataha")) );
    //: Language name agn
    m_langList.append( new Language("agn", "Agutaynen", QObject::tr("Agutaynen")) );
    //: Language name agr
    m_langList.append( new Language("agr", "Aguaruna", QObject::tr("Aguaruna")) );
    //: Language name agt
    m_langList.append( new Language("agt", "Central Cagayan Agta", QObject::tr("Central Cagayan Agta")) );
    //: Language name agu
    m_langList.append( new Language("agu", "Aguacateco", QObject::tr("Aguacateco")) );
    //: Language name agx-Cyrl
    m_langList.append( new Language("agx-Cyrl", "Агъул чӀал", QObject::tr("Aghul")) );
    //: Language name aia
    m_langList.append( new Language("aia", "Arosi", QObject::tr("Arosi")) );
    //: Language name aii
    m_langList.append( new Language("aii", "Assyrian Neo-Aramaic", QObject::tr("Assyrian Neo-Aramaic")) );
    //: Language name ake
    m_langList.append( new Language("ake", "Akawaio", QObject::tr("Akawaio")) );
    //: Language name akh
    m_langList.append( new Language("akh", "Angal Heneng", QObject::tr("Angal Heneng")) );
    //: Language name alp
    m_langList.append( new Language("alp", "Alune", QObject::tr("Alune")) );
    //: Language name alq
    m_langList.append( new Language("alq", "Algonquin", QObject::tr("Algonquin")) );
    //: Language name als
    m_langList.append( new Language("als", "Tosk Albanian", QObject::tr("Tosk Albanian")) );
    //: Language name alt-Cyrl
    m_langList.append( new Language("alt-Cyrl", "Тÿштÿк алтай", QObject::tr("Southern Altai")) );    
    //: Language name aly
    m_langList.append( new Language("aly", "Alyawarr", QObject::tr("Alyawarr")) );
    //: Language name am
    m_langList.append( new Language("am", "Amharic", QObject::tr("Amharic")) );
    //: Language name ame
    m_langList.append( new Language("ame", "Yanesha'", QObject::tr("Yanesha'")) );
    //: Language name amf
    m_langList.append( new Language("amf", "Hamer-Banna", QObject::tr("Hamer-Banna")) );
    //: Language name amk
    m_langList.append( new Language("amk", "Ambai", QObject::tr("Ambai")) );
    //: Language name amm
    m_langList.append( new Language("amm", "Ama (Papua New Guinea)", QObject::tr("Ama (Papua New Guinea)")) );
    //: Language name amn
    m_langList.append( new Language("amn", "Amanab", QObject::tr("Amanab")) );
    //: Language name amp
    m_langList.append( new Language("amp", "Alamblak", QObject::tr("Alamblak")) );
    //: Language name amr
    m_langList.append( new Language("amr", "Amarakaeri", QObject::tr("Amarakaeri")) );
    //: Language name amu
    m_langList.append( new Language("amu", "Amuzgo, Guerrero", QObject::tr("Amuzgo, Guerrero")) );
    //: Language name am
    m_langList.append( new Language("amx", "Anmatyerre", QObject::tr("Anmatyerre")) );
    //: Language name ang
    m_langList.append( new Language("ang", "English, Old (ca.450-1100)", QObject::tr("English, Old (ca.450-1100)")) );
    //: Language name anh
    m_langList.append( new Language("anh", "Nend", QObject::tr("Nend")) );
    //: Language name anv
    m_langList.append( new Language("anv", "Denya", QObject::tr("Denya")) );
    //: Language name aoi
    m_langList.append( new Language("aoi", "Anindilyakwa", QObject::tr("Anindilyakwa")) );
    //: Language name aoj
    m_langList.append( new Language("aoj", "Mufian", QObject::tr("Mufian")) );
    //: Language name aom
    m_langList.append( new Language("aom", "Ömie", QObject::tr("Ömie")) );
    //: Language name aon
    m_langList.append( new Language("aon", "Bumbita Arapesh", QObject::tr("Bumbita Arapesh")) );
    //: Language name apb
    m_langList.append( new Language("apb", "Sa'a", QObject::tr("Sa'a")) );
    //: Language name ape
    m_langList.append( new Language("ape", "Bukiyip", QObject::tr("Bukiyip")) );
    //: Language name apn
    m_langList.append( new Language("apn", "Apinayé", QObject::tr("Apinayé")) );
    //: Language name apr
    m_langList.append( new Language("apr", "Arop-Lokep", QObject::tr("Arop-Lokep")) );
    //: Language name apu
    m_langList.append( new Language("apu", "Apurinã", QObject::tr("Apurinã")) );
    //: Language name apw
    m_langList.append( new Language("apw", "Western Apache", QObject::tr("Western Apache")) );
    //: Language name apy
    m_langList.append( new Language("apy", "Apalaí", QObject::tr("Apalaí")) );
    //: Language name apz
    m_langList.append( new Language("apz", "Safeyoka", QObject::tr("Safeyoka")) );
    //: Language name ar
    m_langList.append( new Language("ar", "Arabic", QObject::tr("Arabic")) );
    //: Language name arb
    m_langList.append( new Language("arb", "Arabic", QObject::tr("Arabic")) );
    //: Language name are
    m_langList.append( new Language("are", "Arabic", QObject::tr("Arabic")) );
    //: Language name arl
    m_langList.append( new Language("arl", "العربية الفصحى", QObject::tr("Standard Arabic")) );
    //: Language name arn
    m_langList.append( new Language("arn", "Mapudungun", QObject::tr("Mapudungun")) );
    //: Language name arp
    m_langList.append( new Language("arp", "Arapaho", QObject::tr("Arapaho")) );
    //: Language name as
    m_langList.append( new Language("as", "Assamese", QObject::tr("Assamese")) );
    //: Language name aso
    m_langList.append( new Language("aso", "Dano", QObject::tr("Dano")) );
    //: Language name ata
    m_langList.append( new Language("ata", "Pele-Ata", QObject::tr("Pele-Ata")) );
    //: Language name atb
    m_langList.append( new Language("atb", "Zaiwa", QObject::tr("Zaiwa")) );
    //: Language name atd
    m_langList.append( new Language("atd", "Ata Manobo", QObject::tr("Ata Manobo")) );
    //: Language name atg
    m_langList.append( new Language("atg", "Ivbie North-Okpela-Arhe", QObject::tr("Ivbie North-Okpela-Arhe")) );
    //: Language name att
    m_langList.append( new Language("att", "Pamplona Atta", QObject::tr("Pamplona Atta")) );
    //: Language name auc
    m_langList.append( new Language("auc", "Waorani", QObject::tr("Waorani")) );
    //: Language name aui
    m_langList.append( new Language("aui", "Anuki", QObject::tr("Anuki")) );
    //: Language name auy
    m_langList.append( new Language("auy", "Awiyaana", QObject::tr("Awiyaana")) );
    //: Language name av-Cyrl
    m_langList.append( new Language("av-Cyrl", "Авар мацӏ", QObject::tr("Avaric (Cyrillic script)")) );
    //: Language name avt
    m_langList.append( new Language("avt", "Au", QObject::tr("Au")) );
    //: Language name awb
    m_langList.append( new Language("awb", "Awa (Papua New Guinea)", QObject::tr("Awa (Papua New Guinea)")) );
    //: Language name awk
    m_langList.append( new Language("awk", "Awabakal", QObject::tr("Awabakal")) );
    //: Language name awx
    m_langList.append( new Language("awx", "Awara", QObject::tr("Awara")) );
    //: Language name az
    m_langList.append( new Language("az", "Azerbaijani", QObject::tr("Azerbaijani")) );
    //: Language name azb
    m_langList.append( new Language("azb", "Azerbaijani, South", QObject::tr("Azerbaijani, South")) );
    //: Language name azg
    m_langList.append( new Language("azg", "San Pedro Amuzgos Amuzgo", QObject::tr("San Pedro Amuzgos Amuzgo")) );
    //: Language name azz
    m_langList.append( new Language("azz", "Highland Puebla Nahuatl", QObject::tr("Highland Puebla Nahuatl")) );
        //: Language name bm
    m_langList.append( new Language("ba-Cyrl", "Башҡортса", QObject::tr("Bashkir (Cyrillic script)")) );
    //: Language name bm
    m_langList.append( new Language("bm", "Bamanankan", QObject::tr("Bambara")) );
        //: Language name bam
    m_langList.append( new Language("bam", "Waimaha", QObject::tr("Waimaha")) );
            //: Language name bao
    m_langList.append( new Language("bao", "Bamanankan", QObject::tr("Bambara")) );
            //: Language name bba
    m_langList.append( new Language("bba", "Baatonum", QObject::tr("Baatonum")) );
            //: Language name bbb
    m_langList.append( new Language("bbb", "Barai", QObject::tr("Barai")) );
            //: Language name bbr
    m_langList.append( new Language("bbr", "Girawa", QObject::tr("Girawa")) );
            //: Language name bch
    m_langList.append( new Language("bch", "Bariai", QObject::tr("Bariai")) );
            //: Language name beo
    m_langList.append( new Language("beo", "Beami", QObject::tr("Beami")) );
    //: Language name bco
    m_langList.append( new Language("bco", "Kaluli", QObject::tr("Kaluli")) );
//: Language name bdd
    m_langList.append( new Language("bdd", "Bunama", QObject::tr("Bunama")) );
//: Language name bea
    m_langList.append( new Language("bea", "Beaver", QObject::tr("Beaver")) );
//: Language name bef
    m_langList.append( new Language("bef", "Benabena", QObject::tr("Benabena")) );
//: Language name beu
    m_langList.append( new Language("beu", "Blagar", QObject::tr("Blagar")) );
//: Language name bgs
    m_langList.append( new Language("bgs", "Tagabawa", QObject::tr("Tagabawa")) );
//: Language name bgt
    m_langList.append( new Language("bgt", "Bughotu", QObject::tr("Bughotu")) );
//: Language name bhg
    m_langList.append( new Language("bhg", "Binandere", QObject::tr("Binandere")) );
//: Language name bhl
    m_langList.append( new Language("bhl", "Bimin", QObject::tr("Bimin")) );
//: Language name big
    m_langList.append( new Language("big", "Biangai", QObject::tr("Biangai")) );
//: Language name bjp
    m_langList.append( new Language("bjp", "Tangga", QObject::tr("Tanga")) );
//: Language name bjr
    m_langList.append( new Language("bjr", "Binumarien", QObject::tr("Binumarien")) );
//: Language name bjv
    m_langList.append( new Language("bjv", "Bedjond", QObject::tr("Bedjond")) );
//: Language name bkd
    m_langList.append( new Language("bkd", "Binukid", QObject::tr("Binukid")) );
//: Language name bki
    m_langList.append( new Language("bki", "Baki", QObject::tr("Baki")) );
//: Language name bkq
    m_langList.append( new Language("bkq", "Bakairí", QObject::tr("Bakairí")) );
//: Language name bkx
    m_langList.append( new Language("bkx", "Baikeno", QObject::tr("Baikeno")) );
//: Language name bla
    m_langList.append( new Language("bla", "Siksika", QObject::tr("Siksika")) );
//: Language name blw
    m_langList.append( new Language("blw", "Balangao", QObject::tr("Balangao")) );
//: Language name blz
    m_langList.append( new Language("blz", "Balantak", QObject::tr("Balantak")) );
//: Language name bmh
    m_langList.append( new Language("bmh", "Kein", QObject::tr("Kein")) );
//: Language name bmk
    m_langList.append( new Language("bmk", "Ghayavi", QObject::tr("Ghayavi")) );
//: Language name bmr
    m_langList.append( new Language("bmr", "Muinane", QObject::tr("Muinane")) );
//: Language name bmu
    m_langList.append( new Language("bmu", "Burum-Mindik", QObject::tr("Burum-Mindik")) );
//: Language name bn
    m_langList.append( new Language("bn", "বাংলা", QObject::tr("Bengali")) );
//: Language name bnp
    m_langList.append( new Language("bnp", "Bola", QObject::tr("Bola")) );
//: Language name boa
    m_langList.append( new Language("boa", "Bora", QObject::tr("Bora")) );
//: Language name boj
    m_langList.append( new Language("boj", "Anjam", QObject::tr("Anjam")) );
//: Language name bon
    m_langList.append( new Language("bon", "Bine", QObject::tr("Bine")) );
//: Language name box
    m_langList.append( new Language("box", "Buamu", QObject::tr("Buamu")) );
//: Language name bpr
    m_langList.append( new Language("bpr", "Koronadal Blaan", QObject::tr("Koronadal Blaan")) );
//: Language name bps
    m_langList.append( new Language("bps", "Sarangani Blaan", QObject::tr("Sarangani Blaan")) );
//: Language name bqc
    m_langList.append( new Language("bqc", "Boko (Bénin)", QObject::tr("Boko (Benin)")) );
//: Language name bqp
    m_langList.append( new Language("bqp", "Busa", QObject::tr("Busa")) );
//: Language name bsn
    m_langList.append( new Language("bsn", "Barasana-Eduria", QObject::tr("Barasana-Eduria")) );
//: Language name bsp
    m_langList.append( new Language("bsp", "Baga Sitemu", QObject::tr("Baga Sitemu")) );
//: Language name bss
    m_langList.append( new Language("bss", "Akoose", QObject::tr("Akoose")) );
//: Language name bua-Cyrl
    m_langList.append( new Language("bua-Cyrl", "буряад хэлэн", QObject::tr("Buriat (Cyrillic script)")) );
//: Language name buk
    m_langList.append( new Language("buk", "Bugawac", QObject::tr("Bugawac")) );
//: Language name bus
    m_langList.append( new Language("bus", "Bokobaru", QObject::tr("Bokobaru")) );
//: Language name bvd
    m_langList.append( new Language("bvd", "Baeggu", QObject::tr("Baeggu")) );
//: Language name bvr
    m_langList.append( new Language("bvr", "Burarra", QObject::tr("Burarra")) );
//: Language name bxh
    m_langList.append( new Language("bxh", "Buhutu", QObject::tr("Buhutu")) );
//: Language name byr
    m_langList.append( new Language("byr", "Baruya", QObject::tr("Baruya")) );
//: Language name byx
    m_langList.append( new Language("byx", "Qaqet", QObject::tr("Qaqet")) );
//: Language name bzd
    m_langList.append( new Language("bzd", "Bribri", QObject::tr("Bribri")) );
//: Language name bzh
    m_langList.append( new Language("bzh", "Mapos Buang", QObject::tr("Mapos Buang")) );
//: Language name bzj
    m_langList.append( new Language("bzj", "Belize Kriol English", QObject::tr("Belize Kriol English")) );
    //: Language name bar
    m_langList.append( new Language("bar", "Bavarian", QObject::tr("Bavarian")) );
    //: Language name be
    m_langList.append( new Language("be", "Belarusian", QObject::tr("Belarusian")) );
    //: Language name bg
    m_langList.append( new Language("bg", "Bulgarian", QObject::tr("Bulgarian")) );
    //  m_langList.append( new Language("bh", "Bihari", QObject::tr("Bihari")) );
    //  m_langList.append( new Language("bi", "Bislama", QObject::tr("Bislama")) );
    //  m_langList.append( new Language("bn", "Bengali", QObject::tr("Bengali")) );
    //  m_langList.append( new Language("bo", "Tibetan", QObject::tr("Tibetan")) );
    //: Language name br
    m_langList.append( new Language("br", "Breton", QObject::tr("Breton")) );
    //: Language name bs
    m_langList.append( new Language("bs", "Bosnian", QObject::tr("Bosnian")) );
    //: Language name caa
    m_langList.append( new Language("caa", "Chortí", QObject::tr("Chortí")) );
//: Language name cab
    m_langList.append( new Language("cab", "Garifuna", QObject::tr("Garifuna")) );
//: Language name cac
    m_langList.append( new Language("cac", "Chuj", QObject::tr("Chuj")) );
//: Language name caf
    m_langList.append( new Language("caf", "Southern Carrier", QObject::tr("Southern Carrier")) );
//: Language name cak
    m_langList.append( new Language("cak", "Kaqchikel", QObject::tr("Kaqchikel")) );
//: Language name cao
    m_langList.append( new Language("cao", "Chácobo", QObject::tr("Chácobo")) );
//: Language name cap
    m_langList.append( new Language("cap", "Chipaya", QObject::tr("Chipaya")) );
//: Language name car
    m_langList.append( new Language("car", "Galibi Carib", QObject::tr("Galibi Carib")) );
//: Language name cav
    m_langList.append( new Language("cav", "Cavineña", QObject::tr("Cavineña")) );
//: Language name cax
    m_langList.append( new Language("cax", "Chiquitano", QObject::tr("Chiquitano")) );
//: Language name cbc
    m_langList.append( new Language("cbc", "Carapana", QObject::tr("Carapana")) );
//: Language name cbi
    m_langList.append( new Language("cbi", "Chachi", QObject::tr("Chachi")) );
//: Language name cbk
    m_langList.append( new Language("cbk", "Chavacano", QObject::tr("Chavacano")) );
//: Language name cbr
    m_langList.append( new Language("cbr", "Cashibo-Cacataibo", QObject::tr("Cashibo-Cacataibo")) );
//: Language name cbs
    m_langList.append( new Language("cbs", "Cashinahua", QObject::tr("Cashinahua")) );
//: Language name cbt
    m_langList.append( new Language("cbt", "Chayahuita", QObject::tr("Chayahuita")) );
//: Language name cbu
    m_langList.append( new Language("cbu", "Candoshi-Shapra", QObject::tr("Candoshi-Shapra")) );
//: Language name cbv
   m_langList.append( new Language("cbv", "Cacua", QObject::tr("Cacua")) );
//: Language name ce-Cyrl
   m_langList.append( new Language("ce-Cyrl", "нохчийн мотт", QObject::tr("Chechen (Cyrillic script)")) );
//: Language name cgc
    m_langList.append( new Language("cgc", "Kagayanen", QObject::tr("Kagayanen")) );
//: Language name chf
    m_langList.append( new Language("chf", "Tabasco Chontal", QObject::tr("Tabasco Chontal")) );
//: Language name chk
    m_langList.append( new Language("chk", "Chuukese", QObject::tr("Chuukese")) );
//: Language name chr
    m_langList.append( new Language("chr", "ᏣᎳᎩ", QObject::tr("Cherokee")) );
//: Language name cjo
    m_langList.append( new Language("cjo", "Ashéninka Pajonal", QObject::tr("Ashéninka Pajonal")) );
//: Language name cjs-Cyrl
    m_langList.append( new Language("cjs-Cyrl", "Шор тили", QObject::tr("Shor (Cyrillic script)")) );
//: Language name cjv
    m_langList.append( new Language("cjv", "Chuave", QObject::tr("Chuave")) );
//: Language name ckb
    m_langList.append( new Language("ckb", "Central Kurdish", QObject::tr("Central Kurdish")) );
//: Language name ckt-Cyrl
    m_langList.append( new Language("ckt-Cyrl", "Ԓыгъоравэтԓьэн йиԓыйиԓ", QObject::tr("Chukot (Cyrillic script)")) );
//: Language name cle
    m_langList.append( new Language("cle", "Lealao Chinantec", QObject::tr("Lealao Chinantec")) );
//: Language name clu
    m_langList.append( new Language("clu", "Caluyanun", QObject::tr("Caluyanun")) );
//: Language name cme
    m_langList.append( new Language("cme", "Cerma", QObject::tr("Cerma")) );
//: Language name cmn
    m_langList.append( new Language("cmn", "官话", QObject::tr("Mandarin Chinese")) );
//: Language name cni
    m_langList.append( new Language("cni", "Asháninka", QObject::tr("Asháninka")) );
//: Language name cof
    m_langList.append( new Language("cof", "Colorado", QObject::tr("Colorado")) );
//: Language name con
    m_langList.append( new Language("con", "Cofán", QObject::tr("Cofán")) );
//: Language name cot
    m_langList.append( new Language("cot", "Caquinte", QObject::tr("Caquinte")) );
//: Language name cpa
    m_langList.append( new Language("cpa", "Palantla Chinantec", QObject::tr("Palantla Chinantec")) );
//: Language name cpb
    m_langList.append( new Language("cpb", "Ucayali-Yurúa Ashéninka", QObject::tr("Ucayali-Yurúa Ashéninka")) );
//: Language name cpc
    m_langList.append( new Language("cpc", "Ajyíninka Apurucayali", QObject::tr("Ajyíninka Apurucayali")) );
//: Language name cpu
    m_langList.append( new Language("cpu", "Pichis Ashéninka", QObject::tr("Pichis Ashéninka")) );
//: Language name cpy
    m_langList.append( new Language("cpy", "South Ucayali Ashéninka", QObject::tr("South Ucayali Ashéninka")) );
//: Language name crh-Cyrl
    m_langList.append( new Language("crh-Cyrl", "Къырымтатар тили", QObject::tr("Crimean Tatar (Cyrillic script)")) );
//: Language name crn
    m_langList.append( new Language("crn", "El Nayar Cora", QObject::tr("El Nayar Cora")) );
//: Language name crx
    m_langList.append( new Language("crx", "Carrier", QObject::tr("Carrier")) );
//: Language name cta
    m_langList.append( new Language("cta", "Tataltepec Chatino", QObject::tr("Tataltepec Chatino")) );
//: Language name ctu
    m_langList.append( new Language("ctu", "Ch'ol", QObject::tr("Chol")) );
//: Language name cub
    m_langList.append( new Language("cub", "Cubeo", QObject::tr("Cubeo")) );
//: Language name cuc
    m_langList.append( new Language("cuc", "Usila Chinantec", QObject::tr("Usila Chinantec")) );
//: Language name cui
    m_langList.append( new Language("cui", "Cuiba", QObject::tr("Cuiba")) );
//: Language name cuk
    m_langList.append( new Language("cuk", "San Blas Kuna", QObject::tr("San Blas Kuna")) );
//: Language name cut
    m_langList.append( new Language("cut", "Teutila Cuicatec", QObject::tr("Teutila Cuicatec")) );
//: Language name cux
    m_langList.append( new Language("cux", "Tepeuxila Cuicatec", QObject::tr("Tepeuxila Cuicatec")) );
//: Language name cv-Cyrl
    m_langList.append( new Language("cv-Cyrl", "Чӑвашла", QObject::tr("Chuvash (Cyrillic script)")) );
//: Language name cwe
    m_langList.append( new Language("cwe", "Kwere", QObject::tr("Kwere")) );
//: Language name cya
    m_langList.append( new Language("cya", "Nopala Chatino", QObject::tr("Nopala Chatino")) );
    //: Language name ca
    m_langList.append( new Language("ca", "Catalan", QObject::tr("Catalan")) );
    //  m_langList.append( new Language("ce", "Chechen", QObject::tr("Chechen")) );
    //: Language name cco
    m_langList.append( new Language("cco", "Chinantec, Comaltepec", QObject::tr("Chinantec, Comaltepec")) );
    //: Language name ceb
    m_langList.append( new Language("ceb", "Cebuano", QObject::tr("Cebuano")) );
    //: Language name ch
    m_langList.append( new Language("ch", "Chamorro", QObject::tr("Chamorro")) );
    //: Language name chd
    m_langList.append( new Language("chd", "Chontal, Highland Oaxaca", QObject::tr("Chontal, Highland Oaxaca")) );
    //: Language name chq
    m_langList.append( new Language("chq", "Chinantec, Quiotepec", QObject::tr("Chinantec, Quiotepec")) );
    //: Language name chz
    m_langList.append( new Language("chz", "Chinantec, Ozumac\u00edn", QObject::tr("Chinantec, Ozumac\u00edn")) );
    //  m_langList.append( new Language("co", "Corsican", QObject::tr("Corsican")) );
    //: Language name ckw
    m_langList.append( new Language("ckw", "Cakchiquel, Western", QObject::tr("Cakchiquel, Western")) );
    //: Language name cnl
    m_langList.append( new Language("cnl", "Chinantec, Lalana", QObject::tr("Chinantec, Lalana")) );
    //: Language name cnt
    m_langList.append( new Language("cnt", "Chinantec, Tepetotutla", QObject::tr("Chinantec, Tepetotutla")) );
    //: Language name cop
    m_langList.append( new Language("cop", "Coptic", QObject::tr("Coptic")) );
    //: Language name cs
    m_langList.append( new Language("cs", "Czech", QObject::tr("Czech")) );
    //: Language name cso
    m_langList.append( new Language("cso", "Chinantec, Sochiapan", QObject::tr("Chinantec, Sochiapan")) );
    //: Language name cti
    m_langList.append( new Language("cti", "Chol, Tila", QObject::tr("Chol, Tila")) );
    //: Language name ctp
    m_langList.append( new Language("ctp", "Chatino, Western Highland", QObject::tr("Chatino, Western Highland")) );
    //: Language name cu
    m_langList.append( new Language("cu", "Church Slavic", QObject::tr("Church Slavic")) );
    //  m_langList.append( new Language("cv", "Chuvash", QObject::tr("Chuvash")) );
    //: Language name cy
    m_langList.append( new Language("cy", "Welsh", QObject::tr("Welsh")) );
    //: Language name da
    m_langList.append( new Language("da", "Danish", QObject::tr("Danish")) );
    //: Language name de
    m_langList.append( new Language("de", "German", QObject::tr("German")) );
    //: Language name dug
    m_langList.append( new Language("dug", "Duruma", QObject::tr("Duruma")) );
    //: Language name daa
    m_langList.append( new Language("daa", "Dangaléat", QObject::tr("Dangaléat")) );
//: Language name dad
    m_langList.append( new Language("dad", "Marik", QObject::tr("Marik")) );
//: Language name dah
    m_langList.append( new Language("dah", "Gwahatike", QObject::tr("Gwahatike")) );
//: Language name dar-Cyrl
    m_langList.append( new Language("dar-Cyrl", "Дарган мез", QObject::tr("Dargwa (Cyrillic script)")) );
//: Language name ded
    m_langList.append( new Language("ded", "Dedua", QObject::tr("Dedua")) );
//: Language name dgc
    m_langList.append( new Language("dgc", "Casiguran Dumagat Agta", QObject::tr("Casiguran Dumagat Agta")) );
//: Language name dgr
    m_langList.append( new Language("dgr", "Dogrib", QObject::tr("Dogrib")) );
//: Language name dgz
    m_langList.append( new Language("dgz", "Daga", QObject::tr("Daga")) );
//: Language name dhg
    m_langList.append( new Language("dhg", "Dhangu", QObject::tr("Dhangu")) );
//: Language name dif
    m_langList.append( new Language("dif", "Dieri", QObject::tr("Dieri")) );
//: Language name dik
    m_langList.append( new Language("dik", "Southwestern Dinka", QObject::tr("Southwestern Dinka")) );
//: Language name dji
    m_langList.append( new Language("dji", "Djinang", QObject::tr("Djinang")) );
//: Language name djk
    m_langList.append( new Language("djk", "Aukan", QObject::tr("Aukan")) );
//: Language name djr
    m_langList.append( new Language("djr", "Djambarrpuyngu", QObject::tr("Djambarrpuyngu")) );
//: Language name dng
    m_langList.append( new Language("dng", "Хуэйзў йүян", QObject::tr("Dungan")) );
//: Language name dob
    m_langList.append( new Language("dob", "Dobu", QObject::tr("Dobu")) );
//: Language name dop
    m_langList.append( new Language("dop", "Lukpa", QObject::tr("Lukpa")) );
//: Language name dwr
    m_langList.append( new Language("dwr", "Dawro", QObject::tr("Dawro")) );
//: Language name dww
    m_langList.append( new Language("dww", "Dawawa", QObject::tr("Dawawa")) );
//: Language name dwy
    m_langList.append( new Language("dwy", "Dhuwaya", QObject::tr("Dhuwaya")) );
    //  m_langList.append( new Language("dz", "Dzongkha", QObject::tr("Dzongkha")) );
    //: Language name el
    m_langList.append( new Language("el", "Greek, Modern (1453-)", QObject::tr("Greek, Modern (1453-)"), makeStringList("gre;ell")) );
    //: Language name en
    m_langList.append( new Language("en", "English", QObject::tr("English")) );
    //: Language name en_US
    m_langList.append( new Language("en_US", "American English", QObject::tr("American English")) );
    //: Language name enm
    m_langList.append( new Language("enm", "English, Middle (1100-1500)", QObject::tr("English, Middle (1100-1500)")) );
    //: Language name eo
    m_langList.append( new Language("eo", "Esperanto", QObject::tr("Esperanto")) );
    //: Language name es
    m_langList.append( new Language("es", "Spanish", QObject::tr("Spanish")) );
    //: Language name et
    m_langList.append( new Language("et", "Estonian", QObject::tr("Estonian")) );
    //: Language name eu
    m_langList.append( new Language("eu", "Basque", QObject::tr("Basque")) );
    //: Language name ebk
    m_langList.append( new Language("ebk", "Bontok", QObject::tr("East Bontoc")) );
//: Language name eko
    m_langList.append( new Language("eko", "Koti", QObject::tr("Koti")) );
//: Language name emi
    m_langList.append( new Language("emi", "Mussau-Emira", QObject::tr("Mussau-Emira")) );
//: Language name emp
    m_langList.append( new Language("emp", "Northern Emberá", QObject::tr("Northern Emberá")) );
//: Language name enq
    m_langList.append( new Language("enq", "Enga", QObject::tr("Enga")) );
//: Language name eri
    m_langList.append( new Language("eri", "Ogea", QObject::tr("Ogea")) );
//: Language name ese
    m_langList.append( new Language("ese", "Ese Ejja", QObject::tr("Ese Ejja")) );
//: Language name esk
    m_langList.append( new Language("esk", "Northwest Alaska Inupiatun", QObject::tr("Northwest Alaska Inupiatun")) );
//: Language name etr
    m_langList.append( new Language("etr", "Edolo", QObject::tr("Edolo")) );
    //: Language name fa
    m_langList.append( new Language("fa", "Persian", QObject::tr("Persian")) );
    //: Language name fi
    m_langList.append( new Language("fi", "Finnish", QObject::tr("Finnish")) );
    //  m_langList.append( new Language("fj", "Fijian", QObject::tr("Fijian")) );
    //  m_langList.append( new Language("fo", "Faroese", QObject::tr("Faroese")) );
    //: Language name fr
    m_langList.append( new Language("fr", "French", QObject::tr("French")) );
    //: Language name fy
    m_langList.append( new Language("fy", "Frisian", QObject::tr("Frisian")) );
    //: Language name faa
    m_langList.append( new Language("faa", "Fasu", QObject::tr("Fasu")) );
//: Language name fai
    m_langList.append( new Language("fai", "Faiwol", QObject::tr("Faiwol")) );
//: Language name far
    m_langList.append( new Language("far", "Fataleka", QObject::tr("Fataleka")) );
//: Language name ffm
    m_langList.append( new Language("ffm", "Maasina Fulfulde", QObject::tr("Maasina Fulfulde")) );
//: Language name for
    m_langList.append( new Language("for", "Fore", QObject::tr("Fore")) );
//: Language name fue
    m_langList.append( new Language("fue", "Borgu Fulfulde", QObject::tr("Borgu Fulfulde")) );
//: Language name fuf
    m_langList.append( new Language("fuf", "Pular", QObject::tr("Pular")) );
//: Language name fuh
    m_langList.append( new Language("fuh", "Fulfuldé du Niger ouest", QObject::tr("Western Niger Fulfulde")) );
    //: Language name ga
    m_langList.append( new Language("ga", "Irish", QObject::tr("Irish")) );
    //: Language name gd
    m_langList.append( new Language("gd", "Gaelic (Scots)", QObject::tr("Gaelic (Scots)")) );
    //: Language name gez
    m_langList.append( new Language("gez", "Geez", QObject::tr("Geez")) );
    //  m_langList.append( new Language("gl", "Gallegan", QObject::tr("Gallegan")) );
    //: Language name got
    m_langList.append( new Language("got", "Gothic", QObject::tr("Gothic")) );
    //: Language name gv
    m_langList.append( new Language("gv", "Manx", QObject::tr("Manx")) );
    //: Language name grc
    m_langList.append( new Language("grc", "Greek, Ancient (to 1453)", QObject::tr("Greek, Ancient (to 1453)")) );
    //: Language name gag-Cyrl
    m_langList.append( new Language("gag-Cyrl", "Гагаузы", QObject::tr("Gagauz (Cyrillic script)")) );
//: Language name gag-Latn
    m_langList.append( new Language("gag-Latn", "Gagauz", QObject::tr("Gagauz (Cyrillic script)")) );
//: Language name gah
    m_langList.append( new Language("gah", "Alekano", QObject::tr("Alekano")) );
//: Language name gai
    m_langList.append( new Language("gai", "Borei", QObject::tr("Borei")) );
//: Language name gam
    m_langList.append( new Language("gam", "Kandawo", QObject::tr("Kandawo")) );
//: Language name gaw
    m_langList.append( new Language("gaw", "Nobonob", QObject::tr("Nobonob")) );
//: Language name gdn
    m_langList.append( new Language("gdn", "Mo'da", QObject::tr("Mo'da")) );
//: Language name gdr
    m_langList.append( new Language("gdr", "Wipi", QObject::tr("Wipi")) );
//: Language name geb
    m_langList.append( new Language("geb", "Kire", QObject::tr("Kire")) );
//: Language name gfk
    m_langList.append( new Language("gfk", "Patpatar", QObject::tr("Patpatar")) );
//: Language name ghs
    m_langList.append( new Language("ghs", "Guhu-Samane", QObject::tr("Guhu-Samane")) );
//: Language name gia
    m_langList.append( new Language("gia", "Kitja", QObject::tr("Kitja")) );
//: Language name glk
    m_langList.append( new Language("glk", "Gilaki", QObject::tr("Gilaki")) );
//: Language name gmv
    m_langList.append( new Language("gmv", "Gamo", QObject::tr("Gamo")) );
//: Language name gng
    m_langList.append( new Language("gng", "Gamo", QObject::tr("Gamo")) );
//: Language name gnn
    m_langList.append( new Language("gnn", "Gumatj", QObject::tr("Gumatj")) );
//: Language name gnw
    m_langList.append( new Language("gnw", "Western Bolivian Guaraní", QObject::tr("Western Bolivian Guaraní")) );
//: Language name gof
    m_langList.append( new Language("gof", "Gofa", QObject::tr("Gofa")) );
//: Language name gu
    m_langList.append( new Language("gu", "ગુજરાતી", QObject::tr("Gujarati")) );
    //: Language name gub
    m_langList.append( new Language("gub", "Guajajára", QObject::tr("Guajajára")) );
//: Language name gue
    m_langList.append( new Language("gue", "Gurinji", QObject::tr("Gurinji")) );
//: Language name guh
    m_langList.append( new Language("guh", "Guahibo", QObject::tr("Guahibo")) );
//: Language name gui
    m_langList.append( new Language("gui", "Eastern Bolivian Guaraní", QObject::tr("Eastern Bolivian Guaraní")) );
//: Language name gul
    m_langList.append( new Language("gul", "Sea Island Creole English", QObject::tr("Sea Island Creole English")) );
//: Language name gum
    m_langList.append( new Language("gum", "Guambiano", QObject::tr("Guambiano")) );
//: Language name gun
    m_langList.append( new Language("gun", "Mbyá Guaraní", QObject::tr("Mbyá Guaraní")) );
//: Language name guo
    m_langList.append( new Language("guo", "Guayabero", QObject::tr("Guayabero")) );
//: Language name gup
    m_langList.append( new Language("gup", "Gunwinggu", QObject::tr("Gunwinggu")) );
//: Language name gvc
    m_langList.append( new Language("gvc", "Guanano", QObject::tr("Guanano")) );
//: Language name gvf
    m_langList.append( new Language("gvf", "Golin", QObject::tr("Golin")) );
//: Language name gvn
    m_langList.append( new Language("gvn", "Kuku-Yalanji", QObject::tr("Kuku-Yalanji")) );
//: Language name gwi
    m_langList.append( new Language("gwi", "Gwichʼin", QObject::tr("Gwichʼin")) );
//: Language name gym
    m_langList.append( new Language("gym", "Ngäbere", QObject::tr("Ngäbere")) );
//: Language name gyr
    m_langList.append( new Language("gyr", "Guarayu", QObject::tr("Guarayu")) );
    //: Language name he
    m_langList.append( new Language("he", "Hebrew", QObject::tr("Hebrew")) );
    //: Language name hau
    m_langList.append( new Language("hau", "Hausa", QObject::tr("Hausa")) );
    //: Language name haw
    m_langList.append( new Language("haw", "Hawaiian", QObject::tr("Hawaiian")) );
    //: Language name hi
    m_langList.append( new Language("hi", "Hindi", QObject::tr("Hindi")) );
    //  m_langList.append( new Language("ho", "Hiri Motu", QObject::tr("Hiri Motu")) );
    //: Language name hr
    m_langList.append( new Language("hr", "Croatian", QObject::tr("Croatian")) );
    //: Language name ht
    m_langList.append( new Language("ht", "Haitian Creole", QObject::tr("Haitian Creole")) );
    //: Language name hu
    m_langList.append( new Language("hu", "Hungarian", QObject::tr("Hungarian")) );
    //: Language name huv
    m_langList.append( new Language("huv", "Huave, San Mateo Del Mar", QObject::tr("Huave, San Mateo Del Mar")) );
    //: Language name hy
    m_langList.append( new Language("hy", "Armenian", QObject::tr("Armenian")) );
    //: Language name hbo
    m_langList.append( new Language("hbo", "עברית מקראית", QObject::tr("Ancient Hebrew")) );
//: Language name hch
    m_langList.append( new Language("hch", "Huichol", QObject::tr("Huichol")) );
//: Language name heg
    m_langList.append( new Language("heg", "Helong", QObject::tr("Helong")) );
//: Language name hix
    m_langList.append( new Language("hix", "Hixkaryána", QObject::tr("Hixkaryána")) );
//: Language name hla
    m_langList.append( new Language("hla", "Halia", QObject::tr("Halia")) );
//: Language name hlt
    m_langList.append( new Language("hlt", "Nga La", QObject::tr("Nga La")) );
//: Language name hns
    m_langList.append( new Language("hns", "Caribbean Hindustani", QObject::tr("Caribbean Hindustani")) );
//: Language name ho
    m_langList.append( new Language("ho", "Hiri Motu", QObject::tr("Hiri Motu")) );
//: Language name hop
    m_langList.append( new Language("hop", "Hopi", QObject::tr("Hopi")) );
//: Language name hot
    m_langList.append( new Language("hot", "Hote", QObject::tr("Hote")) );
//: Language name hto
    m_langList.append( new Language("hto", "Minica Huitoto", QObject::tr("Minica Huitoto")) );
//: Language name hub
    m_langList.append( new Language("hub", "Huambisa", QObject::tr("Huambisa")) );
//: Language name hui
    m_langList.append( new Language("hui", "Huli", QObject::tr("Huli")) );
//: Language name hus
    m_langList.append( new Language("hus", "Huastec", QObject::tr("Huastec")) );
//: Language name huu
    m_langList.append( new Language("huu", "Murui Huitoto", QObject::tr("Murui Huitoto")) );
//: Language name hvn
    m_langList.append( new Language("hvn", "Sabu", QObject::tr("Sabu")) );
//: Language name hwc
    m_langList.append( new Language("hwc", "Hawai'i Creole English", QObject::tr("Hawai'i Creole English")) );
  //: Language name hans
    m_langList.append( new Language("hans", "简体字", QObject::tr("Simplified Chinese")) );
      //: Language name hant
    m_langList.append( new Language("hant", "繁體字", QObject::tr("Traditional Chinese")) ); 
    //  m_langList.append( new Language("hz", "Herero", QObject::tr("Herero")) );
    //  m_langList.append( new Language("ia", "Interlingua", QObject::tr("Interlingua")) );
        //: Language name ian
    m_langList.append( new Language("ian", "Iatmul", QObject::tr("Iatmul")) );
    //: Language name id
    m_langList.append( new Language("id", "Indonesian", QObject::tr("Indonesian")) );
    //  m_langList.append( new Language("ie", "Interlingue", QObject::tr("Interlingue")) );
    //  m_langList.append( new Language("ik", "Inupiaq", QObject::tr("Inupiaq")) );
//: Language name ign
    m_langList.append( new Language("ign", "Ignaciano", QObject::tr("Ignaciano")) );
    //: Language name ikk
    m_langList.append( new Language("ikk", "Ika", QObject::tr("Ika")) );
//: Language name ikw
    m_langList.append( new Language("ikw", "Ikwere", QObject::tr("Ikwere")) );
    //: Language name is
    m_langList.append( new Language("is", "Icelandic", QObject::tr("Icelandic")) );
    //: Language name it
    m_langList.append( new Language("it", "Italian", QObject::tr("Italian")) );
    //: Language name itz
    m_langList.append( new Language("itz", "Itz\u00e1", QObject::tr("Itz\u00e1")) );
    //: Language name ixl
    m_langList.append( new Language("ixl", "Ixil, San Juan Cotzal", QObject::tr("Ixil, San Juan Cotzal")) );
    //: Language name imo
    m_langList.append( new Language("imo", "Imbongu", QObject::tr("Imbongu")) );
//: Language name inb
    m_langList.append( new Language("inb", "Inga", QObject::tr("Inga")) );
//: Language name ino
    m_langList.append( new Language("ino", "Inoke-Yate", QObject::tr("Inoke-Yate")) );
//: Language name iou
    m_langList.append( new Language("iou", "Tuma-Irumu", QObject::tr("Tuma-Irumu")) );
//: Language name ipi
    m_langList.append( new Language("ipi", "Ipili", QObject::tr("Ipili")) );
//: Language name iws
    m_langList.append( new Language("iws", "Sepik Iwam", QObject::tr("Sepik Iwam")) );
    //  m_langList.append( new Language("iu", "Inuktitut", QObject::tr("Inuktitut")) );
    //: Language name ja
    m_langList.append( new Language("ja", "Japanese", QObject::tr("Japanese")) );
    //: Language name jac
    m_langList.append( new Language("jac", "Jacalteco, Eastern", QObject::tr("Jacalteco, Eastern")) );
    //: Language name jvn
    m_langList.append( new Language("jvn", "Javanese, Caribbean", QObject::tr("Javanese, Caribbean")) );
    //: Language name jae
    m_langList.append( new Language("jae", "Yabem", QObject::tr("Yabem")) );
//: Language name jao
    m_langList.append( new Language("jao", "Yanyuwa", QObject::tr("Yanyuwa")) );
//: Language name jic
    m_langList.append( new Language("jic", "Tol", QObject::tr("Tol")) );
//: Language name jiv
    m_langList.append( new Language("jiv", "Shuar", QObject::tr("Shuar")) );
    //: Language name ka
    m_langList.append( new Language("ka", "Georgian", QObject::tr("Georgian")) );
    //: Language name kek
    m_langList.append( new Language("kek", "Kekch\u00ed", QObject::tr("Kekch\u00ed", "kek")) );
        //  m_langList.append( new Language("ki", "Kikuyu", QObject::tr("Kikuyu")) );
    //:  Language name kjh-Cyrl
    m_langList.append( new Language("kjh-Cyrl", "Xaкac тiлi", QObject::tr("Khakas")) );
    //  m_langList.append( new Language("kk", "Kazakh", QObject::tr("Kazakh")) );
    //  m_langList.append( new Language("kl", "Kalaallisut", QObject::tr("Kalaallisut")) );
    //  m_langList.append( new Language("km", "Khmer", QObject::tr("Khmer")) );
    //: Language name knv
    m_langList.append( new Language("knv", "Tabo", QObject::tr("Tabo")) );
    //: Language name ko
    m_langList.append( new Language("ko", "Korean", QObject::tr("Korean")) );
    //  m_langList.append( new Language("ks", "Kashmiri", QObject::tr("Kashmiri")) );
    //: Language name ku
    m_langList.append( new Language("ku", "Kurdish", QObject::tr("Kurdish")) );
    //  m_langList.append( new Language("kv", "Komi", QObject::tr("Komi")) );
    //  m_langList.append( new Language("kw", "Cornish", QObject::tr("Cornish")) );
    //: Language name ky
    m_langList.append( new Language("ky", "Kirghiz", QObject::tr("Kirghiz")) );
    //: Language name kaa-Cyrl
    m_langList.append( new Language("kaa-Cyrl", "Қарақалпақ тили", QObject::tr("Kara-Kalpak (Cyrillic script)")) );
//: Language name kaa-Latn
    m_langList.append( new Language("kaa-Latn", "Karakalpak", QObject::tr("")) );
//: Language name kap-Cyrl
    m_langList.append( new Language("kap-Cyrl", "", QObject::tr("Kara-Kalpak (Latin script)")) );
//: Language name kaq
    m_langList.append( new Language("kaq", "Capanahua", QObject::tr("Capanahua")) );
//: Language name kbc
    m_langList.append( new Language("kbc", "Kadiwéu", QObject::tr("Kadiwéu")) );
//: Language name kbd
    m_langList.append( new Language("kbd", "Адыгэбзэ", QObject::tr("Kabardian")) );
//: Language name kbh
    m_langList.append( new Language("kbh", "Camsá", QObject::tr("Camsá")) );
//: Language name kbm
    m_langList.append( new Language("kbm", "Iwal", QObject::tr("Iwal")) );
//: Language name kbq
    m_langList.append( new Language("kbq", "Kamano", QObject::tr("Kamano")) );
//: Language name kdc
    m_langList.append( new Language("kdc", "Kutu", QObject::tr("Kutu")) );
//: Language name kde
    m_langList.append( new Language("kde", "Makonde", QObject::tr("Makonde")) );
//: Language name kdl
    m_langList.append( new Language("kdl", "Tsikimba", QObject::tr("Tsikimba")) );
//: Language name ken
    m_langList.append( new Language("ken", "Kenyang", QObject::tr("Kenyang")) );
//: Language name kew
    m_langList.append( new Language("kew", "West Kewa", QObject::tr("West Kewa")) );
//: Language name kgf
    m_langList.append( new Language("kgf", "Kube", QObject::tr("Kube")) );
//: Language name kgk
    m_langList.append( new Language("kgk", "Kaiwá", QObject::tr("Kaiwá")) );
//: Language name kgp
    m_langList.append( new Language("kgp", "Kaingang", QObject::tr("Kaingang")) );
//: Language name khs
    m_langList.append( new Language("khs", "Kasua", QObject::tr("Kasua")) );
//: Language name khz
    m_langList.append( new Language("khz", "Keapara", QObject::tr("Keapara")) );
//: Language name kje
    m_langList.append( new Language("kje", "Kisar", QObject::tr("Kisar")) );
//: Language name kjh
    m_langList.append( new Language("kjh", "Xaкac тiлi", QObject::tr("")) );
//: Language name kjs
    m_langList.append( new Language("kjs", "", QObject::tr("Khakas")) );
//: Language name kk
    m_langList.append( new Language("kk", "Қазақша", QObject::tr("Kazakh")) );
//: Language name kkc
    m_langList.append( new Language("kkc", "Odoodee", QObject::tr("Odoodee")) );
//: Language name kkl
    m_langList.append( new Language("kkl", "Kosarek Yale", QObject::tr("Kosarek Yale")) );
//: Language name kky
    m_langList.append( new Language("kky", "Guguyimidjir", QObject::tr("Guguyimidjir")) );
//: Language name klt
    m_langList.append( new Language("klt", "Nukna", QObject::tr("Nukna")) );
//: Language name klv
    m_langList.append( new Language("klv", "Maskelynes", QObject::tr("Maskelynes")) );
//: Language name km
    m_langList.append( new Language("km", "ភាសាខ្មែរ", QObject::tr("Central Khmer")) );
//: Language name kmg
    m_langList.append( new Language("kmg", "Kâte", QObject::tr("Kâte")) );
//: Language name kmh
    m_langList.append( new Language("kmh", "Kalam", QObject::tr("Kalam")) );
//: Language name kmk
    m_langList.append( new Language("kmk", "Limos Kalinga", QObject::tr("Limos Kalinga")) );
//: Language name kmo
    m_langList.append( new Language("kmo", "Kwoma", QObject::tr("Kwoma")) );
//: Language name kmr-Cyrl
    m_langList.append( new Language("kmr-Cyrl", "Кӧрманщи", QObject::tr("")) );
//: Language name kmr-Latn
    m_langList.append( new Language("kmr-Latn", "", QObject::tr("Northern Kurdish (Cyrillic script)")) );
//: Language name kms
    m_langList.append( new Language("kms", "Kamasau", QObject::tr("Kamasau")) );
//: Language name kmu
    m_langList.append( new Language("kmu", "Kanite", QObject::tr("Kanite")) );
//: Language name kn
    m_langList.append( new Language("kn", "ಕನ್ನಡ", QObject::tr("Kannada")) );
//: Language name kne
    m_langList.append( new Language("kne", "Kankanaey", QObject::tr("Kankanaey")) );
//: Language name knf
    m_langList.append( new Language("knf", "Mankanya", QObject::tr("Mankanya")) );
//: Language name knj
    m_langList.append( new Language("knj", "Western Kanjobal", QObject::tr("Western Kanjobal")) );
//: Language name kos
    m_langList.append( new Language("kos", "Kosraean", QObject::tr("Kosraean")) );
//: Language name kpf
    m_langList.append( new Language("kpf", "Komba", QObject::tr("Komba")) );
//: Language name kpg
    m_langList.append( new Language("kpg", "Kapingamarangi", QObject::tr("Kapingamarangi")) );
//: Language name kpj
    m_langList.append( new Language("kpj", "Karajá", QObject::tr("Karajá")) );
//: Language name kpl
    m_langList.append( new Language("kpl", "Kpala", QObject::tr("Kpala")) );
//: Language name kpr
    m_langList.append( new Language("kpr", "Korafe-Yegha", QObject::tr("Korafe-Yegha")) );
//: Language name kpw
    m_langList.append( new Language("kpw", "Kobon", QObject::tr("Kobon")) );
//: Language name kpx
    m_langList.append( new Language("kpx", "Mountain Koiali", QObject::tr("Mountain Koiali")) );
//: Language name kpy-Cyrl
    m_langList.append( new Language("kpy-Cyrl", "Нымылг’ын", QObject::tr("Koryak (Cyrillic script)")) );
//: Language name kqa
    m_langList.append( new Language("kqa", "Mum", QObject::tr("Mum")) );
//: Language name kqc
    m_langList.append( new Language("kqc", "Doromu-Koki", QObject::tr("Doromu-Koki")) );
//: Language name kqf
    m_langList.append( new Language("kqf", "Kakabai", QObject::tr("Kakabai")) );
//: Language name kql
    m_langList.append( new Language("kql", "Kyenele", QObject::tr("Kyenele")) );
//: Language name kqw
    m_langList.append( new Language("kqw", "Kandas", QObject::tr("Kandas")) );
//: Language name ksd
    m_langList.append( new Language("ksd", "Kuanua", QObject::tr("Kuanua")) );
//: Language name ksj
    m_langList.append( new Language("ksj", "Uare", QObject::tr("Uare")) );
//: Language name ksr
    m_langList.append( new Language("ksr", "Borong", QObject::tr("Borong")) );
//: Language name ktm
    m_langList.append( new Language("ktm", "Kurti", QObject::tr("Kurti")) );
//: Language name kto
    m_langList.append( new Language("kto", "Kuot", QObject::tr("Kuot")) );
    //: Language name ktu
    m_langList.append( new Language("ktu", "Kikongo ya Leta", QObject::tr("Kituba (Democratic Republic of Congo)")) );
//: Language name kud
    m_langList.append( new Language("kud", "'Auhelawa", QObject::tr("'Auhelawa")) );
//: Language name kue
    m_langList.append( new Language("kue", "Kuman", QObject::tr("Kuman")) );
//: Language name kum
    m_langList.append( new Language("kum", "Къумукъ тил", QObject::tr("Kumyk")) );
//: Language name kup
    m_langList.append( new Language("kup", "Kunimaipa", QObject::tr("Kunimaipa")) );
//: Language name kvg
    m_langList.append( new Language("kvg", "Kuni-Boazi", QObject::tr("Kuni-Boazi")) );
//: Language name kvn
    m_langList.append( new Language("kvn", "Border Kuna", QObject::tr("Border Kuna")) );
//: Language name kwd
    m_langList.append( new Language("kwd", "Kwaio", QObject::tr("Kwaio")) );
//: Language name kwf
    m_langList.append( new Language("kwf", "Kwara'ae", QObject::tr("Kwara'ae")) );
//: Language name kwi
    m_langList.append( new Language("kwi", "Awa-Cuaiquer", QObject::tr("Awa-Cuaiquer")) );
//: Language name kwj
    m_langList.append( new Language("kwj", "Kwanga", QObject::tr("Kwanga")) );
//: Language name ky-Arab
    m_langList.append( new Language("ky-Arab", "قىرعىز تىلى", QObject::tr("Kirghiz (Arabic script)")) );
//: Language name kyc
    m_langList.append( new Language("kyc", "Kyaka", QObject::tr("Kyaka")) );
//: Language name ky-Cyrl
    m_langList.append( new Language("ky-Cyrl", "Кыргыз тили", QObject::tr("Kirghiz (Cyrillic script)")) );
//: Language name kyf
    m_langList.append( new Language("kyf", "Kouya", QObject::tr("Kouya")) );
//: Language name kyg
    m_langList.append( new Language("kyg", "Keyagana", QObject::tr("Keyagana")) );
//: Language name kyq
    m_langList.append( new Language("kyq", "Kenga", QObject::tr("Kenga")) );
//: Language name kyz
    m_langList.append( new Language("kyz", "Kayabí", QObject::tr("Kayabí")) );
//: Language name kze
    m_langList.append( new Language("kze", "Kosena", QObject::tr("Kosena")) );
    //: Language name la
    m_langList.append( new Language("la", "Latin", QObject::tr("Latin")) );
    //: Language name lac
    m_langList.append( new Language("lac", "Lacandon", QObject::tr("Lacandon")) );
    //  m_langList.append( new Language("lb", "Letzeburgesch", QObject::tr("Letzeburgesch")) );
    //: Language name lmo
    m_langList.append( new Language("lmo", "Lombard", QObject::tr("Lombard")) );
    m_langList.append( new Language("lin", "Lingala", QObject::tr("Lingàla")) );
        m_langList.append( new Language("ln", "Lingala", QObject::tr("Lingàla")) );
    //  m_langList.append( new Language("lo", "Lao", QObject::tr("Lao")) );
    //: Language name lbb
    m_langList.append( new Language("lbb", "Label", QObject::tr("Label")) );
//: Language name lbk
    m_langList.append( new Language("lbk", "Central Bontoc", QObject::tr("Central Bontoc")) );
//: Language name lcm
    m_langList.append( new Language("lcm", "Tungag", QObject::tr("Tungag")) );
//: Language name leu
    m_langList.append( new Language("leu", "Kara (Papua New Guinea)", QObject::tr("Kara (Papua New Guinea)")) );
//: Language name lex
    m_langList.append( new Language("lex", "Luang", QObject::tr("Luang")) );
    //: Language name lez
    m_langList.append( new Language("lez", "Lezghian", QObject::tr("Lezghian")) );
//: Language name lez-Cyrl
    m_langList.append( new Language("lez-Cyrl", "Лезги чIал", QObject::tr("Lezghian (Cyrillic script)")) );
//: Language name lgl
    m_langList.append( new Language("lgl", "Wala", QObject::tr("Wala")) );
//: Language name lid
    m_langList.append( new Language("lid", "Nyindrou", QObject::tr("Nyindrou")) );
//: Language name lif
    m_langList.append( new Language("lif", "Limbu", QObject::tr("Limbu")) );
//: Language name llg
    m_langList.append( new Language("llg", "Lole", QObject::tr("Lole")) );
    //: Language name lt
    m_langList.append( new Language("lt", "Lithuanian", QObject::tr("Lithuanian")) );
    //: Language name lv
    m_langList.append( new Language("lv", "Latvian", QObject::tr("Latvian")) );
    //: Language name lww
    m_langList.append( new Language("lww", "Lewo", QObject::tr("Lewo")) );
//: Language name lzh
    m_langList.append( new Language("lzh", "古文", QObject::tr("Literary Chinese")) );
    //: Language name mg
    m_langList.append( new Language("mg", "Malagasy", QObject::tr("Malagasy")) );
    //  m_langList.append( new Language("mh", "Marshall", QObject::tr("Marshall")) );
    //: Language name mi
    m_langList.append( new Language("mi", "Maori", QObject::tr("Maori")) );
    //: Language name mir
    m_langList.append( new Language("mir", "Mixe, Isthmus", QObject::tr("Mixe, Isthmus")) );
    //: Language name miz
    m_langList.append( new Language("miz", "Mixtec, Coatzospan", QObject::tr("Mixtec, Coatzospan")) );
    //: Language name mk
    m_langList.append( new Language("mk", "Macedonian", QObject::tr("Macedonian")) );
    //: Language name mks
    m_langList.append( new Language("mks", "Mixtec, Silacayoapan", QObject::tr("Mixtec, Silacayoapan")) );

    //  m_langList.append( new Language("mn", "Mongolian", QObject::tr("Mongolian")) );
    //  m_langList.append( new Language("mo", "Moldavian", QObject::tr("Moldavian")) );
    //: Language name mos
    m_langList.append( new Language("mos", "More", QObject::tr("More")) );
    //  m_langList.append( new Language("mr", "Marathi", QObject::tr("Marathi")) );
    //: Language name ms
    m_langList.append( new Language("ms", "Malay", QObject::tr("Malay")) );
    //: Language name mt
    m_langList.append( new Language("mt", "Maltese", QObject::tr("Maltese")) );
    //: Language name mul (meaning that the work has multiple languages)
    m_langList.append( new Language("mul", "(Multiple languages)", QObject::tr("(Multiple languages)")) );
    //: Language name mvc
    m_langList.append( new Language("mvc", "Mam, Central", QObject::tr("Mam, Central")) );
    //: Language name mvj
    m_langList.append( new Language("mvj", "Mam, Todos Santos Cuchumat\u00e1n", QObject::tr("Mam, Todos Santos Cuchumat\u00e1n")) );
    //: Language name mxq
    m_langList.append( new Language("mxq", "Mixe, Juquila", QObject::tr("Mixe, Juquila")) );
    //: Language name mxt
    m_langList.append( new Language("mxt", "Mixtec, Jamiltepec", QObject::tr("Mixtec, Jamiltepec")) );
    //: Language name my
    m_langList.append( new Language("my", "Burmese", QObject::tr("Burmese")) );
    //  m_langList.append( new Language("na", "Nauru", QObject::tr("Nauru")) );
    //: Language name maa
    m_langList.append( new Language("maa", "San Jerónimo Tecóatl Mazatec", QObject::tr("San Jerónimo Tecóatl Mazatec")) );
//: Language name maj
    m_langList.append( new Language("maj", "Jalapa De Díaz Mazatec", QObject::tr("Jalapa De Díaz Mazatec")) );
//: Language name mal
    m_langList.append( new Language("mal", "മലയാളം", QObject::tr("Malayalam")) );
//: Language name mam
    m_langList.append( new Language("mam", "Mam", QObject::tr("Mam")) );
//: Language name maq
    m_langList.append( new Language("maq", "Chiquihuitlán Mazatec", QObject::tr("Chiquihuitlán Mazatec")) );
//: Language name mau
    m_langList.append( new Language("mau", "Huautla Mazatec", QObject::tr("Huautla Mazatec")) );
//: Language name mav
    m_langList.append( new Language("mav", "Sateré-Mawé", QObject::tr("Sateré-Mawé")) );
//: Language name maz
    m_langList.append( new Language("maz", "Central Mazahua", QObject::tr("Central Mazahua")) );
//: Language name mbb
    m_langList.append( new Language("mbb", "Western Bukidnon Manobo", QObject::tr("Western Bukidnon Manobo")) );
//: Language name mbc
    m_langList.append( new Language("mbc", "Macushi", QObject::tr("Macushi")) );
//: Language name mbh
    m_langList.append( new Language("mbh", "Mangseng", QObject::tr("Mangseng")) );
//: Language name mbj
    m_langList.append( new Language("mbj", "Nadëb", QObject::tr("Nadëb")) );
//: Language name mbl
    m_langList.append( new Language("mbl", "Maxakalí", QObject::tr("Maxakalí")) );
//: Language name mbs
    m_langList.append( new Language("mbs", "Sarangani Manobo", QObject::tr("Sarangani Manobo")) );
//: Language name mbt
    m_langList.append( new Language("mbt", "Matigsalug Manobo", QObject::tr("Matigsalug Manobo")) );
//: Language name mca
    m_langList.append( new Language("mca", "Maca", QObject::tr("Maca")) );
//: Language name mcb
    m_langList.append( new Language("mcb", "Machiguenga", QObject::tr("Machiguenga")) );
//: Language name mcd
    m_langList.append( new Language("mcd", "Sharanahua", QObject::tr("Sharanahua")) );
//: Language name mcf
    m_langList.append( new Language("mcf", "Matsés", QObject::tr("Matsés")) );
//: Language name mco
    m_langList.append( new Language("mco", "Coatlán Mixe", QObject::tr("Coatlán Mixe")) );
//: Language name mcp
    m_langList.append( new Language("mcp", "Makaa", QObject::tr("Makaa")) );
//: Language name mcq
    m_langList.append( new Language("mcq", "Ese", QObject::tr("Ese")) );
//: Language name mdy
    m_langList.append( new Language("mdy", "Male (Ethiopia)", QObject::tr("Male (Ethiopia)")) );
//: Language name med
    m_langList.append( new Language("med", "Melpa", QObject::tr("Melpa")) );
//: Language name mee
    m_langList.append( new Language("mee", "Mengen", QObject::tr("Mengen")) );
//: Language name mek
    m_langList.append( new Language("mek", "Mekeo", QObject::tr("Mekeo")) );
//: Language name meq
    m_langList.append( new Language("meq", "Merey", QObject::tr("Merey")) );
//: Language name met
    m_langList.append( new Language("met", "Mato", QObject::tr("Mato")) );
//: Language name meu
    m_langList.append( new Language("meu", "Motu", QObject::tr("Motu")) );
//: Language name mgc
    m_langList.append( new Language("mgc", "Morokodo", QObject::tr("Morokodo")) );
//: Language name mgh
    m_langList.append( new Language("mgh", "Makhuwa-Meetto", QObject::tr("Makhuwa-Meetto")) );
//: Language name mgw
    m_langList.append( new Language("mgw", "Matumbi", QObject::tr("Matumbi")) );
//: Language name mhl
    m_langList.append( new Language("mhl", "Mauwake", QObject::tr("Mauwake")) );
//: Language name mib
    m_langList.append( new Language("mib", "Atatláhuca Mixtec", QObject::tr("Atatláhuca Mixtec")) );
//: Language name mic
    m_langList.append( new Language("mic", "Mi'kmaq", QObject::tr("Mi'kmaq")) );
//: Language name mie
    m_langList.append( new Language("mie", "Ocotepec Mixtec", QObject::tr("Ocotepec Mixtec")) );
//: Language name mig
    m_langList.append( new Language("mig", "San Miguel El Grande Mixtec", QObject::tr("San Miguel El Grande Mixtec")) );
//: Language name mih
    m_langList.append( new Language("mih", "Chayuco Mixtec", QObject::tr("Chayuco Mixtec")) );
//: Language name mil
    m_langList.append( new Language("mil", "Peñoles Mixtec", QObject::tr("Peñoles Mixtec")) );
//: Language name mio
    m_langList.append( new Language("mio", "Pinotepa Nacional Mixtec", QObject::tr("Pinotepa Nacional Mixtec")) );
//: Language name mit
    m_langList.append( new Language("mit", "Southern Puebla Mixtec", QObject::tr("Southern Puebla Mixtec")) );
//: Language name mjc
    m_langList.append( new Language("mjc", "San Juan Colorado Mixtec", QObject::tr("San Juan Colorado Mixtec")) );
//: Language name mkj
    m_langList.append( new Language("mkj", "Mokilese", QObject::tr("Mokilese")) );
//: Language name mkl
    m_langList.append( new Language("mkl", "Mokole", QObject::tr("Mokole")) );
//: Language name mkn
    m_langList.append( new Language("mkn", "Kupang Malay", QObject::tr("Kupang Malay")) );
//: Language name ml
    m_langList.append( new Language("ml", "മലയാളം", QObject::tr("Malayalam")) );
//: Language name mlh
    m_langList.append( new Language("mlh", "Mape", QObject::tr("Mape")) );
//: Language name mlp
    m_langList.append( new Language("mlp", "Bargam", QObject::tr("Bargam")) );
//: Language name mmo
    m_langList.append( new Language("mmo", "Mangga Buang", QObject::tr("Mangga Buang")) );
//: Language name mmx
    m_langList.append( new Language("mmx", "Madak", QObject::tr("Madak")) );
//: Language name mn
    m_langList.append( new Language("mn", "Монгол", QObject::tr("Mongolian")) );
//: Language name mna
    m_langList.append( new Language("mna", "Mbula", QObject::tr("Mbula")) );
//: Language name mop
    m_langList.append( new Language("mop", "Mopán Maya", QObject::tr("Mopán Maya")) );
//: Language name mox
    m_langList.append( new Language("mox", "Molima", QObject::tr("Molima")) );
//: Language name mph
    m_langList.append( new Language("mph", "Maung", QObject::tr("Maung")) );
//: Language name mpj
    m_langList.append( new Language("mpj", "Martu Wangka", QObject::tr("Martu Wangka")) );
//: Language name mpm
    m_langList.append( new Language("mpm", "Yosondúa Mixtec", QObject::tr("Yosondúa Mixtec")) );
//: Language name mpp
    m_langList.append( new Language("mpp", "Migabac", QObject::tr("Migabac")) );
//: Language name mps
    m_langList.append( new Language("mps", "Dadibi", QObject::tr("Dadibi")) );
//: Language name mpt
    m_langList.append( new Language("mpt", "Mian", QObject::tr("Mian")) );
//: Language name mpx
    m_langList.append( new Language("mpx", "Misima-Paneati", QObject::tr("Misima-Paneati")) );
//: Language name mqb
    m_langList.append( new Language("mqb", "Mbuko", QObject::tr("Mbuko")) );
//: Language name mqj
    m_langList.append( new Language("mqj", "Mamasa", QObject::tr("Mamasa")) );
//: Language name mr
    m_langList.append( new Language("mr", "मराठी", QObject::tr("Marathi")) );
//: Language name msb
    m_langList.append( new Language("msb", "Masbatenyo", QObject::tr("Masbatenyo")) );
//: Language name msc
    m_langList.append( new Language("msc", "Sankaran Maninka", QObject::tr("Sankaran Maninka")) );
//: Language name msk
    m_langList.append( new Language("msk", "Mansaka", QObject::tr("Mansaka")) );
//: Language name msm
    m_langList.append( new Language("msm", "Agusan Manobo", QObject::tr("Agusan Manobo")) );
//: Language name msy
    m_langList.append( new Language("msy", "Aruamu", QObject::tr("Aruamu")) );
//: Language name mti
    m_langList.append( new Language("mti", "Maiwa (Papua New Guinea)", QObject::tr("Maiwa (Papua New Guinea)")) );
//: Language name mto
    m_langList.append( new Language("mto", "Totontepec Mixe", QObject::tr("Totontepec Mixe")) );
//: Language name mux
    m_langList.append( new Language("mux", "Bo-Ung", QObject::tr("Bo-Ung")) );
//: Language name muy
    m_langList.append( new Language("muy", "Muyang", QObject::tr("Muyang")) );
//: Language name mva
    m_langList.append( new Language("mva", "Manam", QObject::tr("Manam")) );
//: Language name mvn
    m_langList.append( new Language("mvn", "Minaveha", QObject::tr("Minaveha")) );
//: Language name mwc
    m_langList.append( new Language("mwc", "Are", QObject::tr("Are")) );
//: Language name mwe
    m_langList.append( new Language("mwe", "Mwera (Chimwera)", QObject::tr("Mwera (Chimwera)")) );
//: Language name mwp
    m_langList.append( new Language("mwp", "Kala Lagaw Ya", QObject::tr("Kala Lagaw Ya")) );
//: Language name mxb
    m_langList.append( new Language("mxb", "Tezoatlán Mixtec", QObject::tr("Tezoatlán Mixtec")) );
//: Language name mxp
    m_langList.append( new Language("mxp", "Tlahuitoltepec Mixe", QObject::tr("Tlahuitoltepec Mixe")) );
//: Language name myk
    m_langList.append( new Language("myk", "Mamara Senoufo", QObject::tr("Mamara Senoufo")) );
//: Language name myu
    m_langList.append( new Language("myu", "Mundurukú", QObject::tr("Mundurukú")) );
//: Language name myw
    m_langList.append( new Language("myw", "Muyuw", QObject::tr("Muyuw")) );
//: Language name myy
    m_langList.append( new Language("myy", "Macuna", QObject::tr("Macuna")) );
//: Language name mzz
    m_langList.append( new Language("mzz", "Maiadomu", QObject::tr("Maiadomu")) );
    //: Language name nb
    m_langList.append( new Language("nb", "Norwegian Bokm\u00e5l", QObject::tr("Norwegian Bokm\u00e5l")) );
    //: Language name ncl
    m_langList.append( new Language("ncl", "Nahuatl, Michoac\u00e1n", QObject::tr("Nahuatl, Michoac\u00e1n")) );
    //  m_langList.append( new Language("nd", "Ndebele, North", QObject::tr("Ndebele, North")) );
    //: Language name nds
    m_langList.append( new Language("nds", "Low German; Low Saxon", QObject::tr("Low German; Low Saxon")) );
    //: Language name ne
    m_langList.append( new Language("ne", "Nepali", QObject::tr("Nepali")) );
    //: Language name ngu
    m_langList.append( new Language("ngu", "Nahuatl, Guerrero", QObject::tr("Nahuatl, Guerrero")) );
    //: Language name nhy
    m_langList.append( new Language("nhy", "Nahuatl, Northern Oaxaca", QObject::tr("Nahuatl, Northern Oaxaca")) );
    //  m_langList.append( new Language("ng", "Ndonga", QObject::tr("Ndonga")) );
    //: Language name nl
    m_langList.append( new Language("nl", "Dutch", QObject::tr("Dutch")) );
    //: Language name nn
    m_langList.append( new Language("nn", "Norwegian Nynorsk", QObject::tr("Norwegian Nynorsk")) );
    //: Language name no
    m_langList.append( new Language("no", "Norwegian", QObject::tr("Norwegian")) );
    //: Language name nab
    m_langList.append( new Language("nab", "Southern Nambikuára", QObject::tr("Southern Nambikuára")) );
//: Language name naf
    m_langList.append( new Language("naf", "Nabak", QObject::tr("Nabak")) );
//: Language name nak
    m_langList.append( new Language("nak", "Nakanai", QObject::tr("Nakanai")) );
//: Language name nas
    m_langList.append( new Language("nas", "Naasioi", QObject::tr("Naasioi")) );
//: Language name nay
    m_langList.append( new Language("nay", "Narrinyeri", QObject::tr("Narrinyeri")) );
//: Language name nbq
    m_langList.append( new Language("nbq", "Nggem", QObject::tr("Nggem")) );
//: Language name nca
    m_langList.append( new Language("nca", "Iyo", QObject::tr("Iyo")) );
//: Language name nch
    m_langList.append( new Language("nch", "Central Huasteca Nahuatl", QObject::tr("Central Huasteca Nahuatl")) );
//: Language name ncj
    m_langList.append( new Language("ncj", "Northern Puebla Nahuatl", QObject::tr("Northern Puebla Nahuatl")) );
//: Language name ncu
    m_langList.append( new Language("ncu", "Chumburung", QObject::tr("Chumburung")) );
//: Language name nd
    m_langList.append( new Language("nd", "Sindebele", QObject::tr("Sindebele")) );
//: Language name ndj
    m_langList.append( new Language("ndj", "Ndamba", QObject::tr("Ndamba")) );
//: Language name nfa
    m_langList.append( new Language("nfa", "Dhao", QObject::tr("Dhao")) );
//: Language name ngp
    m_langList.append( new Language("ngp", "Ngulu", QObject::tr("Ngulu")) );
//: Language name nhe
    m_langList.append( new Language("nhe", "Eastern Huasteca Nahuatl", QObject::tr("Eastern Huasteca Nahuatl")) );
//: Language name nhg
    m_langList.append( new Language("nhg", "Tetelcingo Nahuatl", QObject::tr("Tetelcingo Nahuatl")) );
//: Language name nhi
    m_langList.append( new Language("nhi", "Zacatlán-Ahuacatlán-Tepetzintla Nahuatl", QObject::tr("Zacatlán-Ahuacatlán-Tepetzintla Nahuatl")) );
//: Language name nho
    m_langList.append( new Language("nho", "Takuu", QObject::tr("Takuu")) );
//: Language name nhr
    m_langList.append( new Language("nhr", "Naro", QObject::tr("Naro")) );
//: Language name nhu
    m_langList.append( new Language("nhu", "Noone", QObject::tr("Noone")) );
//: Language name nhw
    m_langList.append( new Language("nhw", "Western Huasteca Nahuatl", QObject::tr("Western Huasteca Nahuatl")) );
//: Language name nif
    m_langList.append( new Language("nif", "Nek", QObject::tr("Nek")) );
//: Language name nii
    m_langList.append( new Language("nii", "Nii", QObject::tr("Nii")) );
//: Language name nin
    m_langList.append( new Language("nin", "Ninzo", QObject::tr("Ninzo")) );
//: Language name nko
    m_langList.append( new Language("nko", "Nkonya", QObject::tr("Nkonya")) );
//: Language name nlg
    m_langList.append( new Language("nlg", "Gela", QObject::tr("Gela")) );
//: Language name nmw
    m_langList.append( new Language("nmw", "Nimoa", QObject::tr("Nimoa")) );
//: Language name nna
    m_langList.append( new Language("nna", "Nyangumarta", QObject::tr("Nyangumarta")) );
//: Language name nnq
    m_langList.append( new Language("nnq", "Ngindo", QObject::tr("Ngindo")) );
//: Language name noa
    m_langList.append( new Language("noa", "Woun Meu", QObject::tr("Woun Meu")) );
//: Language name nog-Cyrl
    m_langList.append( new Language("nog-Cyrl", "Ногай тили", QObject::tr("Nogai (Cyrillic script)")) );
//: Language name nop
    m_langList.append( new Language("nop", "Numanggang", QObject::tr("Numanggang")) );
//: Language name not
    m_langList.append( new Language("not", "Nomatsiguenga", QObject::tr("Nomatsiguenga")) );
//: Language name nou
    m_langList.append( new Language("nou", "Ewage-Notu", QObject::tr("Ewage-Notu")) );
//: Language name npi
    m_langList.append( new Language("npi", "नेपाली", QObject::tr("Nepali (individual language)")) );
//: Language name npl
    m_langList.append( new Language("npl", "Southeastern Puebla Nahuatl", QObject::tr("Southeastern Puebla Nahuatl")) );
//: Language name nsn
    m_langList.append( new Language("nsn", "Nehan", QObject::tr("Nehan")) );
//: Language name nss
    m_langList.append( new Language("nss", "Nali", QObject::tr("Nali")) );
//: Language name ntj
    m_langList.append( new Language("ntj", "Ngaanyatjarra", QObject::tr("Ngaanyatjarra")) );
//: Language name ntp
    m_langList.append( new Language("ntp", "Northern Tepehuan", QObject::tr("Northern Tepehuan")) );
//: Language name ntu
    m_langList.append( new Language("ntu", "Natügu", QObject::tr("Natügu")) );
//: Language name nuy
    m_langList.append( new Language("nuy", "Nunggubuyu", QObject::tr("Nunggubuyu")) );
//: Language name nvm
    m_langList.append( new Language("nvm", "Namiae", QObject::tr("Namiae")) );
//: Language name nwi
    m_langList.append( new Language("nwi", "Southwest Tanna", QObject::tr("Southwest Tanna")) );
//: Language name nys
    m_langList.append( new Language("nys", "Nyunga", QObject::tr("Nyunga")) );
//: Language name nyu
    m_langList.append( new Language("nyu", "Nyungwe", QObject::tr("Nyungwe")) );
    //  m_langList.append( new Language("nv", "Navajo", QObject::tr("Navajo")) );
    //  m_langList.append( new Language("ny", "Chichewa; Nyanja", QObject::tr("Chichewa; Nyanja")) );
    //  m_langList.append( new Language("oc", "Occitan (post 1500); Provençal", QObject::tr("Occitan (post 1500); Provençal")) );
    //  m_langList.append( new Language("om", "Oromo", QObject::tr("Oromo")) );
//: Language name obo
    m_langList.append( new Language("obo", "Obo Manobo", QObject::tr("Obo Manobo")) );
//: Language name okv
    m_langList.append( new Language("okv", "Orokaiva", QObject::tr("Orokaiva")) );
//: Language name omw
    m_langList.append( new Language("omw", "South Tairora", QObject::tr("South Tairora")) );
//: Language name ong
    m_langList.append( new Language("ong", "Olo", QObject::tr("Olo")) );
//: Language name ons
    m_langList.append( new Language("ons", "Ono", QObject::tr("Ono")) );
//: Language name ood
    m_langList.append( new Language("ood", "Tohono O'odham", QObject::tr("Tohono O'odham")) );
//: Language name opm
    m_langList.append( new Language("opm", "Oksapmin", QObject::tr("Oksapmin")) );
//: Language name ory
    m_langList.append( new Language("ory", "ଓଡ଼ିଆ", QObject::tr("Oriya (individual language)")) );
//: Language name os
    m_langList.append( new Language("os", "Иронау", QObject::tr("Ossetian")) );
//: Language name ote
    m_langList.append( new Language("ote", "Mezquital Otomi", QObject::tr("Mezquital Otomi")) );
//: Language name otm
    m_langList.append( new Language("otm", "Eastern Highland Otomi", QObject::tr("Eastern Highland Otomi")) );
//: Language name otn
    m_langList.append( new Language("otn", "Tenango Otomi", QObject::tr("Tenango Otomi")) );
//: Language name ots
    m_langList.append( new Language("ots", "Estado de México Otomi", QObject::tr("Estado de México Otomi")) );
    //: Language name otq
    m_langList.append( new Language("otq", "Otomi, Quer\u00e9taro", QObject::tr("Otomi, Quer\u00e9taro")) );
    //  m_langList.append( new Language("pa", "Panjabi", QObject::tr("Panjabi")) );
    //: Language name pap
    m_langList.append( new Language("pap", "Papiamento", QObject::tr("Papiamento")) );
    //  m_langList.append( new Language("pi", "Pali", QObject::tr("Pali")) );
    //: Language name ppk
    m_langList.append( new Language("ppk", "Uma", QObject::tr("Uma")) );
    //: Language name pl
    m_langList.append( new Language("pl", "Polish", QObject::tr("Polish")) );
    //: Language name pot
    m_langList.append( new Language("pot", "Potawatomi", QObject::tr("Potawatomi")) );
    //: Language name ppk
    m_langList.append( new Language("ppk", "Uma", QObject::tr("Uma")) );
    //: Language name prs
    m_langList.append( new Language("prs", "Persian (Dari)", QObject::tr("Persian (Dari)")) );

    //  m_langList.append( new Language("ps", "Pushto", QObject::tr("Pushto")) );
    //: Language name pt
    m_langList.append( new Language("pt", "Portuguese", QObject::tr("Portuguese")) );
    //: Language name pt_BR
    m_langList.append( new Language("pt_BR", "Brazilian Portuguese", QObject::tr("Brazilian Portuguese")) );//added by ourself
    //: Language name pa
    m_langList.append( new Language("pa", "ਪੰਜਾਬੀ", QObject::tr("Panjabi")) );
//: Language name pab
    m_langList.append( new Language("pab", "Parecís", QObject::tr("Parecís")) );
//: Language name pad
    m_langList.append( new Language("pad", "Paumarí", QObject::tr("Paumarí")) );
//: Language name pah
    m_langList.append( new Language("pah", "Tenharim", QObject::tr("Tenharim")) );
//: Language name pao
    m_langList.append( new Language("pao", "Northern Paiute", QObject::tr("Northern Paiute")) );
//: Language name pes
    m_langList.append( new Language("pes", "فارسی", QObject::tr("Western Farsi")) );
//: Language name pib
    m_langList.append( new Language("pib", "Yine", QObject::tr("Yine")) );
//: Language name pio
    m_langList.append( new Language("pio", "Piapoco", QObject::tr("Piapoco")) );
//: Language name pir
    m_langList.append( new Language("pir", "Piratapuyo", QObject::tr("Piratapuyo")) );
//: Language name piu
    m_langList.append( new Language("piu", "Pintupi-Luritja", QObject::tr("Pintupi-Luritja")) );
//: Language name pjt
    m_langList.append( new Language("pjt", "Pitjantjatjara", QObject::tr("Pitjantjatjara")) );
//: Language name pls
    m_langList.append( new Language("pls", "San Marcos Tlalcoyalco Popoloca", QObject::tr("San Marcos Tlalcoyalco Popoloca")) );
//: Language name plu
    m_langList.append( new Language("plu", "Palikúr", QObject::tr("Palikúr")) );
//: Language name pma
    m_langList.append( new Language("pma", "Paama", QObject::tr("Paama")) );
//: Language name poe
    m_langList.append( new Language("poe", "San Juan Atzingo Popoloca", QObject::tr("San Juan Atzingo Popoloca")) );
//: Language name poh
    m_langList.append( new Language("poh", "Poqomchi'", QObject::tr("Poqomchi'")) );
//: Language name poi
    m_langList.append( new Language("poi", "Highland Popoluca", QObject::tr("Highland Popoluca")) );
//: Language name pon
    m_langList.append( new Language("pon", "Pohnpeian", QObject::tr("Pohnpeian")) );
//: Language name poy
    m_langList.append( new Language("poy", "Pogolo", QObject::tr("Pogolo")) );
//: Language name ppo
    m_langList.append( new Language("ppo", "Folopa", QObject::tr("Folopa")) );
//: Language name prf
    m_langList.append( new Language("prf", "Paranan", QObject::tr("Paranan")) );
//: Language name pri
    m_langList.append( new Language("pri", "Paicî", QObject::tr("Paicî")) );
//: Language name ptp
    m_langList.append( new Language("ptp", "Patep", QObject::tr("Patep")) );
//: Language name ptu
    m_langList.append( new Language("ptu", "Bambam", QObject::tr("Bambam")) );
//: Language name pwg
    m_langList.append( new Language("pwg", "Gapapaiwa", QObject::tr("Gapapaiwa")) );
    //  m_langList.append( new Language("qu", "Quechua", QObject::tr("Quechua")) );
    //: Language name qut
    m_langList.append( new Language("qut", "Quich\u00e9, West Central", QObject::tr("Quich\u00e9, West Central")) );
    //: Language name qub
    m_langList.append( new Language("qub", "Huallaga Huánuco Quechua", QObject::tr("Huallaga Huánuco Quechua")) );
//: Language name quc
    m_langList.append( new Language("quc", "K'iche'", QObject::tr("K'iche'")) );
//: Language name quf
    m_langList.append( new Language("quf", "Lambayeque Quechua", QObject::tr("Lambayeque Quechua")) );
//: Language name quh
    m_langList.append( new Language("quh", "South Bolivian Quechua", QObject::tr("South Bolivian Quechua")) );
//: Language name qul
    m_langList.append( new Language("qul", "North Bolivian Quechua", QObject::tr("North Bolivian Quechua")) );
//: Language name qup
    m_langList.append( new Language("qup", "Southern Pastaza Quechua", QObject::tr("Southern Pastaza Quechua")) );
//: Language name qvc
    m_langList.append( new Language("qvc", "Cajamarca Quechua", QObject::tr("Cajamarca Quechua")) );
//: Language name qve
    m_langList.append( new Language("qve", "Eastern Apurímac Quechua", QObject::tr("Eastern Apurímac Quechua")) );
//: Language name qvh
    m_langList.append( new Language("qvh", "Huamalíes-Dos de Mayo Huánuco Quechua", QObject::tr("Huamalíes-Dos de Mayo Huánuco Quechua")) );
//: Language name qvm
    m_langList.append( new Language("qvm", "Margos-Yarowilca-Lauricocha Quechua", QObject::tr("Margos-Yarowilca-Lauricocha Quechua")) );
//: Language name qvn
    m_langList.append( new Language("qvn", "North Junín Quechua", QObject::tr("North Junín Quechua")) );
//: Language name qvs
    m_langList.append( new Language("qvs", "San Martín Quechua", QObject::tr("San Martín Quechua")) );
//: Language name qvw
    m_langList.append( new Language("qvw", "Huaylla Wanca Quechua", QObject::tr("Huaylla Wanca Quechua")) );
//: Language name qvz
    m_langList.append( new Language("qvz", "Northern Pastaza Quichua", QObject::tr("Northern Pastaza Quichua")) );
//: Language name qwh
    m_langList.append( new Language("qwh", "Huaylas Ancash Quechua", QObject::tr("Huaylas Ancash Quechua")) );
//: Language name qxh
    m_langList.append( new Language("qxh", "Panao Huánuco Quechua", QObject::tr("Panao Huánuco Quechua")) );
//: Language name qxn
    m_langList.append( new Language("qxn", "Northern Conchucos Ancash Quechua", QObject::tr("Northern Conchucos Ancash Quechua")) );
//: Language name qxo
    m_langList.append( new Language("qxo", "Southern Conchucos Ancash Quechua", QObject::tr("Southern Conchucos Ancash Quechua")) );
    //  m_langList.append( new Language("rm", "Raeto-Romance", QObject::tr("Raeto-Romance")) );
    //  m_langList.append( new Language("rn", "Rundi", QObject::tr("Rundi")) );
    //: Language name rai
    m_langList.append( new Language("rai", "Ramoaaina", QObject::tr("Ramoaaina")) );
//: Language name rgu
    m_langList.append( new Language("rgu", "Ringgou", QObject::tr("Ringgou")) );
//: Language name rkb
    m_langList.append( new Language("rkb", "Rikbaktsa", QObject::tr("Rikbaktsa")) );
//: Language name rmc
    m_langList.append( new Language("rmc", "Carpathian Romani", QObject::tr("Carpathian Romani")) );
//: Language name rmq
    m_langList.append( new Language("rmq", "Caló", QObject::tr("Caló")) );
//: Language name rmy
    m_langList.append( new Language("rmy", "Vlax Romani", QObject::tr("Vlax Romani")) );
//: Language name roo
    m_langList.append( new Language("roo", "Rotokas", QObject::tr("Rotokas")) );
//: Language name rop
    m_langList.append( new Language("rop", "Kriol", QObject::tr("Kriol")) );
//: Language name row
    m_langList.append( new Language("row", "Dela-Oenale", QObject::tr("Dela-Oenale")) );
//: Language name rro
    m_langList.append( new Language("rro", "Waima", QObject::tr("Waima")) );
//: Language name ruf
    m_langList.append( new Language("ruf", "Luguru", QObject::tr("Luguru")) );
//: Language name rug
    m_langList.append( new Language("rug", "Roviana", QObject::tr("Roviana")) );
//: Language name rwo
    m_langList.append( new Language("rwo", "Rawa", QObject::tr("Rawa")) );
    //: Language name ro
    m_langList.append( new Language("ro", "Romanian", QObject::tr("Romanian")) );
    //: Language name ru
    m_langList.append( new Language("ru", "Russian", QObject::tr("Russian")) );
    //  m_langList.append( new Language("rw", "Kinyarwanda", QObject::tr("Kinyarwanda")) );
    m_langList.append( new Language("sa", "Sanskrit", QObject::tr("Sanskrit")) );
    //  m_langList.append( new Language("sc", "Sardinian", QObject::tr("Sardinian")) );
    //: Language name sco
    m_langList.append( new Language("sco", "Scots", QObject::tr("Scots")) );
    //  m_langList.append( new Language("sd", "Sindhi", QObject::tr("Sindhi")) );
    //  m_langList.append( new Language("se", "Northern Sami", QObject::tr("Northern Sami")) );
    //  m_langList.append( new Language("sg", "Sango", QObject::tr("Sango")) );
    //  m_langList.append( new Language("si", "Sinhalese", QObject::tr("Sinhalese")) );
    //: Language name sk
    m_langList.append( new Language("sk", "Slovak", QObject::tr("Slovak")) );
    //: Language name sl
    m_langList.append( new Language("sl", "Slovenian", QObject::tr("Slovenian")) );
    //  m_langList.append( new Language("sm", "Samoan", QObject::tr("Samoan")) );
    m_langList.append( new Language("sn", "Shona", QObject::tr("Shishona")) );
    //: Language name so
    m_langList.append( new Language("so", "Somali", QObject::tr("Somali")) );
    //: Language name sq
    m_langList.append( new Language("sq", "Albanian", QObject::tr("Albanian")) );
    //  m_langList.append( new Language("sr", "Serbian", QObject::tr("Serbian")) );
    //: Language name srn
    m_langList.append( new Language("srn", "Sranan", QObject::tr("Sranan")) );
    //  m_langList.append( new Language("ss", "Swati", QObject::tr("Swati")) );
    //  m_langList.append( new Language("st", "Sotho, Southern", QObject::tr("Sotho, Southern")) );
    //  m_langList.append( new Language("su", "Sundanese", QObject::tr("Sundanese")) );
    //: Language name sv
    m_langList.append( new Language("sv", "Swedish", QObject::tr("Swedish")) );
    //: Language name sw
    m_langList.append( new Language("sw", "Swahili", QObject::tr("Swahili")) );
    //: Language name syr
    m_langList.append( new Language("syr", "Syriac", QObject::tr("Syriac")) );
    //: Language name sab
    m_langList.append( new Language("sab", "Buglere", QObject::tr("Buglere")) );
//: Language name sah-Cyrl
    m_langList.append( new Language("sah-Cyrl", "Саха тыла", QObject::tr("Yakut (Cyrillic script)")) );
//: Language name sbe
    m_langList.append( new Language("sbe", "Saliba", QObject::tr("Saliba")) );
//: Language name seh
    m_langList.append( new Language("seh", "Sena", QObject::tr("Sena")) );
//: Language name sey
    m_langList.append( new Language("sey", "Secoya", QObject::tr("Secoya")) );
//: Language name sgb
    m_langList.append( new Language("sgb", "Mag-Anchi Ayta", QObject::tr("Mag-Anchi Ayta")) );
//: Language name sgz
    m_langList.append( new Language("sgz", "Sursurunga", QObject::tr("Sursurunga")) );
//: Language name shj
    m_langList.append( new Language("shj", "Shatt", QObject::tr("Shatt")) );
//: Language name shp
    m_langList.append( new Language("shp", "Shipibo-Conibo", QObject::tr("Shipibo-Conibo")) );
//: Language name sim
    m_langList.append( new Language("sim", "Mende (Papua New Guinea)", QObject::tr("Mende (Papua New Guinea)")) );
//: Language name sja
    m_langList.append( new Language("sja", "Epena", QObject::tr("Epena")) );
//: Language name sll
    m_langList.append( new Language("sll", "Salt-Yui", QObject::tr("Salt-Yui")) );
//: Language name smk
    m_langList.append( new Language("smk", "Bolinao", QObject::tr("Bolinao")) );
//: Language name sml
    m_langList.append( new Language("sml", "Central Sama", QObject::tr("Central Sama")) );
//: Language name snc
    m_langList.append( new Language("snc", "Sinaugoro", QObject::tr("Sinaugoro")) );
//: Language name snn
    m_langList.append( new Language("snn", "Siona", QObject::tr("Siona")) );
//: Language name snp
    m_langList.append( new Language("snp", "Siane", QObject::tr("Siane")) );
//: Language name sny
    m_langList.append( new Language("sny", "Saniyo-Hiyewe", QObject::tr("Saniyo-Hiyewe")) );
//: Language name soq
    m_langList.append( new Language("soq", "Kanasi", QObject::tr("Kanasi")) );
//: Language name soy
    m_langList.append( new Language("soy", "Miyobe", QObject::tr("Miyobe")) );
//: Language name spl
    m_langList.append( new Language("spl", "Selepet", QObject::tr("Selepet")) );
//: Language name spm
    m_langList.append( new Language("spm", "Sepen", QObject::tr("Sepen")) );
//: Language name spp
    m_langList.append( new Language("spp", "Supyire Senoufo", QObject::tr("Supyire Senoufo")) );
//: Language name sps
    m_langList.append( new Language("sps", "Saposa", QObject::tr("Saposa")) );
//: Language name spy
    m_langList.append( new Language("spy", "Sabaot", QObject::tr("Sabaot")) );
//: Language name sr
    m_langList.append( new Language("sr", "Srpski", QObject::tr("Srpski")) );
//: Language name sri
    m_langList.append( new Language("sri", "Siriano", QObject::tr("Siriano")) );
//: Language name srm
    m_langList.append( new Language("srm", "Saramaccan", QObject::tr("Saramaccan")) );
//: Language name srq
    m_langList.append( new Language("srq", "Sirionó", QObject::tr("Sirionó")) );
//: Language name ssd
    m_langList.append( new Language("ssd", "Siroi", QObject::tr("Siroi")) );
//: Language name ssg
    m_langList.append( new Language("ssg", "Seimat", QObject::tr("Seimat")) );
//: Language name ssx
    m_langList.append( new Language("ssx", "Samberigi", QObject::tr("Samberigi")) );
//: Language name stp
    m_langList.append( new Language("stp", "Southeastern Tepehuan", QObject::tr("Southeastern Tepehuan")) );
//: Language name sua
    m_langList.append( new Language("sua", "Sulka", QObject::tr("Sulka")) );
//: Language name sue
    m_langList.append( new Language("sue", "Suena", QObject::tr("Suena")) );
//: Language name sus
    m_langList.append( new Language("sus", "Susu", QObject::tr("Susu")) );
//: Language name suz
    m_langList.append( new Language("suz", "Sunwar", QObject::tr("Sunwar")) );
//: Language name swh
    m_langList.append( new Language("swh", "Swahili", QObject::tr("Swahili")) );
//: Language name swp
    m_langList.append( new Language("swp", "Suau", QObject::tr("Suau")) );
//: Language name sxb
    m_langList.append( new Language("sxb", "Suba", QObject::tr("Suba")) );
    //: Language name ta
    m_langList.append( new Language("ta", "Tamil", QObject::tr("Tamil")) );
    //: Language name te
    m_langList.append( new Language("te", "Telugu", QObject::tr("Telugu")) );
    //: Language name tg
    m_langList.append( new Language("tg", "Tajik", QObject::tr("Tajik")) );
    //: Language name th
    m_langList.append( new Language("th", "Thai", QObject::tr("Thai")) );
    //  m_langList.append( new Language("tk", "Turkmen", QObject::tr("Turkmen")) );
    //: Language name tl
    m_langList.append( new Language("tl", "Tagalog", QObject::tr("Tagalog")) );
    //: Language name tlh
    m_langList.append( new Language("tlh", "Klingon", QObject::tr("Klingon")) );
    //: Language name tn
    m_langList.append( new Language("tn", "Tswana", QObject::tr("Tswana")) );
    //: Language name tr
    m_langList.append( new Language("tr", "Turkish", QObject::tr("Turkish")) );
    //  m_langList.append( new Language("ts", "Tsonga", QObject::tr("Tsonga")) );
    //  m_langList.append( new Language("tt", "Tatar", QObject::tr("Tatar")) );
    //: Language name ttc
    m_langList.append( new Language("ttc", "Tektiteko", QObject::tr("Tektiteko")) );
    //  m_langList.append( new Language("tw", "Twi", QObject::tr("Twi")) );
    //: Language name ty
    m_langList.append( new Language("ty", "Tahitian", QObject::tr("Tahitian")) );
    //: Language name tab
    m_langList.append( new Language("tab", "Табасаран чIал", QObject::tr("Tabassaran")) );
//: Language name tac
    m_langList.append( new Language("tac", "Lowland Tarahumara", QObject::tr("Lowland Tarahumara")) );
//: Language name taj
    m_langList.append( new Language("taj", "Eastern Tamang", QObject::tr("Eastern Tamang")) );
//: Language name tav
    m_langList.append( new Language("tav", "Tatuyo", QObject::tr("Tatuyo")) );
//: Language name taw
    m_langList.append( new Language("taw", "Tai", QObject::tr("Tai")) );
//: Language name tbc
    m_langList.append( new Language("tbc", "Takia", QObject::tr("Takia")) );
//: Language name tbg
    m_langList.append( new Language("tbg", "North Tairora", QObject::tr("North Tairora")) );
//: Language name tbl
    m_langList.append( new Language("tbl", "Tboli", QObject::tr("Tboli")) );
//: Language name tbo
    m_langList.append( new Language("tbo", "Tawala", QObject::tr("Tawala")) );
//: Language name tbz
    m_langList.append( new Language("tbz", "Ditammari", QObject::tr("Ditammari")) );
//: Language name tca
    m_langList.append( new Language("tca", "Ticuna", QObject::tr("Ticuna")) );
//: Language name tcs
    m_langList.append( new Language("tcs", "Torres Strait Creole", QObject::tr("Torres Strait Creole")) );
//: Language name tee
    m_langList.append( new Language("tee", "Huehuetla Tepehua", QObject::tr("Huehuetla Tepehua")) );
//: Language name ter
    m_langList.append( new Language("ter", "Tereno", QObject::tr("Tereno")) );
//: Language name tet
    m_langList.append( new Language("tet", "Tetum", QObject::tr("Tetum")) );
//: Language name tew
    m_langList.append( new Language("tew", "Tewa (USA)", QObject::tr("Tewa (USA)")) );
//: Language name tfr
    m_langList.append( new Language("tfr", "Teribe", QObject::tr("Teribe")) );
//: Language name tgo
    m_langList.append( new Language("tgo", "Sudest", QObject::tr("Sudest")) );
//: Language name tgp
    m_langList.append( new Language("tgp", "Tangoa", QObject::tr("Tangoa")) );
//: Language name thd
    m_langList.append( new Language("thd", "Thayore", QObject::tr("Thayore")) );
//: Language name tif
    m_langList.append( new Language("tif", "Tifal", QObject::tr("Tifal")) );
//: Language name tim
    m_langList.append( new Language("tim", "Timbe", QObject::tr("Timbe")) );
//: Language name tiy
    m_langList.append( new Language("tiy", "Tiruray", QObject::tr("Tiruray")) );
//: Language name tk-Cyrl
    m_langList.append( new Language("tk-Cyrl", "Түркменче", QObject::tr("Turkmen (Cyrillic script)")) );
//: Language name tke
    m_langList.append( new Language("tke", "", QObject::tr("")) );
//: Language name tk-Latn
    m_langList.append( new Language("tk-Latn", "Türkmençe", QObject::tr("Turkmen (Latin script)")) );
//: Language name tkr-Cyrl
    m_langList.append( new Language("tkr-Cyrl", "ЦIаьхна миз", QObject::tr("Tsakhur (Cyrillic script)")) );
//: Language name tku
    m_langList.append( new Language("tku", "Upper Necaxa Totonac", QObject::tr("Upper Necaxa Totonac")) );
//: Language name tlf
    m_langList.append( new Language("tlf", "Telefol", QObject::tr("Telefol")) );
//: Language name tna
    m_langList.append( new Language("tna", "Tacana", QObject::tr("Tacana")) );
//: Language name tnc
    m_langList.append( new Language("tnc", "Tanimuca-Retuarã", QObject::tr("Tanimuca-Retuarã")) );
//: Language name tnk
    m_langList.append( new Language("tnk", "Kwamera", QObject::tr("Kwamera")) );
//: Language name tnn
    m_langList.append( new Language("tnn", "North Tanna", QObject::tr("North Tanna")) );
//: Language name tnp
    m_langList.append( new Language("tnp", "Whitesands", QObject::tr("Whitesands")) );
//: Language name to
    m_langList.append( new Language("to", "Tonga (Tonga Islands)", QObject::tr("Tonga (Tonga Islands)")) );
//: Language name toc
    m_langList.append( new Language("toc", "Coyutla Totonac", QObject::tr("Coyutla Totonac")) );
//: Language name tod
    m_langList.append( new Language("tod", "Toma", QObject::tr("Toma")) );
//: Language name toj
    m_langList.append( new Language("toj", "Tojolabal", QObject::tr("Tojolabal")) );
//: Language name too
    m_langList.append( new Language("too", "Xicotepec De Juárez Totonac", QObject::tr("Xicotepec De Juárez Totonac")) );
//: Language name top
    m_langList.append( new Language("top", "Papantla Totonac", QObject::tr("Papantla Totonac")) );
//: Language name tos
    m_langList.append( new Language("tos", "Highland Totonac", QObject::tr("Highland Totonac")) );
//: Language name tpa
    m_langList.append( new Language("tpa", "Taupota", QObject::tr("Taupota")) );
//: Language name tpi
    m_langList.append( new Language("tpi", "Tok Pisin", QObject::tr("Tok Pisin")) );
//: Language name tpt
    m_langList.append( new Language("tpt", "Tlachichilco Tepehua", QObject::tr("Tlachichilco Tepehua")) );
//: Language name tpz
    m_langList.append( new Language("tpz", "Tinputz", QObject::tr("Tinputz")) );
//: Language name trc
    m_langList.append( new Language("trc", "Copala Triqui", QObject::tr("Copala Triqui")) );
//: Language name tsg
    m_langList.append( new Language("tsg", "Tausug", QObject::tr("Tausug")) );
//: Language name tsw
    m_langList.append( new Language("tsw", "Tsishingini", QObject::tr("Tsishingini")) );
//: Language name tt-Cyrl
    m_langList.append( new Language("tt-Cyrl", "Татарча", QObject::tr("Tatar (Cyrillic script)")) );
//: Language name tte
    m_langList.append( new Language("tte", "Bwanabwana", QObject::tr("Bwanabwana")) );
//: Language name tuc
    m_langList.append( new Language("tuc", "Mutu", QObject::tr("Mutu")) );
//: Language name tue
    m_langList.append( new Language("tue", "Tuyuca", QObject::tr("Tuyuca")) );
//: Language name tuf
    m_langList.append( new Language("tuf", "Central Tunebo", QObject::tr("Central Tunebo")) );
//: Language name tuo
    m_langList.append( new Language("tuo", "Tucano", QObject::tr("Tucano")) );
//: Language name tvk
    m_langList.append( new Language("tvk", "Southeast Ambrym", QObject::tr("Southeast Ambrym")) );
//: Language name txq
    m_langList.append( new Language("txq", "Tii", QObject::tr("Tii")) );
//: Language name txu
    m_langList.append( new Language("txu", "Kayapó", QObject::tr("Kayapó")) );
//: Language name tyv
    m_langList.append( new Language("tyv", "Тыва дыл", QObject::tr("Tuvinian")) );
//: Language name tzj
    m_langList.append( new Language("tzj", "Tz'utujil", QObject::tr("Tz'utujil")) );
//: Language name tzo
    m_langList.append( new Language("tzo", "Tzotzil", QObject::tr("Tzotzil")) );
    //: Language name tzz
    m_langList.append( new Language("tzz", "Tzotzil, Zinacant\u00e1n", QObject::tr("Tzotzil, Zinacant\u00e1n")) );
    //  m_langList.append( new Language("ug", "Uighur", QObject::tr("Uighur")) );
    //: Language name uk
    m_langList.append( new Language("uk", "Ukrainian", QObject::tr("Ukrainian")) );
    //  m_langList.append( new Language("ur", "Urdu", QObject::tr("Urdu")) );
    //: Language name ury
    m_langList.append( new Language("ury", "Orya", QObject::tr("Orya")) );
    //: Language name usp
    m_langList.append( new Language("usp", "Uspanteco", QObject::tr("Uspanteco")) );
//: Language name ubr
    m_langList.append( new Language("ubr", "Ubir", QObject::tr("Ubir")) );
//: Language name ubu
    m_langList.append( new Language("ubu", "Umbu-Ungu", QObject::tr("Umbu-Ungu")) );
//: Language name udu
    m_langList.append( new Language("udu", "Uduk", QObject::tr("Uduk")) );
//: Language name ug
    m_langList.append( new Language("ug", "Uyghurche‎", QObject::tr("Uyghurche‎")) );
//: Language name uli
    m_langList.append( new Language("uli", "Ulithian", QObject::tr("Ulithian")) );
//: Language name ulk
    m_langList.append( new Language("ulk", "Meriam", QObject::tr("Meriam")) );
//: Language name upv
    m_langList.append( new Language("upv", "Uripiv-Wala-Rano-Atchin", QObject::tr("Uripiv-Wala-Rano-Atchin")) );
//: Language name ur
    m_langList.append( new Language("ur", "Urdu", QObject::tr("Urdu")) );
//: Language name ura
    m_langList.append( new Language("ura", "", QObject::tr("")) );
//: Language name urb
    m_langList.append( new Language("urb", "", QObject::tr("")) );
//: Language name ur-Deva
    m_langList.append( new Language("ur-Deva", "उर्दू", QObject::tr("Urdu (Deva script)")) );
//: Language name urt
    m_langList.append( new Language("urt", "Urat", QObject::tr("Urat")) );
//: Language name urw
    m_langList.append( new Language("urw", "Sop", QObject::tr("Sop")) );
//: Language name usa
    m_langList.append( new Language("usa", "Usarufa", QObject::tr("Usarufa")) );
//: Language name uvl
    m_langList.append( new Language("uvl", "Lote", QObject::tr("Lote")) );
//: Language name uz-Cyrl
    m_langList.append( new Language("uz-Cyrl", "Ўзбек", QObject::tr("Uzbek (Cyrillic script)")) );
//: Language name uz-Latn
    m_langList.append( new Language("uz-Latn", "O'zbek", QObject::tr("Uzbek (Latin script)")) );
    //: Language name vi
    m_langList.append( new Language("vi", "Vietnamese", QObject::tr("Vietnamese")) );
    //  m_langList.append( new Language("vo", "Volapük", QObject::tr("Volapük")) );
    //  m_langList.append( new Language("wo", "Wolof", QObject::tr("Wolof")) );
    //: Language name vid
    m_langList.append( new Language("vid", "Vidunda", QObject::tr("Vidunda")) );
//: Language name viv
    m_langList.append( new Language("viv", "Iduna", QObject::tr("Iduna")) );
//: Language name vls
    m_langList.append( new Language("vls", "Vlaams", QObject::tr("Vlaams")) );
//: Language name vmy
    m_langList.append( new Language("vmy", "Miwa", QObject::tr("Miwa")) );
    //: Language name waj
    m_langList.append( new Language("waj", "Waffa", QObject::tr("Waffa")) );
//: Language name wal
    m_langList.append( new Language("wal", "Wolaytta", QObject::tr("Wolaytta")) );
//: Language name wap
    m_langList.append( new Language("wap", "Wapishana", QObject::tr("Wapishana")) );
//: Language name wat
    m_langList.append( new Language("wat", "Kaninuwa", QObject::tr("Kaninuwa")) );
//: Language name wbp
    m_langList.append( new Language("wbp", "Warlpiri", QObject::tr("Warlpiri")) );
//: Language name wed
    m_langList.append( new Language("wed", "Wedau", QObject::tr("Wedau")) );
//: Language name wer
    m_langList.append( new Language("wer", "Weri", QObject::tr("Weri")) );
//: Language name wim
    m_langList.append( new Language("wim", "Wik-Mungkan", QObject::tr("Wik-Mungkan")) );
//: Language name wiu
    m_langList.append( new Language("wiu", "Wiru", QObject::tr("Wiru")) );
//: Language name wmt
    m_langList.append( new Language("wmt", "Walmajarri", QObject::tr("Walmajarri")) );
//: Language name wmw
    m_langList.append( new Language("wmw", "Mwani", QObject::tr("Mwani")) );
//: Language name wnc
    m_langList.append( new Language("wnc", "Wantoat", QObject::tr("Wantoat")) );
//: Language name wnu
    m_langList.append( new Language("wnu", "Usan", QObject::tr("Usan")) );
//: Language name wos
    m_langList.append( new Language("wos", "Hanga Hundi", QObject::tr("Hanga Hundi")) );
//: Language name wrk
    m_langList.append( new Language("wrk", "Garrwa", QObject::tr("Garrwa")) );
//: Language name wro
    m_langList.append( new Language("wro", "Worrorra", QObject::tr("Worrorra")) );
//: Language name wrs
    m_langList.append( new Language("wrs", "Waris", QObject::tr("Waris")) );
//: Language name wsk
    m_langList.append( new Language("wsk", "Waskia", QObject::tr("Waskia")) );
//: Language name wuv
    m_langList.append( new Language("wuv", "Wuvulu-Aua", QObject::tr("Wuvulu-Aua")) );
    //: Language name xh
    m_langList.append( new Language("xh", "Xhosa", QObject::tr("Xhosa")) );
    //: Language name xtd
    m_langList.append( new Language("xtd", "Mixtec, Diuxi-Tilantongo", QObject::tr("Mixtec, Diuxi-Tilantongo")) );
    //: Language name xal
    m_langList.append( new Language("xal", "Хальмг келн", QObject::tr("Kalmyk")) );
        //: Language name xal-Cyrl
    m_langList.append( new Language("xal-Cyrl", "Хальмг келн", QObject::tr("Kalmyk (Cyrillic script)")) );
//: Language name xav
    m_langList.append( new Language("xav", "Xavánte", QObject::tr("Xavánte")) );
//: Language name xbi
    m_langList.append( new Language("xbi", "Kombio", QObject::tr("Kombio")) );
//: Language name xed
    m_langList.append( new Language("xed", "Hdi", QObject::tr("Hdi")) );
//: Language name xla
    m_langList.append( new Language("xla", "Kamula", QObject::tr("Kamula")) );
//: Language name xnn
    m_langList.append( new Language("xnn", "Northern Kankanay", QObject::tr("Northern Kankanay")) );
//: Language name xon
    m_langList.append( new Language("xon", "Konkomba", QObject::tr("Konkomba")) );
//: Language name xsi
    m_langList.append( new Language("xsi", "Sio", QObject::tr("Sio")) );
//: Language name xtm
    m_langList.append( new Language("xtm", "Magdalena Peñasco Mixtec", QObject::tr("Magdalena Peñasco Mixtec")) );
    //: Language name yi
    m_langList.append( new Language("yi", "Yiddish", QObject::tr("Yiddish")) );
    //: Language name yo
    m_langList.append( new Language("yo", "èdèe Yorùbá", QObject::tr("Yoruba")) );
    //: Language name yaa
    m_langList.append( new Language("yaa", "Yaminahua", QObject::tr("Yaminahua")) );
//: Language name yad
    m_langList.append( new Language("yad", "Yagua", QObject::tr("Yagua")) );
    //: Language name yaf
    m_langList.append( new Language("yaf", "Kiyaka", QObject::tr("Yaka (Democratic Republic of Congo)")) );
//: Language name yal
    m_langList.append( new Language("yal", "Yalunka", QObject::tr("Yalunka")) );
//: Language name yap
    m_langList.append( new Language("yap", "Yapese", QObject::tr("Yapese")) );
//: Language name yaq
    m_langList.append( new Language("yaq", "Yaqui", QObject::tr("Yaqui")) );
//: Language name yby
    m_langList.append( new Language("yby", "Yaweyuha", QObject::tr("Yaweyuha")) );
//: Language name ycn
    m_langList.append( new Language("ycn", "Yucuna", QObject::tr("Yucuna")) );
//: Language name yij
    m_langList.append( new Language("yij", "Yindjibarndi", QObject::tr("Yindjibarndi")) );
//: Language name yka
    m_langList.append( new Language("yka", "Yakan", QObject::tr("Yakan")) );
//: Language name yle
    m_langList.append( new Language("yle", "Yele", QObject::tr("Yele")) );
//: Language name yml
    m_langList.append( new Language("yml", "Iamalele", QObject::tr("Iamalele")) );
//: Language name yre
    m_langList.append( new Language("yre", "Yaouré", QObject::tr("Yaouré")) );
//: Language name yrk-Cyrl
    m_langList.append( new Language("yrk-Cyrl", "Ненэцяʼ вада", QObject::tr("Nenets (Cyrillic script)")) );
//: Language name yss
    m_langList.append( new Language("yss", "Yessan-Mayo", QObject::tr("Yessan-Mayo")) );
//: Language name yuj
    m_langList.append( new Language("yuj", "Karkar-Yuri", QObject::tr("Karkar-Yuri")) );
//: Language name yut
    m_langList.append( new Language("yut", "Yopno", QObject::tr("Yopno")) );
//: Language name yuw
    m_langList.append( new Language("yuw", "Yau (Morobe Province)", QObject::tr("Yau (Morobe Province)")) );
//: Language name yva
    m_langList.append( new Language("yva", "Yawa", QObject::tr("Yawa")) );
    //  m_langList.append( new Language("za", "Zhuang", QObject::tr("Zhuang")) );
    //: Language name zab
    m_langList.append( new Language("zab", "Zapotec, San Juan Guelav\u00eda", QObject::tr("Zapotec, San Juan Guelav\u00eda")) );
    //: Language name zaw
    m_langList.append( new Language("zaw", "Zapotec, Mitla", QObject::tr("Zapotec, Mitla")) );
    //: Language name zh
    m_langList.append( new Language("zh", "Chinese", QObject::tr("Chinese")) );
    //: Language name zpo
    m_langList.append( new Language("zpo", "Zapotec, Amatl\u00e1n", QObject::tr("Zapotec, Amatl\u00e1n")) );
    //: Language name zpq
    m_langList.append( new Language("zpq", "Zapotec, Zoogocho", QObject::tr("Zapotec, Zoogocho")) );
    //: Language name zpu
    m_langList.append( new Language("zpu", "Zapotec, Yal\u00e1lag", QObject::tr("Zapotec, Yal\u00e1lag")) );
    //: Language name zpv
    m_langList.append( new Language("zpv", "Zapotec, Chichicapan", QObject::tr("Zapotec, Chichicapan")) );
    //: Language name zsr
    m_langList.append( new Language("zsr", "Zapotec, Southern Rincon", QObject::tr("Zapotec, Southern Rincon")) );
    //: Language name ztq
    m_langList.append( new Language("ztq", "Zapotec, Quioquitani-Quier\u00ed", QObject::tr("Zapotec, Quioquitani-Quier\u00ed")) );
    //: Language name zty
    m_langList.append( new Language("zty", "Zapotec, Yatee", QObject::tr("Zapotec, Yatee")) );
    //: Language name zu
    m_langList.append( new Language("zu", "Zulu", QObject::tr("Zulu")) );
    


//: Language name zaa
    m_langList.append( new Language("zaa", "Sierra de Juárez Zapotec", QObject::tr("Sierra de Juárez Zapotec")) );
//: Language name zac
    m_langList.append( new Language("zac", "Ocotlán Zapotec", QObject::tr("Ocotlán Zapotec")) );
//: Language name zad
    m_langList.append( new Language("zad", "Cajonos Zapotec", QObject::tr("Cajonos Zapotec")) );
//: Language name zai
    m_langList.append( new Language("zai", "Isthmus Zapotec", QObject::tr("Isthmus Zapotec")) );
//: Language name zaj
    m_langList.append( new Language("zaj", "Zaramo", QObject::tr("Zaramo")) );
//: Language name zam
    m_langList.append( new Language("zam", "Miahuatlán Zapotec", QObject::tr("Miahuatlán Zapotec")) );
//: Language name zao
    m_langList.append( new Language("zao", "Ozolotepec Zapotec", QObject::tr("Ozolotepec Zapotec")) );
//: Language name zar
    m_langList.append( new Language("zar", "Rincón Zapotec", QObject::tr("Rincón Zapotec")) );
//: Language name zas
    m_langList.append( new Language("zas", "Santo Domingo Albarradas Zapotec", QObject::tr("Santo Domingo Albarradas Zapotec")) );
//: Language name zat
    m_langList.append( new Language("zat", "Tabaa Zapotec", QObject::tr("Tabaa Zapotec")) );
//: Language name zav
    m_langList.append( new Language("zav", "Yatzachi Zapotec", QObject::tr("Yatzachi Zapotec")) );
//: Language name zca
    m_langList.append( new Language("zca", "Coatecas Altas Zapotec", QObject::tr("Coatecas Altas Zapotec")) );
//: Language name zia
    m_langList.append( new Language("zia", "Zia", QObject::tr("Zia")) );
//: Language name ziw
    m_langList.append( new Language("ziw", "Zigula", QObject::tr("Zigula")) );
//: Language name zlm
    m_langList.append( new Language("zlm", "Malay (individual language)", QObject::tr("Malay (individual language)")) );
//: Language name zos
    m_langList.append( new Language("zos", "Francisco León Zoque", QObject::tr("Francisco León Zoque")) );
//: Language name zpc
    m_langList.append( new Language("zpc", "Choapan Zapotec", QObject::tr("Choapan Zapotec")) );
//: Language name zpi
    m_langList.append( new Language("zpi", "Santa María Quiegolani Zapotec", QObject::tr("Santa María Quiegolani Zapotec")) );
//: Language name zpl
    m_langList.append( new Language("zpl", "Lachixío Zapotec", QObject::tr("Lachixío Zapotec")) );
//: Language name zpm
    m_langList.append( new Language("zpm", "Mixtepec Zapotec", QObject::tr("Mixtepec Zapotec")) );
//: Language name zpz
    m_langList.append( new Language("zpz", "Texmelucan Zapotec", QObject::tr("Texmelucan Zapotec")) );
//: Language name zyp
    m_langList.append( new Language("zyp", "Zyphe", QObject::tr("Zyphe")) );

    Q_FOREACH(Language * const lang, m_langList)
        m_langMap.insert( lang->abbrev(), lang);
}
