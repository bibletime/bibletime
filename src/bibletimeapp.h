/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BIBLETIMEAPP_H
#define BIBLETIMEAPP_H

#include <QApplication>
#include <QIcon>
#include <QMap>


/**
  The BibleTimeApp class is used to clean up all instances of the backend and to
  delete all created module objects.
*/
class BibleTimeApp : public QApplication {

    Q_OBJECT

    public: /* Methods: */

        BibleTimeApp(int &argc, char **argv);
        ~BibleTimeApp();

        inline void startInit() { m_init = true; }
        bool initBtConfig();
        bool initDisplayTemplateManager();

        /**
           \param[in] name the name of the icon to return.
           \returns a reference to the icon with the given name or to a NULL
                    icon if no such icon is found.
         */
        const QIcon & getIcon(const QString & name) const;

    private: /* Fields: */

        mutable QMap<QString, QIcon> m_iconCache;
        const QIcon m_nullIcon;
        bool m_init;

};

#define bApp (static_cast<BibleTimeApp *>(QCoreApplication::instance()))

#endif
