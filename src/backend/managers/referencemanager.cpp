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
#include "../../util/tounderlying.h"
#include "../../util/unreachable.h"
#include "../config/btconfig.h"
#include "../keys/cswordversekey.h"
#include "../drivers/cswordmoduleinfo.h"
#include "cswordbackend.h"


/** Returns a hyperlink used to be imbedded in the display windows. At the moment the format is sword://module/key */
QString ReferenceManager::encodeHyperlink(QString const & moduleName,
                                          QString const & key,
                                          ReferenceManager::Type const type)
{
    static auto const initRet =
            [](ReferenceManager::Type const type) -> QString {
                switch (type) {
                    case Bible:         return "sword://Bible/";
                    case Commentary:    return "sword://Commentary/";
                    case Lexicon:       return "sword://Lexicon/";
                    case GenericBook:   return "sword://Book/";
                    case MorphHebrew:   return "morph://Hebrew/";
                    case MorphGreek:    return "morph://Greek/";
                    case StrongsHebrew: return "strongs://Hebrew/";
                    case StrongsGreek:  return "strongs://Greek/";
                    default: return {};
                }
            };
    auto ret(initRet(type).append(moduleName.isEmpty()
                                  ? preferredModule(type) // fallback
                                  : moduleName).append('/'));
    if (type == GenericBook) {
        /* Replace all / of the key (e.g. of a CSwordTreeKey) with the escape
           sequence \/ so we know it's a link internal divider (e.g. of
           CSwordTreeKey): */
        for (auto const c : key) {
            if (c == '/') {
                ret.append("\\/");
            } else {
                ret.append(c);
            }
        }
    } else { // Slashes do not appear in verses and dictionary entries:
        ret.append(key);
    }
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
    QString ref = hyperlink;

    // Remove the trailing slash (unless escaped):
    if (ref.endsWith('/') && !ref.endsWith("\\/"))
        ref.chop(1);

    //find out which type we have by looking at the beginning (protocoll section of URL)
    if (ref.left(8).toLower() == "sword://") { //Bible, Commentary or Lexicon
        ref = ref.mid(8);

        if (ref.left(5).toLower() == "bible") { //a bible hyperlink
            type = ReferenceManager::Bible;
            ref = ref.mid(6); //inclusive trailing slash
        }
        else if (ref.left(10).toLower() == "commentary") { // a Commentary hyperlink
            type = ReferenceManager::Commentary;
            ref = ref.mid(11); //inclusive trailing slash
        }
        else if (ref.left(7).toLower() == "lexicon") { // a Lexicon hyperlink
            type = ReferenceManager::Lexicon;
            ref = ref.mid(8); //inclusive trailing slash
        }
        else if (ref.left(4).toLower() == "book") { // a Book hyperlink
            type = ReferenceManager::GenericBook;
            ref = ref.mid(5); //inclusive trailing slash
        }

        // string up to next slash is the modulename
        if (ref.at(0) != '/' ) { //we have a module given

            while (true) {
                const int pos = ref.indexOf("/");

                if ((pos > 0) && ref.at(pos - 1) != '\\') { //found a slash which is not escaped
                    module = ref.mid(0, pos);
                    ref = ref.mid(pos + 1);
                    break;
                }
                else if (pos == -1) {
                    break;
                }
            }

            // the rest is the key
            key = ref;
        }
        else {
            key = ref.mid(1);
        }

        //the key may be an osis key like "NASBLex:Moses", which sets the module, too
        //   const int modPos = key.find(":");
        //   if (modPos != -1 && key.at(modPos-1).isLetter() && key.at(modPos+1).isLetter()) {
        //    module = key.left(modPos);
        //    key = key.mid(modPos+1);
        //
        //    qWarning("found the module name %s with key %s", module.latin1(), key.latin1());
        //   }

        //replace \/ escapes with /
        key.replace(QRegExp("\\\\/"), "/");
    }
    else if (ref.left(8).toLower() == "morph://" || ref.left(10).toLower() == "strongs://") { //strongs or morph URL have the same format
        enum { IsMorph, IsStrongs } preType = IsMorph;

        if (ref.left(8).toLower() == "morph://") { //morph code hyperlink
            ref = ref.mid(8);
            preType = IsMorph;
        }
        else if (ref.left(10).toLower() == "strongs://") {
            ref = ref.mid(10);
            preType = IsStrongs;
        }

        //part up to next slash is the language
        const int pos = ref.indexOf("/");

        if (pos > 0) { //found
            const QString language = ref.mid(0, pos);

            if (language.toLower() == "hebrew") {
                switch (preType) {

                    case IsMorph:
                        type = ReferenceManager::MorphHebrew;
                        break;

                    case IsStrongs:
                        type = ReferenceManager::StrongsHebrew;
                        break;
                }
            }
            else if (language.toLower() == "greek") {
                switch (preType) {

                    case IsMorph:
                        type = ReferenceManager::MorphGreek;
                        break;

                    case IsStrongs:
                        type = ReferenceManager::StrongsGreek;
                        break;
                }
            }

            key = ref.mid(pos + 1); // the remaining part is the key

            module = preferredModule(type);
        }
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

ReferenceManager::Type
ReferenceManager::typeFromModule(CSwordModuleInfo::ModuleType const type) {
    /* While this could instead be a series of simple casts from one enum to
       another, we need to keep the following switch to ensure we are warned by
       the compiler when the enums change in unintended ways which break the
       assumptions for the simple casts. The compiler is usually smart enough
       to optimize all of the following to something as simple as `mov eax, edi`
       (move argument to return value register) on x86_64. */
    switch(type) {
        #define RET_CASE(Value) \
            case CSwordModuleInfo::Value: \
                static_assert(toUnderlying(Value) \
                              == toUnderlying(CSwordModuleInfo::Value)); \
                return Value
        RET_CASE(Unknown);
        RET_CASE(Bible);
        RET_CASE(Commentary);
        RET_CASE(Lexicon);
        RET_CASE(GenericBook);
        #undef RET_CASE
    };
    /* In C++ enum variables can have values other than what the named values,
       and hence in those cases the above switch would fall through here.
       However, assuming that we do not us such values for type, and relying on
       the compiler to warn us on missing cases in the above switch, we can
       safely assume that control flow never reaches this line. We avoid a
       -Wreturn-type warning here by using UNREACHABLE(). For a discussion on
       why such a warning is given, see GCC PR #87950. */
    UNREACHABLE();
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
