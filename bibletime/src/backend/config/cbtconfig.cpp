/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

//BibleTime includes
#include "cbtconfig.h"

#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/clanguagemgr.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "backend/btmoduletreeitem.h"
#include "util/cpointers.h"
#include "util/directoryutil.h"
#include "frontend/searchdialog/btsearchoptionsarea.h"
#include "frontend/displaywindow/btactioncollection.h"

//Qt includes
#include <QApplication>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QList>
#include <QDebug>
#include <QSettings>
#include <QLocale>
#include <QWebSettings>

//Sword includes
#include <versekey.h> //for range configuration

//init statics
QFont* CBTConfig::m_defaultFont = 0;
CBTConfig::FontCache CBTConfig::fontConfigMap;

/*  No constructor and destructor, because this class only contains static methods.
  It won't be instantiated. */

QString CBTConfig::getKey( const CBTConfig::strings ID) {
	switch ( ID ) {
		case bibletimeVersion:		return "bibletimeVersion";
		case language:				return "language";
		case displayStyle:			return "displayStyle";
		case bookshelfCurrentItem:	return "bookshelfCurrentItem";
	}
	Q_ASSERT(false);
	return QString::null;
}

QString CBTConfig::getDefault( const CBTConfig::strings ID) {
	switch ( ID ) {
		case bibletimeVersion:		return "0.0"; // main() will realize this and set the value to VERSION
		case language:				return QLocale::system().name();
		case displayStyle:			return CDisplayTemplateMgr::defaultTemplate();
		case bookshelfCurrentItem:	return QString();
	}
	return QString::null;
}

QString CBTConfig::getKey( const CBTConfig::modules ID) {
	switch ( ID ) {
		case standardBible:			return "standardBible";
		case standardCommentary:	return "standardCommentary";
		case standardLexicon:		return "standardLexicon";
		case standardDailyDevotional:		return "standardDailyDevotional";
		case standardHebrewStrongsLexicon:	return "standardHebrewLexicon";
		case standardGreekStrongsLexicon:	return "standardGreekLexicon";
		case standardHebrewMorphLexicon:	return "standardHebrewMorphLexicon";
		case standardGreekMorphLexicon:		return "standardGreekMorphLexicon";
	}
	Q_ASSERT(false);
	return QString::null;
}

QString CBTConfig::getDefault( const CBTConfig::modules ID) {
	//  CSwordBackend* b = CPointers::backend();
	switch ( ID ) {
		case standardBible:				return "KJV";
		case standardCommentary:		return "MHC";
		case standardLexicon:			return "ISBE";
		case standardDailyDevotional:	return ""; //no default

		case standardHebrewStrongsLexicon:	return "StrongsHebrew";
		case standardGreekStrongsLexicon:	return "StrongsGreek";
		case standardHebrewMorphLexicon:	return "StrongsHebrew";
		case standardGreekMorphLexicon:		return "StrongsGreek";
	}

	return QString::null;
}

QString CBTConfig::getKey( const CBTConfig::bools ID) {
	switch ( ID ) {
		case firstSearchDialog:		return "firstSearchDialog";
		case readOldBookmarks:		return "readOldBookmarks";

		case toolbar:				return "toolbar";
		case mainIndex:				return "mainIndex";
		case infoDisplay:			return "infoDisplay";

		case autoTileVertical:		return "autoTileVertical";
		case autoTileHorizontal:	return "autoTileHorizontal";
		case autoCascade:			return "autoCascade";

		case lineBreaks:			return "lineBreaks";
		case verseNumbers:			return "verseNumbers";

		case logo:						return "logo";
		case autoDeleteOrphanedIndices:	return "autoDeleteOrphanedIndices";
		case crashedLastTime:			return "crashedLastTime";
		case crashedTwoTimes:			return "crashedTwoTimes";

		case bookshelfShowHidden:		return "bookshelfShowHidden";
		case allowNetworkConnection:		return "allowNetworkConnection";
	}
	Q_ASSERT(false);
	return false;
}

