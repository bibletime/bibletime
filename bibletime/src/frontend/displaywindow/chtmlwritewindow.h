/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CHTMLWRITEWINDOW_H
#define CHTMLWRITEWINDOW_H

//BibleTime includes
#include "cplainwritewindow.h"

class QAction;


/** The WYSIWYG implementation of the editor.
  * @author The BibleTime team
  */
class CHTMLWriteWindow : public CPlainWriteWindow  {
        Q_OBJECT
    public:
        CHTMLWriteWindow(QList<CSwordModuleInfo*> modules, CMDIArea* parent);
        virtual ~CHTMLWriteWindow();

        /**
        * Store the settings of this window in the given CProfileWindow object.
        */
        virtual void storeProfileSettings( Profile::CProfileWindow* );
        /**
        * Store the settings of this window in the given profile window.
        */
        virtual void applyProfileSettings( Profile::CProfileWindow* );

        virtual bool syncAllowed() const;

    protected: // Protected methods
        /**
        * Initialize the state of this widget.
        */
        virtual void initView();
        virtual void initConnections();
        virtual void initToolbars();
        virtual CDisplayWindow::WriteWindowType writeWindowType() {
            return CDisplayWindow::HTMLWindow;
        };

    protected slots:
        /**
        * Is called when the current text was changed.
        */
        virtual void textChanged();
        /**
        * Loads the original text from the module.
        */
        virtual void restoreText();
        /**
        * Saves the text for the current key. Directly writes the changed text into the module.
        */
        virtual void saveCurrentText( const QString& );
    private:
        struct {
            QAction* saveText;
            QAction* restoreText;
            QAction* deleteEntry;
            QAction* syncWindow;
        }
        m_actions;
};

#endif
