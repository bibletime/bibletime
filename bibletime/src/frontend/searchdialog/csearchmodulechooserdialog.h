/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CSEARCHMODULECHOOSERDIALOG_H
#define CSEARCHMODULECHOOSERDIALOG_H

#include "frontend/cmodulechooserdialog.h"


class BTModuleTreeItem;
class CSwordModuleInfo;
class QTreeWidgetItem;

namespace Search {

class CSearchModuleChooserDialog : public CModuleChooserDialog {
        Q_OBJECT
    public:
        CSearchModuleChooserDialog(QWidget* parent, QString title, QString label, QList<CSwordModuleInfo*> selectedModules);
        ~CSearchModuleChooserDialog();

    protected: // Protected methods
        virtual void initModuleItem(BTModuleTreeItem* btItem, QTreeWidgetItem* widgetItem);

    private:
        QList<CSwordModuleInfo*> m_selectedModules;
        BTModuleTreeItem::HiddenOff* m_hiddenFilter;
};

} //end of namespace Search

#endif
