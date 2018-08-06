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

#include "fileinterface.h"
#include <QFile>
#include <QTextStream>
#include <QUrl>

namespace btm {

FileInterface::FileInterface(QObject *parent) :
    QObject(parent) {
}

void FileInterface::setSource(const QString& source) {
    QUrl url(source);
    m_source = url.path();
}

QString FileInterface::read() {
    if (m_source.isEmpty()){
        return QString();
    }

    QFile file(m_source);
    QString fileContent;
    if ( file.open(QIODevice::ReadOnly) ) {
        QString line;
        QTextStream t( &file );
        do {
            line = t.readLine();
            fileContent += line + "\n";
        } while (!line.isNull());

        file.close();
    } else {
        return QString();
    }
    return fileContent;
}

bool FileInterface::write(const QString& data) {
    if (m_source.isEmpty())
        return false;

    QFile file(m_source);
    if (!file.open(QFile::WriteOnly | QFile::Truncate))
        return false;

    QTextStream out(&file);
    out << data;

    file.close();

    return true;
}

}
