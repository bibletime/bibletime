/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
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
        /** Constructor. May only be called from sublasses because this class contains pure virtual methods.
        * @param module The module which belongs to this key, may be NULL
        */
        CSwordKey(CSwordModuleInfo* const module = 0); //protected constructor, because CSwordKey shouldn't be used (it's an abstract base class).
        /** Copy constructor.
        */
        CSwordKey(const CSwordKey&); //copy constructor

    public:
        enum TextRenderType {
            Normal = 0,
            HTMLEscaped = 1,
            ProcessEntryAttributesOnly = 2	// in this case, renderText() will not return text, but only cause EntryAttribute processing
        };
        /** Destructor.
        * Public, not protected like the constructor, because CSwordKey pointers may be deleted by all others.
        */
        virtual ~CSwordKey();

        //pure virtual functions
        /** Returns the current key.
         * @return The current key which belongs to the current object.
         */
        virtual QString key() const = 0;
        /** Sets the current key. Sets the key using a utf8 enabled QString.
         * @param key The key which should be used to set the current one
         */
        virtual bool key(const QString& key) = 0;
        /** Set the key using a utf8-decoded c-string
         * @param key The key which should be used to set the current one
         */
        virtual bool key(const char* key) = 0;
        /** Clone this object. Clone this current object and return it.
         * @return A clone of the current object.
         */
        virtual CSwordKey* copy() const = 0;

        //implemented functions
        /** Set/get the module. Set and get the module which belongs to this key.
         * @return The module which belongs to this key.
         */
        inline virtual CSwordModuleInfo* module(CSwordModuleInfo* const newModule = 0);
        /** Returns the raw, unchanged text. Returns the text without any filter modifications,
         * just in the way it comes out of the module.
         */
        virtual QString rawText();
        /** Returns the rendered text. Returns the text of the current key after passign it through the
         * modules filters.
         */
        virtual QString renderedText( const CSwordKey::TextRenderType mode = CSwordKey::Normal );
        /** Stripped down text. Returns the text after removing all markup tags from it.
         */
        virtual QString strippedText();
        /**
        *
        */
        const BtSignal* signaler();
        /**
         * This returns a new object of the right CSwordKey* implementation
         * (e.g. CSwordVerseKey or CSwordLDKey)
         * The type is determined by the type of the module.
         * @see CSwordModuleInfo, CSwordBibleModuleInfo, CSwordCommentaryModuleInfo, CSwordLexiconModukleInfo
         */
        static CSwordKey* createInstance(CSwordModuleInfo * const module);

    protected:
        /**
         * Returns the encoded key appropriate for use directly with Sword.
         */
        virtual const char * rawKey() const = 0;
        static const QTextCodec* cp1252Codec();
        CSwordModuleInfo* m_module; //module pointer used by all keys
        QPointer<BtSignal> m_signal;

    private:
        /**
         * Disable the assignment operator
         */
        CSwordKey& operator= ( const CSwordKey & );

};

inline CSwordModuleInfo* CSwordKey::module(CSwordModuleInfo* const newModule) {
    if (newModule) {
        m_module = newModule;
    }
    return m_module;
}

#endif
