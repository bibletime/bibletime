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

#include "btbookshelfwizardpage.h"

#include <QObject>
#include <QString>


class BtSourcesThread;
class QLabel;
class QProgressBar;
class QPushButton;
class QWidget;

class BtBookshelfSourcesProgressPage final: public BtBookshelfWizardPage {

    Q_OBJECT

public: // methods:

    BtBookshelfSourcesProgressPage(QWidget * parent = nullptr);
    ~BtBookshelfSourcesProgressPage() noexcept override { destroyThread(); }

    void destroyThread() noexcept;

    void initializePage() final override;
    bool isComplete() const final override;
    int nextId() const final override;

public Q_SLOTS:
    void slotStopInstall();

private Q_SLOTS:

    void slotThreadFinished();

private: // methods:

    void retranslateUi();

private: // methods:

    bool m_installCompleted = false;
    QLabel * m_msgLabel;
    QProgressBar * m_progressBar;
    QPushButton * m_stopButton;
    BtSourcesThread * m_thread = nullptr;

};
