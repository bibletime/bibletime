/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
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

        virtual void storeProfileSettings(QString const & windowGroup) const override;
        virtual void applyProfileSettings(const QString & windowGroup) override;

        /**
         * Returns true if the sync toolbar is enabled.
         */
        virtual bool syncAllowed() const override;

    protected:
        /**
        * Initialize the state of this widget.
        */
        virtual void initView() override;
        virtual void initConnections() override;
        virtual void initToolbars() override;

        virtual WriteWindowType writeWindowType() const override {
            return HTMLWindow;
        }
        /**
         * Called to add actions to mainWindow toolbars
         */
        virtual void setupMainWindowToolBars() override;

    protected slots:
        /**
        * Is called when the current text was changed.
        */
        virtual void textChanged() override;
        /**
        * Loads the original text from the module.
        */
        virtual void restoreText() override;
        /**
        * Saves the text for the current key. Directly writes the changed text into the module.
        */
        virtual void saveCurrentText( const QString& ) override;

};

#endif
