/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "bticons.h"

#include <QByteArray>
#include <QBuffer>
#include <QPixmap>
#include <QDebug>
#include <QIconEngine>
#include <QPainter>
#include "directory.h"


namespace {

class BtOverlayIconEngine: public QIconEngine {

public: /* Methods: */

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

private: /* Fields: */

    QIcon const & m_icon;
    QIcon const & m_overlayIcon;

};

} // anonymous namespace

BtIcons::RegularIcon::RegularIcon(char const * const name)
    : QIcon(util::directory::getIconDir().canonicalPath() + "/" + name + ".svg")
{}

BtIcons::OverlayedIcon::OverlayedIcon(QIcon const & icon,
                                      QIcon const & overlayedIcon)
    : QIcon(new BtOverlayIconEngine(icon, overlayedIcon))
{}

BtIcons * BtIcons::m_instance = nullptr;

BtIcons::BtIcons()
    /* Regular icons: */
    : icon_add("add")
    , icon_automatically("automatically")
    , icon_back("back")
    , icon_bible("bible")
    , icon_bibletime("bibletime")
    , icon_book("book")
    , icon_bookmark("bookmark")
    , icon_books("books")
    , icon_calendar("calendar")
    , icon_cascade("cascade")
    , icon_checkbox("checkbox")
    , icon_commentary("commentary")
    , icon_configure("configure")
    , icon_contents2("contents2")
    , icon_delete("delete")
    , icon_dictionary("dictionary")
    , icon_displayconfig("displayconfig")
    , icon_document_magnifier("document_magnifier")
    , icon_edit_clear_locationbar("edit_clear_locationbar")
    , icon_edit_copy("edit_copy")
    , icon_exit("exit")
    , icon_export("export")
    , icon_file_save("file_save")
    , icon_fileclose("fileclose")
    , icon_find("find")
    , icon_flag("flag")
    , icon_folder_open("folder-open")
    , icon_folder("folder")
    , icon_fonts("fonts")
    , icon_forward("forward")
    , icon_import("import")
    , icon_info("info")
    , icon_key_bindings("key_bindings")
    , icon_layer_visible_on("layer-visible-on")
    , icon_lexicon("lexicon")
    , icon_light_bulb("light_bulb")
    , icon_lock("lock")
    , icon_manual("manual")
    , icon_map("map")
    , icon_pencil("pencil")
    , icon_plus("plus")
    , icon_pointing_arrow("pointing_arrow")
    , icon_print("print")
    , icon_remove("remove")
    , icon_questionable("questionable")
    , icon_questionmark("questionmark")
    , icon_refresh("refresh")
    , icon_startconfig("startconfig")
    , icon_stop("stop")
    , icon_swordconfig("swordconfig")
    , icon_sync("sync")
    , icon_tabbed("tabbed")
    , icon_text_bold("text_bold")
    , icon_text_center("text_center")
    , icon_text_italic("text_italic")
    , icon_text_leftalign("text_leftalign")
    , icon_text_rightalign("text_rightalign")
    , icon_text_under("text_under")
    , icon_tile("tile")
    , icon_tile_horiz("tile_horiz")
    , icon_tile_vert("tile_vert")
    , icon_trash("trash")
    , icon_unlock("unlock")
    , icon_view_tree("view-tree")
    , icon_view_index("view_index")
    , icon_view_mag("view_mag")
    , icon_view_profile("view_profile")
    , icon_window_fullscreen("window_fullscreen")
    /* Overlayed icons: */
    , icon_bible_add(icon_bible, icon_add)
    , icon_bible_install(icon_bible, icon_plus)
    , icon_bible_locked(icon_bible, icon_lock)
    , icon_bible_remove(icon_bible, icon_remove)
    , icon_bible_uninstall(icon_bible, icon_delete)
    , icon_book_add(icon_book, icon_add)
    , icon_book_locked(icon_book, icon_lock)
    , icon_cascade_auto(icon_cascade, icon_automatically)
    , icon_commentary_add(icon_commentary, icon_add)
    , icon_commentary_locked(icon_commentary, icon_lock)
    , icon_folder_add(icon_folder, icon_add)
    , icon_lexicon_add(icon_lexicon, icon_add)
    , icon_lexicon_locked(icon_lexicon, icon_lock)
    , icon_tile_auto(icon_tile, icon_automatically)
    , icon_tile_horiz_auto(icon_tile_horiz, icon_automatically)
    , icon_tile_vert_auto(icon_tile_vert, icon_automatically)
{
    BT_ASSERT(!m_instance);
    m_instance = this;
}

QString iconToHtml(QIcon const & icon, int const extent) {
    QByteArray bytes;
    QBuffer buffer(&bytes);
    icon.pixmap(extent).save(&buffer, "PNG");
    return "<img src=\"data:image/png;base64," + bytes.toBase64() + "\" />";
}
