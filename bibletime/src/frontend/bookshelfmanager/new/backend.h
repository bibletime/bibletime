#include <QString>

#include "backend/drivers/cswordmoduleinfo.h"

#include <installmgr.h>


namespace backend {

/** Adds the source to the backend. */
bool addSource(sword::InstallSource& source);

/** Returns the source struct. */
sword::InstallSource source(QString name);

/** Deletes the source. */
bool deleteSource(QString name);

/** Refreshes the remote source module list. */
bool refreshSource(QString name);

/** Returns the moduleinfo list for the source. */
ListCSwordModuleInfo moduleList(QString name);

/** Tells if the source is remote or local. */
bool isRemote(const sword::InstallSource& source);

/** Returns the list of available install target paths. */
QStringList targetList();

QStringList sourceList();

/** Returns the path of the sword installer configuration file. */
const QString configPath();

/** Returns the name of the sword installer configuration file. */
const QString configFilename();

/** Sets the passive mode for as default.
* TODO: see if we can en/disable this per source.
*/
void initPassiveFtpMode();

/** Returns the file name for the Sword config file. */
const QString swordConfigFilename();

/** Returns backend Sword manager for the source. */
CSwordBackend* backend( const sword::InstallSource& is);

};