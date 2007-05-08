/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cfiltertool.h"
#include "creferencemanager.h"

#include <qregexp.h>
#include <qstring.h>


#include <string>

// using std::string;
//
// CFilterTool::CFilterTool() {
//   updateSettings();
// }
//
// CFilterTool::~CFilterTool(){
// }
//
// void CFilterTool::updateSettings(){
//  m_standardBible = CReferenceManager::preferredModule(CReferenceManager::Bible);
// }
//
// const sword::SWBuf CFilterTool::thmlRefEnd(){
//  return "</a>";
// }
//
// char CFilterTool::ProcessRWPRefs(sword::SWBuf & buf, sword::SWModule* const module){
//   /** Markup verse refs which are marked by #ref1[,;]ref2|
//   *
//   * 1. Search start marker (#)
//   * 2. Search end marker (|)
//   * 3. Replace found ref by parsed result!
//   */
//
//   std::string target(buf.c_str());
//   unsigned int idx_start = target.find_first_of("#",0); //find ref start
//   unsigned int idx_end;
//
//   while (idx_start != std::string::npos) {
//     idx_end = target.find_first_of("|", idx_start); //find end marker
//
//     if ((idx_end != std::string::npos) && (idx_end > idx_start+1)) { //found marker with content
//       // Our length of the ref without markers is idx_end - (idx_start+1) = idx_end - idx_start - 1
//
//       // Parse ref without start and end markers!
//       const string ref = string(parseRef( target.substr(idx_start + 1, idx_end - idx_start - 1).c_str(),  module, 0 ).c_str());
//
//       // Replace original ref sourrounded by # and | by the parsed ref in target!
//       target.replace( idx_start, idx_end - idx_start + 1, ref ); //remove marker, too
//
//       // Start searching for next ref start behind current one! It's faster!
//       idx_start += ref.length();
//     }
//
//     idx_start = target.find_first_of("#", idx_start); //find ref start
//   };
//
//   buf = target.c_str(); //copy new content back into text!
//   return 0;
// }
//
// /** Parses the verse reference ref and returns it. */
// const sword::SWBuf CFilterTool::parseRef(const sword::SWBuf ref, sword::SWModule * const module, sword::SWKey * const key, const char* lang, const bool insertFullRef){
//   /**
//   * This function should be able to parse references like "John 1:3; 3:1-10; Matthew 1:1-3:3"
//   * without problems.
//   */
//   const sword::SWBuf moduleName( module ? module->Name() : m_standardBible.latin1() );
//
//   sword::VerseKey parseKey;
//   parseKey.setLocale( lang ? lang : (module ? module->Lang() : "en") ); //we assume that the keys are in english or in the module's language
//
//   parseKey = key ? (const char*)*key : "Genesis 1:1"; //use the current key if there's any
//
//   sword::ListKey list;
//   sword::SWBuf ret;
//
//   QStringList refList = QStringList::split(QRegExp("[,.;]|(?:\\s(?=\\d?[A-Z]))", false), QString::fromUtf8(ref.c_str()));
//  int pos = 0;
//
//   if ( !insertFullRef && (refList.count() >= 1) ) {
//     /* We can't handle refs which defined their own caption if we would have to insert multiple refs
//     * If our own caption should not be inserted and we have more than one ref return so the original
//     * ref and caption will be used.
//     */
//     return sword::SWBuf("<a class=\"reference\" href=\"sword://Bible/") + moduleName + "/" + ref + "\">";
//   }
//
//  QStringList::const_iterator end = refList.constEnd();
//  for ( QStringList::const_iterator it = refList.begin(); it != end; ++it, pos++ ) {
//    list = parseKey.ParseVerseList((*it).local8Bit(), parseKey, true);
//
//    const int count = list.Count();
//     sword::SWKey* key = 0;
//    for(int i = 0; i < count; i++) {
//     key = list.GetElement(i);
// //      qWarning("%s (%s) [%s] (%s)", key->getRangeText(), ref.c_str(), parseKey.getLocale(), module->Lang());
//     ret += sword::SWBuf("<a id=\"reference\" href=\"sword://Bible/") + moduleName + "/";
//     if ( sword::VerseKey* vk = dynamic_cast<sword::VerseKey*>(key) ) {
//      vk->setLocale("en");
//         ret += sword::SWBuf(vk->getRangeText()) + "\">";
//         parseKey = *vk;
//       }
//       else {
//         sword::VerseKey vk = key->getText();
//      vk.setLocale("en");
//         ret += sword::SWBuf(vk.getRangeText()) + "\">";
//         parseKey = vk;
//       }
//
//       /* This is kind of a hack. This will only work if we process simple refs which won't get splitted.
//       * But since insertFullRef is true by default, the end markers will be left out only in special cases like ThML refs
//       * with own caption.
//       */
//       if (insertFullRef) { //HTML will only be valid if we hide only the end of one cross reference
//         ret += sword::SWBuf( (const char*)(*it).utf8() ) + "</a>";
//       (pos+1 < (int)refList.count()) ? ret.append(", ") : ret.append("");
//       }
//    }
//
//  }
//   return ret;
// }
//
// const sword::SWBuf CFilterTool::parseThMLRef(const sword::SWBuf& ref, const char* mod) {
//   updateSettings();
//
//   const char* moduleName = (mod ? mod : m_standardBible.latin1());
//   sword::SWModule* module = 0;
//   if ( CSwordModuleInfo* m = CPointers::backend()->findModuleByName(moduleName) ) {
//     module = m->module();
//   }
//   return parseRef( ref, module, 0, module ? module->Lang() : "en", false );
// }
//
// const sword::SWBuf CFilterTool::parseSimpleRef(const sword::SWBuf& ref, const char* lang) {
//   updateSettings();
//
//   if ( CSwordModuleInfo* m = CPointers::backend()->findModuleByName(m_standardBible.latin1()) ) {
//     return parseRef( ref, m->module(), 0, lang);
//   }
//   return sword::SWBuf();
// }
