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

#include "btinstallmgr.h"

#include <QByteArray>
#include <QtGlobal>
#include <type_traits>
#include <utility>
#include "../util/btassert.h"
#include "btinstallbackend.h"


// Sword includes:
#include <defs.h>
#include <installmgr.h>


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
    if (total <= 0.0)
        return 100;

    return normalizeCompletionPercentage<int>((done / total) * 100);
}

template <typename T, class = void>
struct HasSetTimeoutMillis : std::false_type {};

template <typename T>
struct HasSetTimeoutMillis<T,
        std::void_t<decltype(std::declval<T &>().setTimeoutMillis(0))>>
    : std::true_type
{};

} // anonymous namespace

using namespace sword;

BtInstallMgr::BtInstallMgr(QObject * parent)
        : QObject(parent)
        , InstallMgr(BtInstallBackend::configPath().toLatin1(), this)
        , m_totalBytes(1)
        , m_completedBytes(0)
        , m_firstCallOfPreStatus(true)
{
    setFTPPassive(true);
    if constexpr (HasSetTimeoutMillis<BtInstallMgr>::value)
        setTimeoutMillis(0);
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

void BtInstallMgr::statusUpdate(double dltotal, double dlnow) {
    /**
      \warning Note that these *might be* rough measures due to the double data
               type being used by Sword to store the number of bytes. Special
               care must be taken to work around this, since the arguments may
               contain weird values which would otherwise break this logic.
    */

    if (dltotal < 0.0) // Special care (see warning above)
        dltotal = 0.0;
    if (dlnow < 0.0) // Special care (see warning above)
        dlnow = 0.0;

    const int totalPercent = calculateIntPercentage<double>(dlnow + m_completedBytes,
                                                            m_totalBytes);
    const int filePercent  = calculateIntPercentage(dlnow, dltotal);

    //qApp->processEvents();
    Q_EMIT percentCompleted(totalPercent, filePercent);
}


void BtInstallMgr::preStatus(long totalBytes,
                             long completedBytes,
                             const char * message)
{
    Q_UNUSED(message)
    BT_ASSERT(completedBytes <= totalBytes);
    if (m_firstCallOfPreStatus) {
        m_firstCallOfPreStatus = false;
        Q_EMIT downloadStarted();
    }
    m_completedBytes = completedBytes;
    m_totalBytes = totalBytes;
}
