/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BT_SEARCH_INTERFACE_H
#define BT_SEARCH_INTERFACE_H

#include <QObject>
#include <QString>
#include "mobile/models/roleitemmodel.h"

class CSwordKey;
class CSwordVerseKey;
class CSwordModuleInfo;
class BtModuleTextModel;

namespace btm {

class BtSearchInterface : public QObject {

    Q_OBJECT

    Q_PROPERTY(QString searchText READ getSearchText WRITE setSearchText)
    Q_PROPERTY(QString findChoice READ getFindChoice WRITE setFindChoice)
    Q_PROPERTY(QString moduleList READ getModuleList WRITE setModuleList)

public:
    Q_INVOKABLE bool performSearch();

    BtSearchInterface(QObject *parent = 0);
    ~BtSearchInterface();

    QString getSearchText() const;
    QString getFindChoice() const;
    QString getModuleList() const;

    void setSearchText(const QString& searchText);
    void setFindChoice(const QString& findChoice);
    void setModuleList(const QString& moduleList);

signals:

private slots:

private:
    QString m_searchText;
    QString m_findChoice;
    QString m_moduleList;
};

} // end namespace

#endif
