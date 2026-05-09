/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2026 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "bticons.h"

#include <QBrush>
#include <QBuffer>
#include <QByteArray>
#include <QDir>
#include <QIconEngine>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QPoint>
#include <QRect>
#include <QString>
#include <Qt>
#include <QtGui>
#include "directory.h"


class QSize;

namespace {

class BtRegularIcon: public QIcon {

public: // methods:

    BtRegularIcon(QString const & name)
        : QIcon(util::directory::getIconDir().filePath(name))
    {}

};

class BtOverlayIconEngine: public QIconEngine {

public: // methods:

    BtOverlayIconEngine(QIcon const & icon, QIcon const & overlay)
        : m_icon(icon)
        , m_overlayIcon(overlay)
    {}

    QIconEngine * clone() const override
    { return new BtOverlayIconEngine(m_icon, m_overlayIcon); }

    void paint(QPainter * painter,
               QRect const & rect,
               QIcon::Mode mode,
               QIcon::State state) override
    {
        {
            QBrush brush(painter->background());
            brush.setColor(Qt::transparent);
            painter->setBackground(brush);
        }
        painter->eraseRect(rect);
        painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
        m_icon.paint(painter, rect, Qt::AlignCenter, mode, state);
        m_overlayIcon.paint(painter,
                            rect.adjusted(0.4 * rect.width(),
                                          0.4 * rect.height(),
                                          0,
                                          0),
                            Qt::AlignCenter,
                            mode,
                            state);
    }

    QPixmap pixmap(QSize const & size,
                   QIcon::Mode mode,
                   QIcon::State state) override
    {
        QImage img(size, QImage::Format_ARGB32);
        img.fill(qRgba(0,0,0,0));
        QPixmap pix = QPixmap::fromImage(img, Qt::NoFormatConversion);
        QPainter painter(&pix);
        this->paint(&painter, QRect(QPoint(0, 0), size), mode, state);
        return pix;
    }

private: // fields:

    QIcon const & m_icon;
    QIcon const & m_overlayIcon;

};

class BtOverlayedIcon: public QIcon {

public: // methods:

    BtOverlayedIcon(QIcon const & icon, QIcon const & overlayedIcon)
        : QIcon(new BtOverlayIconEngine(icon, overlayedIcon))
    {}

};

} // anonymous namespace

