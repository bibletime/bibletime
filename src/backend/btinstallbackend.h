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


class CSwordBackend;
namespace sword { class InstallSource; }

namespace BtInstallBackend {

/** Adds the source to the backend. */
bool addSource(sword::InstallSource& source);

/** Returns the source struct. */
sword::InstallSource source(const QString &name);

/** Deletes the source. */
bool deleteSource(const QString &name);

/** Tells if the source is remote or local. */
bool isRemote(const sword::InstallSource& source);

QStringList sourceNameList();

/** Returns the path of the sword installer configuration file. */
QString configPath();

/** Returns the name of the sword installer configuration file. */
QString configFilename();

/** Returns backend Sword manager for the source. */
std::unique_ptr<CSwordBackend> backend(sword::InstallSource const & is);

} // namespace BtInstallBackend
