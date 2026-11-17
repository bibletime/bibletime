/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2026 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include "btphossyncengine.h"

#include <atomic>
#include <QByteArray>
#include <QString>


/**
  The OCS API of the Phos app on one Nextcloud server. Requests block the
  calling thread (a local event loop runs while waiting), so use this from a
  worker thread only.
*/
class BtPhosHttpClient final: public BtPhosHttp {

public: // methods:

    /** \param cancel when set, a running request is aborted. */
    BtPhosHttpClient(QString const & server,
                     QString user,
                     QString password,
                     QString userAgent = QStringLiteral("BibleTime"),
                     std::atomic_bool const * cancel = nullptr);

    QJsonObject get(QString const & path, Params const & params = {}) final override;
    QJsonObject post(QString const & path, QJsonObject const & body) final override;

    /** Scheme added, trailing slashes removed. */
    static QString normalizeServerUrl(QString const & server);

    /** Nextcloud app password: 72 characters, or 5 groups of 5 with dashes. */
    static bool looksLikeAppPassword(QString const & password);

    /**
      Trades a login password for a new app password
      (/ocs/v2.php/core/getapppassword); an app password is returned as it is.
      Throws BtPhosHttpError.
    */
    static QString appPassword(QString const & server,
                               QString const & user,
                               QString const & password,
                               QString const & userAgent,
                               std::atomic_bool const * cancel = nullptr);

private: // methods:

    QJsonObject call(QByteArray const & method,
                     QString const & url,
                     QByteArray const & body);

    static std::pair<int, QByteArray> request(QByteArray const & method,
                                              QString const & url,
                                              QString const & user,
                                              QString const & password,
                                              QString const & userAgent,
                                              QByteArray const & body,
                                              std::atomic_bool const * cancel);

private: // fields:

    QString m_base;
    QString m_user;
    QString m_password;
    QString m_userAgent;
    std::atomic_bool const * m_cancel;

};
