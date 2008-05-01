/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

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
class CSearchAnalysisScene : public QGraphicsScene {
	Q_OBJECT
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
	QHash<QString, CSearchAnalysisItem*>* getSearchAnalysisItemList();
	void reset();

public slots:
	void saveAsHTML();

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
	QHash<QString, CSearchAnalysisItem*> m_itemList;
	QMap<CSwordModuleInfo*,unsigned int> m_lastPosList;
	int m_maxCount;
	double m_scaleFactor;
	CSearchAnalysisLegendItem* m_legend;


};

}

#endif