QString CBTConfig::getKey( const CBTConfig::ints ID) {
	switch ( ID ) {
		case footnotes:				return "footnotes";
		case strongNumbers:			return "strongNumbers";
		case headings:				return "headings";
		case morphTags:				return "morphTags";
		case lemmas:				return "lemmas";
		case hebrewPoints:			return "hebrewPoints";
		case hebrewCantillation:	return "hebrewCantillation";
		case greekAccents:			return "greekAccents";
		case textualVariants:		return "textualVariants";
		case scriptureReferences:	return "scriptureReferences";
		case morphSegmentation:		return "morphSegmentation";
		case bookshelfContentsX:	return "bookshelfContentsX";
		case bookshelfContentsY:	return "bookshelfContentsY";
		case magDelay:				return "magDelay";
		case bookshelfGrouping:		return "bookshelfGrouping";
		case mainindexActiveTab:	return "mainindexActiveTab";
		case searchDialogWidth:		return "searchDialogWidth";
		case searchDialogHeight:	return "searchDialogHeight";
		case searchDialogX:			return "searchDialogX";
		case searchDialogY:			return "searchDialogY";
		case searchType:			return "searchType";
		case installPathIndex:		return "installPathIndex";
		case bookshelfPosX:			return "bookshelfPosX";
		case bookshelfPosY:			return "bookshelfPosY";
		case bookshelfHeight:		return "bookshelfHeight";
		case bookshelfWidth:		return "bookshelfWidth";
		case configDialogPosX:		return "configDialogPosX";
		case configDialogPosY:		return "configDialogPosY";
		case configDialogHeight:	return "configDialogHeight";
		case configDialogWidth:		return "configDialogWidth";
	}
	Q_ASSERT(false);
	return QString::null;
}

bool CBTConfig::getDefault( const CBTConfig::bools ID) {
	switch ( ID ) {
		case firstSearchDialog:		return true;
		case readOldBookmarks:		return false;

		case toolbar:		return true;
		case mainIndex:		return true;
		case infoDisplay:		return true;

		case autoTileVertical:		return true;
		case autoTileHorizontal:	return false;
		case autoCascade:			return false;

		case lineBreaks:		return false;
		case verseNumbers:		return true;

		case logo:		return true;
		case autoDeleteOrphanedIndices:		return true;
		case crashedLastTime:		return false;
		case crashedTwoTimes:		return false;
		case bookshelfShowHidden:	return false;
		case allowNetworkConnection:	return false;
	}
	return false;
}

int CBTConfig::getDefault( const CBTConfig::ints ID) {
	switch ( ID ) {
		case footnotes:		return int(true);
		case strongNumbers:	return int(true);
		case headings:		return int(true);
		case morphTags:		return int(true);
		case lemmas:		return int(true);
		case hebrewPoints:	return int(true);
		case hebrewCantillation:	return int(true);
		case greekAccents:			return int(true);
		case textualVariants:		return int(false);
		case scriptureReferences:	return int(true);
		case morphSegmentation:		return int(true);
		case bookshelfContentsX:	return 0;
		case bookshelfContentsY:	return 0;
		case magDelay:				return 400;
		case bookshelfGrouping:		return BTModuleTreeItem::CatLangMod;
		case searchDialogWidth:		return 200;
		case searchDialogHeight:	return 400;
		case searchDialogX:			return 200;
		case searchDialogY:			return 200;
		case searchType:			return Search::BtSearchOptionsArea::AndType;
		case mainindexActiveTab:	return 0;
		case installPathIndex:  	return 0;
		case bookshelfPosX:  		return 1;
		case bookshelfPosY:  		return 1;
		case bookshelfHeight:  		return 1;
		case bookshelfWidth:  		return 1;
		case configDialogPosX:  	return 1;
		case configDialogPosY:  	return 1;
		case configDialogHeight:	return 1;
		case configDialogWidth: 	return 1;

	}
	return 0;
}

