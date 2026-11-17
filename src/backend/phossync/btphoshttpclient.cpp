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

#include "btphoshttpclient.h"

#include <algorithm>
#include <QCoreApplication>
#include <QEventLoop>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QUrl>


namespace {

QString const API = QStringLiteral("/ocs/v2.php/apps/phos/api/v1");

QString tr(char const * text) {
    return QCoreApplication::translate("BtPhosHttpClient", text);
}

bool isAsciiAlnum(QChar const c) {
    auto const u = c.unicode();
    return (u >= u'0' && u <= u'9') || (u >= u'a' && u <= u'z')
           || (u >= u'A' && u <= u'Z');
}

} // anonymous namespace

BtPhosHttpClient::BtPhosHttpClient(QString const & server,
                                   QString user,
                                   QString password,
                                   QString userAgent,
                                   std::atomic_bool const * cancel)
    : m_base(normalizeServerUrl(server))
    , m_user(std::move(user))
    , m_password(std::move(password))
    , m_userAgent(std::move(userAgent))
    , m_cancel(cancel)
{}

QString BtPhosHttpClient::normalizeServerUrl(QString const & server) {
    auto s = server.trimmed();
    if (s.isEmpty())
        return s;
    if (!s.contains(QStringLiteral("://")))
        s.prepend(QStringLiteral("https://"));
    while (s.endsWith(u'/'))
        s.chop(1);
    return s;
}

bool BtPhosHttpClient::looksLikeAppPassword(QString const & pw) {
    if (pw.size() == 72)
        return std::all_of(pw.begin(), pw.end(), isAsciiAlnum);
    if (pw.size() != 29)
        return false;
    for (qsizetype i = 0; i < pw.size(); ++i) {
        if ((i + 1) % 6 == 0) {
            if (pw[i] != u'-')
                return false;
        } else if (!isAsciiAlnum(pw[i])) {
            return false;
        }
    }
    return true;
}

QJsonObject BtPhosHttpClient::get(QString const & path, Params const & params) {
    auto url = m_base + API + path;
    QChar sep = u'?';
    for (auto const & [k, v] : params) {
        url += sep + QString::fromLatin1(QUrl::toPercentEncoding(k)) + u'='
               + QString::fromLatin1(QUrl::toPercentEncoding(v));
        sep = u'&';
    }
    return call("GET", url, {});
}

QJsonObject BtPhosHttpClient::post(QString const & path, QJsonObject const & body) {
    return call("POST", m_base + API + path,
                QJsonDocument(body).toJson(QJsonDocument::Compact));
}

QJsonObject BtPhosHttpClient::call(QByteArray const & method,
                                   QString const & url,
                                   QByteArray const & body)
{
    auto const [code, text] =
            request(method, url, m_user, m_password, m_userAgent, body,
                    m_cancel);
    auto const root = QJsonDocument::fromJson(text).object();
    auto const ocs = root.value(QStringLiteral("ocs")).toObject();
    auto const dataValue = ocs.value(QStringLiteral("data"));
    if (code < 200 || code > 299) {
        auto msg = dataValue.toObject().value(QStringLiteral("error")).toString();
        if (msg.isEmpty())
            msg = ocs.value(QStringLiteral("meta")).toObject()
                  .value(QStringLiteral("message")).toString();
        switch (code) {
        case 401:
            throw BtPhosHttpError(code, tr("Login refused (401): user name or "
                                           "app password wrong"));
        case 404:
            throw BtPhosHttpError(code, tr("The Phos app is not installed on "
                                           "this Nextcloud (404)"));
        case 503:
            throw BtPhosHttpError(code, tr("The Phos app needs an update on "
                                           "the server (503)"));
        default:
            throw BtPhosHttpError(code, msg.isEmpty()
                                  ? QStringLiteral("HTTP %1").arg(code)
                                  : QStringLiteral("HTTP %1: %2").arg(code).arg(msg));
        }
    }
    if (dataValue.isObject())
        return dataValue.toObject();
    // some endpoints answer with an empty list as data
    if (root.contains(QStringLiteral("ocs")))
        return {};
    throw BtPhosHttpError(code, tr("Unexpected answer from the server"));
}

std::pair<int, QByteArray> BtPhosHttpClient::request(QByteArray const & method,
                                                     QString const & url,
                                                     QString const & user,
                                                     QString const & password,
                                                     QString const & userAgent,
                                                     QByteArray const & body,
                                                     std::atomic_bool const * cancel)
{
    QNetworkAccessManager nam;
    QNetworkRequest req{QUrl(url, QUrl::StrictMode)};
    if (!req.url().isValid())
        throw BtPhosHttpError(0, tr("Invalid server address"), true);
    req.setRawHeader("Authorization",
                     "Basic " + (user + u':' + password).toUtf8().toBase64());
    req.setRawHeader("OCS-APIRequest", "true");
    req.setRawHeader("Accept", "application/json");
    req.setHeader(QNetworkRequest::UserAgentHeader, userAgent);
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                     QNetworkRequest::NoLessSafeRedirectPolicy);
    req.setTransferTimeout(90000);
    QNetworkReply * reply;
    if (body.isNull()) {
        reply = nam.sendCustomRequest(req, method);
    } else {
        req.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/json; charset=utf-8"));
        reply = nam.sendCustomRequest(req, method, body);
    }
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QTimer cancelCheck;
    if (cancel) {
        QObject::connect(&cancelCheck, &QTimer::timeout, reply,
                         [cancel, reply] { if (*cancel) reply->abort(); });
        cancelCheck.start(100);
    }
    if (!reply->isFinished())
        loop.exec();
    cancelCheck.stop();
    auto const code =
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    auto const data = reply->readAll();
    auto const error = reply->error();
    auto const errorString = reply->errorString();
    delete reply;
    if (cancel && *cancel)
        throw BtPhosHttpError(0, tr("Cancelled"), true);
    if (code == 0 && error != QNetworkReply::NoError)
        throw BtPhosHttpError(0, errorString, true);
    return {code, data};
}

QString BtPhosHttpClient::appPassword(QString const & server,
                                      QString const & user,
                                      QString const & password,
                                      QString const & userAgent,
                                      std::atomic_bool const * cancel)
{
    if (looksLikeAppPassword(password))
        return password;
    auto const [code, text] =
            request("GET",
                    normalizeServerUrl(server)
                    + QStringLiteral("/ocs/v2.php/core/getapppassword"),
                    user, password, userAgent, {}, cancel);
    if (code == 401)
        throw BtPhosHttpError(401, tr("Login refused: user name or password "
                                      "wrong"));
    if (code == 403)
        return password; // already an app password
    if (code < 200 || code > 299)
        throw BtPhosHttpError(code, tr("No app password from the server "
                                       "(HTTP %1)").arg(code));
    auto const pw = QJsonDocument::fromJson(text).object()
                    .value(QStringLiteral("ocs")).toObject()
                    .value(QStringLiteral("data")).toObject()
                    .value(QStringLiteral("apppassword")).toString();
    if (pw.isEmpty())
        throw BtPhosHttpError(code, tr("No app password in the server's "
                                       "answer"));
    return pw;
}
