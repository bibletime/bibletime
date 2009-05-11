/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cswordlexiconmoduleinfo.h"
#include "util/directoryutil.h"

//Qt
#include <QFile>
#include <QDataStream>
#include <QTextCodec>

//Sword
#include <swmodule.h>

//STL includes
#include <algorithm>

//Change it once the format changed to make all systems rebuild their caches
#define CACHE_FORMAT "2"

CSwordLexiconModuleInfo::CSwordLexiconModuleInfo( sword::SWModule* module, CSwordBackend* const backend ) : CSwordModuleInfo(module, backend) {
	m_entryList = 0;
}

CSwordLexiconModuleInfo::CSwordLexiconModuleInfo( const CSwordLexiconModuleInfo& m ) : CSwordModuleInfo(m) {
	delete m_entryList;
	m_entryList = 0;

	if (m.m_entryList) {
		m_entryList = new QStringList();
		*m_entryList = *m.m_entryList;//copy list items
	}
}

CSwordLexiconModuleInfo::~CSwordLexiconModuleInfo() {
	delete m_entryList;
	m_entryList = 0;
}

/** Returns the entries of the module. */
QStringList* CSwordLexiconModuleInfo::entries() {
	if (!module()) {
		return 0;
	}

	sword::SWModule* my_module = module();
	bool is_unicode = isUnicode();

	if (!m_entryList) {
		m_entryList = new QStringList();
		bool read = false;

		//Check for buggy modules! They will not be loaded any more.

		if ( name() == QString("ZhEnglish")) {
			qWarning("Module ZhEnglish is buggy and will not be loaded.");
			return m_entryList;
		}

		if ( name() == QString("EReo_en")) {
			qWarning("Module EReo_en is buggy and will not be loaded.");
			return m_entryList;
		}

		QString dir(util::filesystem::DirectoryUtil::getUserCacheDir().absolutePath());
		QFile f1(
			QString(dir)
			.append("/")
			.append(name())
		);

		if ( f1.open( QIODevice::ReadOnly ) ) {
			QDataStream s( &f1 );
			QString mod_ver, prog_ver;
			s >> mod_ver;
			s >> prog_ver;

			if ((mod_ver == config(ModuleVersion)) && (prog_ver == CACHE_FORMAT)) {
				s >> *m_entryList;
				read = true;
			}

			f1.close();
			//    qWarning("read entries %d",m_entryList->count());
		}

		//   Q_ASSERT(read);
		//   Q_ASSERT(m_entryList->count());
		if (!read || !m_entryList->count()) {
			my_module->setSkipConsecutiveLinks(true);
			(*my_module) = sword::TOP;
			snap(); //snap to top entry

			//    qWarning("Reading in module" );
			int i = 0;

			do {
				if ( is_unicode ) {
					m_entryList->append(QString::fromUtf8(my_module->KeyText()));
					//      qWarning("Entry: %s", my_module->KeyText() );
				}
				else { //for latin1 modules use fromLatin1 because of speed
					//           m_entryList->append(QString::fromLatin1(my_module->KeyText()));
					QTextCodec* codec = QTextCodec::codecForName("Windows-1252");
					m_entryList->append(codec->toUnicode(my_module->KeyText()));
				}

				(*my_module)++;
				i++;
			}
			while ( !my_module->Error() );

			//     qWarning("Reading finished. Module has %d entries.", i );

			(*my_module) = sword::TOP; //back to the first entry

			my_module->setSkipConsecutiveLinks(false);

			if (m_entryList->count()) {
				m_entryList->first().simplified();

				if (m_entryList->first().trimmed().isEmpty()) {
					m_entryList->erase( m_entryList->begin() );
				}

				//now sort the list, this is necesssary because Sword doesn't do Unicode ordering
				//     qWarning("sorting");
				//      QStringList::iterator start(m_entryList->begin());
				//      QStringList::iterator end(m_entryList->end());
				//      std::sort( start, end, myLocaleAwareCompare() ); //stl sort
				//      m_entryList->sort(); //make sure the module is sorted by utf-8
			}

			qWarning("Writing cache file.");

			if (m_entryList->count()) {
				//create cache
				QString dir = util::filesystem::DirectoryUtil::getUserCacheDir().absolutePath();
				//QFile f2( QString::fromLatin1("%1/%2").arg(dir).arg( name() ) );
				QFile f2( QString(dir).append("/").append(name()) );


				if (f2.open( QIODevice::WriteOnly )) {
					QDataStream s( &f2 );
					s << config(CSwordModuleInfo::ModuleVersion); //store module version
					s << QString(CACHE_FORMAT); //store BT version -- format may change
					s << *m_entryList;
					f2.close();
				}
			}
		}
	}

	return m_entryList;
}

/** Jumps to the closest entry in the module.  */
bool CSwordLexiconModuleInfo::snap() {
	if(module()->getRawEntry()) { // Snap to the current entry
		return true;
	}

	return false;
}

/** No descriptions */
CSwordModuleInfo* CSwordLexiconModuleInfo::clone() {
	return new CSwordLexiconModuleInfo(*this);
}