QString CBTConfig::getKey( const CBTConfig::intLists ID) {
	switch ( ID ) {
		case leftPaneSplitterSizes:	return "leftPaneSplitterSizes";
		case mainSplitterSizes:		return "mainSplitterSizes";
		case searchMainSplitterSizes:	return "searchMainSplitterSizes";
		case searchResultSplitterSizes:	return "searchResultSplitterSizes";
	}
	Q_ASSERT(false);
	return QString::null;
}

QList<int> CBTConfig::getDefault( const CBTConfig::intLists /*ID*/) {
	QList<int> result;
	/*switch ( ID ) {
		case leftPaneSplitterSizes: break;
		case mainSplitterSizes: break;
		case searchMainSplitterSizes: break;
		case searchResultSplitterSizes: break;*/
	return result;
}

QString CBTConfig::getKey( const CBTConfig::stringLists ID) {
	switch ( ID ) {
		case searchCompletionTexts:	return QString("searchCompletionTexts");
		case searchTexts:			return QString("searchTexts");
		case searchModulesHistory:	return QString("searchModulesHistory");
		case bookshelfOpenGroups:	return QString("bookshelfOpenGroups");
		case hiddenModules:			return QString("hiddenModules");
	}
	Q_ASSERT(false);
	return QString::null;
}

QStringList CBTConfig::getDefault( const CBTConfig::stringLists ID) {
	switch ( ID ) {
		case searchTexts: {
			QStringList list;
			list.append(QString::null);
			return list;
		}
		case searchCompletionTexts:		return QStringList();
		case bookshelfOpenGroups:		return QStringList();
		case hiddenModules:				return QStringList();
		case searchModulesHistory:				return QStringList();
	}
	return QStringList();
}

QString CBTConfig::getKey( const CBTConfig::stringMaps ID) {
	switch (ID) {
		case searchScopes:
		return QString("SearchScopes");
	};
	Q_ASSERT(false);
	return QString::null;
}

CBTConfig::StringMap CBTConfig::getDefault( const CBTConfig::stringMaps ID) {
	switch ( ID ) {
		case searchScopes: {
			CBTConfig::StringMap map;
			map.insert(QObject::tr("Old testament"),         QString("Gen - Mal"));
			map.insert(QObject::tr("Moses/Pentateuch/Torah"),QString("Gen - Deut"));
			map.insert(QObject::tr("History"),               QString("Jos - Est"));
			map.insert(QObject::tr("Prophets"),              QString("Isa - Mal"));
			map.insert(QObject::tr("New testament"),         QString("Mat - Rev"));
			map.insert(QObject::tr("Gospels"),               QString("Mat - Joh"));
			map.insert(QObject::tr("Letters/Epistles"),      QString("Rom - Jude"));
			map.insert(QObject::tr("Paul's Epistles"),       QString("Rom - Phile"));

			//make the list to the current bookname language!
			CBTConfig::StringMap::Iterator it;
			sword::VerseKey vk;
			vk.setLocale("en_US");

			for (it = map.begin(); it != map.end(); ++it) {
				sword::ListKey list = vk.ParseVerseList(it.value().toLocal8Bit(), "Genesis 1:1", true);
				QString data;
				for (int i = 0; i < list.Count(); ++i) {
					data += QString::fromUtf8(list.GetElement(i)->getRangeText()) + "; ";
				}
				map[it.key()] = data; //set the new data
			};

			return map;
		};
		default:
		return CBTConfig::StringMap();
	}

	return CBTConfig::StringMap();
}


QString CBTConfig::getKey( const CLanguageMgr::Language* const language ) {
	Q_ASSERT(!language->name().isEmpty());
	return language->name();
}

QFont& CBTConfig::getDefault( const CLanguageMgr::Language* const) 
{
	//language specific lookup of the font name
	if (m_defaultFont) 
	{
		return *m_defaultFont;
	}

// TODO - make the font name and size a configuration option
	//int fontSize = QWebSettings::globalSettings()->fontSize(QWebSettings::DefaultFontSize);
    int fontSize =  12;
	QString fontName = QWebSettings::globalSettings()->fontFamily(QWebSettings::StandardFont);

	m_defaultFont = new QFont(fontName, fontSize); //TODO: there may be a mem leak here!
	return *m_defaultFont;
}

