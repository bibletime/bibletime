/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTFINDIDGET_H
#define BTFINDIDGET_H

#include <QWidget>
class QString;
class QToolButton;

class QPaintEvent;

class BtFindWidget : public QWidget {
        Q_OBJECT

    public:
        BtFindWidget(QWidget* parent = 0);
        ~BtFindWidget();

    private slots:
        void closeWidget();

    private:
        QToolButton* createToolButton(const QString& iconName);

};

#endif


