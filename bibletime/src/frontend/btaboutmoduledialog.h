//
// C++ Interface: btaboutmoduledialog
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BTABOUTMODULEDIALOG_H
#define BTABOUTMODULEDIALOG_H

#include <QDialog>

class CSwordModuleInfo;
class QWidget;

/**
	Dialog to show the information about a module.
	
	@author The BibleTime team <info@bibletime.info>
*/
class BTAboutModuleDialog : public QDialog
{
public:
    BTAboutModuleDialog(QWidget* parent, CSwordModuleInfo* info);
};

#endif
