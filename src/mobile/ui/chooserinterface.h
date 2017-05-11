/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef CHOOSER_INTERFACE_H
#define CHOOSER_INTERFACE_H

#include <QObject>
#include <QString>
#include <QStringList>

namespace btm {

class ChooserInterface : public QObject {
    Q_OBJECT

public:
    ChooserInterface();

    Q_INVOKABLE QStringList getBooks(const QString& module) const;
    Q_INVOKABLE QStringList getChapters(const QString& module, const QString& book) const;
    Q_INVOKABLE QStringList getVerses(const QString& module, const QString& book, const QString& chapter) const;

};

}

#endif
