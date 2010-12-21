#include "btconfig.h"
#include "util/directory.h" // DU::getUserBaseDir()

#include <cstddef> // NULL macro
#include <QLocale>

#include "backend/managers/cdisplaytemplatemgr.h"
#include "btconfigtypes.h"
#include "backend/btmoduletreeitem.h"
#include "frontend/searchdialog/btsearchoptionsarea.h"

/*
 * set the instance variable initially to NULL, so it can be safely checked
 * whether the variable has been initialized yet.
 */
BtConfig* BtConfig::m_instance = NULL;

BtConfig::BtConfig() : m_defaults(), m_sessionSettings(), m_settings(util::directory::getUserBaseDir().absolutePath() + "/bibletimerc", QSettings::IniFormat)
{
    // construct defaults
        m_defaults.reserve(512); //TODO: check whether this value can be calculated automatically...

        m_defaults.insert("bibletimeVersion", "0.0");
        m_defaults.insert("language", QLocale::system().name());
        m_defaults.insert("gui/displayStyle", CDisplayTemplateMgr::defaultTemplate());
        m_defaults.insert("state/bookshelfCurrentItem", QVariant(QString::null));

        m_defaults.insert("settings/defaults/standardBible", "KJV");
        m_defaults.insert("settings/defaults/standardCommentary", "MHC");
        m_defaults.insert("settings/defaults/standardLexicon", "ISBE");
        m_defaults.insert("settings/defaults/standardDailyDevotional", "");
        m_defaults.insert("settings/defaults/standardHebrewStrongsLexicon", "StrongsHebrew");
        m_defaults.insert("settings/defaults/standardGreekStrongsLexicon", "StrongsGreek");
        m_defaults.insert("settings/defaults/standardHebrewMorphLexicon", "StrongsHebrew");
        m_defaults.insert("settings/defaults/standardGreekMorphLexicon", "StrongsGreek");

        m_defaults.insert("gui/showMainToolbar", true); // rename: toolbar -> showMainToolbar

        /*
        m_defaults.insert("gui/autoTileVertical", true);
        m_defaults.insert("gui/autoTileHorizontal", false);
        m_defaults.insert("gui/autoTile", false);
        m_defaults.insert("gui/autoTabbed", false);
        m_defaults.insert("gui/autoCascade", false);
        */
        m_defaults.insert("gui/alignmentMode", QVariant::fromValue(btconfigtypes::autoTileVertical));

        m_defaults.insert("presentation/lineBreaks", false);
        m_defaults.insert("presentation/verseNumbers", false);

        m_defaults.insert("gui/logo", true);
        m_defaults.insert("settings/behaviour/autoDeleteOrphanedIndices", true);
        m_defaults.insert("state/crashedLastTime", false);
        m_defaults.insert("state/crashedTwoTimes", false);
        m_defaults.insert("gui/bookshelfShowHidden", false);
        m_defaults.insert("settings/behaviour/allowNetworkConnection", false);
        m_defaults.insert("gui/showTextWindowHeaders", true);
        m_defaults.insert("gui/showTextWindowNavigator", true);
        m_defaults.insert("gui/showTextWindowToolButtons", true);
        m_defaults.insert("gui/showFormatToolbarButtons", true);
        m_defaults.insert("gui/showToolbarsInEachWindow", true);
        m_defaults.insert("gui/showTipAtStartup", true);

        // TODO: are the following booleans or ints, what do they do? rename to: show***?
        m_defaults.insert("gui/footnotes", true);
        m_defaults.insert("gui/strongNumbers", true);
        m_defaults.insert("gui/headings", true);
        m_defaults.insert("gui/morphTags", true);
        m_defaults.insert("gui/lemmas", true);
        m_defaults.insert("gui/hebrewPoints", true);
        m_defaults.insert("gui/hebrewCantillation", true);
        m_defaults.insert("gui/greekAccents", true);
        m_defaults.insert("gui/textualVariants", false);
        m_defaults.insert("gui/scriptureReferences", true);
        m_defaults.insert("gui/morphSegmentation", true);
        m_defaults.insert("gui/windows/bookshelfContentsX", 0);
        m_defaults.insert("gui/windows/bookshelfContentsY", 0);
        m_defaults.insert("gui/windows/magDelay", 400);
        m_defaults.insert("gui/windows/bookshelfGrouping", BTModuleTreeItem::CatLangMod);
        m_defaults.insert("gui/windows/searchDialogWidth", 200);
        m_defaults.insert("gui/windows/searchDialogHeight", 400);
        m_defaults.insert("gui/windows/searchDialogX", 200);
        m_defaults.insert("gui/windows/searchDialogY", 200);
        m_defaults.insert("gui/windows/searchType", Search::BtSearchOptionsArea::AndType);
        m_defaults.insert("gui/mainindexActiveTab", 0);
        m_defaults.insert("gui/installPathIndex", 0);
        m_defaults.insert("gui/windows/bookshelfPosX", 1);
        m_defaults.insert("gui/windows/bookshelfPosY", 1);
        m_defaults.insert("gui/windows/bookshelfHeight", 1);
        m_defaults.insert("gui/windows/bookshelfWidth", 1);
        m_defaults.insert("gui/windows/configDialogPosX", 1);
        m_defaults.insert("gui/windows/configDialogPosY", 1);
        m_defaults.insert("gui/windows/configDialogHeight", 1);
        m_defaults.insert("gui/windows/configDialogWidth", 1);
        m_defaults.insert("state/tipNumber", 0);
        //TODO: continue here

        // TODO: these variables seems not to be used, check and remove
        /*
         m _defaults.insert("firstSearchDialog", true);
        m_defaults.insert("readOldBookmars", false);
        m_defaults.insert("mainIndex", true);
        m_defaults.insert("infoDisplay", true);
        */

        //TODO: save defaults somewhere so they can be loaded directly on next startup

    // load session settings
        m_sessionSettings.insert("showTextWindowHeaders");
        //TODO: continue here
}

BtConfig::~BtConfig() {}

BtConfig& BtConfig::getInstance()
{
    if(m_instance == NULL)
        m_instance = new BtConfig;
    return *m_instance;
}
