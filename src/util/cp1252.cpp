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

#include "cp1252.h"

#include <QDebug>
#ifdef BT_CP1252_USE_QT
#include <QStringDecoder>
#include <QStringEncoder>
#else
#include <optional>
#endif
#include <unicode/ucnv.h>
#ifndef BT_CP1252_USE_QT
#include "btassert.h"
#endif


namespace util {
namespace cp1252 {

#ifdef BT_CP1252_USE_QT

QString toUnicode(QByteArray const & data, bool & error) {
    QStringDecoder decoder("Windows-1252", QStringDecoder::Flag::Stateless);
    // Do not use auto here due to QTBUG-117705/QTBUG-117902:
    QString result = decoder(data);
    error = decoder.hasError();
    return result;
}

QByteArray fromUnicode(QString const & str, bool & error) {
    QStringEncoder encoder("Windows-1252", QStringEncoder::Flag::Stateless);
    // Do not use auto here due to QTBUG-117705/QTBUG-117902:
    QByteArray result = encoder(str);
    error = encoder.hasError();
    return result;
}

#else

namespace {

class Converter final {

public: // Methods:

    Converter()
        : m_converter([]{
            ::UErrorCode err = ::U_ZERO_ERROR;
            auto const r = ::ucnv_open("cp1252", &err);
            if (::U_FAILURE(err))
                qFatal() << "Failed to initialize CP1252 converter:"
                         << ::u_errorName(err);
            return r;
        }())
    { BT_ASSERT(m_converter); }

    Converter(Converter &&) = delete;
    Converter(Converter const &) = delete;

    ~Converter() { ::ucnv_close(m_converter); }

    Converter & operator=(Converter &&) = delete;
    Converter & operator=(Converter const &) = delete;

    QString toUnicode(QByteArray const & data, bool & error) {
        ::UErrorCode err = ::U_ZERO_ERROR;
        auto const length =
                ::ucnv_toUChars(
                    m_converter,
                    nullptr,
                    0,
                    data.constData(),
                    data.size(),
                    &err);
        QString result;
        if (err == ::U_BUFFER_OVERFLOW_ERROR) {
            result.resize(length, '?');
            err = ::U_ZERO_ERROR;
            ::ucnv_toUChars(
                m_converter,
                reinterpret_cast<::UChar *>(result.data()),
                length,
                data.constData(),
                data.size(),
                &err);
        }
        error = ::U_FAILURE(err);
        return result;
    }

    QByteArray fromUnicode(QString const & str, bool & error) {
        ::UErrorCode err = ::U_ZERO_ERROR;
        auto const length =
                ucnv_fromUChars(
                    m_converter,
                    nullptr,
                    0,
                    reinterpret_cast<::UChar const *>(str.constData()),
                    str.size(),
                    &err);
        QByteArray result;
        if (err == ::U_BUFFER_OVERFLOW_ERROR) {
            result.resize(length, '?');
            err = ::U_ZERO_ERROR;
            ::ucnv_fromUChars(
                m_converter,
                result.data(),
                length,
                reinterpret_cast<::UChar const *>(str.constData()),
                str.size(),
                &err);
        }
        error = ::U_FAILURE(err);
        return result;
    }

private: // Fields:

    UConverter * const m_converter;

};

thread_local std::optional<Converter> tl_converter;

inline Converter & getConverter() {
    if (!tl_converter.has_value())
        tl_converter.emplace();
    return *tl_converter;
}

} // anonymous namespace

QString toUnicode(QByteArray const & data, bool & error)
{ return getConverter().toUnicode(data, error); }

QByteArray fromUnicode(QString const & str, bool & error)
{ return getConverter().fromUnicode(str, error); }

#endif

} /* namespace cp1252 { */
} /* namespace util { */
