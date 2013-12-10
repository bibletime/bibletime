/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/rendering/cplaintextexportrendering.h"

#include <QSharedPointer>
#include "backend/keys/cswordkey.h"


namespace Rendering {

CPlainTextExportRendering::CPlainTextExportRendering(
        bool addText,
        const DisplayOptions &displayOptions,
        const FilterOptions &filterOptions)
        : CHTMLExportRendering(addText, displayOptions, filterOptions)
{
    // Intentionally empty
}

QString CPlainTextExportRendering::renderEntry(const KeyTreeItem &i,
                                               CSwordKey * k)
{
    Q_UNUSED(k);

    if (!m_addText)
        return QString(i.key()).append("\n");

    const QList<const CSwordModuleInfo*> modules = i.modules();
    CSwordKey * key = CSwordKey::createInstance(modules.first());
    QString renderedText = QString(i.key()).append(":\n");

    QString entry;
    Q_FOREACH(const CSwordModuleInfo * module, modules) {
        key->setModule(module);
        key->setKey(i.key());

        /// \todo Check this code
        entry.append(key->strippedText()).append("\n");
        renderedText.append( entry );
    }

    delete key;
    return renderedText;
}

QString CPlainTextExportRendering::finishText(const QString &text, const KeyTree &tree) {
    Q_UNUSED(tree);
    return text;
}

}
