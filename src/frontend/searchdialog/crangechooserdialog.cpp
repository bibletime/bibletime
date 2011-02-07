/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/searchdialog/crangechooserdialog.h"

#include <QDialogButtonBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QString>
#include <QTextEdit>
#include <QVBoxLayout>
#include "backend/config/cbtconfig.h"
#include "backend/config/btconfig.h"
#include "util/dialogutil.h"

// Sword includes:
#include "versekey.h"
#include "listkey.h"


namespace Search {

/********** RangeItem ******************/
CRangeChooserDialog::RangeItem::RangeItem(QListWidget* view, QListWidgetItem* /*afterThis*/, const QString caption, const QString range)
        : QListWidgetItem(view) {
    setCaption(caption);
    setRange(range);
}

CRangeChooserDialog::RangeItem::~RangeItem() {}

const QString& CRangeChooserDialog::RangeItem::range() const {
    //  qWarning("range is %s", (const char*)m_range.utf8());
    return m_range;
}

void CRangeChooserDialog::RangeItem::setRange(QString newRange) {
    m_range = newRange;
}

QString CRangeChooserDialog::RangeItem::caption() const {
    return text();
}

void CRangeChooserDialog::RangeItem::setCaption(const QString newCaption) {
    setText(newCaption);
}


/*********** Dialog ***************/

CRangeChooserDialog::CRangeChooserDialog( QWidget* parentDialog )
        : QDialog(parentDialog) {
    //Set the flag to destroy when closed - otherwise eats memory
    setAttribute(Qt::WA_DeleteOnClose);
    initView();
    initConnections();

    //add the existing scopes
    BtConfig::StringMap map = BtConfig::getInstance().getSearchScopesForCurrentLocale();
    BtConfig::StringMap::Iterator it;
    for (it = map.begin(); it != map.end(); ++it) {
        new RangeItem(m_rangeList, 0, it.key(), it.value());
    };

    editRange(0);
    if (RangeItem* i = dynamic_cast<RangeItem*>(m_rangeList->currentItem())
       ) {
        nameChanged(i->caption());
    }
}

/** Initializes the view of this object. */
void CRangeChooserDialog::initView() {
    setWindowTitle(tr("Setup Search Scopes"));

    QVBoxLayout* vboxLayout = new QVBoxLayout(this);

    QHBoxLayout* hboxLayout = new QHBoxLayout();
    QVBoxLayout* vboxLayout1 = new QVBoxLayout();
    QLabel* rangeListLabel = new QLabel(tr("S&earch range:"), this);
    vboxLayout1->addWidget(rangeListLabel);

    m_rangeList = new QListWidget(this);
    m_rangeList->setToolTip(tr("Select a scope from the list to edit the search ranges"));
    vboxLayout1->addWidget(m_rangeList);

    QHBoxLayout* hboxLayout1 = new QHBoxLayout();
    m_newRangeButton = new QPushButton(tr("&Add new scope"), this);
    m_newRangeButton->setToolTip(tr("Add a new search scope. First enter an appropriate name, then edit the search ranges."));
    hboxLayout1->addWidget(m_newRangeButton);
    m_deleteRangeButton = new QPushButton(tr("Delete current &scope"), this);
    m_deleteRangeButton->setToolTip(tr("Delete the selected search scope"));
    hboxLayout1->addWidget(m_deleteRangeButton);

    vboxLayout1->addLayout(hboxLayout1);
    hboxLayout->addLayout(vboxLayout1);

    QVBoxLayout* vboxLayout2 = new QVBoxLayout();
    QHBoxLayout* hboxLayout2 = new QHBoxLayout();
    QLabel* nameEditLabel = new QLabel(tr("&Name:"), this);
    hboxLayout2->addWidget(nameEditLabel);

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setToolTip(tr("Change the name of the selected search scope"));
    hboxLayout2->addWidget(m_nameEdit);
    vboxLayout2->addLayout(hboxLayout2);

    QLabel* rangeEditLabel = new QLabel(tr("Edi&t current range:"), this);
    vboxLayout2->addWidget(rangeEditLabel);

    m_rangeEdit = new QTextEdit(this);
    m_rangeEdit->setToolTip(tr("Change the search ranges of the selected search scope item. Have a look at the predefined search scopes to see how search ranges are constructed."));

    vboxLayout2->addWidget(m_rangeEdit);

    QLabel* resultListLabel = new QLabel(tr("Parsed search range:"), this);
    vboxLayout2->addWidget(resultListLabel);

    m_resultList = new QListWidget(this);
    m_resultList->setToolTip(tr("The search ranges which will be used for the search, parsed to the canonical form"));
    vboxLayout2->addWidget(m_resultList);

    hboxLayout->addLayout(vboxLayout2);
    vboxLayout->addLayout(hboxLayout);

    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    vboxLayout->addWidget(line);

    m_buttonBox = new QDialogButtonBox(this);
    m_buttonBox->setOrientation(Qt::Horizontal);
    m_buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::NoButton | QDialogButtonBox::Ok | QDialogButtonBox::RestoreDefaults);
    util::prepareDialogBox(m_buttonBox);
    vboxLayout->addWidget(m_buttonBox);

