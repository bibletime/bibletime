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

#ifndef REFERENCEMANAGER_H
#define REFERENCEMANAGER_H

#include <QString>
#include "../drivers/cswordmoduleinfo.h"


/** Contains functions to work with references used for Drag & Drop and for hyperlinks used in our
 * rendered HTML code.
 */
namespace ReferenceManager {
enum Type {
    Bible, /**< Bibles */
    Commentary, /**< Commentary */
    Lexicon, /**< Lexicon */
    GenericBook, /**< Generic Book */
    MorphHebrew, /**< Module for hebrew morphology*/
    MorphGreek, /**< Module for greek morphology */
    StrongsHebrew, /**< Module for hebrew strongs */
    StrongsGreek, /**< Module for greek strongs */
    Unknown /**< Unknown */
};

/** Turn a hyperlink into module, key and type.
* Decodes the given hyperlink into module, key and type.
* @param hyperlink The hyperlink to decode
* @param module The string which will contain the module name after decoding
* @param key The string which will contain the key after decoding
* @param type The type param will contain the reference type after decoding
*/
bool decodeHyperlink( const QString& hyperlink, QString& module, QString& key, Type& type);
/**
* Returns a hyperlink used to be embedded in the display windows.
* At the moment the format is sword://module/key
* @param module The module which is used to encode the hyperlink
* @param key The key which is used to encode the hyperlink
* @param type The type which is used to encode the hyperlink
* @return The encoded hyperlink
*/
const QString encodeHyperlink( const QString module, const QString key, const Type type);

/**
* Returns true if the parameter is a hyperlink.
* @param hyperlink The string which is tested
* @return True if the passed string is a hyperlink
*/
bool isHyperlink( const QString& hyperlink );
/**
* Returns the preferred module name for the given type.
* @param type The type which is used to find the module
* @return The default module name for the passed type
*/
const QString preferredModule( const Type type );
/**
* Returns the type of the passed module type
* @param type The CSwordModuleInfo module typpe
* @return The ReferenceManager type
*/
ReferenceManager::Type typeFromModule( const CSwordModuleInfo::ModuleType type );


struct ParseOptions {

/* Methods: */

    inline ParseOptions(QString const & refDestinationModule_ = QString(),
                        QString const & refBase_ = QString(),
                        QString const & sourceLanguage_ = QString(),
                        QString const & destinationLanguage_ = "en")
        : refDestinationModule(refDestinationModule_)
        , refBase(refBase_)
        , sourceLanguage(sourceLanguage_)
        , destinationLanguage(destinationLanguage_)
    {}

/* Fields: */

    QString refDestinationModule;
    /* The following are only valid for verse-based destination modules: */
    QString refBase;
    QString sourceLanguage;
    QString destinationLanguage;

};

/** Parses the given verse references using the given language and the module.
* @param moduleName The name of the module to use. Required for the language checking before parsing the key.
* @param ref The verse reference.
* @param lang The language of the verse reference
* @param newLang The language of the reference, which will be returned. For example: If BibleTime using an english environment parses a spanish ref (lang=es) the returned ref should be in english (newLang=en), because his english standard module only understands en.
*/
const QString parseVerseReference( const QString& ref, const ParseOptions& options);

};

#endif