namespace BtIcons {

#define BT_ICON_(type,name,...) \
    QIcon const & name() { \
        static type const s_icon __VA_ARGS__; \
        return s_icon; \
    }
#define BT_ICON(type,name,...) BT_ICON_(type,name,(__VA_ARGS__))
#define BT_REGULAR_ICON(name,filename) \
    BT_ICON(BtRegularIcon, name, QStringLiteral(filename))
#define BT_OVERLAYED_ICON(name, icon, icon2) \
    BT_ICON(BtOverlayedIcon, name, icon(), icon2())

BT_ICON_(QIcon, icon_null)

BT_REGULAR_ICON(icon_add, "add.svg")
BT_REGULAR_ICON(icon_automatically, "automatically.svg")
BT_REGULAR_ICON(icon_back, "back.svg")
BT_REGULAR_ICON(icon_bible, "bible.svg")
BT_REGULAR_ICON(icon_bibletime, "bibletime.svg")
BT_REGULAR_ICON(icon_book, "book.svg")
BT_REGULAR_ICON(icon_bookmark, "bookmark.svg")
BT_REGULAR_ICON(icon_books, "books.svg")
BT_REGULAR_ICON(icon_calendar, "calendar.svg")
BT_REGULAR_ICON(icon_cascade, "cascade.svg")
BT_REGULAR_ICON(icon_checkbox, "checkbox.svg")
BT_REGULAR_ICON(icon_commentary, "commentary.svg")
BT_REGULAR_ICON(icon_configure, "configure.svg")
BT_REGULAR_ICON(icon_contents2, "contents2.svg")
BT_REGULAR_ICON(icon_delete, "delete.svg")
BT_REGULAR_ICON(icon_dictionary, "dictionary.svg")
BT_REGULAR_ICON(icon_displayconfig, "displayconfig.svg")
BT_REGULAR_ICON(icon_document_magnifier, "document_magnifier.svg")
BT_REGULAR_ICON(icon_edit_clear_locationbar, "edit_clear_locationbar.svg")
BT_REGULAR_ICON(icon_edit_copy, "edit_copy.svg")
BT_REGULAR_ICON(icon_exit, "exit.svg")
BT_REGULAR_ICON(icon_export, "export.svg")
BT_REGULAR_ICON(icon_file_save, "file_save.svg")
BT_REGULAR_ICON(icon_fileclose, "fileclose.svg")
BT_REGULAR_ICON(icon_find, "find.svg")
BT_REGULAR_ICON(icon_flag, "flag.svg")
BT_REGULAR_ICON(icon_folder_open, "folder-open.svg")
BT_REGULAR_ICON(icon_folder, "folder.svg")
BT_REGULAR_ICON(icon_fonts, "fonts.svg")
BT_REGULAR_ICON(icon_forward, "forward.svg")
BT_REGULAR_ICON(icon_import, "import.svg")
BT_REGULAR_ICON(icon_info, "info.svg")
BT_REGULAR_ICON(icon_key_bindings, "key_bindings.svg")
BT_REGULAR_ICON(icon_layer_visible_on, "layer-visible-on.svg")
BT_REGULAR_ICON(icon_lexicon, "lexicon.svg")
BT_REGULAR_ICON(icon_light_bulb, "light_bulb.svg")
BT_REGULAR_ICON(icon_lock, "lock.svg")
BT_REGULAR_ICON(icon_manual, "manual.svg")
BT_REGULAR_ICON(icon_map, "map.svg")
BT_REGULAR_ICON(icon_plus, "plus.svg")
BT_REGULAR_ICON(icon_pointing_arrow, "pointing_arrow.svg")
BT_REGULAR_ICON(icon_print, "print.svg")
BT_REGULAR_ICON(icon_remove, "remove.svg")
BT_REGULAR_ICON(icon_questionable, "questionable.svg")
BT_REGULAR_ICON(icon_speaker, "speaker.svg")
BT_REGULAR_ICON(icon_startconfig, "startconfig.svg")
BT_REGULAR_ICON(icon_stop, "stop.svg")
BT_REGULAR_ICON(icon_swordconfig, "swordconfig.svg")
BT_REGULAR_ICON(icon_sync, "sync.svg")
BT_REGULAR_ICON(icon_tabbed, "tabbed.svg")
BT_REGULAR_ICON(icon_text_bold, "text_bold.svg")
BT_REGULAR_ICON(icon_text_center, "text_center.svg")
BT_REGULAR_ICON(icon_text_italic, "text_italic.svg")
BT_REGULAR_ICON(icon_text_leftalign, "text_leftalign.svg")
BT_REGULAR_ICON(icon_text_rightalign, "text_rightalign.svg")
BT_REGULAR_ICON(icon_text_under, "text_under.svg")
BT_REGULAR_ICON(icon_tile, "tile.svg")
BT_REGULAR_ICON(icon_tile_horiz, "tile_horiz.svg")
BT_REGULAR_ICON(icon_tile_vert, "tile_vert.svg")
BT_REGULAR_ICON(icon_trash, "trash.svg")
BT_REGULAR_ICON(icon_unlock, "unlock.svg")
BT_REGULAR_ICON(icon_view_tree, "view-tree.svg")
BT_REGULAR_ICON(icon_view_index, "view_index.svg")
BT_REGULAR_ICON(icon_view_mag, "view_mag.svg")
BT_REGULAR_ICON(icon_view_profile, "view_profile.svg")
BT_REGULAR_ICON(icon_window_fullscreen, "window_fullscreen.svg")

BT_OVERLAYED_ICON(icon_bible_add, icon_bible, icon_add)
BT_OVERLAYED_ICON(icon_bible_locked, icon_bible, icon_lock)
BT_OVERLAYED_ICON(icon_bible_remove, icon_bible, icon_remove)
BT_OVERLAYED_ICON(icon_book_add, icon_book, icon_add)
BT_OVERLAYED_ICON(icon_book_locked, icon_book, icon_lock)
BT_OVERLAYED_ICON(icon_calendar_add, icon_calendar, icon_add)
BT_OVERLAYED_ICON(icon_calendar_locked, icon_calendar, icon_lock)
BT_OVERLAYED_ICON(icon_cascade_auto, icon_cascade, icon_automatically)
BT_OVERLAYED_ICON(icon_commentary_add, icon_commentary, icon_add)
BT_OVERLAYED_ICON(icon_commentary_locked, icon_commentary, icon_lock)
BT_OVERLAYED_ICON(icon_dictionary_add, icon_dictionary, icon_add)
BT_OVERLAYED_ICON(icon_dictionary_locked, icon_dictionary, icon_lock)
BT_OVERLAYED_ICON(icon_folder_add, icon_folder, icon_add)
BT_OVERLAYED_ICON(icon_lexicon_add, icon_lexicon, icon_add)
BT_OVERLAYED_ICON(icon_lexicon_locked, icon_lexicon, icon_lock)
BT_OVERLAYED_ICON(icon_map_add, icon_map, icon_add)
BT_OVERLAYED_ICON(icon_map_locked, icon_map, icon_lock)
BT_OVERLAYED_ICON(icon_questionable_add, icon_questionable, icon_add)
BT_OVERLAYED_ICON(icon_questionable_locked, icon_questionable, icon_lock)
BT_OVERLAYED_ICON(icon_tile_auto, icon_tile, icon_automatically)
BT_OVERLAYED_ICON(icon_tile_horiz_auto, icon_tile_horiz, icon_automatically)
BT_OVERLAYED_ICON(icon_tile_vert_auto, icon_tile_vert, icon_automatically)

#undef BT_OVERLAYED_ICON
#undef BT_REGULAR_ICON
#undef BT_ICON
#undef BT_ICON_

} // namespace BtIcons

QString iconToHtml(QIcon const & icon, int const extent) {
    QByteArray bytes;
    QBuffer buffer(&bytes);
    icon.pixmap(extent).save(&buffer, "PNG");
    return "<img src=\"data:image/png;base64," + bytes.toBase64() + "\" />";
}
