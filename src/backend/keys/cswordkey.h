/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CSWORDKEY_H
#define CSWORDKEY_H

#include <QPointer>
#include <QString>


class CSwordModuleInfo;
class QTextCodec;
class BtSignal;

/** Base class for all keys.
 * The base class for all Sword based keys.
 * @author The BibleTime team
 * @version $Id: cswordkey.h,v 1.27 2006/10/30 19:53:32 mgruner Exp $
 */
class CSwordKey {
    protected:
        /**
          \param module The module which belongs to this key, may be NULL
        */
        CSwordKey(const CSwordModuleInfo * const module = 0);

        CSwordKey(const CSwordKey &copy);

    public:
        enum TextRenderType {
            Normal = 0,
            HTMLEscaped = 1,
            ProcessEntryAttributesOnly = 2    // in this case, renderText() will not return text, but only cause EntryAttribute processing
        };

        virtual ~CSwordKey();

        /** Returns the current key.
         * @return The key which belongs to the current object.
         */
        virtual QString key() const = 0;

        /**
          Sets the current key. Sets the key using a utf8 enabled QString.
          \param key The key which should be used to set the current one.
        */
        virtual bool setKey(const QString &key) = 0;

        /**
          Set the key using a utf8-decoded c-string.
          \param key The key which should be used to set the current one.
        */
        virtual bool setKey(const char *key) = 0;

        /** Clone this object. Clone this current object and return it.
         * @return A clone of the current object.
         */
        virtual CSwordKey* copy() const = 0;

        /**
          \returns the module which belongs to this key.
        */
        inline const CSwordModuleInfo *module() const {
            return m_module;
        }

        /**
          Sets the module which belongs to this key.
          \param[in] newModule the module to set.
        */
        virtual inline void setModule(const CSwordModuleInfo *newModule) {
            m_module = newModule;
        }

        /** Returns the raw, unchanged text. Returns the text without any filter modifications,
         * just in the way it comes out of the module.
         */
        virtual QString rawText();
        /** Returns the rendered text. Returns the text of the current key after passing it through the
         * modules filters.
         */
        virtual QString renderedText( const CSwordKey::TextRenderType mode = CSwordKey::Normal );
        /** Stripped down text. Returns the text after removing all markup tags from it.
         */
        virtual QString strippedText();

        const BtSignal *signaler();

        /**
         * This returns a new object of the right CSwordKey* implementation
         * (e.g. CSwordVerseKey or CSwordLDKey)
         * The type is determined by the type of the module.
         * @see CSwordModuleInfo, CSwordBibleModuleInfo, CSwordCommentaryModuleInfo, CSwordLexiconModukleInfo
         */
        static CSwordKey* createInstance(const CSwordModuleInfo *module);

        /**
         * This is called before a key change to emit a signal
         * */
        void emitBeforeChanged();
        /**
         * This is called after a key change to emit a signal
         * */
        void emitChanged();

    protected:
        /**
         * Returns the encoded key appropriate for use directly with Sword.
         */
        virtual const char * rawKey() const = 0;
        static inline const QTextCodec *cp1252Codec() { return m_cp1252Codec; };

    private:
        /**
         * Disable the assignment operator
         */
        CSwordKey& operator= ( const CSwordKey & );

    protected:
        static const QTextCodec *m_cp1252Codec;
        const CSwordModuleInfo *m_module;
        QPointer<BtSignal> m_signal;
};

#endif
