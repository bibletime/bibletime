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

#include "btsearchresultarea.h"

#include <QApplication>
#include <QFrame>
#include <QMenu>
#include <QPushButton>
#include <QSize>
#include <QSplitter>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>
#include "../../backend/config/btconfig.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../backend/managers/colormanager.h"
#include "../../backend/keys/cswordversekey.h"
#include "../../backend/rendering/cdisplayrendering.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/tool.h"
#include "cmoduleresultview.h"
#include "csearchresultview.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <swmodule.h>
#pragma GCC diagnostic pop


namespace {
auto const MainSplitterSizesKey =
        QStringLiteral("GUI/SearchDialog/SearchResultsArea/mainSplitterSizes");
auto const ResultSplitterSizesKey =
        QStringLiteral(
            "GUI/SearchDialog/SearchResultsArea/resultSplitterSizes");
} // anonymous namespace

namespace Search {

BtSearchResultArea::BtSearchResultArea(QWidget * parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout;
    QWidget *resultListsWidget;
    QVBoxLayout *resultListsWidgetLayout;

    //Size is calculated from the font rather than set in pixels,
    // maybe this is better in different kinds of displays?
    auto const mWidth = util::tool::mWidth(*this, 1);
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
    BT_CONNECT(m_moduleListBox, &CModuleResultView::moduleSelected,
               m_resultListBox, &CSearchResultView::setupTree);
    BT_CONNECT(m_moduleListBox, &CModuleResultView::strongsSelected,
               m_resultListBox, &CSearchResultView::setupStrongsTree);
    BT_CONNECT(m_resultListBox, &CSearchResultView::keySelected,
               this,            &BtSearchResultArea::updatePreview);
    BT_CONNECT(m_resultListBox, &CSearchResultView::keyDeselected,
               this,            &BtSearchResultArea::clearPreview);
    m_resultListSplitter->addWidget(m_resultListBox);
    resultListsWidgetLayout->addWidget(m_resultListSplitter);

    m_mainSplitter->addWidget(resultListsWidget);

    //Preview ("info") area
    m_displayFrame = new QFrame(m_mainSplitter);
    m_displayFrame->setFrameShape(QFrame::NoFrame);
    m_displayFrame->setFrameShadow(QFrame::Plain);
    m_mainSplitter->addWidget(m_displayFrame);

    mainLayout->addWidget(m_mainSplitter);

    m_contextMenu = new QMenu(this);
        m_selectAllAction = new QAction(tr("Select all"), this);
        m_selectAllAction->setShortcut(QKeySequence::SelectAll);
        m_contextMenu->addAction(m_selectAllAction);

        m_copyAction = new QAction(tr("Copy"));
        m_copyAction->setShortcut(QKeySequence::Copy);
        m_contextMenu->addAction(m_copyAction);

    QVBoxLayout* frameLayout = new QVBoxLayout(m_displayFrame);
    frameLayout->setContentsMargins(0, 0, 0, 0);
    m_previewDisplay = new BtTextBrowser(this);
    m_previewDisplay->setOpenLinks(false);
    m_previewDisplay->setToolTip(tr("Text of the selected search result item"));
    m_previewDisplay->setContextMenuPolicy(Qt::CustomContextMenu);
    BT_CONNECT(m_selectAllAction, &QAction::triggered,
               m_previewDisplay,  &QTextBrowser::selectAll);
    BT_CONNECT(m_copyAction,     &QAction::triggered,
               m_previewDisplay, &QTextBrowser::copy);
    BT_CONNECT(m_moduleListBox,  &CModuleResultView::moduleChanged,
               m_previewDisplay, &QTextBrowser::clear);
    BT_CONNECT(m_previewDisplay, &QTextBrowser::customContextMenuRequested,
               [this](QPoint const & loc) { m_contextMenu->exec(loc); });
    frameLayout->addWidget(m_previewDisplay);

    loadDialogSettings();
}

void BtSearchResultArea::setSearchResult(QString searchedText,
                                         CSwordModuleSearch::Results results)
{
    reset(); //clear current modules

    m_searchedText = std::move(searchedText);
    m_results = std::move(results);

    // Populate listbox:
    m_moduleListBox->setupTree(m_results, searchedText);

    // Pre-select the first module in the list:
    m_moduleListBox->setCurrentItem(m_moduleListBox->topLevelItem(0), 0);
}

void BtSearchResultArea::reset() {
    m_moduleListBox->clear();
    m_resultListBox->clear();
    clearPreview();
}

void BtSearchResultArea::clearPreview() {
    m_previewDisplay->setText(
                QStringLiteral("<html><head/><body></body></html>"));
}

void BtSearchResultArea::updatePreview(const QString& key) {
    using namespace Rendering;

    CSwordModuleInfo* module = m_moduleListBox->activeModule();
    if ( module ) {
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
            static_cast<sword::VerseKey *>(module->swordModule().getKey())
                    ->setIntros(true);

            //first go back and then go forward the keys to be in context
            vk.previous();
            vk.previous();

            //include Headings in display, they are indexed and searched too
            if (vk.verse() == 1) {
                if (vk.chapter() == 1) {
                    vk.setChapter(0);
                }
                vk.setVerse(0);
            }

            auto const startKey = vk;

            vk.setKey(key);

            vk.next();
            vk.next();

            settings.keyRenderingFace = CTextRendering::KeyTreeItem::Settings::CompleteShort;
            text = render.renderKeyRange(startKey, vk, modules, key, settings);
        }
        //for commentaries only one verse, but with heading
        else if (module->type() == CSwordModuleInfo::Commentary) {
            CSwordVerseKey vk(module);
            vk.setIntros(true);
            vk.setKey(key);

            // HACK: enable headings for VerseKeys:
            static_cast<sword::VerseKey *>(module->swordModule().getKey())
                    ->setIntros(true);

            //include Headings in display, they are indexed and searched too
            if (vk.verse() == 1) {
                if (vk.chapter() == 1) {
                    vk.setChapter(0);
                }
                vk.setVerse(0);
            }
            auto const startKey = vk;

            vk.setKey(key);

            settings.keyRenderingFace = CTextRendering::KeyTreeItem::Settings::NoKey;
            text = render.renderKeyRange(startKey, vk, modules, key, settings);
        }
        else {
            text = render.renderSingleKey(key, modules, settings);
        }

        if (modules.count() > 0)
            setBrowserFont(modules.at(0));

        QString text2 =
                CSwordModuleSearch::highlightSearchedText(text, m_searchedText);
        text2.replace(QStringLiteral("#CHAPTERTITLE#"), QString());
        text2.replace(QStringLiteral("#TEXT_ALIGN#"), QStringLiteral("left"));
        text2 = ColorManager::replaceColors(text2);
        m_previewDisplay->setText(text2);
        m_previewDisplay->scrollToAnchor( CDisplayRendering::keyToHTMLAnchor(key) );
    }
}

void BtSearchResultArea::setBrowserFont(const CSwordModuleInfo* const module) {
    if (module) {
            auto const lang = module->language();
            m_previewDisplay->setFont(btConfig().getFontForLanguage(*lang).second);
    } else {
        m_previewDisplay->setFont(btConfig().getDefaultFont());
    }
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

} //namespace Search
