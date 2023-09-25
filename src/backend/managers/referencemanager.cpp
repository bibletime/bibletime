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

#include "referencemanager.h"

#include <QDebug>
#include <utility>
#include "../../util/btassert.h"
#include "../config/btconfig.h"
#include "../keys/cswordversekey.h"
#include "../drivers/cswordmoduleinfo.h"
#include "btlocalemgr.h"
#include "cswordbackend.h"


/** Returns a hyperlink used to be imbedded in the display windows. At the moment the format is sword://module/key */
QString ReferenceManager::encodeHyperlink(CSwordModuleInfo const & module,
                                          QString const & key)
{
    QString type;
    switch (module.type()) {
        case CSwordModuleInfo::Bible:
            type = QStringLiteral("Bible");
            break;
        case CSwordModuleInfo::Commentary:
            type = QStringLiteral("Commentary");
            break;
        case CSwordModuleInfo::Lexicon:
            type = QStringLiteral("Lexicon");
            break;
        case CSwordModuleInfo::GenericBook:
            type = QStringLiteral("Book");
            break;
        default:
            return {};
    }
    BT_ASSERT(!module.name().isEmpty());
    return QStringLiteral("sword://%1/%2/%3").arg(type, module.name(), key);
}

/** Decodes the given hyperlink. */
std::optional<ReferenceManager::DecodedHyperlink>
ReferenceManager::decodeHyperlink(QString const & hyperlink) {
    /*
       The following types of URLs are supported:
         sword://bible/<module>/<key>
         sword://commentary/<module>/<key>
         sword://lexicon/<module>/<key>
         sword://book/<module>/<key>
         morph://hebrew/<key>
         morph://greek/<key>
         strongs://hebrew/<key>
         strongs://greek/<key>
       where <module> is the name of a module and <key> is any non-empty string.
       If <module> is empty, or not present, the preferred module is returned
       (if present).
    */

    QStringView ref(hyperlink);

    static auto const preferredModule =
            [](ReferenceManager::Type const type) -> CSwordModuleInfo * {
                QString typeStr;
                switch (type) {
                    #define RET_CASE(t,str) \
                        case t: typeStr = QStringLiteral("standard" str); break
                    RET_CASE(Bible, "Bible");
                    RET_CASE(Commentary, "Commentary");
                    RET_CASE(Lexicon, "Lexicon");
                    RET_CASE(StrongsHebrew, "HebrewStrongsLexicon");
                    RET_CASE(StrongsGreek, "GreekStrongsLexicon");
                    RET_CASE(MorphHebrew, "HebrewMorphLexicon");
                    RET_CASE(MorphGreek, "GreekMorphLexicon");
                    #undef RET_CASE
                    case Unknown: default: return nullptr;
                }
                return btConfig().getDefaultSwordModuleByType(typeStr);
            };

    DecodedHyperlink ret;
    static auto const removeCaseInsensitivePrefix =
            [](QStringView & ref, QString const & prefix) {
                if (ref.startsWith(prefix, Qt::CaseInsensitive)) {
                    ref = ref.mid(prefix.size() - 1);
                    return true;
                }
                return false;
            };
    int slashPos; // position of the last parsed slash
    if (removeCaseInsensitivePrefix(ref, QStringLiteral("sword://"))) { //Bible, Commentary or Lexicon
        if (removeCaseInsensitivePrefix(ref, QStringLiteral("bible/"))) {
            ret.type = ReferenceManager::Bible;
        } else if (removeCaseInsensitivePrefix(ref,
                                               QStringLiteral("commentary/")))
        {
            ret.type = ReferenceManager::Commentary;
        } else if (removeCaseInsensitivePrefix(ref, QStringLiteral("lexicon/")))
        {
            ret.type = ReferenceManager::Lexicon;
        } else if (removeCaseInsensitivePrefix(ref, QStringLiteral("book/"))) {
            ret.type = ReferenceManager::GenericBook;
        } else {
            return {};
        }

        // string up to next slash is the modulename
        slashPos = ref.indexOf('/');
        if (slashPos < 0) // if key is empty
            return {};
        if (slashPos == 0) {
            ret.module = preferredModule(ret.type);
        } else { // We have a module given
            ret.module = CSwordBackend::instance().findModuleByName(
                             ref.left(slashPos).toString());
            if (!ret.module)
                ret.module = preferredModule(ret.type);
        }
    } else {
        struct { Type hebrew; Type greek; } types;
        if (removeCaseInsensitivePrefix(ref, QStringLiteral("morph://"))) {
            types = {MorphHebrew, MorphGreek};
        } else if (removeCaseInsensitivePrefix(ref,
                                               QStringLiteral("strongs://")))
        {
            types = {StrongsHebrew, StrongsGreek};
        } else {
            return {};
        }

        // Part up to next slash is the language:
        slashPos = ref.indexOf('/');
        if (slashPos <= 0) // if language or key is empty (0 or -1)
            return {};
        auto const language(ref.left(slashPos).toString().toLower());
        if (language == QStringLiteral("hebrew")) {
            ret.type = types.hebrew;
        } else if (language == QStringLiteral("greek")) {
            ret.type = types.greek;
        } else {
            return {};
        }

        ret.module = preferredModule(ret.type);
    }
    ref = ref.mid(slashPos + 1); // The remaining part is the key
    if (ref.isEmpty()) // require non-empty key
        return {};
    ret.key = ref.toString();
    return ret;
}

