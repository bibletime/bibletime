/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CEXPORTMANAGER_H
#define CEXPORTMANAGER_H

#include <QList>
#include <QString>
#include "backend/config/btconfig.h"
#include "btglobal.h"

class CSwordKey;
class CSwordModuleInfo;
class ListKey;
class QProgressDialog;

/** Contains the functions to export text to disk, clipboard or printer.
  * @author The BibleTime team
  */
class CExportManager {
    public:
        /** The format the export actions should have
        */
        enum Format {
            HTML,
            Text
        };

        CExportManager(const QString &caption,
                       const bool showProgress = true,
                       const QString &progressLabel = QString::null,
                       const FilterOptions &filterOptions = BtConfig::getInstance().getFilterOptions(),
                       const DisplayOptions &displayOptions = BtConfig::getInstance().getDisplayOptions());

        bool saveKey(CSwordKey* key, const Format format, const bool addText);

        bool saveKeyList(const sword::ListKey &list,
                         const CSwordModuleInfo *module,
                         Format format,
                         bool addText);

        bool saveKeyList(const QList<CSwordKey*> &list,
                         Format format,
                         const bool addText );

        bool copyKey(CSwordKey* key, const Format format, const bool addText);

        bool copyKeyList(const sword::ListKey &list,
                         const CSwordModuleInfo *module,
                         Format format,
                         bool addText);

        bool copyKeyList(const QList<CSwordKey*> &list,
                         Format format,
                         bool addText);

        bool printKey(const CSwordKey *key,
                      const DisplayOptions &displayOptions,
                      const FilterOptions &filterOptions);

        bool printKey(const CSwordModuleInfo *module,
                      const QString &startKey,
                      const QString &stopKey,
                      const DisplayOptions &displayOptions,
                      const FilterOptions &filterOptions);

        /**
          Prints a key using the hyperlink created by CReferenceManager.
        */
        bool printByHyperlink(const QString &hyperlink,
                              const DisplayOptions &displayOptions,
                              const FilterOptions &filterOptions);

        bool printKeyList(const sword::ListKey &list,
                          const CSwordModuleInfo *module,
                          const DisplayOptions &displayOptions,
                          const FilterOptions &filterOptions);

        bool printKeyList(const QStringList &list,
                          const CSwordModuleInfo *module,
                          const DisplayOptions &displayOptions,
                          const FilterOptions &filterOptions);

    protected: // Protected methods
        /**
        * Returns the string for the filedialogs to show the correct files.
        */
        const QString filterString( const Format format );
        /**
        * Returns a filename to save a file.
        */
        const QString getSaveFileName(const Format format);

    private:
        QString m_caption; ///< \todo Useless field
        QString m_progressLabel;
        bool m_showProgress;
        FilterOptions m_filterOptions;
        DisplayOptions m_displayOptions;

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