    rangeListLabel->setBuddy(m_rangeList);
    nameEditLabel->setBuddy(m_nameEdit);
    rangeEditLabel->setBuddy(m_rangeEdit);
    resultListLabel->setBuddy(m_resultList);
}



void CRangeChooserDialog::initConnections() {
    // Signals from text/list widgets
    QObject::connect(m_rangeList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
                     this, SLOT(editRange(QListWidgetItem*)));

    QObject::connect(m_rangeEdit, SIGNAL(textChanged()),
                     this, SLOT(parseRange()));
    QObject::connect(m_rangeEdit, SIGNAL(textChanged()),
                     this, SLOT(rangeChanged()));

    QObject::connect(m_nameEdit, SIGNAL(textChanged(const QString&)),
                     this, SLOT(nameChanged(const QString&)));

    // Buttons
    QObject::connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(slotOk()));
    QObject::connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(close()));
    QObject::connect(m_newRangeButton, SIGNAL(clicked()), this, SLOT(addNewRange()));
    QObject::connect(m_deleteRangeButton, SIGNAL(clicked()), this, SLOT(deleteCurrentRange()));
    //restore defaults!
    QPushButton* defaultsButton = m_buttonBox->button(QDialogButtonBox::RestoreDefaults);
    QObject::connect(defaultsButton, SIGNAL(clicked()), this, SLOT(slotDefault()));
}

/** Adds a new range to the list. */
void CRangeChooserDialog::addNewRange() {
    //qDebug() << "CRangeChooserDialog::addNewRange";
    //RangeItem* i = new RangeItem(m_rangeList, m_rangeList->lastItem(), tr("New range"));
    RangeItem* i = new RangeItem(m_rangeList, 0, tr("New range"));
    //use just setCurrentItem... m_rangeList->setSelected(i, true);
    m_rangeList->setCurrentItem(i);
    editRange(i);

    m_nameEdit->setFocus();
}

/** No descriptions */
void CRangeChooserDialog::editRange(QListWidgetItem* item) {
    //qDebug() << "CRangeChooserDialog::editRange";
    RangeItem* const range = dynamic_cast<RangeItem*>(item);

    m_nameEdit->setEnabled( range ); //only if an item is selected enable the edit part
    m_rangeEdit->setEnabled( range );
    m_resultList->setEnabled( range );
    m_deleteRangeButton->setEnabled( range );

    if (range) {
        m_nameEdit->setText(range->caption());
        m_rangeEdit->setText(range->range());
    }
}

