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

#ifndef BTFONTSIZEWIDGET_H
#define BTFONTSIZEWIDGET_H

#include <QComboBox>


class QIntValidator;

class BtFontSizeWidget: public QComboBox {

    Q_OBJECT

public: /* Methods: */

    BtFontSizeWidget(QWidget * parent = nullptr);

    int fontSize() const;

public slots:

    void setFontSize(int size);

private slots:

    virtual void changed(QString const & text);

signals:

    void fontSizeChanged(int);

private: /* Fields: */

    QIntValidator * const m_validator;

}; /* class BtFontSizeWidget { */

#endif
