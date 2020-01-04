/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef CEXPORTMANAGER_H
#define CEXPORTMANAGER_H

#include <memory>
#include <QList>
#include <QString>
#include "../backend/btglobal.h"
#include "../backend/config/btconfig.h"


class CSwordKey;
class CSwordModuleInfo;
class ListKey;
class QProgressDialog;
namespace Rendering {
class CTextRendering;
}

class CExportManager {

public: /* Types: */

    /** The format the export actions should have. */
    enum Format {
        HTML,
        Text
    };

public: /* Methods: */

    CExportManager(const bool showProgress = true,
                   const QString &progressLabel = QString(),
                   const FilterOptions &filterOptions = btConfig().getFilterOptions(),
                   const DisplayOptions &displayOptions = btConfig().getDisplayOptions());
    ~CExportManager();

    bool saveKey(CSwordKey const * const key,
                 Format const format,
                 bool const addText,
                 const BtConstModuleList& modules);

    bool saveKeyList(sword::ListKey const & list,
                     CSwordModuleInfo const * const module,
                     Format const format,
                     bool const addText);

    bool saveKeyList(QList<CSwordKey *> const & list,
                     Format const format,
                     bool const addText);

    bool copyKey(CSwordKey const * const key,
                 Format const format,
                 bool const addText);

    bool copyKeyList(sword::ListKey const & list,
                     CSwordModuleInfo const * const module,
                     Format const format,
                     bool const addText);

    bool copyKeyList(QList<CSwordKey *> const & list,
                     Format const format,
                     bool const addText);

    bool printKey(CSwordKey const * const key,
                  DisplayOptions const & displayOptions,
                  FilterOptions const & filterOptions);

    bool printKey(CSwordModuleInfo const * const module,
                  QString const & startKey,
                  QString const & stopKey,
                  DisplayOptions const & displayOptions,
                  FilterOptions const & filterOptions);

    /**
      \brief Prints a key using the hyperlink created by CReferenceManager.
    */
    bool printByHyperlink(QString const & hyperlink,
                          DisplayOptions const & displayOptions,
                          FilterOptions const & filterOptions);

    bool printKeyList(sword::ListKey const & list,
                      CSwordModuleInfo const * const module,
                      DisplayOptions const & displayOptions,
                      FilterOptions const & filterOptions);

    bool printKeyList(QStringList const & list,
                      CSwordModuleInfo const * const module,
                      DisplayOptions const & displayOptions,
                      FilterOptions const & filterOptions);

protected: /* Methods: */

    /** \returns a filename to save a file. */
    const QString getSaveFileName(Format const format);

private: /* Methods: */

    std::unique_ptr<Rendering::CTextRendering> newRenderer(Format const format,
                                                           bool const addText);

    /** \returns the CSS string used in HTML pages. */
    void setProgressRange(int const items);

    /** \brief Increments the progress by one item. */
    void incProgress();

    bool progressWasCancelled();

    /** \brief Closes the progress dialog immediately. */
    void closeProgressDialog();

private: /* Fields: */

    FilterOptions m_filterOptions;
    DisplayOptions m_displayOptions;

    QProgressDialog * m_progressDialog;

};

#endif
