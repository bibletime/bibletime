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

#include "btconfigcore.h"

#ifndef NDEBUG
#include <QRegularExpression>
#endif
#include <QSettings>
#include <QScopeGuard>
#include "../../util/btassert.h"
#include "../../util/btdebugonly.h"


namespace {

auto groupGuard(QSettings & settings, QString const & group) {
    settings.beginGroup(group);
    return qScopeGuard([&settings]() { settings.endGroup(); });
}

} // anonymous namespace

BtConfigCore::BtConfigCore(std::shared_ptr<QSettings> state,
                           QString groupPrefix)
    : m_state(std::move(state))
    , m_groupPrefix(std::move(groupPrefix))
{
    BT_DEBUG_ONLY(
            static QRegularExpression const groupRegExp(
                    QStringLiteral("^([^/]+/)*$"));)
    BT_ASSERT(groupRegExp.match(m_groupPrefix).hasMatch());
}

BtConfigCore::~BtConfigCore() = default;

QVariant BtConfigCore::qVariantValue(QString const & key,
                                     QVariant const & defaultValue) const
{ return m_state->value(m_groupPrefix + key, defaultValue); }

QStringList BtConfigCore::childKeys() const {
    if (m_groupPrefix.isEmpty())
        return m_state->childKeys();
    auto const cleanup = groupGuard(*m_state, m_groupPrefix);
    return m_state->childKeys();
}

QStringList BtConfigCore::childGroups() const {
    if (m_groupPrefix.isEmpty())
        return m_state->childGroups();
    auto const cleanup = groupGuard(*m_state, m_groupPrefix);
    return m_state->childGroups();
}

void BtConfigCore::remove(QString const & key)
{ m_state->remove(m_groupPrefix + key); }

void BtConfigCore::sync() { m_state->sync(); }

void BtConfigCore::setValue_(QString const & key, QVariant value)
{ m_state->setValue(m_groupPrefix + key, value); }
