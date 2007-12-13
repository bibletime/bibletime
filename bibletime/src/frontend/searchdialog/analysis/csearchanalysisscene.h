//
// C++ Interface: csearchanalysisscene
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SEARCHCSEARCHANALYSISSCENE_H
#define SEARCHCSEARCHANALYSISSCENE_H

#include "csearchanalysisitem.h"

#include "backend/drivers/cswordmoduleinfo.h"


#include <QGraphicsScene>
#include <QColor>
#include <QMap>
#include <QHash>

namespace Search {

class CSearchAnalysisLegendItem;

/**
	@author The BibleTime team <info@bibletime.info>
*/
class CSearchAnalysisScene : public QGraphicsScene
{
public:
    CSearchAnalysisScene(QObject* parent);

    virtual ~CSearchAnalysisScene() {}

	/**
	* Starts the analysis of the search result.
	* This should be called only once because
	* QCanvas handles the updates automatically.
	*/
	void analyse(ListCSwordModuleInfo modules);
	/**
	* This function returns a color for each module
	* @return The color at position index in the list
	*/
	static QColor getColor(int index);
	/**
	* This function returns a pointer to the list of AnalysisItems
	*/
	QHash<CSearchAnalysisItem>* getSearchAnalysisItemList();
	void reset();

protected slots: // Protected slots
	/**
	* No descriptions
	*/
	void slotResized();

protected:
	void setModules(ListCSwordModuleInfo modules);

private:
	/**
	* Returns the count of the book in the module
	*/
	const unsigned int getCount( const QString book, CSwordModuleInfo* module );

	ListCSwordModuleInfo m_moduleList;
	QHash<CSearchAnalysisItem> m_itemList;
	QMap<CSwordModuleInfo*,unsigned int> m_lastPosList;
	int m_maxCount;
	double m_scaleFactor;
	CSearchAnalysisLegendItem* m_legend;

public slots: // Public slots
	void saveAsHTML();

};

}

#endif
