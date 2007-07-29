/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cswordsetupmodulelistview.h"
#include "btinstallmgr.h"

#include "backend/cswordbackend.h"
#include "backend/cswordmoduleinfo.h"

#include "util/cresmgr.h"
#include "util/ctoolclass.h"

//QT includes
#include <qtooltip.h>

//KDE includes
#include <klocale.h>
#include <kiconloader.h>

using namespace sword;

namespace BookshelfManager {

    /** Tooltip implementation for QListView widgets.
     * @short Tooltip for InstallationManager listviews
     * @author Joachim Ansorg
     */
    class ToolTip : public QToolTip
	{
    public:
        /** Constructor which takes the listview to operate on.
          * @param listview We operate on this widget to request tooltips from it'd child items.
         * @short Constructor.
         */
        ToolTip(CSwordSetupModuleListView* listview)
                : QToolTip( listview->viewport() ),
        m_parent( listview ) {}

        /** Reimplementation of QToolTip::maybeTip. It's requested if a new tooltip may be displayed.
         * @short Display a tooltip if we're over an item
         */
        virtual void maybeTip(const QPoint& pos) {
            Q3ListViewItem* i = m_parent->itemAt(pos);
            Q_ASSERT(i);

            const QRect rect = m_parent->itemRect(i);
            if (m_parent->showTooltip(i, pos, 0)) {
                const QString tipText = m_parent->tooltip(i, 0);
                tip(rect, tipText);
            }
        }

    protected:
        CSwordSetupModuleListView* m_parent;

	}; //class ToolTip

    /** Listview specially made for the installation manager.
     * @short InstallationManager module listviews
     * @author Martin Gruner
     */
    CSwordSetupModuleListView::CSwordSetupModuleListView(QWidget *parent, bool is_remote, sword::InstallSource* installSource)
            : KListView(parent), m_is_remote( is_remote ) {
        Q_ASSERT(installSource);
        new BookshelfManager::ToolTip(this);
        m_backend = installSource ? BTInstallMgr::Tool::backend(installSource) : CPointers::backend();
		
		//columns: use setColumnCount; setHeaderLabels; columnWidth. WidthMode???; Alignment: item-level, not really needed; 
        addColumn(i18n("Name"));
        setColumnWidthMode( 0, Q3ListView::Maximum );
        setColumnWidth( 0, 200 ); //don`t get too broad

        addColumn(i18n("Status"));
        setColumnAlignment(1, Qt::AlignRight);
        addColumn(i18n("Installed version")); //version
        setColumnAlignment(2, Qt::AlignHCenter);

        if (m_is_remote) {
            addColumn(i18n("Remote version")); //version
        } else {
            addColumn(i18n("Location"));
        }
        setColumnAlignment(3, Qt::AlignLeft);

        setAllColumnsShowFocus(true);
        setFullWidth(true);
        setRootIsDecorated(true);
        setResizeMode(Q3ListView::LastColumn);
        setTooltipColumn(0);

        init();
    }

    CSwordSetupModuleListView::~CSwordSetupModuleListView() {
        if (m_is_remote) {
            delete m_backend;
        }
    }

