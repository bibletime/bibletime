/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CEXPORTMANAGER_H
#define CEXPORTMANAGER_H

#include "util/cpointers.h"

#include <QList>
#include <QString>
#include "backend/config/cbtconfig.h"
#include "backend/managers/cswordbackend.h"


class CSwordKey;
class CSwordModuleInfo;
class ListKey;
class QProgressDialog;

/** Contains the functions to export text to disk, clipboard or printer.
  * @author The BibleTime team
  */
class CExportManager : CPointers {
    public:
        /** The format the export actions should have
        */
        enum Format {
            HTML,
            Text
        };

        CExportManager(const QString& caption, const bool showProgress = true, const QString& progressLabel = QString::null, const CSwordBackend::FilterOptions filterOptions = CBTConfig::getFilterOptionDefaults(), const CSwordBackend::DisplayOptions displayOptions = CBTConfig::getDisplayOptionDefaults());

        bool saveKey(CSwordKey* key, const Format format, const bool addText);
        bool saveKeyList(sword::ListKey* list, CSwordModuleInfo* module, const Format format, const bool addText);
        bool saveKeyList(QList<CSwordKey*>& list, const Format format, const bool addText );

        bool copyKey(CSwordKey* key, const Format format, const bool addText);
        bool copyKeyList(sword::ListKey* list, CSwordModuleInfo* module, const Format format, const bool addText);
        bool copyKeyList(QList<CSwordKey*>& list, const Format format, const bool addText );

        bool printKey(CSwordKey* key, CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions);
        bool printKey( CSwordModuleInfo* module, const QString& startKey, const QString& stopKey, CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions );
        bool printByHyperlink(const QString& hyperlink, CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions);
        bool printKeyList(sword::ListKey* list, CSwordModuleInfo* module, CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions);
        bool printKeyList(const QStringList& list, CSwordModuleInfo* module, CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions);

    protected: // Protected methods
        /**
        * Returns the string for the filedialogs to show the correct files.
        */
        const QString filterString( const Format format );
        /**
        * Returns a filename to save a file.
        */
        const QString getSaveFileName(const Format format);
        /**
        * Returns a string containing the linebreak for the current format.
        */
        const QString lineBreak( const Format format );

    private:
        QString m_caption;
        QString m_progressLabel;
        bool m_showProgress;
        CSwordBackend::FilterOptions m_filterOptions;
        CSwordBackend::DisplayOptions m_displayOptions;

        QProgressDialog* m_progressDialog;

        /**
        * Creates the progress dialog with the correct settings.
        */
        QProgressDialog* progressDialog();
        /**
        * Returns the CSS string used in HTML pages.
        */
        void setProgressRange( const int item );
        /**
        * Increments the progress by one item.
        */
        inline void incProgress();
        bool progressWasCancelled();
        /**
        * Closes the progress dialog immediately.
        */
        void closeProgressDialog();
};

#endif
