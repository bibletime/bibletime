/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef CSWORDKEY_H
#define CSWORDKEY_H

#include <QPointer>
#include <QString>
#include "../btsignal.h"


class CSwordModuleInfo;
class QTextCodec;

/** Base class for all Sword based keys. */
class CSwordKey {

public: /* Types: */

    enum TextRenderType {
        Normal = 0,
        HTMLEscaped = 1,
        ProcessEntryAttributesOnly = 2    // in this case, renderText() will not return text, but only cause EntryAttribute processing
    };

public: /* Methods: */

    CSwordKey & operator=(CSwordKey const &) = delete;

    virtual inline ~CSwordKey() { delete m_beforeChangedSignaller; }

    /**
      \returns The key which belongs to the current object.
    */
    virtual QString key() const = 0;

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
    inline const CSwordModuleInfo * module() const {
        return m_module;
    }

    /**
      Sets the module which belongs to this key.
      \param[in] newModule the module to set.
    */
    virtual inline void setModule(const CSwordModuleInfo * newModule) {
        m_module = newModule;
    }

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

    const BtSignal * beforeChangedSignaller();
    const BtSignal * afterChangedSignaller();

    /**
      \returns a new CSwordkey subclass instance for the given module, depending
               on the type of the module.
    */
    static CSwordKey * createInstance(const CSwordModuleInfo * module);

    /** Check whether key is valid. Can be invalidated during av11n mapping. */
    inline bool isValid() const { return m_valid; }

    /**
      This is called before a key change to emit a signal
    */
    void emitBeforeChanged();

    /**
      This is called after a key change to emit a signal
    */
    void emitAfterChanged();

protected: /* Methods: */

    inline CSwordKey(const CSwordModuleInfo * const module = nullptr)
        : m_module(module)
        , m_valid(true) {}

    inline CSwordKey(const CSwordKey & copy)
        : m_module(copy.m_module)
        , m_valid(copy.m_valid) {}

    /**
      \returns the encoded key appropriate for use directly with Sword.
    */
    virtual const char * rawKey() const = 0;

    static inline const QTextCodec * cp1252Codec() { return m_cp1252Codec; }

protected: /* Fields: */

    static const QTextCodec * m_cp1252Codec;
    const CSwordModuleInfo * m_module;
    QPointer<BtSignal> m_beforeChangedSignaller;
    QPointer<BtSignal> m_afterChangedSignaller;

    bool m_valid;
};

#endif
