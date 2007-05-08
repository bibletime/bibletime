/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CREFERENCEMANAGER_H
#define CREFERENCEMANAGER_H

//Qt includes
#include <qstring.h>

#include "cswordmoduleinfo.h"

/** Contains static functions to work with referneces used for Drag & Drop and for hyperlinks used in our
 * rendered HTML code.
 * @author The BibleTime team
 */

class CReferenceManager {

public:
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
	static const bool decodeHyperlink( const QString& hyperlink, QString& module, QString& key, Type& type);
	/**
	* Returns a hyperlink used to be imbedded in the display windows.
	* At the moment the format is sword://module/key
	* @param module The module which is used to encode the hyperlink
	* @param key The key which is used to encode the hyperlink
	* @param type The type which is used to encode the hyperlink
	* @return The encoded hyperlink
	*/
	static const QString encodeHyperlink( const QString module, const QString key, const Type type);
	/**
	* Puts a module Name and a Reference together in the 'draggable' form
	* (module)reference
	* @param module The name of the module
	* @param reference The key reference as text
	* @return The encoded reference using module and reference
	* @author Martin Gruner
	*/
	static const QString encodeReference(const QString &module, const QString &reference);
	/**
	* decodes a 'draggable' reference into a modulename and a reference
	* @author Martin Gruner
	*/
	static void decodeReference(QString &dragreference, QString &module, QString &reference);
	/**
	* Returns true if the parameter is a hyperlink.
	* @param hyperlink The string which is tested
	* @return True if the passed string is a hyperlink
	*/
	static const bool isHyperlink( const QString& hyperlink );
	/**
	* Returns the preferred module name for the given type.
	* @param type The type which is used to find the module
	* @return The default module name for the passed type
	*/
	static const QString preferredModule( const Type type );
	/**
	* Returns the type of the passed module type
	* @param type The CSwordModuleInfo module typpe
	* @return The ReferenceManager type
	*/
	static CReferenceManager::Type typeFromModule( const CSwordModuleInfo::ModuleType type );


	struct ParseOptions {
		QString refDestinationModule;
		QString refBase; /* only valid for verse based destination modules*/
		QString sourceLanguage; /* only valid for verse based destination modules*/
		QString destinationLanguage; /* only valid for verse based destination modules*/

		ParseOptions() {
			destinationLanguage = "en";
		};
	};

	/** Parses the given verse references using the given language and the module.
	* @param moduleName The name of the module to use. Required for the language checking before parsing the key.
	* @param ref The verse reference.
	* @param lang The language of the verse reference
	* @param newLang The language of the reference, which will be returned. For example: If BibleTime using an english environment parses a spanish ref (lang=es) the returned ref should be in english (newLang=en), because his english standard module only understands en.
	*/
	static const QString parseVerseReference( const QString& ref, const ParseOptions& options);
};

#endif

