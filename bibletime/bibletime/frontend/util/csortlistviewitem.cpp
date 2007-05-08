/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "csortlistviewitem.h"

//#include <versekey.h>

namespace util {

CSortListViewItem::CSortListViewItem(QListViewItem* parent) : KListViewItem(parent)
{
}

CSortListViewItem::CSortListViewItem( QListView * parent, QListViewItem * after )
	: KListViewItem ( parent, after )
{
}

CSortListViewItem::CSortListViewItem( QListViewItem * parent, QListViewItem * after )
	: KListViewItem ( parent, after )
{
}
	
CSortListViewItem::CSortListViewItem( QListView * parent, QString label1, QString label2, QString label3, QString label4, QString label5, QString label6, QString label7, QString label8 )
	: KListViewItem( parent, label1, label2, label3, label4, label5, label6, label7, label8 )
{
}

CSortListViewItem::CSortListViewItem( QListViewItem * parent, QString label1, QString label2, QString label3, QString label4, QString label5, QString label6, QString label7, QString label8 )
	: KListViewItem ( parent, label1, label2, label3, label4, label5, label6, label7, label8 )
{
}

CSortListViewItem::CSortListViewItem( QListView * parent, QListViewItem * after, QString label1, QString label2, QString label3, QString label4, QString label5, QString label6, QString label7, QString label8 )
	: KListViewItem ( parent, after, label1, label2, label3, label4, label5, label6, label7, label8 )
{
}

CSortListViewItem::CSortListViewItem( QListViewItem * parent, QListViewItem * after, QString label1, QString label2, QString label3, QString label4, QString label5, QString label6, QString label7, QString label8 )
	: KListViewItem ( parent, after, label1, label2, label3, label4, label5, label6, label7, label8 )
{

}


CSortListViewItem::~CSortListViewItem() {
}

int CSortListViewItem::compare( QListViewItem* i, int col, bool ascending ) const {
	int ret = 0;

	if ( !columnSorting.contains(col) ) {
		return KListViewItem::compare(i, col, ascending);
	}
	
	Type t = columnSorting[col];
	if (t == String) {
		ret = KListViewItem::compare(i, col, ascending);
	}
	else if (t == Number) {
		ret = (int) (text(col).toLong() - (i->text(col).toLong()));
	}
// 	else if (t == BibleKey) {
// 		//ret = (int) (text(col).toLong() - (i->text(col).toLong()));
// 		ret = sword::VerseKey(  (const char*) text(col).utf8() )._compare( sword::VerseKey( (const char*) i->text(col).utf8()) );
// 	}

	return ret;
}

void CSortListViewItem::setColumnSorting(int column, CSortListViewItem::Type type) {
	columnSorting[ column ] = type;
}

}