QString CBTConfig::get( const CBTConfig::strings ID)
{
	QString result;
	getConfig()->beginGroup("strings");
	result = getConfig()->value(getKey(ID), getDefault(ID)).toString();
	getConfig()->endGroup();
	return result;

}

CSwordModuleInfo* CBTConfig::get( const CBTConfig::modules ID)
{
	CSwordModuleInfo* result;
	getConfig()->beginGroup("modules");
	result = CPointers::backend()->findModuleByName( getConfig()->value(getKey(ID), getDefault(ID)).toString() );
	getConfig()->endGroup();
	return result;
}

bool CBTConfig::get( const CBTConfig::bools ID)
{
	bool result;
	getConfig()->beginGroup("bools");
	result = getConfig()->value(getKey(ID), getDefault(ID)).toBool();
	getConfig()->endGroup();
	return result;
}

int CBTConfig::get( const CBTConfig::ints ID)
{
	int result;
	getConfig()->beginGroup("ints");
	result = getConfig()->value(getKey(ID), getDefault(ID)).toInt();
	getConfig()->endGroup();
	return result;
}

QList<int> CBTConfig::get( const CBTConfig::intLists ID )
{
	QList<int> result;
	getConfig()->beginGroup("intlists");
	result = StringToIntList( getConfig()->value(getKey(ID), IntListToString( getDefault(ID) )).toString() );
	getConfig()->endGroup();
	return result;
}

QStringList CBTConfig::get( const CBTConfig::stringLists ID )
{
	QStringList result;
	getConfig()->beginGroup("stringlists");
	result = getConfig()->value(getKey(ID), getDefault(ID)).toStringList();
	getConfig()->endGroup();
	return result;
}

CBTConfig::StringMap CBTConfig::get( const CBTConfig::stringMaps ID )
{
	getConfig()->beginGroup(getKey(ID));
	CBTConfig::StringMap map;

	QStringList keys(getConfig()->childKeys());
 	if (!keys.isEmpty()) {
		switch (ID) {
			case searchScopes: { //make sure we return the scopes in the chosen language. saved keys are in english
				sword::VerseKey vk;
				foreach (QString key, keys) {
					Q_ASSERT(!key.isEmpty());
					sword::ListKey list = vk.ParseVerseList(getConfig()->value(key).toString().toUtf8(), "Genesis 1:1", true);
					QString data;
					for (int i = 0; i < list.Count(); ++i) {
						data += QString::fromUtf8(list.GetElement(i)->getRangeText()) + "; ";
					}
					map[key] = data; //set the new data
				}
			}
			default: break;
		}
	}
	else
	{
		map = getDefault(ID);
	}
	getConfig()->endGroup();
	return map;
}

CBTConfig::FontSettingsPair CBTConfig::get( const CLanguageMgr::Language* const language )
{
	if (fontConfigMap.contains(language)) {
		return fontConfigMap.find(language).value();
	}

	FontSettingsPair settings;

	getConfig()->beginGroup("font standard settings");
	settings.first = getConfig()->value(getKey(language), QVariant(false)).toBool();
	getConfig()->endGroup();
	getConfig()->beginGroup("fonts");

	QFont font;
	if (settings.first)
		font.fromString(getConfig()->value(getKey(language), getDefault(language)).toString());
	else
		font = getDefault(language);

	settings.second = font;
	getConfig()->endGroup();

	fontConfigMap.insert(language, settings); //cache the value
	return settings;
}

void CBTConfig::set( const CBTConfig::strings ID, const QString value )
{
// 	KConfigGroup cg = CBTConfig::getConfig()->group("strings");
// 	cg.writeEntry(getKey(ID), value);
	getConfig()->beginGroup("strings");
	getConfig()->setValue(getKey(ID), value);
	getConfig()->endGroup();
}

void CBTConfig::set( const CBTConfig::modules ID, CSwordModuleInfo* const value )
{
// 	KConfigGroup cg = CBTConfig::getConfig()->group("modules");
// 	cg.writeEntry(getKey(ID), value ? value->name() : QString::null);
	getConfig()->beginGroup("modules");
	getConfig()->setValue(getKey(ID), value ? value->name() : QString::null);
	getConfig()->endGroup();
}

