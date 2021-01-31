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

#include "referencemanager.h"

#include <algorithm>
#include <QRegExp>
#include <QDebug>
#include "../../util/btassert.h"
#include "../config/btconfig.h"
#include "../keys/cswordversekey.h"
#include "../drivers/cswordmoduleinfo.h"
#include "cswordbackend.h"


namespace {

template <typename Prefix>
bool removeCaseInsensitivePrefix(QStringRef & ref, Prefix && prefix) {
    using Prefix_ = std::remove_reference_t<Prefix>;
    static_assert(std::is_array_v<Prefix_>);
    static_assert(std::extent_v<Prefix_> > 0u);
    static_assert(std::is_same_v<char const, std::remove_extent_t<Prefix_>>);
    if (ref.startsWith(prefix, Qt::CaseInsensitive)) {
        ref = ref.mid(sizeof(prefix) - 1u);
        return true;
    }
    return false;
}

} // anonymous namespace

/** Returns a hyperlink used to be imbedded in the display windows. At the moment the format is sword://module/key */
QString ReferenceManager::encodeHyperlink(CSwordModuleInfo const & module,
                                          QString const & key)
{
    auto const initStr =
        [&module]() noexcept -> char const * {
            switch (module.type()) {
                case CSwordModuleInfo::Bible: return "sword://Bible/";
                case CSwordModuleInfo::Commentary: return "sword://Commentary/";
                case CSwordModuleInfo::Lexicon: return "sword://Lexicon/";
                case CSwordModuleInfo::GenericBook: return "sword://Book/";
                case CSwordModuleInfo::Unknown: default: return nullptr;
            }
        }();
    if (!initStr)
        return {};
    QString ret(initStr);
    BT_ASSERT(!module.name().isEmpty());
    ret.append(module.name());
    ret.append('/');
    ret.append(key);
    return ret;
}

/** Decodes the given hyperlink to module and key. */
bool ReferenceManager::decodeHyperlink( const QString& hyperlink, QString& module, QString& key, ReferenceManager::Type& type ) {
    /**
    * We have to decide between three types of URLS: sword://Type/Module/Key, morph://Testament/key and strongs://Testament/Key
    */
    module = QString();
    key = QString();

    type = Unknown; //not yet known
    QStringRef ref(&hyperlink);


    //find out which type we have by looking at the beginning (protocoll section of URL)
    if (removeCaseInsensitivePrefix(ref, "sword://")) { //Bible, Commentary or Lexicon
        if (removeCaseInsensitivePrefix(ref, "bible/")) {
            type = ReferenceManager::Bible;
        } else if (removeCaseInsensitivePrefix(ref, "commentary/")) {
            type = ReferenceManager::Commentary;
        } else if (removeCaseInsensitivePrefix(ref, "lexicon/")) {
            type = ReferenceManager::Lexicon;
        } else if (removeCaseInsensitivePrefix(ref, "book/")) {
            type = ReferenceManager::GenericBook;
        }

        // string up to next slash is the modulename
        if (ref.at(0) != '/' ) { //we have a module given

            auto const slashPos = ref.indexOf('/');
            if (slashPos < 0) // if key is empty
                return false;
            if (slashPos == 0)
                module = preferredModule(type);

            // the rest is the key
            key = ref.toString();
        }
        else {
            key = ref.mid(1).toString();
        }
    } else {
        auto const handleMorphOrStrongs =
                [&ref, &key, &module, &type](Type const hebrewType,
                                             Type const greekType)
                {
                    //part up to next slash is the language
                    auto const pos = ref.indexOf("/");
                    if (pos > 0) { //found
                        auto const language(ref.left(pos).toString().toLower());

                        if (language == "hebrew") {
                            type = hebrewType;
                        } else if (language == "greek") {
                            type = greekType;
                        } /// \bug or else?

                        // the remaining part is the key:
                        key = ref.mid(pos + 1).toString();
                        module = preferredModule(type);
                    } /// \bug or else?
                };

        if (removeCaseInsensitivePrefix(ref, "morph://")) {
            handleMorphOrStrongs(MorphHebrew, MorphGreek);
        } else if (removeCaseInsensitivePrefix(ref, "strongs://")) {
            handleMorphOrStrongs(StrongsHebrew, StrongsGreek);
        } /// \bug or else?
    }

    if (key.isEmpty() && module.isEmpty())
        return false;

    return true;
}

/** Returns true if the parameter is a hyperlink. */
bool ReferenceManager::isHyperlink( const QString& hyperlink ) {
    return hyperlink.startsWith("sword://")
           || hyperlink.startsWith("strongs://")
           || hyperlink.startsWith("morph://");
}

