/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef UTILCSORTLISTVIEWITEM_H
#define UTILCSORTLISTVIEWITEM_H


#include <qmap.h>

#include <klistview.h>

namespace util {

/**
 * A implementation of a QListViewItem to allow special sort orders for it's columns.
 * @author The BibleTime team <info@bibletime.info>
*/
class CSortListViewItem : public KListViewItem {
public:
	enum Type {
		Number, String/*, BibleKey*/
	};
	
    CSortListViewItem( QListViewItem* parent );
	
	CSortListViewItem( QListView * parent, QListViewItem * after );
	
	CSortListViewItem( QListViewItem * parent, QListViewItem * after );
	
	CSortListViewItem( QListView * parent, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );

	CSortListViewItem( QListViewItem * parent, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );

	CSortListViewItem( QListView * parent, QListViewItem * after, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );

	CSortListViewItem( QListViewItem * parent, QListViewItem * after, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );

	virtual ~CSortListViewItem();
	
	virtual int compare( QListViewItem* i, int col, bool ascending ) const;

	void setColumnSorting(int column, Type type);

private:
	QMap<int, Type> columnSorting;
};

}

#endif
