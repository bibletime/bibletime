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

#ifndef CINFODISPLAY_H
#define CINFODISPLAY_H

#include <QWidget>

#include <QList>
#include <QPair>
#include "backend/rendering/btinforendering.h"


class CReadDisplay;
class QAction;
class QSize;
class BibleTime;
class QTextBrowser;

namespace InfoDisplay {

class CInfoDisplay: public QWidget {

    Q_OBJECT

public: /* Methods: */

    CInfoDisplay(BibleTime * parent = nullptr);

    void unsetInfo();
    void setInfo(const QString & renderedData,
                 const QString & lang = QString());
    void setInfo(Rendering::InfoType const, QString const & data);
    void setInfo(Rendering::ListInfoData const &);
    QSize sizeHint() const override;

public slots:

    void setInfo(CSwordModuleInfo * module);

private slots:

    void lookupInfo(const QUrl &);
    void selectAll();
    void slotContextMenu(const QPoint& point);

private:
    void setBrowserFont(const CSwordModuleInfo* const module);

private: /* Fields: */

    CReadDisplay * m_htmlPart;
    QTextBrowser * m_textBrowser;
    BibleTime * m_mainWindow;

};

} //end of InfoDisplay namespace

#endif
