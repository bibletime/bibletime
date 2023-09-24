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

#include "cplaintextexportrendering.h"

#include <memory>
#include <QtGlobal>
#include "../../util/btassert.h"
#include "../drivers/btmodulelist.h"
#include "../drivers/cswordmoduleinfo.h"
#include "../keys/cswordkey.h"



namespace Rendering {

QString CPlainTextExportRendering::renderEntry(KeyTreeItem const & i,
                                               CSwordKey * k) const
{
    Q_UNUSED(k)

    if (!m_addText)
        return QString(i.key()).append('\n');

    auto const modules = i.modules();
    BT_ASSERT(!modules.isEmpty());
    std::unique_ptr<CSwordKey> const key(modules.first()->createKey());
    auto renderedText = i.key();
    if (modules.count() > 1)
        for (auto const * const module : modules)
            renderedText += QStringLiteral("   ") + module->name();
    renderedText += QStringLiteral(":\n");

    for (auto const * const module : modules) {
        key->setModule(module);
        key->setKey(i.key());
        QString entry = key->strippedText().append('\n');
        if (modules.count() > 1)
            entry.append('\n');
        renderedText.append(entry);
    }
    return renderedText;
}

QString CPlainTextExportRendering::finishText(QString const & text,
                                              KeyTree const & tree) const
{
    Q_UNUSED(tree)
    return text;
}

}
