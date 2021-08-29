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

#include "../../../backend/models/btmoduletextmodel.h"

#include <QString>


/**
 * \brief This filter provides a method for modifying text generated
 * by BtModuleTextModel.
 *
 * The "rich text" used by QML does not support
 * attributes within a href tag. These attributes are added to the url
 * so they can be obtained later for use of the MAG window.
 * It also does some other miscellaneous processing.
 */


class BtTextFilter: public BtModuleTextFilter {

public:
    BtTextFilter();
    ~BtTextFilter() override;

    QString processText(const QString& text) override;
    void setShowReferences(bool on);

private:
    void fixDoubleBR(QStringList & parts);
    int rewriteFootnoteAsLink(QStringList & parts, int i, const QString& part);
    int rewriteHref(QStringList & parts, int i, const QString& part);
    int rewriteLemmaOrMorphAsLink(QStringList & parts, int i, const QString& part);
    QStringList splitText(const QString& text);

    bool m_showReferences = false;
};