void CBTConfig::set( const CBTConfig::modules ID, const QString& value )
{
	CSwordModuleInfo* module = CPointers::backend()->findModuleByName(value);
	if (module) {
		CBTConfig::set(ID, module);
	}
}

void CBTConfig::set(const CBTConfig::bools ID,const  bool value )
{
	getConfig()->beginGroup("bools");
	getConfig()->setValue(getKey(ID), value);
	getConfig()->endGroup();
}

void CBTConfig::set(const CBTConfig::ints ID, const int value )
{
	getConfig()->beginGroup("ints");
	getConfig()->setValue(getKey(ID), value);
	getConfig()->endGroup();
}

void CBTConfig::set( const CBTConfig::intLists ID, const QList<int> value )
{
	getConfig()->beginGroup("intlists");
	getConfig()->setValue(getKey(ID), IntListToString(value));
	getConfig()->endGroup();
}

void CBTConfig::set( const CBTConfig::stringLists ID, const QStringList value )
{
	getConfig()->beginGroup("stringlists");
	getConfig()->setValue(getKey(ID), value);
	getConfig()->endGroup();
}

void CBTConfig::set( const CBTConfig::stringMaps ID, const CBTConfig::StringMap value )
{
	getConfig()->beginGroup(getKey(ID));
	getConfig()->remove(""); //clear all entries of this group to make sure old stuff gets removed

	switch (ID) {
		case searchScopes: {
			/**
			* We want to make sure that the search scopes are saved with english key names so loading them
			* will always work with each locale set.
			*/
			CBTConfig::StringMap::ConstIterator it;
			QString data;// = QString::null;

			sword::VerseKey vk;
			for (it = value.begin(); it != value.end(); ++it) {
				sword::ListKey list = vk.ParseVerseList(it.value().toUtf8(), "Genesis 1:1", true);
				data = QString::null;
				for (int i = 0; i < list.Count(); ++i) {
					if ( sword::VerseKey* range = dynamic_cast<sword::VerseKey*>(list.GetElement(i)) ) {
						range->setLocale("en");
						data += QString::fromUtf8( range->getRangeText() ) + ";";
					}
				}
				getConfig()->setValue(it.key(), data);
			}
			break;
		}
		default: {
			for (CBTConfig::StringMap::ConstIterator it = value.begin(); it != value.end(); ++it) {
				getConfig()->setValue(it.key(), it.value());
			}
			break;
		}
	}
	getConfig()->endGroup();
}


void CBTConfig::set( const CLanguageMgr::Language* const language, const FontSettingsPair& value )
{
	getConfig()->beginGroup("fonts");
	getConfig()->setValue(getKey(language), value.second.toString());
	getConfig()->endGroup();
	getConfig()->beginGroup("font standard settings");
	getConfig()->setValue(getKey(language), value.first);
	getConfig()->endGroup();

	if (fontConfigMap.contains(language)) {
		fontConfigMap.remove
		(language); //remove it from the cache
	}
}

CSwordBackend::DisplayOptions CBTConfig::getDisplayOptionDefaults()
{
	CSwordBackend::DisplayOptions options;
	options.lineBreaks   =  get(CBTConfig::lineBreaks);
	options.verseNumbers =  get(CBTConfig::verseNumbers);

	return options;
}

CSwordBackend::FilterOptions CBTConfig::getFilterOptionDefaults()
{
	CSwordBackend::FilterOptions options;

	options.footnotes = true; //required for the info display
	options.strongNumbers =    true; //get(CBTConfig::strongNumbers);
	options.headings = get(CBTConfig::headings);
	options.morphTags = true;//required for the info display
	options.lemmas = true;//required for the info display
	options.redLetterWords = true;
	options.hebrewPoints =     get(CBTConfig::hebrewPoints);
	options.hebrewCantillation =  get(CBTConfig::hebrewCantillation);
	options.greekAccents =     get(CBTConfig::greekAccents);
	options.textualVariants =   get(CBTConfig::textualVariants);
	options.scriptureReferences = get(CBTConfig::scriptureReferences);
	options.morphSegmentation  = get(CBTConfig::morphSegmentation);

	return options;
}

