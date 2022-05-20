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

#include <QString>


class CSwordModuleInfo;
namespace sword { class SWKey; }

/** Base class for all Sword based keys. */
class CSwordKey {

public: // types:

    enum TextRenderType {
        Normal,
        ProcessEntryAttributesOnly // in this case, renderText() will not return text, but only cause EntryAttribute processing
    };

public: // methods:

    virtual ~CSwordKey() noexcept;

    CSwordKey & operator=(CSwordKey const &) = delete;

    /** \returns a reference to this object as a sword::SWKey. */
    virtual sword::SWKey const & asSwordKey() const noexcept = 0;

    /**
      \returns The key which belongs to the current object.
    */
    virtual QString key() const = 0;

    /// \returns the normalized (English) key.
    virtual QString normalizedKey() const;

    /**
      Sets the current key using a utf8 enabled QString.
      \param[in] key The key which should be used to set the current one.
    */
    virtual bool setKey(const QString & key) = 0;

    /**
      Set the key using a utf8-decoded c-string.
      \param[in] key The key which should be used to set the current one.
    */
    virtual bool setKey(const char * key) = 0;

    /**
      \returns a clone of this object.
    */
    virtual CSwordKey * copy() const = 0;

    /**
      \returns the module which belongs to this key.
    */
    CSwordModuleInfo const * module() const { return m_module; }

    /**
      Sets the module which belongs to this key.
      \param[in] newModule the module to set.
    */
    virtual void setModule(const CSwordModuleInfo * newModule)
    { m_module = newModule; }

    /**
      \returns the raw, unchanged text from the module (i.e. without any filter
               modifications).
    */
    QString rawText();

    /**
      \returns the rendered text by passing the text under the current key
               through the filters.
    */
    QString renderedText(const CSwordKey::TextRenderType mode = CSwordKey::Normal);

    /**
      \returns the text after removing all markup tags from it.
    */
    QString strippedText();

    /** Check whether key is valid. Can be invalidated during av11n mapping. */
    bool isValid() const { return m_valid; }

protected: // methods:

    CSwordKey(CSwordModuleInfo const * const module = nullptr)
        : m_module(module)
    {}

    CSwordKey(CSwordKey const & copy) = default;

    /**
      \returns the encoded key appropriate for use directly with Sword.
    */
    virtual const char * rawKey() const = 0;

protected: // fields:

    const CSwordModuleInfo * m_module;
    bool m_valid = true;

};