/** Parses the given verse references using the given language and the module.*/
QString ReferenceManager::parseVerseReference(
        QString const & ref,
        ReferenceManager::ParseOptions const & options)
{
    auto & backend = CSwordBackend::instance();
    auto const * const mod =
            backend.findModuleByName(options.refDestinationModule);
    if (!mod) // Parsing of non-verse based references is not supported:
        return ref;

    switch (mod->type()) {
    case CSwordModuleInfo::Bible: case CSwordModuleInfo::Commentary: break;
    default:
        qDebug() << "CReferenceManager: Only verse based modules are supported "
                    "as ref destination module";
        return {};
    }

    QString sourceLanguage = options.sourceLanguage;

    bool const haveLocaleForSourceLanguage =
            [&locale = std::as_const(sourceLanguage)]() {
                if (locale == QStringLiteral("locales"))
                    return false;
                auto const & locales = BtLocaleMgr::internalSwordLocales();
                return locales.find(locale.toUtf8().constData())
                        != locales.end();
            }();
    if (!haveLocaleForSourceLanguage)
        sourceLanguage = QStringLiteral("en_US");

    auto const * const destinationLanguage =
            haveLocaleForSourceLanguage ? "en" : "en_US";

    CSwordVerseKey baseKey(nullptr);
    baseKey.setLocale(sourceLanguage.toUtf8().constData());
    baseKey.setKey(options.refBase); // Probably in the sourceLanguage
    baseKey.setLocale("en_US"); // English works in all environments as base

    /* HACK: We have to workaround a Sword bug, we have to set the default
       locale to the same as the sourceLanguage! */
    auto const oldLocaleName(backend.booknameLanguage());
    backend.setBooknameLanguage(sourceLanguage);

    sword::VerseKey dummy;
    dummy.setLocale(sourceLanguage.toUtf8().constData());
    BT_ASSERT(!strcmp(dummy.getLocale(), sourceLanguage.toUtf8().constData()));

    QString ret;
    for (auto const & ref : ref.split(';')) {
        /* The listkey may contain more than one item, because a ref like
           "Gen 1:3,5" is parsed into two single refs */
        auto lk(dummy.parseVerseList(ref.toUtf8().constData(),
                                     baseKey.key().toUtf8().constData(),
                                     true));
        BT_ASSERT(!dummy.popError());

        if (!lk.getCount()) {
            ret.append(ref); //don't change the original
            continue;
        }

        for (int i = 0; i < lk.getCount(); ++i) {
            if (auto * const k =
                        dynamic_cast<sword::VerseKey *>(lk.getElement(i)))
            {
                k->setLocale(destinationLanguage);
                ret.append(QStringLiteral("%1; ")
                           .arg(QString::fromUtf8(k->getRangeText())));
            } else { // A single ref
                sword::VerseKey vk;
                vk.setLocale(sourceLanguage.toUtf8().constData());
                vk = lk.getElement(i)->getText();
                vk.setLocale(destinationLanguage);
                ret.append(QStringLiteral("%1; ")
                           .arg(QString::fromUtf8(vk.getText())));
            }
        }
    }
    backend.setBooknameLanguage(oldLocaleName);
    return ret;
}
