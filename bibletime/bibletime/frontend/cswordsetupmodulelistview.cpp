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
    class ToolTip : public QToolTip {
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
            QListViewItem* i = m_parent->itemAt(pos);
            Q_ASSERT(i);

            const QRect rect = m_parent->itemRect(i);
            if (m_parent->showTooltip(i, pos, 0)) {
                const QString tipText = m_parent->tooltip(i, 0);
                tip(rect, tipText);
            }
        }

    protected:
        CSwordSetupModuleListView* m_parent;
    };

    /** Listview specially made for the installation manager.
     * @short InstallationManager module listviews
     * @author Martin Gruner
     */
    CSwordSetupModuleListView::CSwordSetupModuleListView(QWidget *parent, bool is_remote, sword::InstallSource* installSource)
            : KListView(parent), m_is_remote( is_remote ) {
        Q_ASSERT(installSource);
        new BookshelfManager::ToolTip(this);
        m_backend = installSource ? BTInstallMgr::Tool::backend(installSource) : CPointers::backend();

        addColumn(i18n("Name"));
        setColumnWidthMode( 0, QListView::Maximum );
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
        setResizeMode(QListView::LastColumn);
        setTooltipColumn(0);

        init();
    }

    CSwordSetupModuleListView::~CSwordSetupModuleListView() {
        if (m_is_remote) {
            delete m_backend;
        }
    }

    void CSwordSetupModuleListView::init() {
#if QT_VERSION >= 0x030200
        m_categoryBible = new QCheckListItem(this, i18n("Bibles"), QCheckListItem::CheckBoxController);
        m_categoryCommentary = new QCheckListItem(this, i18n("Commentaries"), QCheckListItem::CheckBoxController);
        m_categoryLexicon = new QCheckListItem(this, i18n("Lexicons"), QCheckListItem::CheckBoxController);
        m_categoryBook = new QCheckListItem(this, i18n("Books"), QCheckListItem::CheckBoxController);
        m_categoryDevotionals = new QCheckListItem(this, i18n("Daily Devotionals"), QCheckListItem::CheckBoxController);
        m_categoryGlossaries = new QCheckListItem(this, i18n("Glossaries"), QCheckListItem::CheckBoxController);
#else
        //  Qt <= 3.1.x doesn't support the CheckBoxController!, remove the define as soon as we switch to the new Qt
        m_categoryBible = new QCheckListItem(this, i18n("Bibles"), QCheckListItem::Controller);
        m_categoryCommentary = new QCheckListItem(this, i18n("Commentaries"), QCheckListItem::Controller);
        m_categoryLexicon = new QCheckListItem(this, i18n("Lexicons"), QCheckListItem::Controller);
        m_categoryBook = new QCheckListItem(this, i18n("Books"), QCheckListItem::Controller);
        m_categoryDevotionals = new QCheckListItem(this, i18n("Daily Devotionals"), QCheckListItem::Controller);
        m_categoryGlossaries = new QCheckListItem(this, i18n("Glossaries"), QCheckListItem::Controller);
#endif

        m_categoryBible->setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));
        m_categoryCommentary->setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));
        m_categoryLexicon->setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));
        m_categoryBook->setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));
        m_categoryDevotionals->setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));
        m_categoryGlossaries->setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));

        m_categoryBible->setOpen(true);
        m_categoryCommentary->setOpen(true);
        m_categoryLexicon->setOpen(true);
        m_categoryBook->setOpen(true);
        m_categoryDevotionals->setOpen(true);
        m_categoryGlossaries->setOpen(true);

        //   connect(this, SIGNAL(executed(QListViewItem*)), SLOT(slotItemClicked(QListViewItem*)));
        connect(this, SIGNAL(clicked(QListViewItem*)), SLOT(slotItemClicked(QListViewItem*))); //items have to be clicked only once in double click mode
        connect(this, SIGNAL(spacePressed(QListViewItem*)), SLOT(slotItemClicked(QListViewItem*)));
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
        QListView::clear();
        init();
    }

    void CSwordSetupModuleListView::addModule(CSwordModuleInfo* module, QString localVersion) {

        QListViewItem* parent = 0;
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

        QListViewItem * langFolder = 0;
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
#if QT_VERSION >= 0x030200
            langFolder = new QCheckListItem(parent, langName, QCheckListItem::CheckBoxController);
#else

            langFolder = new QCheckListItem(parent, langName, QCheckListItem::Controller);
#endif

            langFolder->setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));
            langFolder->setOpen(false);
        }

        Q_ASSERT(langFolder);

        QListViewItem* newItem = 0;
        if (langFolder) {
            newItem = new QCheckListItem(langFolder, module->name(), QCheckListItem::CheckBox);
        } else { //shouldn't happen
            newItem = new QCheckListItem(this, module->name(), QCheckListItem::CheckBox);
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

        QListViewItemIterator list_it( this );
        while ( list_it.current() ) {
            QCheckListItem* i = dynamic_cast<QCheckListItem*>( list_it.current() );
            if (i && i->isOn() && i->type() == QCheckListItem::CheckBox ) {
                moduleList << i->text(0);
            }
            ++list_it;
        }

        return moduleList;
    }

    void CSwordSetupModuleListView::slotItemClicked(QListViewItem*) {
        emit selectedModulesChanged();
    }

    bool CSwordSetupModuleListView::showTooltip(QListViewItem* i, const QPoint&, int) const {
        QCheckListItem* checkItem = dynamic_cast<QCheckListItem*>( i );
        Q_ASSERT(checkItem);

        return (checkItem && (checkItem->type() == QCheckListItem::CheckBox));
    }

    QString CSwordSetupModuleListView::tooltip(QListViewItem* i, int /*column*/) const {
        QString ret;
        QCheckListItem* checkItem = dynamic_cast<QCheckListItem*>( i );

        if (checkItem && (checkItem->type() == QCheckListItem::CheckBox)) {
            const QString moduleName = checkItem->text(0);
            CSwordModuleInfo* module = m_backend->findModuleByName(moduleName);

            ret = CToolClass::moduleToolTip(module);
        }

        return ret;
    }

} //NAMESPACE