    void CSwordSetupModuleListView::init() {

        m_categoryBible = new Q3CheckListItem(this, i18n("Bibles"), Q3CheckListItem::CheckBoxController);
        m_categoryCommentary = new Q3CheckListItem(this, i18n("Commentaries"), Q3CheckListItem::CheckBoxController);
        m_categoryLexicon = new Q3CheckListItem(this, i18n("Lexicons"), Q3CheckListItem::CheckBoxController);
        m_categoryBook = new Q3CheckListItem(this, i18n("Books"), Q3CheckListItem::CheckBoxController);
        m_categoryDevotionals = new Q3CheckListItem(this, i18n("Daily Devotionals"), Q3CheckListItem::CheckBoxController);
        m_categoryGlossaries = new Q3CheckListItem(this, i18n("Glossaries"), Q3CheckListItem::CheckBoxController);


        m_categoryBible->setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));
        m_categoryCommentary->setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));
        m_categoryLexicon->setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));
        m_categoryBook->setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));
        m_categoryDevotionals->setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));
        m_categoryGlossaries->setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));

		//open is not necessary (and not good)
        m_categoryBible->setOpen(true);
        m_categoryCommentary->setOpen(true);
        m_categoryLexicon->setOpen(true);
        m_categoryBook->setOpen(true);
        m_categoryDevotionals->setOpen(true);
        m_categoryGlossaries->setOpen(true);

        //   connect(this, SIGNAL(executed(QListViewItem*)), SLOT(slotItemClicked(QListViewItem*)));
        connect(this, SIGNAL(clicked(Q3ListViewItem*)), SLOT(slotItemClicked(Q3ListViewItem*))); //items have to be clicked only once in double click mode
        connect(this, SIGNAL(spacePressed(Q3ListViewItem*)), SLOT(slotItemClicked(Q3ListViewItem*)));
    }

    void CSwordSetupModuleListView::finish() {
        if (!m_categoryBible->childCount())
            delete m_categoryBible;
        if (!m_categoryCommentary->childCount())
            delete m_categoryCommentary;
        if (!m_categoryBook->childCount())
            delete m_categoryBook;
        if (!m_categoryLexicon->childCount())
            delete m_categoryLexicon;
        if (!m_categoryDevotionals->childCount())
            delete m_categoryDevotionals;
        if (!m_categoryGlossaries->childCount())
            delete m_categoryGlossaries;
    }

    void CSwordSetupModuleListView::clear() {
        Q3ListView::clear();
        init();
    }

    void CSwordSetupModuleListView::addModule(CSwordModuleInfo* module, QString localVersion) {

        Q3ListViewItem* parent = 0;
        switch ( module->type() ) {
        case CSwordModuleInfo::Bible:
            parent = m_categoryBible;
            break;
        case CSwordModuleInfo::Commentary:
            parent = m_categoryCommentary;
            break;
        case CSwordModuleInfo::Lexicon:
            parent = m_categoryLexicon;
            break;
        case CSwordModuleInfo::GenericBook:
            parent = m_categoryBook;
            break;
        default:
            parent = 0;
            break;
        }

        //handling for special module types
        if ((parent == m_categoryLexicon) && (module->category() == CSwordModuleInfo::Glossary)) {
            parent = m_categoryGlossaries;
        }
        if ((parent == m_categoryLexicon) && (module->category() == CSwordModuleInfo::DailyDevotional)) {
            parent = m_categoryDevotionals;
        }

        //now we know the category, find the right language group in that category
        const CLanguageMgr::Language* const lang = module->language();
        QString langName = lang->translatedName();
        if (!lang->isValid()) {
            langName = QString(module->module()->Lang());
        }

        Q3ListViewItem * langFolder = 0;
        if (parent) {
            langFolder = parent->firstChild();

            while( langFolder ) { //try to find language folder if it exsists
                if (langFolder->text(0) == langName) { //found right folder
                    break;
                }

                langFolder = langFolder->nextSibling();
            }
        }

        if (!langFolder) { //not yet there

            langFolder = new Q3CheckListItem(parent, langName, Q3CheckListItem::CheckBoxController);

            langFolder->setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));
            langFolder->setOpen(false);
        }

        Q_ASSERT(langFolder);

        Q3ListViewItem* newItem = 0;
        if (langFolder) {
            newItem = new Q3CheckListItem(langFolder, module->name(), Q3CheckListItem::CheckBox);
        } else { //shouldn't happen
            newItem = new Q3CheckListItem(this, module->name(), Q3CheckListItem::CheckBox);
        }

        newItem->setPixmap(0, CToolClass::getIconForModule(module));
        if (m_is_remote) {
            newItem->setText(1, localVersion.isEmpty() ? i18n("New") : i18n("Updated"));
        } else {
            newItem->setText(1, i18n("Installed") );
        }

        newItem->setText(2, localVersion);
        if (m_is_remote) {
            newItem->setText(3, module->config(CSwordModuleInfo::ModuleVersion));
        } else {
            newItem->setText(3, module->config(CSwordModuleInfo::AbsoluteDataPath));
        }
    }

    QStringList CSwordSetupModuleListView::selectedModules() {
        QStringList moduleList;

        Q3ListViewItemIterator list_it( this );
        while ( list_it.current() ) {
            Q3CheckListItem* i = dynamic_cast<Q3CheckListItem*>( list_it.current() );
            if (i && i->isOn() && i->type() == Q3CheckListItem::CheckBox ) {
                moduleList << i->text(0);
            }
            ++list_it;
        }

        return moduleList;
    }

    void CSwordSetupModuleListView::slotItemClicked(Q3ListViewItem*) {
        emit selectedModulesChanged();
    }

    bool CSwordSetupModuleListView::showTooltip(Q3ListViewItem* i, const QPoint&, int) const {
        Q3CheckListItem* checkItem = dynamic_cast<Q3CheckListItem*>( i );
        Q_ASSERT(checkItem);

        return (checkItem && (checkItem->type() == Q3CheckListItem::CheckBox));
    }

    QString CSwordSetupModuleListView::tooltip(Q3ListViewItem* i, int /*column*/) const {
        QString ret;
        Q3CheckListItem* checkItem = dynamic_cast<Q3CheckListItem*>( i );

        if (checkItem && (checkItem->type() == Q3CheckListItem::CheckBox)) {
            const QString moduleName = checkItem->text(0);
            CSwordModuleInfo* module = m_backend->findModuleByName(moduleName);

            ret = CToolClass::moduleToolTip(module);
        }

        return ret;
    }

} //NAMESPACE

