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

#ifndef CHTMLWRITEWINDOW_H
#define CHTMLWRITEWINDOW_H

#include "frontend/displaywindow/cplainwritewindow.h"


/**
 * The write window class which offers a WYSIWYG text editor for creating a personal commentary.
  *
  * Inherits CPlainWriteWindow.
  *
  * @author The BibleTime team
  */
class CHTMLWriteWindow : public CPlainWriteWindow  {
        Q_OBJECT
    public:
        CHTMLWriteWindow(const QList<CSwordModuleInfo *> & modules, CMDIArea * parent);

        void storeProfileSettings(QString const & windowGroup) const override;
        void applyProfileSettings(const QString & windowGroup) override;

        bool syncAllowed() const override;

    protected:
        void initView() override;
        void initConnections() override;
        void initToolbars() override;

        WriteWindowType writeWindowType() const override {
            return HTMLWindow;
        }

        void setupMainWindowToolBars() override;

    protected slots:

        void textChanged() override;
        void restoreText() override;
        void saveCurrentText(QString const &) override;

};

#endif
