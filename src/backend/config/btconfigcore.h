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

#include <memory>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <utility>


class QSettings;

/**
  \note Session keys are QStrings because we even want to handle cases where the
        configuration file is manually changed. When creating new sessions, they
        are still generated from unsigned integers.
*/
class BtConfigCore {

    friend class BtConfig;

public: // methods:

    BtConfigCore(BtConfigCore &&) = default;
    BtConfigCore(BtConfigCore const &) = default;
    BtConfigCore & operator=(BtConfigCore &&) = default;
    BtConfigCore & operator=(BtConfigCore const &) = default;

    ~BtConfigCore();

    /**
      \brief Returns the settings value for the given global key.

      \param[in] key Key to get the value for.
      \param[in] defaultValue The value to return if no saved value is found.
      \returns the value of type specified by the template parameter.
    */
    template <typename T>
    T value(QString const & key, T const & defaultValue = T()) const {
        return qVariantValue(
                    key,
                    QVariant::fromValue(defaultValue)).template value<T>();
    }

    /**
      \brief Returns the settings value for the given global key as a QVariant.

      \param[in] key Key to get the value for.
      \param[in] defaultValue The value to return if no saved value is found.
      \returns the value.
    */
    QVariant qVariantValue(QString const & key,
                           QVariant const & defaultValue = QVariant()) const;

    /**
      \brief Sets a value for a key.

      \param[in] key Ket to set.
      \param[in] value Value to set.
    */
    template <typename T>
    void setValue(QString const & key, T const & value)
    { return setValue_(key, QVariant::fromValue(value)); }

    /** \returns a list of first-level keys in the current group. */
    QStringList childKeys() const;

    /** \returns a list of first-level groups in the current group. */
    QStringList childGroups() const;

    /**
      \brief removes a key (and its children) from the current group.
      \param[in] key the key to remove
    */
    void remove(QString const & key);

    /**
      \param[in] prefix the prefix to append
      \returns a BtConfigCore object for a subgroup.
    */
    template <typename Prefix>
    BtConfigCore group(Prefix && prefix) const &
    { return {m_state, m_groupPrefix + prefix + '/'}; }

    /**
      \param[in] prefix the prefix to append
      \returns a BtConfigCore object for a subgroup.
    */
    template <typename Prefix>
    BtConfigCore group(Prefix && prefix) && {
        m_groupPrefix.append(prefix).append('/');
        return std::move(*this);
    }

    /** \brief Synchronizes the configuration to disk. */
    void sync();

private: // methods:

    BtConfigCore(std::shared_ptr<QSettings> state,
                 QString groupPrefix = QString());

    void setValue_(QString const & key, QVariant value);

private: // fields:

    std::shared_ptr<QSettings> m_state;
    QString m_groupPrefix; ///< Empty or absolute path with trailing slash

}; /* class BtConfigCore */