/** Parses the entered text and prints out the result in the list box below the edit area. */
void CRangeChooserDialog::parseRange() {
    //qDebug() << "CRangeChooserDialog::parseRange";
    m_resultList->clear();

    /// \todo remove this hack:
    //HACK: repair range to  work with Sword 1.5.6
    QString range( m_rangeEdit->toPlainText() );
    range.replace(QRegExp("\\s{0,}-\\s{0,}"), "-" );

    sword::VerseKey key;
    sword::ListKey verses = key.ParseVerseList((const char*)range.toUtf8(), "Genesis 1:1", true);
    for (int i = 0; i < verses.Count(); ++i) {
        new QListWidgetItem(QString::fromUtf8(verses.GetElement(i)->getRangeText()), m_resultList );
        //    qWarning("range=%s, text=%s",verses.GetElement(i)->getRangeText(), verses.GetElement(i)->getText() );
    }

}

/** No descriptions */
void CRangeChooserDialog::rangeChanged() {
    //qDebug() << "CRangeChooserDialog::rangeChanged";
    if (RangeItem* i = dynamic_cast<RangeItem*>(m_rangeList->currentItem())
       ) {
        QString range( m_rangeEdit->toPlainText() );
        /// \todo remove this hack:
        //HACK: repair range to work with Sword 1.5.6
        range.replace(QRegExp("\\s{0,}-\\s{0,}"), "-" );
        i->setRange(range);
    };
}

/** No descriptions */
void CRangeChooserDialog::nameChanged(const QString& newCaption) {
    //qDebug() << "CRangeChooserDialog::nameChanged";
    m_rangeEdit->setEnabled(!newCaption.isEmpty());
    m_resultList->setEnabled(!newCaption.isEmpty());
    //m_resultList->header()->setEnabled(!newCaption.isEmpty());

    if (RangeItem* i = dynamic_cast<RangeItem*>(m_rangeList->currentItem())
       ) {
        if (!newCaption.isEmpty()) {
            //enable some items (see "else" below)
            m_newRangeButton->setEnabled(true);
            m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
            m_rangeList->setDisabled(false);
            i->setCaption(newCaption);
        }
        else { //invalid name
            i->setCaption(tr("<invalid name of search range>"));
            //disable some items to prevent saving invalid range
            m_newRangeButton->setEnabled(false);
            m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
            m_rangeList->setDisabled(true);
        };
    };
}

/** Deletes the selected range. */
void CRangeChooserDialog::deleteCurrentRange() {
    //qDebug() << "CRangeChooserDialog::deleteCurrentRange";
    if (RangeItem* i = dynamic_cast<RangeItem*>(m_rangeList->currentItem()) ) {
        int row = m_rangeList->row(i);
        m_rangeList->takeItem(row);
        delete i;
    }
    editRange(m_rangeList->currentItem());
}

void CRangeChooserDialog::slotOk() {
    m_rangeList->sortItems(); //sorted first because the order will be saved
    //save the new map of search scopes
    BtConfig::StringMap map;
    for (int i = 0; i < m_rangeList->count(); i++) {
        if ( RangeItem* item = dynamic_cast<RangeItem*>(m_rangeList->item(i)) ) {
            map[item->caption()] = item->range();
        };
    }
    BtConfig::getInstance().setSearchScopesWithCurrentLocale(map);

    QDialog::accept();
}

void CRangeChooserDialog::slotDefault() {
    //qDebug() << "CRangeChooserDialog::slotDefault";
    m_rangeList->clear();
    BtConfig::getInstance().deleteValue("properties/searchScopes");
    BtConfig::StringMap map = BtConfig::getInstance().getSearchScopesForCurrentLocale();
    BtConfig::StringMap::Iterator it;
    for (it = map.begin(); it != map.end(); ++it) {
        new RangeItem(m_rangeList, 0, it.key(), it.value());
    };
    m_rangeList->setCurrentItem(0);

    editRange(0);
    if (RangeItem* i = dynamic_cast<RangeItem*>(m_rangeList->currentItem())
       ) {
        nameChanged(i->caption());
    }

}


} //end of namespace Search
