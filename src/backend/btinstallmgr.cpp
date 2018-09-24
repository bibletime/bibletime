/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btinstallmgr.h"

#include "../util/btassert.h"
#include "btinstallbackend.h"


namespace {

template <typename T>
inline T normalizeCompletionPercentage(const T value) {
    if (value < 0)
        return 0;
    if (value > 100)
        return 100;
    return value;
}

template <typename T>
inline int calculateIntPercentage(T done, T total) {
    BT_ASSERT(done >= 0);
    BT_ASSERT(total >= 0);

    // Special care (see warning in BtInstallMgr::statusUpdate()).
    if (done > total)
        done = total;
    if (total == 0)
        return 100;

    return normalizeCompletionPercentage(
                static_cast<int>(
                    (static_cast<double>(done) / static_cast<double>(total))
                    * 100.0));
}

} // anonymous namespace

BtInstallMgr::BtInstallMgr(QObject * parent)
        : QObject(parent)
        , swordxx::InstallMgr(BtInstallBackend::configPath().toStdString(),
                              this)
        , m_totalBytes(1u)
        , m_completedBytes(0u)
        , m_firstCallOfPreStatus(true)
{ // Use this class also as status reporter:
    this->setFTPPassive(true);
}

BtInstallMgr::~BtInstallMgr() {
    //doesn't really help because it only sets a flag
    this->terminate(); // make sure to close the connection
}

bool BtInstallMgr::isUserDisclaimerConfirmed() const {
    //// \todo Check from config if it's been confirmed with "don't show this anymore" checked.
    // Create a dialog with the message, checkbox and Continue/Cancel, Cancel as default.
    return true;
}

void BtInstallMgr::update(std::size_t dltotal, std::size_t dlnow) noexcept {
    auto const totalPercent(calculateIntPercentage(dlnow + m_completedBytes,
                                                   m_totalBytes));
    auto const filePercent(calculateIntPercentage(dlnow, dltotal));

    //qApp->processEvents();
    emit percentCompleted(totalPercent, filePercent);
}


void BtInstallMgr::preStatus(std::size_t totalBytes,
                             std::size_t completedBytes,
                             const char * message) noexcept
{
    Q_UNUSED(message);
    BT_ASSERT(completedBytes <= totalBytes);
    if (m_firstCallOfPreStatus) {
        m_firstCallOfPreStatus = false;
        emit downloadStarted();
    }
    m_completedBytes = completedBytes;
    m_totalBytes = totalBytes;
}
