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

#pragma once

#include <QString>
#include <optional>
#include "../drivers/cswordmoduleinfo.h"


/** Contains functions to work with references used for Drag & Drop and for hyperlinks used in our
 * rendered HTML code.
 */
namespace ReferenceManager {
enum Type {
    Unknown, /**< Unknown */
    Bible, /**< Bibles */
    Commentary, /**< Commentary */
    Lexicon, /**< Lexicon */
    GenericBook, /**< Generic Book */
    MorphHebrew, /**< Module for hebrew morphology*/
    MorphGreek, /**< Module for greek morphology */
    StrongsHebrew, /**< Module for hebrew strongs */
    StrongsGreek, /**< Module for greek strongs */
};

struct DecodedHyperlink {
    Type type;
    CSwordModuleInfo * module;
    QString key;
};

/** Turn a hyperlink into module, key and type.
* Decodes the given hyperlink into module, key and type.
* @param hyperlink The hyperlink to decode
* @param module The string which will contain the module name after decoding
* @param key The string which will contain the key after decoding
* @param type The type param will contain the reference type after decoding
*/
std::optional<DecodedHyperlink> decodeHyperlink(QString const & hyperlink);

/**
* Returns a hyperlink used to be embedded in the display windows.
* At the moment the format is sword://module/key
* @param module The module to which to link.
* @param key The key which is used to encode the hyperlink
* @return The encoded hyperlink
*/
QString encodeHyperlink(CSwordModuleInfo const & module, QString const & key);

struct ParseOptions {
    QString refDestinationModule;
    /* The following are only valid for verse-based destination modules: */
    QString refBase;
    QString sourceLanguage;
};

/** Parses the given verse references using the given language and the module.
* @param moduleName The name of the module to use. Required for the language checking before parsing the key.
* @param ref The verse reference.
* @param lang The language of the verse reference
* @param newLang The language of the reference, which will be returned. For example: If BibleTime using an english environment parses a spanish ref (lang=es) the returned ref should be in english (newLang=en), because his english standard module only understands en.
*/
QString parseVerseReference(QString const & ref, ParseOptions const & options);

}
