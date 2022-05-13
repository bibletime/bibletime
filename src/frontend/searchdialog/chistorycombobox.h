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

#include <QComboBox>

#include <QObject>
#include <QString>
#include <QStringList>


class QWidget;

namespace Search {

class CHistoryComboBox : public QComboBox {
        Q_OBJECT
    public:
        CHistoryComboBox(QWidget* parent = nullptr);
        ~CHistoryComboBox() override;
        void addToHistory(const QString& item);
        QStringList historyItems() const;

    protected:

    private:
};

} //end of namespace Search
