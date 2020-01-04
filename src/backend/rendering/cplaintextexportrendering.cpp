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

#include "cplaintextexportrendering.h"

#include "../keys/cswordkey.h"


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

    const BtConstModuleList modules = i.modules();
    CSwordKey * key = CSwordKey::createInstance(modules.first());
    QString renderedText = QString(i.key());
    if (modules.count() > 1) {
        Q_FOREACH(CSwordModuleInfo const * const module, modules)
            renderedText += "   " + module->name();
    }
    renderedText += ":\n";

    Q_FOREACH(CSwordModuleInfo const * const module, modules) {
        key->setModule(module);
        key->setKey(i.key());
        QString entry = key->strippedText().append("\n");
        if (modules.count() > 1)
            entry.append("\n");
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
