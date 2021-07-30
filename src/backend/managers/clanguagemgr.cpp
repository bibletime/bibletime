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

#include <QLocale>
#include <utility>
#include "../drivers/cswordmoduleinfo.h"
#include "cswordbackend.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <localemgr.h>
#include <swlocale.h>
#pragma GCC diagnostic pop


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

CLanguageMgr::CLanguageMgr() {
    m_availableModulesCache.moduleCount = 0;
    init();
}

CLanguageMgr::~CLanguageMgr() {
    qDeleteAll(m_abbrLangMap);
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
        for (auto const & abbrev : abbrevs)
            m_availableModulesCache.availableLanguages.insert(
                        abbrev,
                        languageForAbbrev(abbrev));
    }
    return m_availableModulesCache.availableLanguages;
}

const CLanguageMgr::Language* CLanguageMgr::languageForAbbrev( const QString& abbrev ) const {
    {
        auto const it = m_langMap.find(abbrev);
        if (it != m_langMap.constEnd())
            return *it; //Language is already here
    }

    //try to search in the alternative abbrevs
    Q_FOREACH(const Language * const lang, m_langList)
        if (lang->alternativeAbbrevs().contains(abbrev))
            return lang;
    Q_ASSERT(abbrev != "en");

    auto const it = m_abbrLangMap.find(abbrev);
    if (it != m_abbrLangMap.end())
        return it.value();

    Language * newLang;
    if (auto * const locale =
                sword::LocaleMgr::getSystemLocaleMgr()->getLocale("locales"))
    {
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

        // Attempt to retrieve translated name:
        QString trName = englishName;
        {
            QStringList tryTranslateNames;
            {
                auto localeName = QLocale().name();
                while (!localeName.isEmpty() && localeName != "en") {
                    tryTranslateNames.append(abbrev + '.' + localeName);
                    if (localeName == abbrev)
                        tryTranslateNames.append(localeName);
                    while (localeName.operator[](localeName.size() - 1).isLetterOrNumber()) {
                        localeName.chop(1);
                        if (localeName.isEmpty())
                            break;
                    }
                    while (!localeName.isEmpty()
                           && !localeName.operator[](localeName.size() - 1).isLetterOrNumber())
                        localeName.chop(1);
                }
            }
            for (auto const & translateName : tryTranslateNames) {
                auto newTrName(
                            QString::fromUtf8(
                                locale->translate(
                                    translateName.toUtf8().constData())));
                if (newTrName != translateName) {
                    trName = std::move(newTrName);
                    break;
                }
            }
        }

        newLang = new Language(abbrev, englishName, trName);
    } else {
        newLang = new Language(abbrev, abbrev, abbrev);
    }
    m_abbrLangMap.insert(abbrev, newLang);

    return newLang;
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
    m_langList.append( new Language("af", "Afrikaans", QObject::tr("Afrikaans")) );
    //  m_langList.append( new Language("am", "Amharic", QObject::tr("Amharic")) );
    m_langList.append( new Language("amu", "Amuzgo, Guerrero", QObject::tr("Amuzgo, Guerrero")) );
    m_langList.append( new Language("ang", "English, Old (ca.450-1100)", QObject::tr("English, Old (ca.450-1100)")) );
    m_langList.append( new Language("ar", "Arabic", QObject::tr("Arabic")) );
    //  m_langList.append( new Language("as", "Assamese", QObject::tr("Assamese")) );
    m_langList.append( new Language("az", "Azerbaijani", QObject::tr("Azerbaijani")) );
    m_langList.append( new Language("azb", "Azerbaijani, South", QObject::tr("Azerbaijani, South")) );
    //  m_langList.append( new Language("ba", "Bashkir", QObject::tr("Bashkir")) );
    m_langList.append( new Language("bar", "Bavarian", QObject::tr("Bavarian")) );
    m_langList.append( new Language("be", "Belarusian", QObject::tr("Belarusian")) );
    m_langList.append( new Language("bg", "Bulgarian", QObject::tr("Bulgarian")) );
    //  m_langList.append( new Language("bh", "Bihari", QObject::tr("Bihari")) );
    //  m_langList.append( new Language("bi", "Bislama", QObject::tr("Bislama")) );
    //  m_langList.append( new Language("bn", "Bengali", QObject::tr("Bengali")) );
    //  m_langList.append( new Language("bo", "Tibetan", QObject::tr("Tibetan")) );
    m_langList.append( new Language("br", "Breton", QObject::tr("Breton")) );
    m_langList.append( new Language("bs", "Bosnian", QObject::tr("Bosnian")) );
    m_langList.append( new Language("ca", "Catalan", QObject::tr("Catalan")) );
    //  m_langList.append( new Language("ce", "Chechen", QObject::tr("Chechen")) );
    m_langList.append( new Language("cco", "Chinantec, Comaltepec", QObject::tr("Chinantec, Comaltepec")) );
    m_langList.append( new Language("ceb", "Cebuano", QObject::tr("Cebuano")) );
    m_langList.append( new Language("ch", "Chamorro", QObject::tr("Chamorro")) );
    m_langList.append( new Language("chd", "Chontal, Highland Oaxaca", QObject::tr("Chontal, Highland Oaxaca")) );
    m_langList.append( new Language("chq", "Chinantec, Quiotepec", QObject::tr("Chinantec, Quiotepec")) );
    m_langList.append( new Language("chz", "Chinantec, Ozumac\u00edn", QObject::tr("Chinantec, Ozumac\u00edn")) );
    //  m_langList.append( new Language("co", "Corsican", QObject::tr("Corsican")) );
    m_langList.append( new Language("ckw", "Cakchiquel, Western", QObject::tr("Cakchiquel, Western")) );
    m_langList.append( new Language("cnl", "Chinantec, Lalana", QObject::tr("Chinantec, Lalana")) );
    m_langList.append( new Language("cnt", "Chinantec, Tepetotutla", QObject::tr("Chinantec, Tepetotutla")) );
    m_langList.append( new Language("cop", "Coptic", QObject::tr("Coptic")) );
    m_langList.append( new Language("cs", "Czech", QObject::tr("Czech")) );
    m_langList.append( new Language("cso", "Chinantec, Sochiapan", QObject::tr("Chinantec, Sochiapan")) );
    m_langList.append( new Language("cti", "Chol, Tila", QObject::tr("Chol, Tila")) );
    m_langList.append( new Language("ctp", "Chatino, Western Highland", QObject::tr("Chatino, Western Highland")) );
    m_langList.append( new Language("cu", "Church Slavic", QObject::tr("Church Slavic")) );
    //  m_langList.append( new Language("cv", "Chuvash", QObject::tr("Chuvash")) );
    m_langList.append( new Language("cy", "Welsh", QObject::tr("Welsh")) );
    m_langList.append( new Language("da", "Danish", QObject::tr("Danish")) );
    m_langList.append( new Language("de", "German", QObject::tr("German")) );
    m_langList.append( new Language("dug", "Duruma", QObject::tr("Duruma")) );
    //  m_langList.append( new Language("dz", "Dzongkha", QObject::tr("Dzongkha")) );
    m_langList.append( new Language("el", "Greek, Modern (1453-)", QObject::tr("Greek, Modern (1453-)"), makeStringList("gre;ell")) );
    m_langList.append( new Language("en", "English", QObject::tr("English")) );
    m_langList.append( new Language("en_US", "American English", QObject::tr("American English")) );
    m_langList.append( new Language("enm", "English, Middle (1100-1500)", QObject::tr("English, Middle (1100-1500)")) );
    m_langList.append( new Language("eo", "Esperanto", QObject::tr("Esperanto")) );
    m_langList.append( new Language("es", "Spanish", QObject::tr("Spanish")) );
    m_langList.append( new Language("et", "Estonian", QObject::tr("Estonian")) );
    m_langList.append( new Language("eu", "Basque", QObject::tr("Basque")) );
    m_langList.append( new Language("fa", "Persian", QObject::tr("Persian")) );
    m_langList.append( new Language("fi", "Finnish", QObject::tr("Finnish")) );
    //  m_langList.append( new Language("fj", "Fijian", QObject::tr("Fijian")) );
    //  m_langList.append( new Language("fo", "Faroese", QObject::tr("Faroese")) );
    m_langList.append( new Language("fr", "French", QObject::tr("French")) );
    m_langList.append( new Language("fy", "Frisian", QObject::tr("Frisian")) );
    m_langList.append( new Language("ga", "Irish", QObject::tr("Irish")) );
    m_langList.append( new Language("gd", "Gaelic (Scots)", QObject::tr("Gaelic (Scots)")) );
    m_langList.append( new Language("gez", "Geez", QObject::tr("Geez")) );
    //  m_langList.append( new Language("gl", "Gallegan", QObject::tr("Gallegan")) );
    //  m_langList.append( new Language("gn", "Guarani", QObject::tr("Guarani")) );
    //  m_langList.append( new Language("gn", "Gujarati", QObject::tr("Gujarati")) );
    m_langList.append( new Language("got", "Gothic", QObject::tr("Gothic")) );
    m_langList.append( new Language("gv", "Manx", QObject::tr("Manx")) );
    m_langList.append( new Language("grc", "Greek, Ancient (to 1453)", QObject::tr("Greek, Ancient (to 1453)")) );
    m_langList.append( new Language("hau", "Hausa", QObject::tr("Hausa")) );
    m_langList.append( new Language("haw", "Hawaiian", QObject::tr("Hawaiian")) );
    m_langList.append( new Language("hbo", "Hebrew, Ancient", QObject::tr("Hebrew, Ancient")) );
    m_langList.append( new Language("he", "Hebrew", QObject::tr("Hebrew")) );
    m_langList.append( new Language("hi", "Hindi", QObject::tr("Hindi")) );
    //  m_langList.append( new Language("ho", "Hiri Motu", QObject::tr("Hiri Motu")) );
    m_langList.append( new Language("hr", "Croatian", QObject::tr("Croatian")) );
    m_langList.append( new Language("ht", "Haitian Creole", QObject::tr("Haitian Creole")) );
    m_langList.append( new Language("hu", "Hungarian", QObject::tr("Hungarian")) );
    m_langList.append( new Language("huv", "Huave, San Mateo Del Mar", QObject::tr("Huave, San Mateo Del Mar")) );
    m_langList.append( new Language("hy", "Armenian", QObject::tr("Armenian")) );
    //  m_langList.append( new Language("hz", "Herero", QObject::tr("Herero")) );
    //  m_langList.append( new Language("ia", "Interlingua", QObject::tr("Interlingua")) );
    m_langList.append( new Language("id", "Indonesian", QObject::tr("Indonesian")) );
    //  m_langList.append( new Language("ie", "Interlingue", QObject::tr("Interlingue")) );
    //  m_langList.append( new Language("ik", "Inupiaq", QObject::tr("Inupiaq")) );
    m_langList.append( new Language("is", "Icelandic", QObject::tr("Icelandic")) );
    m_langList.append( new Language("it", "Italian", QObject::tr("Italian")) );
    m_langList.append( new Language("itz", "Itz\u00e1", QObject::tr("Itz\u00e1")) );
    m_langList.append( new Language("ixl", "Ixil, San Juan Cotzal", QObject::tr("Ixil, San Juan Cotzal")) );
    //  m_langList.append( new Language("iu", "Inuktitut", QObject::tr("Inuktitut")) );
    m_langList.append( new Language("ja", "Japanese", QObject::tr("Japanese")) );
    m_langList.append( new Language("jac", "Jacalteco, Eastern", QObject::tr("Jacalteco, Eastern")) );
    m_langList.append( new Language("jvn", "Javanese, Caribbean", QObject::tr("Javanese, Caribbean")) );
    m_langList.append( new Language("ka", "Georgian", QObject::tr("Georgian")) );
    m_langList.append( new Language("kek", "Kekch\u00ed", QObject::tr("Kekch\u00ed", "kek")) );
    //  m_langList.append( new Language("ki", "Kikuyu", QObject::tr("Kikuyu")) );
    //  m_langList.append( new Language("kj", "Kuanyama", QObject::tr("Kuanyama")) );
    //  m_langList.append( new Language("kk", "Kazakh", QObject::tr("Kazakh")) );
    //  m_langList.append( new Language("kl", "Kalaallisut", QObject::tr("Kalaallisut")) );
    //  m_langList.append( new Language("km", "Khmer", QObject::tr("Khmer")) );
    //  m_langList.append( new Language("kn", "Kannada", QObject::tr("Kannada")) );
    m_langList.append( new Language("ko", "Korean", QObject::tr("Korean")) );
    //  m_langList.append( new Language("ks", "Kashmiri", QObject::tr("Kashmiri")) );
    m_langList.append( new Language("ku", "Kurdish", QObject::tr("Kurdish")) );
    //  m_langList.append( new Language("kv", "Komi", QObject::tr("Komi")) );
    //  m_langList.append( new Language("kw", "Cornish", QObject::tr("Cornish")) );
    m_langList.append( new Language("ky", "Kirghiz", QObject::tr("Kirghiz")) );
    m_langList.append( new Language("la", "Latin", QObject::tr("Latin")) );
    m_langList.append( new Language("lac", "Lacandon", QObject::tr("Lacandon")) );
    //  m_langList.append( new Language("lb", "Letzeburgesch", QObject::tr("Letzeburgesch")) );
    m_langList.append( new Language("lmo", "Lombard", QObject::tr("Lombard")) );
    //  m_langList.append( new Language("ln", "Lingala", QObject::tr("Lingala")) );
    //  m_langList.append( new Language("lo", "Lao", QObject::tr("Lao")) );
    m_langList.append( new Language("lt", "Lithuanian", QObject::tr("Lithuanian")) );
    m_langList.append( new Language("lv", "Latvian", QObject::tr("Latvian")) );
    m_langList.append( new Language("mg", "Malagasy", QObject::tr("Malagasy")) );
    //  m_langList.append( new Language("mh", "Marshall", QObject::tr("Marshall")) );
    m_langList.append( new Language("mi", "Maori", QObject::tr("Maori")) );
    m_langList.append( new Language("mir", "Mixe, Isthmus", QObject::tr("Mixe, Isthmus")) );
    m_langList.append( new Language("miz", "Mixtec, Coatzospan", QObject::tr("Mixtec, Coatzospan")) );
    m_langList.append( new Language("mk", "Macedonian", QObject::tr("Macedonian")) );
    m_langList.append( new Language("mks", "Mixtec, Silacayoapan", QObject::tr("Mixtec, Silacayoapan")) );
    //  m_langList.append( new Language("ml", "Malayalam", QObject::tr("Malayalam")) );
    //  m_langList.append( new Language("mn", "Mongolian", QObject::tr("Mongolian")) );
    //  m_langList.append( new Language("mo", "Moldavian", QObject::tr("Moldavian")) );
    m_langList.append( new Language("mos", "More", QObject::tr("More")) );
    //  m_langList.append( new Language("mr", "Marathi", QObject::tr("Marathi")) );
    m_langList.append( new Language("ms", "Malay", QObject::tr("Malay")) );
    m_langList.append( new Language("mt", "Maltese", QObject::tr("Maltese")) );
    m_langList.append( new Language("mul", "(Multiple languages)", QObject::tr("(Multiple languages)")) );
    m_langList.append( new Language("mvc", "Mam, Central", QObject::tr("Mam, Central")) );
    m_langList.append( new Language("mvj", "Mam, Todos Santos Cuchumat\u00e1n", QObject::tr("Mam, Todos Santos Cuchumat\u00e1n")) );
    m_langList.append( new Language("mxq", "Mixe, Juquila", QObject::tr("Mixe, Juquila")) );
    m_langList.append( new Language("mxt", "Mixtec, Jamiltepec", QObject::tr("Mixtec, Jamiltepec")) );
    m_langList.append( new Language("my", "Burmese", QObject::tr("Burmese")) );
    //  m_langList.append( new Language("na", "Nauru", QObject::tr("Nauru")) );
    m_langList.append( new Language("nb", "Norwegian Bokm\u00e5l", QObject::tr("Norwegian Bokm\u00e5l")) );
    m_langList.append( new Language("ncl", "Nahuatl, Michoac\u00e1n", QObject::tr("Nahuatl, Michoac\u00e1n")) );
    //  m_langList.append( new Language("nd", "Ndebele, North", QObject::tr("Ndebele, North")) );
    m_langList.append( new Language("nds", "Low German; Low Saxon", QObject::tr("Low German; Low Saxon")) );
    m_langList.append( new Language("ne", "Nepali", QObject::tr("Nepali")) );
    m_langList.append( new Language("ngu", "Nahuatl, Guerrero", QObject::tr("Nahuatl, Guerrero")) );
    m_langList.append( new Language("nhy", "Nahuatl, Northern Oaxaca", QObject::tr("Nahuatl, Northern Oaxaca")) );
    //  m_langList.append( new Language("ng", "Ndonga", QObject::tr("Ndonga")) );
    m_langList.append( new Language("nl", "Dutch", QObject::tr("Dutch")) );
    m_langList.append( new Language("nn", "Norwegian Nynorsk", QObject::tr("Norwegian Nynorsk")) );
    m_langList.append( new Language("no", "Norwegian", QObject::tr("Norwegian")) );
    //  m_langList.append( new Language("nr", "Ndebele, South", QObject::tr("Ndebele, South")) );
    //  m_langList.append( new Language("nv", "Navajo", QObject::tr("Navajo")) );
    //  m_langList.append( new Language("ny", "Chichewa; Nyanja", QObject::tr("Chichewa; Nyanja")) );
    //  m_langList.append( new Language("oc", "Occitan (post 1500); Provençal", QObject::tr("Occitan (post 1500); Provençal")) );
    //  m_langList.append( new Language("om", "Oromo", QObject::tr("Oromo")) );
    //  m_langList.append( new Language("or", "Oriya", QObject::tr("Oriya")) );
    //  m_langList.append( new Language("os", "Ossetian; Ossetic", QObject::tr("Ossetian; Ossetic")) );
    m_langList.append( new Language("otq", "Otomi, Quer\u00e9taro", QObject::tr("Otomi, Quer\u00e9taro")) );
    //  m_langList.append( new Language("pa", "Panjabi", QObject::tr("Panjabi")) );
    m_langList.append( new Language("pap", "Papiamento", QObject::tr("Papiamento")) );
    //  m_langList.append( new Language("pi", "Pali", QObject::tr("Pali")) );
    m_langList.append( new Language("ppk", "Uma", QObject::tr("Uma")) );
    m_langList.append( new Language("pl", "Polish", QObject::tr("Polish")) );
    m_langList.append( new Language("pot", "Potawatomi", QObject::tr("Potawatomi")) );
    m_langList.append( new Language("ppk", "Uma", QObject::tr("Uma")) );
    m_langList.append( new Language("prs", "Persian (Dari)", QObject::tr("Persian (Dari)")) );
    //  m_langList.append( new Language("ps", "Pushto", QObject::tr("Pushto")) );
    m_langList.append( new Language("pt", "Portuguese", QObject::tr("Portuguese")) );
    m_langList.append( new Language("pt_BR", "Brazilian Portuguese", QObject::tr("Brazilian Portuguese")) );//added by ourself
    //  m_langList.append( new Language("qu", "Quechua", QObject::tr("Quechua")) );
    m_langList.append( new Language("qut", "Quich\u00e9, West Central", QObject::tr("Quich\u00e9, West Central")) );
    //  m_langList.append( new Language("rm", "Raeto-Romance", QObject::tr("Raeto-Romance")) );
    //  m_langList.append( new Language("rn", "Rundi", QObject::tr("Rundi")) );
    m_langList.append( new Language("ro", "Romanian", QObject::tr("Romanian")) );
    m_langList.append( new Language("ru", "Russian", QObject::tr("Russian")) );
    //  m_langList.append( new Language("rw", "Kinyarwanda", QObject::tr("Kinyarwanda")) );
    //  m_langList.append( new Language("sa", "Sanskrit", QObject::tr("Sanskrit")) );
    //  m_langList.append( new Language("sc", "Sardinian", QObject::tr("Sardinian")) );
    m_langList.append( new Language("sco", "Scots", QObject::tr("Scots")) );
    //  m_langList.append( new Language("sd", "Sindhi", QObject::tr("Sindhi")) );
    //  m_langList.append( new Language("se", "Northern Sami", QObject::tr("Northern Sami")) );
    //  m_langList.append( new Language("sg", "Sango", QObject::tr("Sango")) );
    //  m_langList.append( new Language("si", "Sinhalese", QObject::tr("Sinhalese")) );
    m_langList.append( new Language("sk", "Slovak", QObject::tr("Slovak")) );
    m_langList.append( new Language("sl", "Slovenian", QObject::tr("Slovenian")) );
    //  m_langList.append( new Language("sm", "Samoan", QObject::tr("Samoan")) );
    //  m_langList.append( new Language("sn", "Shona", QObject::tr("Shona")) );
    m_langList.append( new Language("so", "Somali", QObject::tr("Somali")) );
    m_langList.append( new Language("sq", "Albanian", QObject::tr("Albanian")) );
    //  m_langList.append( new Language("sr", "Serbian", QObject::tr("Serbian")) );
    m_langList.append( new Language("srn", "Sranan", QObject::tr("Sranan")) );
    //  m_langList.append( new Language("ss", "Swati", QObject::tr("Swati")) );
    //  m_langList.append( new Language("st", "Sotho, Southern", QObject::tr("Sotho, Southern")) );
    //  m_langList.append( new Language("su", "Sundanese", QObject::tr("Sundanese")) );
    m_langList.append( new Language("sv", "Swedish", QObject::tr("Swedish")) );
    m_langList.append( new Language("sw", "Swahili", QObject::tr("Swahili")) );
    m_langList.append( new Language("syr", "Syriac", QObject::tr("Syriac")) );
    m_langList.append( new Language("ta", "Tamil", QObject::tr("Tamil")) );
    //  m_langList.append( new Language("te", "Telugu", QObject::tr("Telugu")) );
    //  m_langList.append( new Language("tg", "Tajik", QObject::tr("Tajik")) );
    m_langList.append( new Language("th", "Thai", QObject::tr("Thai")) );
    //  m_langList.append( new Language("tk", "Turkmen", QObject::tr("Turkmen")) );
    m_langList.append( new Language("tl", "Tagalog", QObject::tr("Tagalog")) );
    m_langList.append( new Language("tlh", "Klingon", QObject::tr("Klingon")) );
    m_langList.append( new Language("tn", "Tswana", QObject::tr("Tswana")) );
    m_langList.append( new Language("tr", "Turkish", QObject::tr("Turkish")) );
    //  m_langList.append( new Language("ts", "Tsonga", QObject::tr("Tsonga")) );
    //  m_langList.append( new Language("tt", "Tatar", QObject::tr("Tatar")) );
    m_langList.append( new Language("ttc", "Tektiteko", QObject::tr("Tektiteko")) );
    //  m_langList.append( new Language("tw", "Twi", QObject::tr("Twi")) );
    m_langList.append( new Language("ty", "Tahitian", QObject::tr("Tahitian")) );
    m_langList.append( new Language("tzz", "Tzotzil, Zinacant\u00e1n", QObject::tr("Tzotzil, Zinacant\u00e1n")) );
    //  m_langList.append( new Language("ug", "Uighur", QObject::tr("Uighur")) );
    m_langList.append( new Language("uk", "Ukrainian", QObject::tr("Ukrainian")) );
    //  m_langList.append( new Language("ur", "Urdu", QObject::tr("Urdu")) );
    m_langList.append( new Language("ury", "Orya", QObject::tr("Orya")) );
    m_langList.append( new Language("usp", "Uspanteco", QObject::tr("Uspanteco")) );
    //  m_langList.append( new Language("uz", "Uzbek", QObject::tr("Uzbek")) );
    m_langList.append( new Language("vi", "Vietnamese", QObject::tr("Vietnamese")) );
    //  m_langList.append( new Language("vo", "Volapük", QObject::tr("Volapük")) );
    //  m_langList.append( new Language("wo", "Wolof", QObject::tr("Wolof")) );
    m_langList.append( new Language("xh", "Xhosa", QObject::tr("Xhosa")) );
    m_langList.append( new Language("xtd", "Mixtec, Diuxi-Tilantongo", QObject::tr("Mixtec, Diuxi-Tilantongo")) );
    m_langList.append( new Language("yi", "Yiddish", QObject::tr("Yiddish")) );
    m_langList.append( new Language("yo", "Yoruba", QObject::tr("Yoryba")) );
    //  m_langList.append( new Language("za", "Zhuang", QObject::tr("Zhuang")) );
    m_langList.append( new Language("zab", "Zapotec, San Juan Guelav\u00eda", QObject::tr("Zapotec, San Juan Guelav\u00eda")) );
    m_langList.append( new Language("zaw", "Zapotec, Mitla", QObject::tr("Zapotec, Mitla")) );
    m_langList.append( new Language("zh", "Chinese", QObject::tr("Chinese")) );
    m_langList.append( new Language("zpo", "Zapotec, Amatl\u00e1n", QObject::tr("Zapotec, Amatl\u00e1n")) );
    m_langList.append( new Language("zpq", "Zapotec, Zoogocho", QObject::tr("Zapotec, Zoogocho")) );
    m_langList.append( new Language("zpu", "Zapotec, Yal\u00e1lag", QObject::tr("Zapotec, Yal\u00e1lag")) );
    m_langList.append( new Language("zpv", "Zapotec, Chichicapan", QObject::tr("Zapotec, Chichicapan")) );
    m_langList.append( new Language("zsr", "Zapotec, Southern Rincon", QObject::tr("Zapotec, Southern Rincon")) );
    m_langList.append( new Language("ztq", "Zapotec, Quioquitani-Quier\u00ed", QObject::tr("Zapotec, Quioquitani-Quier\u00ed")) );
    m_langList.append( new Language("zty", "Zapotec, Yatee", QObject::tr("Zapotec, Yatee")) );
    m_langList.append( new Language("zu", "Zulu", QObject::tr("Zulu")) );

    Q_FOREACH(Language * const lang, m_langList)
        m_langMap.insert( lang->abbrev(), lang);
}
