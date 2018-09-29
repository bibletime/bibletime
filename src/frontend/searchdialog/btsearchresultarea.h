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

#ifndef BTSEARCHRESULTAREA_H
#define BTSEARCHRESULTAREA_H

#include <QList>
#include <QSplitter>
#include <QStringList>
#include <QTextBrowser>
#include <QWidget>
#include "backend/managers/cswordbackend.h"
#include "backend/cswordmodulesearch.h"
#include "frontend/display/creaddisplay.h"
#include "frontend/searchdialog/analysis/csearchanalysisdialog.h"


namespace Search {
class CModuleResultView;
class CSearchResultView;
}
class CSwordModuleInfo;
class QFrame;
class QHBoxLayout;
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
    public: /* Methods: */
        inline StrongsResult() {}
        inline StrongsResult(const QString &text,
                             const QString &keyName)
            : m_text(text)
        {
            m_keyNameList.append(keyName);
        }

        const QString &keyText() const { return m_text; }
        inline int keyCount() const { return m_keyNameList.count(); }
        inline void addKeyName(const QString &keyName) {
            if (m_keyNameList.contains(keyName)) return;
            m_keyNameList.append(keyName);
        }

        inline const QStringList &getKeyList() const { return m_keyNameList; }

    private: /* Fields: */
        QString m_text;
        QStringList m_keyNameList;
};

/**
* \todo Fix comment!
* This class is used to keep track of the text strongs results.
* It keeps track of all instances of all strongs text results.
* This class makes use of the above class StrongsResult.
*/
class StrongsResultList: public QList<StrongsResult> {
    public: /* Methods: */
        StrongsResultList(const CSwordModuleInfo *module,
                          const sword::ListKey &results,
                          const QString &strongsNumber);

    private: /* Methods: */
        QString getStrongsNumberText(const QString &verseContent,
                                     int &startIndex,
                                     const QString &lemmaText);
};


/** The page of the search dialog which contains the search result part.
  * @author The BibleTime team
  */
class BtSearchResultArea : public QWidget {
        Q_OBJECT
    public: /* Methods: */
        BtSearchResultArea(QWidget *parent = nullptr);
        inline ~BtSearchResultArea() override { saveDialogSettings(); }

        /**
        * Sets the modules which contain the result of each.
        */
        void setSearchResult(
            const CSwordModuleSearch::Results &results);

        QSize sizeHint() const override {
            return baseSize();
        }

        QSize minimumSizeHint() const override {
            return minimumSize();
        }

    public slots:
        /**
        * Resets the current list of modules and the displayed list of found entries.
        */
        void reset();

    protected: /* Methods: */
        /**
        * Initializes the view of this widget.
        */
        void initView();

        /**
        * Initializes the signal slot conections of the child widgets
        */
        void initConnections();

        /**
        * Load the settings from the resource file
        */
        void loadDialogSettings();

        /**
        * Save the settings to the resource file
        */
        void saveDialogSettings() const;

        void setBrowserFont(const CSwordModuleInfo* const module);

    protected slots:
        /**
        * Update the preview of the selected key.
        */
        void updatePreview(const QString& key);

        /**
         * Clear the preview of the selected key.
         */
        void clearPreview();

        /**
        * Shows a dialog with the search analysis of the current search.
        */
        inline void showAnalysis() {
            CSearchAnalysisDialog(m_results, this).exec();
        }

        /**
        * Select all text
        */
        inline void selectAll() { m_previewDisplay->selectAll(); }

        /**
        * Copy selected text
        */
        inline void copySelection() {
            m_previewDisplay->copy();
        }

        void slotContextMenu(const QPoint& point);

    private: /* Fields: */
        CSwordModuleSearch::Results m_results;

        CModuleResultView* m_moduleListBox;
        CSearchResultView* m_resultListBox;

        QFrame *m_displayFrame;
        QTextBrowser* m_previewDisplay;

        QSplitter *m_mainSplitter;
        QSplitter *m_resultListSplitter;
};

} //namespace Search

#endif
