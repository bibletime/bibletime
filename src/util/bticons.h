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

#include <QIcon>
#include <QString>
#include "btassert.h"


class BtIcons {

public: // types:

    class RegularIcon: public QIcon {

    public: // methods:

        RegularIcon(QString const & name);

    };

    class OverlayedIcon: public QIcon {

    public: // methods:

        OverlayedIcon(QIcon const & icon, QIcon const & overlayedIcon);

    };

public: // methods:

    BtIcons();

    static BtIcons & instance() {
        BT_ASSERT(m_instance);
        return *m_instance;
    }

private: // fields:

    static BtIcons * m_instance;

public: // fields:

    QIcon const icon_null;

    /* Regular icons: */
    RegularIcon const icon_add;
    RegularIcon const icon_audio;
    RegularIcon const icon_automatically;
    RegularIcon const icon_back;
    RegularIcon const icon_bible;
    RegularIcon const icon_bibletime;
    RegularIcon const icon_book;
    RegularIcon const icon_bookmark;
    RegularIcon const icon_books;
    RegularIcon const icon_calendar;
    RegularIcon const icon_cascade;
    RegularIcon const icon_checkbox;
    RegularIcon const icon_commentary;
    RegularIcon const icon_configure;
    RegularIcon const icon_contents2;
    RegularIcon const icon_delete;
    RegularIcon const icon_dictionary;
    RegularIcon const icon_displayconfig;
    RegularIcon const icon_document_magnifier;
    RegularIcon const icon_edit_clear_locationbar;
    RegularIcon const icon_edit_copy;
    RegularIcon const icon_exit;
    RegularIcon const icon_export;
    RegularIcon const icon_file_save;
    RegularIcon const icon_fileclose;
    RegularIcon const icon_find;
    RegularIcon const icon_flag;
    RegularIcon const icon_folder_open;
    RegularIcon const icon_folder;
    RegularIcon const icon_fonts;
    RegularIcon const icon_forward;
    RegularIcon const icon_import;
    RegularIcon const icon_info;
    RegularIcon const icon_key_bindings;
    RegularIcon const icon_layer_visible_on;
    RegularIcon const icon_lexicon;
    RegularIcon const icon_light_bulb;
    RegularIcon const icon_lock;
    RegularIcon const icon_manual;
    RegularIcon const icon_map;
    RegularIcon const icon_pencil;
    RegularIcon const icon_plus;
    RegularIcon const icon_pointing_arrow;
    RegularIcon const icon_print;
    RegularIcon const icon_remove;
    RegularIcon const icon_questionable;
    RegularIcon const icon_questionmark;
    RegularIcon const icon_refresh;
    RegularIcon const icon_startconfig;
    RegularIcon const icon_stop;
    RegularIcon const icon_swordconfig;
    RegularIcon const icon_sync;
    RegularIcon const icon_tabbed;
    RegularIcon const icon_text_bold;
    RegularIcon const icon_text_center;
    RegularIcon const icon_text_italic;
    RegularIcon const icon_text_leftalign;
    RegularIcon const icon_text_rightalign;
    RegularIcon const icon_text_under;
    RegularIcon const icon_tile;
    RegularIcon const icon_tile_horiz;
    RegularIcon const icon_tile_vert;
    RegularIcon const icon_trash;
    RegularIcon const icon_unlock;
    RegularIcon const icon_view_tree;
    RegularIcon const icon_view_index;
    RegularIcon const icon_view_mag;
    RegularIcon const icon_view_profile;
    RegularIcon const icon_window_fullscreen;

    /* Overlayed icons: */
    OverlayedIcon const icon_bible_add;
    OverlayedIcon const icon_bible_install;
    OverlayedIcon const icon_bible_locked;
    OverlayedIcon const icon_bible_remove;
    OverlayedIcon const icon_bible_uninstall;
    OverlayedIcon const icon_book_add;
    OverlayedIcon const icon_book_locked;
    OverlayedIcon const icon_cascade_auto;
    OverlayedIcon const icon_commentary_add;
    OverlayedIcon const icon_commentary_locked;
    OverlayedIcon const icon_folder_add;
    OverlayedIcon const icon_lexicon_add;
    OverlayedIcon const icon_lexicon_locked;
    OverlayedIcon const icon_tile_auto;
    OverlayedIcon const icon_tile_horiz_auto;
    OverlayedIcon const icon_tile_vert_auto;

};

QString iconToHtml(QIcon const & icon, int const extent = 32);
