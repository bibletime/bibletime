/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CDISPLAYTEMPLATEMGR_H
#define CDISPLAYTEMPLATEMGR_H

#include <QMap>
#include <QString>
#include <QStringList>


class CSwordModuleInfo;

/**
  Manages the display templates used in the filters and display classes.
  \note This is a singleton.
*/
class CDisplayTemplateMgr {

    public: /* Types: */

        /**
          Settings which are used to fill the content into the template.
        */
        struct Settings {

            inline Settings() : pageDirection("ltr") {}

            /** The list of modules */
            QList<const CSwordModuleInfo*> modules;

            /** The title which is used for the new processed HTML page */
            QString title;

            /** The language for the HTML page. */
            QString langAbbrev;

            /** The language direction for the HTML page. */
            QString pageDirection;

            /** The CSS ID which is used in the content part of the page */
            QString pageCSS_ID;

        };

    public: /* Methods: */

        /**
          \param[out] errorMessage Set to error string on error, otherwise set
                                   to QString::null.
        */
        explicit CDisplayTemplateMgr(QString &errorMessage);

        /**
          \returns the list of available templates.
        */
        inline const QStringList availableTemplates() const {
            return m_cssMap.keys();
        }

        /**
          \brief Fills the template.

          Fills rendered content into the template given by the name.

          \param name The name of the template to fill.
          \param content The content which should be filled into the template.
          \param settings The settings which are used to process the templating
                          process.

          \returns The full HTML template HTML code including the CSS data.
        */
        QString fillTemplate(const QString &name, const QString &content,
                             const Settings &settings);

        /**
          \returns the name of the default template.
        */
        static inline const char * defaultTemplateName() { return "Blue.css"; }

        /**
          \returns the name of the active template.
        */
        static QString activeTemplateName();

        /**
          \returns The singleton instance of the instance of this class.
        */
        static inline CDisplayTemplateMgr *instance() {
            Q_ASSERT(m_instance != 0);
            return m_instance;
        }

    private: /* Methods: */

        /** Preloads a single template from disk: */
        void loadTemplate(const QString &filename);
        void loadCSSTemplate(const QString &filename);

    private: /* Fields: */

        QMap<QString, QString> m_templateMap;
        QMap<QString, QString> m_cssMap;
        static CDisplayTemplateMgr *m_instance;

};

#endif