/** Returns the preferred module name for the given type. */
QString ReferenceManager::preferredModule(ReferenceManager::Type const type) {
    static auto const getModuleTypeString =
            [](ReferenceManager::Type const type) -> char const * {
                switch (type) {
                    #define RET_CASE(t,str) \
                        case ReferenceManager::t: return "standard" str
                    RET_CASE(Bible, "Bible");
                    RET_CASE(Commentary, "Commentary");
                    RET_CASE(Lexicon, "Lexicon");
                    RET_CASE(StrongsHebrew, "HebrewStrongsLexicon");
                    RET_CASE(StrongsGreek, "GreekStrongsLexicon");
                    RET_CASE(MorphHebrew, "HebrewMorphLexicon");
                    RET_CASE(MorphGreek, "GreekMorphLexicon");
                    #undef RET_CASE
                    default: return nullptr;
                }
            };
    if (auto const typeStr = getModuleTypeString(type))
        if (auto const module = btConfig().getDefaultSwordModuleByType(typeStr))
            return module->name();
    return {};
}

/** Parses the given verse references using the given language and the module.*/
QString ReferenceManager::parseVerseReference(
        QString const & ref,
        ReferenceManager::ParseOptions const & options)
{
    CSwordModuleInfo* const mod = CSwordBackend::instance()->findModuleByName(options.refDestinationModule);
    //BT_ASSERT(mod); tested later

    if (!mod) {
        //parsing of non-verse based references is not supported
        return ref;
    }

    if ((mod->type() != CSwordModuleInfo::Bible) && (mod->type() != CSwordModuleInfo::Commentary)) {
        qDebug() << "CReferenceManager: Only verse based modules are supported as ref destination module";
        return QString();
    }

    QString sourceLanguage = options.sourceLanguage;
    QString destinationLanguage = options.destinationLanguage;

    sword::StringList locales = sword::LocaleMgr::getSystemLocaleMgr()->getAvailableLocales();
    if (/*options.sourceLanguage == "en" ||*/ std::find(locales.begin(), locales.end(), sourceLanguage.toUtf8().constData()) == locales.end()) { //sourceLanguage not available
        sourceLanguage = "en_US";
    }

    if (/*options.destinationLanguage == "en" ||*/ std::find(locales.begin(), locales.end(), sourceLanguage.toUtf8().constData()) == locales.end()) { //destination not available
        destinationLanguage = "en_US";
    }

    QString ret;
    QStringList refList = ref.split(";");

    CSwordVerseKey baseKey(nullptr);
    baseKey.setLocale( sourceLanguage.toUtf8().constData() );
    baseKey.setKey(options.refBase); //probably in the sourceLanguage
    baseKey.setLocale( "en_US" ); //english works in all environments as base

//     CSwordVerseKey dummy(0);
    //HACK: We have to workaround a Sword bug, we have to set the default locale to the same as the sourceLanguage !
    const QString oldLocaleName = CSwordBackend::instance()->booknameLanguage();
    CSwordBackend::instance()->booknameLanguage(sourceLanguage);

    sword::VerseKey dummy;
    dummy.setLocale( sourceLanguage.toUtf8().constData() );
    BT_ASSERT(!strcmp(dummy.getLocale(), sourceLanguage.toUtf8().constData()));

//     qDebug("Parsing '%s' in '%s' using '%s' as base, source lang '%s', dest lang '%s'", ref.latin1(), options.refDestinationModule.latin1(), baseKey.key().latin1(), sourceLanguage.latin1(), destinationLanguage.latin1());

    for (auto const & ref : refList) {
        //The listkey may contain more than one item, because a ref lik "Gen 1:3,5" is parsed into two single refs
        sword::ListKey lk = dummy.parseVerseList(ref.toUtf8().constData(), baseKey.key().toUtf8().constData(), true);
        BT_ASSERT(!dummy.popError());

        //BT_ASSERT(lk.Count());
        if (!lk.getCount()) {
            ret.append(ref); //don't change the original
            continue;
        }

        for (int i = 0; i < lk.getCount(); ++i) {
            if (dynamic_cast<sword::VerseKey*>(lk.getElement(i))) { // a range
                sword::VerseKey* k = dynamic_cast<sword::VerseKey*>(lk.getElement(i));
                BT_ASSERT(k);
                k->setLocale( destinationLanguage.toUtf8().constData() );

                ret.append( QString::fromUtf8(k->getRangeText()) ).append("; ");
            }
            else { // a single ref
                sword::VerseKey vk;
                vk.setLocale( sourceLanguage.toUtf8().constData() );
                vk = lk.getElement(i)->getText();
                vk.setLocale( destinationLanguage.toUtf8().constData() );

                ret.append( QString::fromUtf8(vk.getText()) ).append("; ");
            }
        }

    }

    CSwordBackend::instance()->booknameLanguage(oldLocaleName);
    return ret;
}
