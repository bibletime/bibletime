/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/cmodulechooserdialog.h"

#include <QButtonGroup>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QSpacerItem>
#include <QStringList>
#include <QTreeWidget>
#include <QVBoxLayout>
#include "backend/btmoduletreeitem.h"
#include "backend/config/cbtconfig.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/cswordbackend.h"
#include "util/cpointers.h"
#include "util/cresmgr.h"
#include "util/tool.h"
#include "util/dialogutil.h"
#include "util/directory.h"


CModuleChooserDialog::CModuleChooserDialog( QWidget* parent, QString title, QString label, QList<CSwordModuleInfo*>*  allModules)
        : QDialog(parent),
        m_title(title),
        m_labelText(label) {
    m_grouping = (BTModuleTreeItem::Grouping)CBTConfig::get(CBTConfig::bookshelfGrouping);
    m_filters = QList<BTModuleTreeItem::Filter*>();
    if (!allModules) {
        m_moduleList = CPointers::backend()->moduleList();
    }
    else m_moduleList = *allModules;
}

/**
* Call init() after the constructor, either in the end of your own constructor or from outside.
*/
void CModuleChooserDialog::init() {
    //Set the flag to destroy when closed - otherwise eats memory
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(m_title);
    initView();
    initTree();
}

/** Initializes the view of this dialog */
void CModuleChooserDialog::initView() {
    //TODO: choose the button text?

    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;

    vboxLayout = new QVBoxLayout(this);

    QLabel* label = util::tool::explanationLabel(this, QString::null, m_labelText);
    vboxLayout->addWidget(label);

    m_moduleChooser = new QTreeWidget(this);
    m_moduleChooser->header()->hide();

    vboxLayout->addWidget(m_moduleChooser);

    hboxLayout = new QHBoxLayout();

    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout->addItem(spacerItem);

    m_buttonBox = new QDialogButtonBox(this);
    m_buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    util::prepareDialogBox(m_buttonBox);
    hboxLayout->addWidget(m_buttonBox);

    vboxLayout->addLayout(hboxLayout);

    QObject::connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(slotOk()) );
    //The QDialog doc is a bit unclear but calling reject also destroys the dialog
    // in this situation.
    QObject::connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()) );
}


void CModuleChooserDialog::initTree() {
    //qDebug() << "CModuleChooserDialog::initTree";

    // See BTModuleTreeItem documentation.
    BTModuleTreeItem root(m_filters, m_grouping, &m_moduleList);
    createModuleTree(&root, m_moduleChooser->invisibleRootItem());

}

void CModuleChooserDialog::createModuleTree(BTModuleTreeItem* item, QTreeWidgetItem* widgetItem) {
    foreach (BTModuleTreeItem* i, item->children()) {
        createModuleTree(i, new QTreeWidgetItem(widgetItem));
    }
    if (item->type() != BTModuleTreeItem::Root) {
        widgetItem->setText(0, item->text());
        if (item->type() == BTModuleTreeItem::Category || item->type() == BTModuleTreeItem::Language) {
            widgetItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsTristate);
        }
        if (item->type() == BTModuleTreeItem::Module) {
            initModuleItem(item, widgetItem);
        }
    }
}


/** Emits the signal with the list of the selected modules. */
void CModuleChooserDialog::slotOk() {
    Q_ASSERT(m_moduleChooser);
    //create the list of selected modules
    QList<CSwordModuleInfo*> mods;
    QTreeWidgetItemIterator it( m_moduleChooser );
    for ( ; *it; ++it ) {
        //add the module to list if the box is checked
        if ((*it)->checkState(0) == Qt::Checked) {
            qDebug() << "was checked";
            for (QList<CSwordModuleInfo*>::iterator all_iter(m_moduleList.begin()); all_iter != m_moduleList.end(); ++all_iter) {
                if ((*all_iter)->name() == (*it)->text(0)) {
                    qDebug() << "append";
                    mods.append(*all_iter);
                    break;
                }
            }

        }
    }

    // The selection is handled first, then the dialog is closed and destroyed.
    emit modulesChanged(mods, m_moduleChooser);
    QDialog::done(QDialog::Accepted);
}

QPushButton* CModuleChooserDialog::okButton() {
    return m_buttonBox->button(QDialogButtonBox::Ok);
}
