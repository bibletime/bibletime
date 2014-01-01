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

#ifndef BTFONTSIZEWIDGET_H
#define BTFONTSIZEWIDGET_H

#include <QComboBox>


class BtFontSizeWidget : public QComboBox {
        Q_OBJECT

    public:
        BtFontSizeWidget(QWidget* parent = 0);
        ~BtFontSizeWidget();
        int fontSize() const;

    public slots:
        void setFontSize(int size);

    private slots:
        virtual void changed(const QString& text);

    signals:
        void fontSizeChanged( int );
};

#endif
