
#include "bookmodulechooser.h"

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
#include "btbookinterface.h"
#include "gridchooser.h"

namespace btm {

BookModuleChooser::BookModuleChooser(QtQuick2ApplicationViewer* viewer, BtBookInterface* bookInterface)
    : viewer_(viewer),
      gridChooser_(0),
      bookInterface_(bookInterface),
      state_(CLOSED ) {
    gridChooser_ = new GridChooser(viewer_);
    bool ok = connect(gridChooser_, SIGNAL(accepted(const QString&)),
                      this, SLOT(stringAccepted(const QString&)));
    Q_ASSERT(ok);
}

void BookModuleChooser::open() {
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
        if (module->category() != CSwordModuleInfo::Books)
            continue;

//        QString lang = module->language()->abbrev();
//        if (lang != "en")
//            continue;

        QString text = index.data().toString();
        moduleNames << text;
    }
    gridChooser_->open(moduleNames, "");
}

QString BookModuleChooser::moduleName() const {
    return moduleName_;
}

void BookModuleChooser::stringAccepted(const QString& value) {
    if (state_ == MODULE)
    {
        moduleName_ = value;
        bookInterface_->setModuleName(value);
        state_ = CLOSED;
    }
}

void BookModuleChooser::stringCanceled() {
}

} // end namespace
