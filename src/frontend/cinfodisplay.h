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

#include <QWidget>

#include <QPair>
#include "../backend/rendering/btinforendering.h"


class QAction;
class QSize;
class BibleTime;
class BtTextBrowser;

namespace InfoDisplay {

class CInfoDisplay: public QWidget {

    Q_OBJECT

public: // methods:

    CInfoDisplay(BibleTime * parent = nullptr);

    void unsetInfo();
    void setInfo(const QString & renderedData,
                 const QString & lang = QString());
    void setInfo(Rendering::ListInfoData const &);
    QSize sizeHint() const override;
    void updateColors();

public Q_SLOTS:

    void setInfo(Rendering::InfoType const, QString const & data);
    void setInfo(CSwordModuleInfo * module);

private:
    void setBrowserFont(const CSwordModuleInfo* const module);

private: // fields:

    BtTextBrowser * m_textBrowser;
    BibleTime * m_mainWindow;

};

} //end of InfoDisplay namespace
