/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTICONS_H
#define BTICONS_H

#include <QIcon>
#include <QMap>
#include <QString>


class BtIcons {

private: /* Types: */

    typedef QMap<QString, QIcon> IconCache;

public: /* Methods: */

    BtIcons();

    static inline BtIcons & instance()
    { return (Q_ASSERT(m_instance), *m_instance); }

private: /* Methods: */

    QIcon const & loadIcon(QString const & name);

private: /* Fields: */

    static BtIcons * m_instance;
    IconCache m_iconCache;

public: /* Fields: */

    QIcon const icon_null;

    /* Regular icons: */
    QIcon const icon_add;
    QIcon const icon_back;
    QIcon const icon_bible;
    QIcon const icon_bibletime;
    QIcon const icon_book;
    QIcon const icon_bookmark;
    QIcon const icon_books;
    QIcon const icon_calendar;
    QIcon const icon_cascade;
    QIcon const icon_cascade_auto;
    QIcon const icon_checkbox;
    QIcon const icon_commentary;
    QIcon const icon_configure;
    QIcon const icon_contents2;
    QIcon const icon_default;
    QIcon const icon_dictionary;
    QIcon const icon_displayconfig;
    QIcon const icon_displaytranslit;
    QIcon const icon_document_magnifier;
    QIcon const icon_edit_clear;
    QIcon const icon_edit_clear_locationbar;
    QIcon const icon_edit_copy;
    QIcon const icon_edit_delete;
    QIcon const icon_edit_undo;
    QIcon const icon_exit;
    QIcon const icon_export;
    QIcon const icon_file_save;
    QIcon const icon_fileclose;
    QIcon const icon_fileprint;
    QIcon const icon_find;
    QIcon const icon_flag;
    QIcon const icon_folder_new;
    QIcon const icon_folder_open;
    QIcon const icon_folder;
    QIcon const icon_fontconfig;
    QIcon const icon_fonts;
    QIcon const icon_forward;
    QIcon const icon_import;
    QIcon const icon_info;
    QIcon const icon_key_bindings;
    QIcon const icon_layer_visible_on;
    QIcon const icon_lexicon;
    QIcon const icon_light_bulb;
    QIcon const icon_lock;
    QIcon const icon_map;
    QIcon const icon_pencil;
    QIcon const icon_plus;
    QIcon const icon_pointing_arrow;
    QIcon const icon_print;
    QIcon const icon_remove;
    QIcon const icon_questionable;
    QIcon const icon_questionmark;
    QIcon const icon_refresh;
    QIcon const icon_startconfig;
    QIcon const icon_stop;
    QIcon const icon_swordconfig;
    QIcon const icon_sync;
    QIcon const icon_tabbed;
    QIcon const icon_text_bold;
    QIcon const icon_text_center;
    QIcon const icon_text_italic;
    QIcon const icon_text_leftalign;
    QIcon const icon_text_rightalign;
    QIcon const icon_text_under;
    QIcon const icon_tile;
    QIcon const icon_tile_auto;
    QIcon const icon_tile_horiz;
    QIcon const icon_tile_vert;
    QIcon const icon_trash;
    QIcon const icon_unlock;
    QIcon const icon_view_tree;
    QIcon const icon_view_index;
    QIcon const icon_view_mag;
    QIcon const icon_view_profile;
    QIcon const icon_window_fullscreen;

    /* Overlayed icons: */
    QIcon const icon_bible_add;
    QIcon const icon_bible_locked;
    QIcon const icon_bible_remove;
    QIcon const icon_book_add;
    QIcon const icon_book_locked;
    QIcon const icon_commentary_add;
    QIcon const icon_commentary_locked;
    QIcon const icon_lexicon_add;
    QIcon const icon_lexicon_locked;

};

QString iconToHtml(QIcon const & icon, int const extent = 32);

#endif /* BTICONS_H */