void CBTConfig::setupAccelSettings(const CBTConfig::keys type, BtActionCollection* const actionCollection)
{
 	qDebug("CBTConfig::setupAccelSettings");
 	QString groupName;
 	switch (type) {
 		case allWindows : {
 			groupName = "Displaywindow shortcuts";
 			break;
 		};
 		case writeWindow : {
 			groupName = "Writewindow shortcuts";
 			break;
 		};
 		case readWindow : {
 			groupName = "Readwindow shortcuts";
 			break;
 		};
 		case bookWindow : {
 			groupName = "Book shortcuts";
 			break;
 		};
 		case bibleWindow : {
 			groupName =  "Bible shortcuts";
 			break;
 		};
 		case commentaryWindow : {
 			groupName = "Commentary shortcuts";
 			break;
 		};
 		case lexiconWindow : {
 			groupName = "Lexicon shortcuts";
 			break;
 		};
 		case application : {
 			groupName = "Application shortcuts";
 			break;
 		};
 	};
 	qDebug() << groupName;
 	Q_ASSERT(CBTConfig::getConfig());

 	actionCollection->setConfigGroup(groupName);
 	actionCollection->readSettings();
 	qDebug("CBTConfig::setupAccelSettings end");
}

void CBTConfig::saveAccelSettings(const CBTConfig::keys type, BtActionCollection* const actionCollection)
{
 	qDebug("CBTConfig::saveAccelSettings");
 	QString groupName;
 	switch (type) {
 		case allWindows : {
 			groupName = "Displaywindow shortcuts";
 			break;
 		};
 		case writeWindow : {
 			groupName = "Writewindow shortcuts";
 			break;
 		};
 		case readWindow : {
 			groupName = "Readwindow shortcuts";
 			break;
 		};
 		case bookWindow : {
 			groupName = "Book shortcuts";
 			break;
 		};
 		case bibleWindow : {
 			groupName =  "Bible shortcuts";
 			break;
 		};
 		case commentaryWindow : {
 			groupName = "Commentary shortcuts";
 			break;
 		};
 		case lexiconWindow : {
 			groupName = "Lexicon shortcuts";
 			break;
 		};
 		case application : {
 			groupName = "Application shortcuts";
 			break;
 		};
 	};

 	actionCollection->setConfigGroup(groupName);
 	actionCollection->writeSettings();
 	qDebug("CBTConfig::saveAccelSettings end");
}


QString CBTConfig::getModuleEncryptionKey( const QString& module )
{
	Q_ASSERT(!module.isEmpty());
	QString result;
	getConfig()->beginGroup("Module keys");
	result = getConfig()->value(module, QVariant(QString::null)).toString();
	getConfig()->endGroup();
	return result;
}

void CBTConfig::setModuleEncryptionKey( const QString& module, const QString& key )
{
	getConfig()->beginGroup("Module keys");
	getConfig()->setValue(module, key);
	getConfig()->endGroup();
}

QSettings* CBTConfig::getConfig()
{
	static QSettings config(util::filesystem::DirectoryUtil::getUserBaseDir().absolutePath() + "/bibletimerc", QSettings::IniFormat);
	return &config;
}

void CBTConfig::syncConfig()
{
	CBTConfig::getConfig()->sync();
}

QString CBTConfig::IntListToString( const QList<int> intList )
{
	QStringList intStrings;
	foreach(int i, intList)
	{
		intStrings << QString::number(i);
	}
	return intStrings.join(",");
}

QList<int> CBTConfig::StringToIntList( const QString intListString )
{
	QList<int> intList;
	if (!intListString.isEmpty() && intListString.contains(','))
	{
		foreach(QString intString, intListString.split(","))
		{
			intList << intString.trimmed().toInt();
		}
	}
	return intList;
}
