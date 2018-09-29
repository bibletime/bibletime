/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "frontend/searchdialog/btsearchresultarea.h"

#include <QApplication>
#include <QFrame>
#include <QMenu>
#include <QProgressDialog>
#include <QPushButton>
#include <QSize>
#include <QSplitter>
#include <QStringList>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QWidget>
#include "backend/keys/cswordversekey.h"
#include "backend/rendering/cdisplayrendering.h"
#include "backend/config/btconfig.h"
#include "frontend/searchdialog/cmoduleresultview.h"
#include "frontend/searchdialog/csearchdialog.h"
#include "frontend/searchdialog/csearchresultview.h"
#include "util/btassert.h"
#include "util/btconnect.h"
#include "util/tool.h"


namespace {
const QString MainSplitterSizesKey = "GUI/SearchDialog/SearchResultsArea/mainSplitterSizes";
const QString ResultSplitterSizesKey = "GUI/SearchDialog/SearchResultsArea/resultSplitterSizes";
} // anonymous namespace

namespace Search {

BtSearchResultArea::BtSearchResultArea(QWidget *parent)
        : QWidget(parent) {
    initView();
    initConnections();
}

void BtSearchResultArea::initView() {
    QVBoxLayout *mainLayout;
    QWidget *resultListsWidget;
    QVBoxLayout *resultListsWidgetLayout;

    //Size is calculated from the font rather than set in pixels,
    // maybe this is better in different kinds of displays?
    int mWidth = util::tool::mWidth(this, 1);
    this->setMinimumSize(QSize(mWidth*40, mWidth*15));
    mainLayout = new QVBoxLayout(this);
    m_mainSplitter = new QSplitter(this);
    m_mainSplitter->setOrientation(Qt::Horizontal);

    resultListsWidget = new QWidget(m_mainSplitter);

    resultListsWidgetLayout = new QVBoxLayout(resultListsWidget);
    resultListsWidgetLayout->setContentsMargins(0, 0, 0, 0);

    //Splitter for two result lists
    m_resultListSplitter = new QSplitter(resultListsWidget);
    m_resultListSplitter->setOrientation(Qt::Vertical);
    m_moduleListBox = new CModuleResultView(m_resultListSplitter);
    m_resultListSplitter->addWidget(m_moduleListBox);
    m_resultListBox = new CSearchResultView(m_resultListSplitter);
    m_resultListSplitter->addWidget(m_resultListBox);
    resultListsWidgetLayout->addWidget(m_resultListSplitter);

    m_mainSplitter->addWidget(resultListsWidget);

    //Preview ("info") area
    m_displayFrame = new QFrame(m_mainSplitter);
    m_displayFrame->setFrameShape(QFrame::NoFrame);
    m_displayFrame->setFrameShadow(QFrame::Plain);
    m_mainSplitter->addWidget(m_displayFrame);

    mainLayout->addWidget(m_mainSplitter);

    QVBoxLayout* frameLayout = new QVBoxLayout(m_displayFrame);
    frameLayout->setContentsMargins(0, 0, 0, 0);
    m_previewDisplay = new QTextBrowser(this);
    m_previewDisplay->setToolTip(tr("Text of the selected search result item"));
    frameLayout->addWidget(m_previewDisplay);

    m_previewDisplay->setContextMenuPolicy(Qt::CustomContextMenu);
    BT_CONNECT(m_previewDisplay, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(slotContextMenu(const QPoint&)));

    loadDialogSettings();
}

void BtSearchResultArea::slotContextMenu(const QPoint& /* point */ ) {

    QAction selectAllAction(tr("Select all"));
    selectAllAction.setShortcut(QKeySequence::SelectAll);
    BT_CONNECT(&selectAllAction, SIGNAL(triggered()),
               this,            SLOT(selectAll()));

    QAction copyAction(tr("Copy"));
    copyAction.setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    BT_CONNECT(&copyAction,                     SIGNAL(triggered()),
               m_previewDisplay, SLOT(copy()));

    QMenu menu;
    menu.addAction(&selectAllAction);
    menu.addAction(&copyAction);
    menu.exec(QCursor::pos());
}

void BtSearchResultArea::setSearchResult(
        const CSwordModuleSearch::Results &results)
{
    const QString searchedText = CSearchDialog::getSearchDialog()->searchText();
    reset(); //clear current modules

    m_results = results;

    // Populate listbox:
    m_moduleListBox->setupTree(results, searchedText);

    // Pre-select the first module in the list:
    m_moduleListBox->setCurrentItem(m_moduleListBox->topLevelItem(0), 0);

    BT_ASSERT(qobject_cast<CSearchDialog *>(parent()));
    static_cast<CSearchDialog*>(parent())->m_analyseButton->setEnabled(true);
}

void BtSearchResultArea::reset() {
    m_moduleListBox->clear();
    m_resultListBox->clear();
    m_previewDisplay->setText("<html><head/><body></body></html>");
    qobject_cast<CSearchDialog*>(parent())->m_analyseButton->setEnabled(false);
}

void BtSearchResultArea::clearPreview() {
    m_previewDisplay->setText("<html><head/><body></body></html>");
}

void BtSearchResultArea::updatePreview(const QString& key) {
    using namespace Rendering;

    CSwordModuleInfo* module = m_moduleListBox->activeModule();
    if ( module ) {
        const QString searchedText = CSearchDialog::getSearchDialog()->searchText();

        QString text;
        CDisplayRendering render;

        BtConstModuleList modules;
        modules.append(module);

        CTextRendering::KeyTreeItem::Settings settings;

        //for bibles render 5 context verses
        if (module->type() == CSwordModuleInfo::Bible) {
            CSwordVerseKey vk(module);
            vk.setIntros(true);
            vk.setKey(key);

            // HACK: enable headings for VerseKeys:
            static_cast<sword::VerseKey *>(module->module().getKey())
                    ->setIntros(true);

            //first go back and then go forward the keys to be in context
            vk.previous(CSwordVerseKey::UseVerse);
            vk.previous(CSwordVerseKey::UseVerse);

            //include Headings in display, they are indexed and searched too
            if (vk.getVerse() == 1) {
                if (vk.getChapter() == 1) {
                    vk.setChapter(0);
                }
                vk.setVerse(0);
            }

            const QString startKey = vk.key();

            vk.setKey(key);

            vk.next(CSwordVerseKey::UseVerse);
            vk.next(CSwordVerseKey::UseVerse);
            const QString endKey = vk.key();

            settings.keyRenderingFace = CTextRendering::KeyTreeItem::Settings::CompleteShort;
            text = render.renderKeyRange(startKey, endKey, modules, key, settings);
        }
        //for commentaries only one verse, but with heading
        else if (module->type() == CSwordModuleInfo::Commentary) {
            CSwordVerseKey vk(module);
            vk.setIntros(true);
            vk.setKey(key);

            // HACK: enable headings for VerseKeys:
            static_cast<sword::VerseKey *>(module->module().getKey())
                    ->setIntros(true);

            //include Headings in display, they are indexed and searched too
            if (vk.getVerse() == 1) {
                if (vk.getChapter() == 1) {
                    vk.setChapter(0);
                }
                vk.setVerse(0);
            }
            const QString startKey = vk.key();

            vk.setKey(key);
            const QString endKey = vk.key();

            settings.keyRenderingFace = CTextRendering::KeyTreeItem::Settings::NoKey;
            text = render.renderKeyRange(startKey, endKey, modules, key, settings);
        }
        else {
            text = render.renderSingleKey(key, modules, settings);
        }

        if (modules.count() > 0)
            setBrowserFont(modules.at(0));

        QString text2 = CSwordModuleSearch::highlightSearchedText(text, searchedText);
        text2.replace("#CHAPTERTITLE#", "");
        m_previewDisplay->setText(text2);
        m_previewDisplay->scrollToAnchor( CDisplayRendering::keyToHTMLAnchor(key) );
    }
}

void BtSearchResultArea::setBrowserFont(const CSwordModuleInfo* const module) {
    if (module) {
            const CLanguageMgr::Language* lang = module->language();
            m_previewDisplay->setFont(btConfig().getFontForLanguage(*lang).second);
    } else {
        m_previewDisplay->setFont(btConfig().getDefaultFont());
    }
}

/** Initializes the signal slot conections of the child widgets, */
void BtSearchResultArea::initConnections() {
    BT_CONNECT(m_resultListBox, SIGNAL(keySelected(QString const &)),
               this,            SLOT(updatePreview(QString const &)));
    BT_CONNECT(m_resultListBox, SIGNAL(keyDeselected()), this, SLOT(clearPreview()));
    BT_CONNECT(m_moduleListBox,
               SIGNAL(moduleSelected(CSwordModuleInfo const *,
                                     sword::ListKey const &)),
               m_resultListBox,
               SLOT(setupTree(CSwordModuleInfo const *,
                              sword::ListKey const &)));
    BT_CONNECT(m_moduleListBox,                      SIGNAL(moduleChanged()),
               m_previewDisplay, SLOT(clear()));

    // connect the strongs list
    BT_CONNECT(m_moduleListBox,
               SIGNAL(strongsSelected(CSwordModuleInfo *, QStringList const &)),
               m_resultListBox,
               SLOT(setupStrongsTree(CSwordModuleInfo *, QStringList const &)));
}

/**
* Load the settings from the resource file
*/
void BtSearchResultArea::loadDialogSettings() {
    QList<int> mainSplitterSizes = btConfig().value< QList<int> >(MainSplitterSizesKey, QList<int>());
    if (mainSplitterSizes.count() > 0)
        m_mainSplitter->setSizes(mainSplitterSizes);
    else
    {
        int w = this->size().width();
        int w2 = m_moduleListBox->sizeHint().width();
        mainSplitterSizes << w2 << w - w2;
        m_mainSplitter->setSizes(mainSplitterSizes);
    }

    QList<int> resultSplitterSizes = btConfig().value< QList<int> >(ResultSplitterSizesKey, QList<int>());
    if (resultSplitterSizes.count() > 0)
        m_resultListSplitter->setSizes(resultSplitterSizes);
}

/**
* Save the settings to the resource file
*/
void BtSearchResultArea::saveDialogSettings() const {
    btConfig().setValue(MainSplitterSizesKey, m_mainSplitter->sizes());
    btConfig().setValue(ResultSplitterSizesKey, m_resultListSplitter->sizes());
}

/******************************************************************************
* StrongsResultList:
******************************************************************************/

StrongsResultList::StrongsResultList(const CSwordModuleInfo *module,
                                     const sword::ListKey & result,
                                     const QString &strongsNumber)
{
    using namespace Rendering;

    int count = result.getCount();
    if (!count)
        return;

    CTextRendering::KeyTreeItem::Settings settings;
    BtConstModuleList modules;
    modules.append(module);
    clear();

    // for whatever reason the text "Parsing...translations." does not appear.
    // this is not critical but the text is necessary to get the dialog box
    // to be wide enough.
    QProgressDialog progress(QObject::tr("Parsing Strong's Numbers"), nullptr, 0, count);
    //0, "progressDialog", tr("Parsing Strong's Numbers"), tr("Parsing Strong's numbers for translations."), true);
    //progress->setAllowCancel(false);
    //progress->setMinimumDuration(0);
    progress.show();
    progress.raise();

    qApp->processEvents(QEventLoop::AllEvents, 1); //1 ms only

    for (int index = 0; index < count; index++) {
        progress.setValue(index);
        qApp->processEvents(QEventLoop::AllEvents, 1); //1 ms only

        QString key = QString::fromUtf8(result.getElement(index)->getText());
        QString text = CDisplayRendering().renderSingleKey(key, modules, settings);
        for (int sIndex = 0;;) {
            continueloop:
            QString rText = getStrongsNumberText(text, sIndex, strongsNumber);
            if (rText.isEmpty()) break;

            for (iterator it = begin(); it != end(); ++it) {
                if ((*it).keyText() == rText) {
                    (*it).addKeyName(key);
                    goto continueloop; // break, then continue
                }
            }
            append(StrongsResult(rText, key));
        }
    }
}

QString StrongsResultList::getStrongsNumberText(const QString &verseContent,
                                                int &startIndex,
                                                const QString &lemmaText)
{
    // get the strongs text
    int idx1, idx2, index;
    QString sNumber, strongsText;
    //const bool cs = CSwordModuleSearch::caseSensitive;
    const Qt::CaseSensitivity cs = Qt::CaseInsensitive;

    if (startIndex == 0) {
        index = verseContent.indexOf("<body");
    }
    else {
        index = startIndex;
    }

    // find all the "lemma=" inside the the content
    while ((index = verseContent.indexOf("lemma=", index, cs)) != -1) {
        // get the strongs number after the lemma and compare it with the
        // strongs number we are looking for
        idx1 = verseContent.indexOf("\"", index) + 1;
        idx2 = verseContent.indexOf("\"", idx1 + 1);
        sNumber = verseContent.mid(idx1, idx2 - idx1);
        if (sNumber == lemmaText) {
            // strongs number is found now we need to get the text of this node
            // search right until the ">" is found.  Get the text from here to
            // the next "<".
            index = verseContent.indexOf(">", index, cs) + 1;
            idx2  = verseContent.indexOf("<", index, cs);
            strongsText = verseContent.mid(index, idx2 - index);
            index = idx2;
            startIndex = index;

            return(strongsText);
        }
        else {
            index += 6; // 6 is the length of "lemma="
        }
    }
    return QString::null;
}



} //namespace Search
