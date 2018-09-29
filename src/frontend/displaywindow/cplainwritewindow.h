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

#ifndef CPLAINWRITEWINDOW_H
#define CPLAINWRITEWINDOW_H

#include "frontend/displaywindow/cdisplaywindow.h"


class BtActionCollection;
class CPlainWriteDisplay;

/** The write window class which offers a plain text editor for creating a personal commentary.
  *
  * Inherits CWriteWindow.
  *
  * Inherited by CHTMLWriteWindow.
  *
  * @author The BibleTime team
  */
class CPlainWriteWindow : public CDisplayWindow {
        Q_OBJECT
    public:

        enum WriteWindowType {
            HTMLWindow = 1,
            PlainTextWindow = 2
        };

        CPlainWriteWindow(const QList<CSwordModuleInfo *> & modules, CMDIArea * parent);

        /**
        * Set the displayWidget.
        */
        void setDisplayWidget( CDisplay* display ) override;

        void storeProfileSettings(QString const & windowGroup) const override;
        void applyProfileSettings(const QString & windowGroup) override;

        void setupPopupMenu() override;

        bool syncAllowed() const override;

    public slots:

        void lookupSwordKey(CSwordKey * key) override;

    protected: // Protected methods
        void initView() override;
        void initConnections() override;
        void initToolbars() override;
        virtual WriteWindowType writeWindowType() const {
            return PlainTextWindow;
        }

        void setupMainWindowToolBars() override;
        void initActions() override;
        /**
        * Insert the keyboard accelerators of this window into the given KAccel object.
        */
        static void insertKeyboardActions( BtActionCollection* const a );

        bool queryClose() override;

    protected slots: // Protected slots

        void saveCurrentText();

        /**
        * Saves the text for the current key. Directly writes the changed text into the module.
        */
        virtual void saveCurrentText( const QString& );
        /**
        * Is called when the current text was changed.
        */
        virtual void textChanged();
        /**
        * Loads the original text from the module.
        */
        virtual void restoreText();
        /**
        * Deletes the module entry and clears the edit widget.
        */
        virtual void deleteEntry();

        virtual void beforeKeyChange();

    protected: /* Fields: */

        CPlainWriteDisplay * m_writeDisplay;

};

#endif
