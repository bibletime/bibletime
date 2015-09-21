# Clucene from Qt distribution required:
# 1. download qtttols (https://qt.gitorious.org/qt/qttools/archive-tarball/stable)
# 2. extract .\src\assistant\3rdparty\clucene folder somewhere or on the same level as BibleTime repository
# 3. make sure variable CLUCENE_PATH points to correct location, it  would be corrected in main *.pro file
#    or by passing additional argument "CLUCENE_PATH = ..." to qmake
# This library could not be built separately at moment
# Symbian: apply platfoms/symbian/btmini/compiler.h.diff to corresponding file

isEmpty(CLUCENE_PATH):CLUCENE_PATH = ../../../../clucene/src
isEmpty(CLUCENE_LIB):CLUCENE_LIB = libclucene.a

DEFINES += _CL_DISABLE_MULTITHREADING LUCENE_DISABLE_MEMTRACKING
!symbian:DEFINES += _UCS2


INCLUDEPATH += \
        $${CLUCENE_PATH} \
        ../../common/clucene \

clucenelib {

}
else {
# Building lib
!clucene {
TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $${PWD}
}

SOURCES += \
    $${CLUCENE_PATH}/CLucene/analysis/AnalysisHeader.cpp \
    $${CLUCENE_PATH}/CLucene/analysis/Analyzers.cpp \
    $${CLUCENE_PATH}/CLucene/analysis/standard/StandardAnalyzer.cpp \
    $${CLUCENE_PATH}/CLucene/analysis/standard/StandardFilter.cpp \
    $${CLUCENE_PATH}/CLucene/analysis/standard/StandardTokenizer.cpp \
    $${CLUCENE_PATH}/CLucene/config/gunichartables.cpp \
    $${CLUCENE_PATH}/CLucene/config/repl_lltot.cpp \
    $${CLUCENE_PATH}/CLucene/config/repl_tcscasecmp.cpp \
    $${CLUCENE_PATH}/CLucene/config/repl_tcslwr.cpp \
    $${CLUCENE_PATH}/CLucene/config/repl_tcstoll.cpp \
    $${CLUCENE_PATH}/CLucene/config/threads.cpp \
    $${CLUCENE_PATH}/CLucene/config/utf8.cpp \
    $${CLUCENE_PATH}/CLucene/debug/condition.cpp \
    $${CLUCENE_PATH}/CLucene/debug/error.cpp \
    $${CLUCENE_PATH}/CLucene/debug/memtracking.cpp \
    $${CLUCENE_PATH}/CLucene/document/Document.cpp \
    $${CLUCENE_PATH}/CLucene/document/Field.cpp \
    $${CLUCENE_PATH}/CLucene/index/DocumentWriter.cpp \
    $${CLUCENE_PATH}/CLucene/index/CompoundFile.cpp \
    $${CLUCENE_PATH}/CLucene/index/FieldInfos.cpp \
    $${CLUCENE_PATH}/CLucene/index/FieldsReader.cpp \
    $${CLUCENE_PATH}/CLucene/index/FieldsWriter.cpp \
    $${CLUCENE_PATH}/CLucene/index/IndexReader.cpp \
    $${CLUCENE_PATH}/CLucene/index/IndexWriter.cpp \
    $${CLUCENE_PATH}/CLucene/index/MultiReader.cpp \
    $${CLUCENE_PATH}/CLucene/index/SegmentInfos.cpp \
    $${CLUCENE_PATH}/CLucene/index/SegmentMergeInfo.cpp \
    $${CLUCENE_PATH}/CLucene/index/SegmentMergeQueue.cpp \
    $${CLUCENE_PATH}/CLucene/index/SegmentMerger.cpp \
    $${CLUCENE_PATH}/CLucene/index/SegmentReader.cpp \
    $${CLUCENE_PATH}/CLucene/index/SegmentTermDocs.cpp \
    $${CLUCENE_PATH}/CLucene/index/SegmentTermEnum.cpp \
    $${CLUCENE_PATH}/CLucene/index/SegmentTermPositions.cpp \
    $${CLUCENE_PATH}/CLucene/index/SegmentTermVector.cpp \
    $${CLUCENE_PATH}/CLucene/index/Term.cpp \
    $${CLUCENE_PATH}/CLucene/index/TermInfo.cpp \
    $${CLUCENE_PATH}/CLucene/index/TermInfosReader.cpp \
    $${CLUCENE_PATH}/CLucene/index/TermInfosWriter.cpp \
    $${CLUCENE_PATH}/CLucene/index/TermVectorReader.cpp \
    $${CLUCENE_PATH}/CLucene/index/TermVectorWriter.cpp \
    $${CLUCENE_PATH}/CLucene/queryParser/Lexer.cpp \
    $${CLUCENE_PATH}/CLucene/queryParser/QueryParser.cpp \
    $${CLUCENE_PATH}/CLucene/queryParser/QueryParserBase.cpp \
    $${CLUCENE_PATH}/CLucene/queryParser/QueryToken.cpp \
    $${CLUCENE_PATH}/CLucene/queryParser/TokenList.cpp \
    $${CLUCENE_PATH}/CLucene/search/BooleanQuery.cpp \
    $${CLUCENE_PATH}/CLucene/search/BooleanScorer.cpp \
    $${CLUCENE_PATH}/CLucene/search/ConjunctionScorer.cpp \
    $${CLUCENE_PATH}/CLucene/search/ExactPhraseScorer.cpp \
    $${CLUCENE_PATH}/CLucene/search/Explanation.cpp \
    $${CLUCENE_PATH}/CLucene/search/FieldCache.cpp \
    $${CLUCENE_PATH}/CLucene/search/FieldCacheImpl.cpp \
    $${CLUCENE_PATH}/CLucene/search/FieldDocSortedHitQueue.cpp \
    $${CLUCENE_PATH}/CLucene/search/FieldSortedHitQueue.cpp \
    $${CLUCENE_PATH}/CLucene/search/FilteredTermEnum.cpp \
    $${CLUCENE_PATH}/CLucene/search/FuzzyQuery.cpp \
    $${CLUCENE_PATH}/CLucene/search/HitQueue.cpp \
    $${CLUCENE_PATH}/CLucene/search/Hits.cpp \
    $${CLUCENE_PATH}/CLucene/search/IndexSearcher.cpp \
    $${CLUCENE_PATH}/CLucene/search/MultiTermQuery.cpp \
    $${CLUCENE_PATH}/CLucene/search/PhrasePositions.cpp \
    $${CLUCENE_PATH}/CLucene/search/PhraseQuery.cpp \
    $${CLUCENE_PATH}/CLucene/search/PhraseScorer.cpp \
    $${CLUCENE_PATH}/CLucene/search/PrefixQuery.cpp \
    $${CLUCENE_PATH}/CLucene/search/Similarity.cpp \
    $${CLUCENE_PATH}/CLucene/search/SloppyPhraseScorer.cpp \
    $${CLUCENE_PATH}/CLucene/search/Sort.cpp \
    $${CLUCENE_PATH}/CLucene/search/RangeQuery.cpp \
    $${CLUCENE_PATH}/CLucene/search/SearchHeader.cpp \
    $${CLUCENE_PATH}/CLucene/search/TermQuery.cpp \
    $${CLUCENE_PATH}/CLucene/search/TermScorer.cpp \
    $${CLUCENE_PATH}/CLucene/search/WildcardTermEnum.cpp \
    $${CLUCENE_PATH}/CLucene/search/WildcardQuery.cpp \
    $${CLUCENE_PATH}/CLucene/StdHeader.cpp \
    $${CLUCENE_PATH}/CLucene/store/IndexInput.cpp \
    $${CLUCENE_PATH}/CLucene/store/IndexOutput.cpp \
    $${CLUCENE_PATH}/CLucene/store/FSDirectory.cpp \
    $${CLUCENE_PATH}/CLucene/store/Lock.cpp \
    $${CLUCENE_PATH}/CLucene/store/TransactionalRAMDirectory.cpp \
    $${CLUCENE_PATH}/CLucene/store/RAMDirectory.cpp \
    $${CLUCENE_PATH}/CLucene/util/BitSet.cpp \
    $${CLUCENE_PATH}/CLucene/util/Equators.cpp \
    $${CLUCENE_PATH}/CLucene/util/FastCharStream.cpp \
    $${CLUCENE_PATH}/CLucene/util/fileinputstream.cpp \
    $${CLUCENE_PATH}/CLucene/util/Misc.cpp \
    $${CLUCENE_PATH}/CLucene/util/StringBuffer.cpp \
    $${CLUCENE_PATH}/CLucene/util/StringIntern.cpp \
    $${CLUCENE_PATH}/CLucene/util/Reader.cpp \
    $${CLUCENE_PATH}/CLucene/util/ThreadLocal.cpp \


!android:!windows:SOURCES += $${CLUCENE_PATH}/CLucene/config/repl_tprintf.cpp
}
