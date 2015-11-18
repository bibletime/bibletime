# Clucene from Qt distribution required:
# 1. download qtttols (https://qt.gitorious.org/qt/qttools/archive-tarball/stable)
# 2. extract .\src\assistant\3rdparty\clucene folder somewhere or on the same level as BibleTime repository
# 3. make sure variable CLUCENE_PATH points to correct location, it  would be corrected in main *.pro file
#    or by passing additional argument "CLUCENE_PATH = ..." to qmake
# This library could not be built separately at moment
# Symbian: apply platfoms/symbian/btmini/compiler.h.diff to corresponding file

isEmpty(CLUCENE_PATH):CLUCENE_PATH = ../../../../clucene
isEmpty(CLUCENE_LIB):CLUCENE_LIB = libclucene.a

DEFINES += _CL_DISABLE_MULTITHREADING LUCENE_DISABLE_MEMTRACKING
!symbian:DEFINES += _UCS2


INCLUDEPATH += \
        $${CLUCENE_PATH}/src \
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
    $${CLUCENE_PATH}/src/CLucene/analysis/AnalysisHeader.cpp \
    $${CLUCENE_PATH}/src/CLucene/analysis/Analyzers.cpp \
    $${CLUCENE_PATH}/src/CLucene/analysis/standard/StandardAnalyzer.cpp \
    $${CLUCENE_PATH}/src/CLucene/analysis/standard/StandardFilter.cpp \
    $${CLUCENE_PATH}/src/CLucene/analysis/standard/StandardTokenizer.cpp \
    $${CLUCENE_PATH}/src/CLucene/config/gunichartables.cpp \
    $${CLUCENE_PATH}/src/CLucene/config/repl_lltot.cpp \
    $${CLUCENE_PATH}/src/CLucene/config/repl_tcscasecmp.cpp \
    $${CLUCENE_PATH}/src/CLucene/config/repl_tcslwr.cpp \
    $${CLUCENE_PATH}/src/CLucene/config/repl_tcstoll.cpp \
    $${CLUCENE_PATH}/src/CLucene/config/threads.cpp \
    $${CLUCENE_PATH}/src/CLucene/config/utf8.cpp \
    $${CLUCENE_PATH}/src/CLucene/debug/condition.cpp \
    $${CLUCENE_PATH}/src/CLucene/debug/error.cpp \
    $${CLUCENE_PATH}/src/CLucene/debug/memtracking.cpp \
    $${CLUCENE_PATH}/src/CLucene/document/Document.cpp \
    $${CLUCENE_PATH}/src/CLucene/document/Field.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/DocumentWriter.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/CompoundFile.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/FieldInfos.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/FieldsReader.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/FieldsWriter.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/IndexReader.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/IndexWriter.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/MultiReader.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/SegmentInfos.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/SegmentMergeInfo.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/SegmentMergeQueue.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/SegmentMerger.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/SegmentReader.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/SegmentTermDocs.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/SegmentTermEnum.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/SegmentTermPositions.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/SegmentTermVector.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/Term.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/TermInfo.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/TermInfosReader.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/TermInfosWriter.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/TermVectorReader.cpp \
    $${CLUCENE_PATH}/src/CLucene/index/TermVectorWriter.cpp \
    $${CLUCENE_PATH}/src/CLucene/queryParser/Lexer.cpp \
    $${CLUCENE_PATH}/src/CLucene/queryParser/QueryParser.cpp \
    $${CLUCENE_PATH}/src/CLucene/queryParser/QueryParserBase.cpp \
    $${CLUCENE_PATH}/src/CLucene/queryParser/QueryToken.cpp \
    $${CLUCENE_PATH}/src/CLucene/queryParser/TokenList.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/BooleanQuery.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/BooleanScorer.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/ConjunctionScorer.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/ExactPhraseScorer.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/Explanation.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/FieldCache.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/FieldCacheImpl.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/FieldDocSortedHitQueue.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/FieldSortedHitQueue.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/FilteredTermEnum.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/FuzzyQuery.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/HitQueue.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/Hits.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/IndexSearcher.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/MultiTermQuery.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/PhrasePositions.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/PhraseQuery.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/PhraseScorer.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/PrefixQuery.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/Similarity.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/SloppyPhraseScorer.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/Sort.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/RangeQuery.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/SearchHeader.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/TermQuery.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/TermScorer.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/WildcardTermEnum.cpp \
    $${CLUCENE_PATH}/src/CLucene/search/WildcardQuery.cpp \
    $${CLUCENE_PATH}/src/CLucene/StdHeader.cpp \
    $${CLUCENE_PATH}/src/CLucene/store/IndexInput.cpp \
    $${CLUCENE_PATH}/src/CLucene/store/IndexOutput.cpp \
    $${CLUCENE_PATH}/src/CLucene/store/FSDirectory.cpp \
    $${CLUCENE_PATH}/src/CLucene/store/Lock.cpp \
    $${CLUCENE_PATH}/src/CLucene/store/TransactionalRAMDirectory.cpp \
    $${CLUCENE_PATH}/src/CLucene/store/RAMDirectory.cpp \
    $${CLUCENE_PATH}/src/CLucene/util/BitSet.cpp \
    $${CLUCENE_PATH}/src/CLucene/util/Equators.cpp \
    $${CLUCENE_PATH}/src/CLucene/util/FastCharStream.cpp \
    $${CLUCENE_PATH}/src/CLucene/util/fileinputstream.cpp \
    $${CLUCENE_PATH}/src/CLucene/util/Misc.cpp \
    $${CLUCENE_PATH}/src/CLucene/util/StringBuffer.cpp \
    $${CLUCENE_PATH}/src/CLucene/util/StringIntern.cpp \
    $${CLUCENE_PATH}/src/CLucene/util/Reader.cpp \
    $${CLUCENE_PATH}/src/CLucene/util/ThreadLocal.cpp \


!android:!windows:SOURCES += $${CLUCENE_PATH}/src/CLucene/config/repl_tprintf.cpp
}
