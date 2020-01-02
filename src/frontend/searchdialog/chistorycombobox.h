/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef CHISTORYCOMBOBOX_H
#define CHISTORYCOMBOBOX_H

#include <QComboBox>


namespace Search {

class CHistoryComboBox : public QComboBox {
        Q_OBJECT
    public:
        CHistoryComboBox(QWidget* parent = nullptr);
        ~CHistoryComboBox();
        void addToHistory(const QString& item);
        QStringList historyItems() const;

    protected:

    private:
};

} //end of namespace Search

#endif
