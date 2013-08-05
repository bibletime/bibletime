
#include "modulechooser.h"

#include "qtquick2applicationviewer.h"

#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/managers/cswordbackend.h"
#include <cmath>
#include <QQuickItem>
#include <QQmlProperty>
#include <QQmlContext>
#include <QDebug>
#include <QQmlProperty>
#include <QCoreApplication>
#include "btwindowinterface.h"
#include "gridchooser.h"

namespace btm {

ModuleChooser::ModuleChooser(QtQuick2ApplicationViewer* viewer, BtWindowInterface* bibleVerse)
    : viewer_(viewer),
      gridChooser_(0),
      bibleVerse_(bibleVerse),
      state_(CLOSED ) {
    gridChooser_ = new GridChooser(viewer_);
    bool ok = connect(gridChooser_, SIGNAL(accepted(const QString&)),
                      this, SLOT(stringAccepted(const QString&)));
    Q_ASSERT(ok);
}

void ModuleChooser::open() {
    state_ = MODULE;
    BtBookshelfModel* bookshelfModel = CSwordBackend::instance()->model();
    int count = bookshelfModel->rowCount();
    QStringList moduleNames;
    typedef BtBookshelfModel::ModuleRole MRole;
    static const MRole HR(BtBookshelfModel::ModuleHiddenRole);
    static const MRole PR(BtBookshelfModel::ModulePointerRole);
    static const MRole IR(BtBookshelfModel::ModuleHasIndexRole);
    static const MRole CR(BtBookshelfModel::ModuleCategoryRole);

    for (int row=0; row<count; ++row) {
        QModelIndex index = bookshelfModel->index(row);

        CSwordModuleInfo *module(
            static_cast<CSwordModuleInfo *>(
                bookshelfModel->data(index, PR).value<void*>()
            )
        );
        if (module->category() != CSwordModuleInfo::Bibles &&
                module->category() != CSwordModuleInfo::Commentaries)
            continue;

//        QString lang = module->language()->abbrev();
//        if (lang != "en")
//            continue;

        QString text = index.data().toString();
        moduleNames << text;
    }
    gridChooser_->open(moduleNames, "");
}

QString ModuleChooser::moduleName() const {
    return moduleName_;
}

void ModuleChooser::stringAccepted(const QString& value) {
    if (state_ == MODULE)
    {
        moduleName_ = value;
        bibleVerse_->setModuleName(value);
        state_ = CLOSED;
    }
}

void ModuleChooser::stringCanceled() {
}

} // end namespace
