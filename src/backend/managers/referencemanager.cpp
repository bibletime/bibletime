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


/** Returns a hyperlink used to be imbedded in the display windows. At the moment the format is sword://module/key */
const QString ReferenceManager::encodeHyperlink( const QString moduleName, const QString key, const ReferenceManager::Type type) {
    QString ret = QString();

    switch (type) {

        case Bible:
            ret = QString("sword://Bible/");
            break;
        case Commentary:
            ret = QString("sword://Commentary/");
            break;
        case Lexicon:
            ret = QString("sword://Lexicon/");
            break;
        case GenericBook:
            ret = QString("sword://Book/");
            break;
        case MorphHebrew:
            ret = QString("morph://Hebrew/");
            break;
        case MorphGreek:
            ret = QString("morph://Greek/");
            break;
        case StrongsHebrew:
            ret = QString("strongs://Hebrew/");
            break;
        case StrongsGreek:
            ret = QString("strongs://Greek/");
            break;
        default:
            break;
    }

    if (!moduleName.isEmpty()) {
        ret.append( moduleName ).append('/');
    }
    else { //if module is empty use fallback module
        ret.append( preferredModule(type) ).append('/');
    }

    if (type == GenericBook) {
        const QString s = (!key.isEmpty() ? key : QString());
        QString newKey = QString();
        //replace all / of the key (e.g. of a CSwordTreeKey) with
        // the escape sequence \/ so we know it's a link internal divider (e.g. of CSwordTreeKey)!

        QChar c;

        for (int i = 0; i < s.length(); ++i) {
            c = s.at(i);

            if (c == '/') {
                newKey.append("\\/");
            }
            else {
                newKey.append(c);
            }
        }

        ret.append( newKey );
    }
    else { //slashes do not appear in verses and dictionary entries

        switch (type) {

            case Bible: //bibles or commentary keys need parsing

            case Commentary: {
                /*                  CSwordModuleInfo* mod = CSwordBackend::instance()()->findModuleByName(moduleName);

                                  ParseOptions options;
                                  options.refDestinationModule = mod->name();
                                  options.refBase =
                                  options.sourceLanguage = mod->module()->Lang();
                                  options.destinationLanguage = "en";

                                ret.append( parseVerseReference(key, options) ); //we add the english key, so drag and drop will work in all cases*/
                ret.append(key);
                break;
            }

            default:
                ret.append( key ); //use the standard key, no parsing required
                break;
        }
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
    //remove the trailing slash

    if (ref.right(1) == "/" && ref.right(2) != "\\/") //trailing slash, but not escaped
        ref = ref.left(ref.length() - 1);

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
        enum PreType {IsMorph, IsStrongs};
        PreType preType = IsMorph;

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

            ref = ref.mid(pos + 1);
            key = ref; //the remaining part is the key

            module = preferredModule(type);
        }
    }

    if (key.isEmpty() && module.isEmpty())
        return false;

    return true;
}

/** Returns true if the parameter is a hyperlink. */
bool ReferenceManager::isHyperlink( const QString& hyperlink ) {
    return (    hyperlink.left(8)  == "sword://")
           || (hyperlink.left(10) == "strongs://")
           || (hyperlink.left(8)  == "morph://");
}

/** Returns the preferred module name for the given type. */
const QString ReferenceManager::preferredModule( const ReferenceManager::Type type ) {
    QString moduleName = QString();
    CSwordModuleInfo* module = nullptr;

    switch (type) {

        case ReferenceManager::Bible:
            module = btConfig().getDefaultSwordModuleByType( "standardBible" );
            break;
        case ReferenceManager::Commentary:
            module = btConfig().getDefaultSwordModuleByType( "standardCommentary" );
            break;
        case ReferenceManager::Lexicon:
            module = btConfig().getDefaultSwordModuleByType( "standardLexicon" );
            break;
        case ReferenceManager::StrongsHebrew:
            module = btConfig().getDefaultSwordModuleByType( "standardHebrewStrongsLexicon" );
            break;
        case ReferenceManager::StrongsGreek:
            module = btConfig().getDefaultSwordModuleByType( "standardGreekStrongsLexicon" );
            break;
        case ReferenceManager::MorphHebrew:
            module = btConfig().getDefaultSwordModuleByType( "standardHebrewMorphLexicon" );
            break;
        case ReferenceManager::MorphGreek:
            module = btConfig().getDefaultSwordModuleByType( "standardGreekMorphLexicon" );
            break;
        default:
            module = nullptr;
            break;
    }

    return module ? module->name() : QString();
}

/** No descriptions */
ReferenceManager::Type ReferenceManager::typeFromModule( const CSwordModuleInfo::ModuleType type) {
    switch (type) {

        case CSwordModuleInfo::Bible:
            return ReferenceManager::Bible;

        case CSwordModuleInfo::Commentary:
            return ReferenceManager::Commentary;

        case CSwordModuleInfo::Lexicon:
            return ReferenceManager::Lexicon;

        case CSwordModuleInfo::GenericBook:
            return ReferenceManager::GenericBook;

        default:
            return ReferenceManager::Unknown;
    }
}

/** Parses the given verse references using the given language and the module.*/
const QString ReferenceManager::parseVerseReference( const QString& ref, const ReferenceManager::ParseOptions& options) {

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

    for (QStringList::iterator it = refList.begin(); it != refList.end(); ++it) {
        //The listkey may contain more than one item, because a ref lik "Gen 1:3,5" is parsed into two single refs
        sword::ListKey lk = dummy.parseVerseList((*it).toUtf8().constData(), baseKey.key().toUtf8().constData(), true);
        BT_ASSERT(!dummy.popError());

        //BT_ASSERT(lk.Count());
        if (!lk.getCount()) {
            ret.append( *it ); //don't change the original
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
