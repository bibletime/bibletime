/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include <QList>
#include <QSplitter>
#include <QStringList>
#include <QWidget>
#include "../../backend/cswordmodulesearch.h"
#include "analysis/csearchanalysisdialog.h"
#include "../bttextbrowser.h"


namespace Search {
class CModuleResultView;
class CSearchResultView;
}
class CSwordModuleInfo;
class QAction;
class QFrame;
class QHBoxLayout;
class QMenu;
class QTreeWidget;

namespace Search {

/**
* This class is used to keep track of the text strongs results.
* It only keeps track of one instance of a strongs text result.
*
*    The functions of the class are:
*       - Store an instance of a strongs text result.
*       - Each strongs text result will contain a list of verses (keyNames).
*       - The number of verses (keyNames) is returned by keyCount().
*       - The text for the strongs text result is returned by keyText().
*       - The list of verses (keyNames) is returned by getKeyList() [as QStringList].
*
*   To add a new verse to a strongs text result use addKeyName.
*/
class StrongsResult {
    public: // methods:
        StrongsResult() {}
        StrongsResult(QString const & text,
                      QString const & keyName)
            : m_text(text)
        {
            m_keyNameList.append(keyName);
        }

        const QString &keyText() const { return m_text; }
        int keyCount() const { return m_keyNameList.count(); }
        void addKeyName(QString const & keyName) {
            if (m_keyNameList.contains(keyName)) return;
            m_keyNameList.append(keyName);
        }

        QStringList const & getKeyList() const { return m_keyNameList; }

    private: // fields:
        QString m_text;
        QStringList m_keyNameList;
};


/** The page of the search dialog which contains the search result part.
  * @author The BibleTime team
  */
class BtSearchResultArea : public QWidget {
        Q_OBJECT
    public: // methods:
        BtSearchResultArea(QWidget * parent = nullptr);
        ~BtSearchResultArea() override { saveDialogSettings(); }

        /**
        * Sets the modules which contain the result of each.
        */
        void setSearchResult(QString searchedText,
                             CSwordModuleSearch::Results results);

        QSize sizeHint() const override {
            return baseSize();
        }

        QSize minimumSizeHint() const override {
            return minimumSize();
        }

    public Q_SLOTS:
        /**
        * Resets the current list of modules and the displayed list of found entries.
        */
        void reset();

        /** Shows a dialog with the search analysis of the current search. */
        void showAnalysis()
        { CSearchAnalysisDialog(m_searchedText, m_results, this).exec(); }

    protected: // methods:

        /**
        * Load the settings from the resource file
        */
        void loadDialogSettings();

        /**
        * Save the settings to the resource file
        */
        void saveDialogSettings() const;

        void setBrowserFont(const CSwordModuleInfo* const module);

    protected Q_SLOTS:
        /**
        * Update the preview of the selected key.
        */
        void updatePreview(const QString& key);

        /**
         * Clear the preview of the selected key.
         */
        void clearPreview();

        /**
        * Copy selected text
        */
        void copySelection() {
            m_previewDisplay->copy();
        }

    private: // fields:
        QString m_searchedText;
        CSwordModuleSearch::Results m_results;

        CModuleResultView* m_moduleListBox;
        CSearchResultView* m_resultListBox;

        QFrame *m_displayFrame;

        QMenu * m_contextMenu;
            QAction * m_selectAllAction;
            QAction * m_copyAction;

        BtTextBrowser* m_previewDisplay;

        QSplitter *m_mainSplitter;
        QSplitter *m_resultListSplitter;
};

} //namespace Search
