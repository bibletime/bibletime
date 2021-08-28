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

#include <QWizardPage>

#include <QObject>
#include <QString>


class QGroupBox;
class QRadioButton;
class QVBoxLayout;
class QWidget;

class BtPlainOrHtmlPage final: public QWizardPage {

    Q_OBJECT

public:

    BtPlainOrHtmlPage(QWidget * parent = nullptr);
    bool htmlMode() const;
    void setHtmlMode(bool html);

private:

    void retranslateUi();

    QRadioButton * m_plainTextRadioButton;
    QRadioButton * m_htmlRadioButton;
    QGroupBox * m_groupBox;
    QVBoxLayout * m_verticalLayout;
};
