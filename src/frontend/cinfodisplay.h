/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CINFODISPLAY_H
#define CINFODISPLAY_H

#include <QWidget>

#include <QList>
#include <QPair>
#include "backend/rendering/ctextrendering.h"


class CReadDisplay;
class QAction;
class QSize;
class BibleTime;


namespace InfoDisplay {

class CInfoDisplay: public QWidget {

    Q_OBJECT

public: /* Types: */

    enum InfoType {
        Abbreviation,
        CrossReference,
        Footnote,
        Lemma,
        Morph,
        WordTranslation,
        WordGloss,
        Text
    };

    typedef QPair<InfoType, QString> InfoData;
    typedef QList<InfoData> ListInfoData;

public: /* Methods: */

    CInfoDisplay(BibleTime * parent = NULL);

    void unsetInfo();
    void setInfo(const QString & renderedData,
                 const QString & lang = QString());
    void setInfo(const InfoType, const QString & data);
    void setInfo(const ListInfoData &);
    QSize sizeHint() const;

public slots:

    void setInfo(CSwordModuleInfo * module);

private: /* Methods: */

    QString decodeAbbreviation(QString const & data);
    QString decodeCrossReference(QString const & data);
    QString decodeFootnote(QString const & data);
    QString decodeStrongs(QString const & data);
    QString decodeMorph(QString const & data);
    QString getWordTranslation(QString const & data);

private slots:

    void lookupInfo(const QString &, const QString &);
    void selectAll();

private: /* Fields: */

    CReadDisplay * m_htmlPart;
    BibleTime * m_mainWindow;

};

} //end of InfoDisplay namespace

#endif
