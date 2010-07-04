/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTSEARCHRESULTAREA_H
#define BTSEARCHRESULTAREA_H

#include <QList>
#include <QSplitter>
#include <QStringList>
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
    public:
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

    private:
        QString m_text;
        QStringList m_keyNameList;
};

/**
* This class is used to keep track of the text strongs results.
* It keeps track of all instances of all strongs text results.
* This class makes use of the above class StrongsResult.
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
class StrongsResultClass {
    public:
        inline StrongsResultClass(CSwordModuleInfo *module,
                                  const QString &strongsNumber)
                : m_srModule(module), m_lemmaText(strongsNumber)
        {
            initStrongsResults();
        }

        inline QString keyText(int index) const {
            return m_srList[index].keyText();
        }
        inline int keyCount(int index) const {
            return m_srList[index].keyCount();
        }
        inline const QStringList &getKeyList(int index) const {
            return m_srList[index].getKeyList();
        }
        inline int count() const {
            return m_srList.count();
        }

    private:
        void initStrongsResults(void);
        QString getStrongsNumberText(const QString& verseContent, int *startIndex);

        QList<StrongsResult> m_srList;
        CSwordModuleInfo *m_srModule;
        QString m_lemmaText;
};


/** The page of the search dialog which contains the search result part.
  * @author The BibleTime team
  */
class BtSearchResultArea : public QWidget {
        Q_OBJECT
    public:
        BtSearchResultArea(QWidget *parent = 0);
        inline ~BtSearchResultArea() { saveDialogSettings(); }

        /**
        * Sets the modules which contain the result of each.
        */
        void setSearchResult(const QList<CSwordModuleInfo*> &modules);

        /**
          Reimplemented from QWidget::sizeHint().
        */
        virtual QSize sizeHint() const {
            return baseSize();
        }

        /**
          Reimplemented from QWidget::minimumSizeHint().
        */
        virtual QSize minimumSizeHint() const {
            return minimumSize();
        }

    public slots:
        /**
        * Resets the current list of modules and the displayed list of found entries.
        */
        void reset();

    protected:
        /**
        * Initializes the view of this widget.
        */
        void initView();

        /**
        * Initializes the signal slot conections of the child widgets
        */
        void initConnections();

        /**
        * This function breakes the queryString into clucene tokens
        */
        QStringList queryParser(const QString& queryString);

        /**
        * This function highlights the searched text in the content using the search type given by search flags
        */
        QString highlightSearchedText(const QString& content, const QString& searchedText);

        /**
        * Load the settings from the resource file
        */
        void loadDialogSettings();

        /**
        * Save the settings to the resource file
        */
        void saveDialogSettings();

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
            CSearchAnalysisDialog(m_modules, this).exec();
        }

        /**
        * Select all text
        */
        inline void selectAll() { m_previewDisplay->selectAll(); }

        /**
        * Copy selected text
        */
        inline void copySelection() {
            m_previewDisplay->connectionsProxy()->copySelection();
        }

    private:

        CModuleResultView* m_moduleListBox;
        CSearchResultView* m_resultListBox;

        QFrame *m_displayFrame;
        CReadDisplay* m_previewDisplay;

        QList<CSwordModuleInfo*> m_modules;

        QSplitter *m_mainSplitter;
        QSplitter *m_resultListSplitter;
};

} //namespace Search

#endif
