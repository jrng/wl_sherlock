#include <cui.h>

static const uint32_t WIDGET_TYPE_LIST_VIEW  = CUI_WIDGET_TYPE_CUSTOM + 0;
static const uint32_t WIDGET_TYPE_GRAPH_VIEW = CUI_WIDGET_TYPE_CUSTOM + 1;

static const CuiColorTheme color_theme = {
    /* window_titlebar_background           */ CuiHexColorLiteral(0xFF242A3B),
    /* window_titlebar_border               */ CuiHexColorLiteral(0xAF000000),
    /* window_titlebar_text                 */ CuiHexColorLiteral(0xFFFFFFFF),
    /* window_titlebar_icon                 */ CuiHexColorLiteral(0xFFD7DAE0),
    /* window_drop_shadow                   */ CuiHexColorLiteral(0x6F000000),
    /* window_outline                       */ CuiHexColorLiteral(0xAF050607),
    /* default_bg                           */ CuiHexColorLiteral(0xFF282C34),
    /* default_fg                           */ CuiHexColorLiteral(0xFFD7DAE0),
    /* default_border                       */ CuiHexColorLiteral(0xFF151721),
    /* default_button_normal_background     */ CuiHexColorLiteral(0xFF1E2332),
    /* default_button_normal_box_shadow     */ CuiHexColorLiteral(0x3F000000),
    /* default_button_normal_border         */ CuiHexColorLiteral(0xFF151721),
    /* default_button_normal_text           */ CuiHexColorLiteral(0xFF8C96A9),
    /* default_button_normal_icon           */ CuiHexColorLiteral(0xFFB7BAC0),
    /* default_textinput_normal_background  */ CuiHexColorLiteral(0xFF1a1e2d),
    /* default_textinput_normal_box_shadow  */ CuiHexColorLiteral(0x3F000000),
    /* default_textinput_normal_border      */ CuiHexColorLiteral(0xFF151721),
    /* default_textinput_normal_text        */ CuiHexColorLiteral(0xFFafb7c4),
    /* default_textinput_normal_placeholder */ CuiHexColorLiteral(0xFF333846),
    /* default_textinput_normal_icon        */ CuiHexColorLiteral(0xFFB7BAC0),
};

static const CuiColor blue_background   = CuiHexColorLiteral(0xFF284263);
static const CuiColor blue_foreground   = CuiHexColorLiteral(0xFF2D89B9);
// static const CuiColor green_background  = CuiHexColorLiteral(0xFF2C5948);
static const CuiColor green_foreground  = CuiHexColorLiteral(0xFF33B97B);
// static const CuiColor red_background    = CuiHexColorLiteral(0xFF4E2630);
static const CuiColor red_foreground    = CuiHexColorLiteral(0xFFC22630);
// static const CuiColor yellow_background = CuiHexColorLiteral(0xFF46431F);
static const CuiColor yellow_foreground = CuiHexColorLiteral(0xFFD5D84D);

typedef struct
{
    int32_t integer_part;
    int32_t fractional_part;
} ScrollOffset;

typedef struct
{
    CuiString str;

    CuiString connection_name;
    CuiString queue_name;
    CuiString interface_name;
    CuiString message_name;
    uint32_t id;

    uint64_t timestamp_us;
} Message;

typedef struct
{
    CuiWidget base;

    int32_t px1;
    int32_t px2;
    int32_t px4;
    int32_t px6;
    int32_t px8;
    int32_t px12;
    int32_t px16;

    ScrollOffset scroll_offset;

    CuiRect list_rect;
    CuiRect header_rect;
    CuiRect scroll_bar_rect;
    CuiRect scroll_handle_rect;

    bool scrolling;
    int32_t mouse_offset_y;
} ListView;

typedef struct
{
    CuiWidget base;

    int32_t px1;
    int32_t px2;
    int32_t px4;
    int32_t px16;

    ScrollOffset scroll_offset;

    CuiRect graph_rect;
    CuiRect scroll_bar_rect;
    CuiRect scroll_handle_rect;

    bool scrolling;
    int32_t mouse_offset_x;
} GraphView;

typedef struct
{
    uint32_t message_index;
    uint32_t time_delta;
} FilterItem;

typedef struct
{
    CuiString interface_name;
    CuiString message_name;
    uint32_t id;
} Filter;

typedef struct
{
    CuiArena temporary_memory;
    CuiArena widget_arena;
    CuiArena title_arena;

    bool file_loaded;
    CuiString file_content;

    uint32_t message_allocated;
    uint32_t message_count;
    Message *messages;

    uint32_t filter_item_allocated;
    uint32_t filter_item_count;
    FilterItem *filter_items;

    Filter filter;

    CuiString id_character;

    CuiWindow *window;
    CuiWidget *root_widget;

    float font_size;
    float line_height;
    CuiFontId list_view_font;

    CuiWidget open_file_button;
    CuiWidget filter_input;
    CuiWidget filter_checkbox;
    CuiWidget bottom_container;
    CuiWidget info_panel;
    ListView list_view;
    GraphView graph_view;
} Application;

static Application app;

static inline bool
filter_is_empty(void)
{
    return (app.filter.id == 0) && (app.filter.interface_name.count == 0) && (app.filter.message_name.count == 0);
}

static inline void
set_default_font_size(void)
{
    app.font_size   = 14.0f;
    app.line_height = 1.0f;
}

static inline void
reset_font_size(void)
{
    set_default_font_size();

    cui_window_update_font(app.window, app.list_view_font, app.font_size, app.line_height);
    cui_widget_layout(app.root_widget, app.root_widget->rect);
    cui_window_request_redraw(app.window);
}

static inline void
increase_font_size(void)
{
    app.font_size = app.font_size + 1.0f;

    if (app.font_size < 10.0f)
    {
        app.font_size = 10.0f;
    }

    if (app.font_size > 20.0f)
    {
        app.font_size = 20.0f;
    }

    cui_window_update_font(app.window, app.list_view_font, app.font_size, app.line_height);
    cui_widget_layout(app.root_widget, app.root_widget->rect);
    cui_window_request_redraw(app.window);
}

static inline void
decrease_font_size(void)
{
    app.font_size = app.font_size - 1.0f;

    if (app.font_size < 10.0f)
    {
        app.font_size = 10.0f;
    }

    if (app.font_size > 20.0f)
    {
        app.font_size = 20.0f;
    }

    cui_window_update_font(app.window, app.list_view_font, app.font_size, app.line_height);
    cui_widget_layout(app.root_widget, app.root_widget->rect);
    cui_window_request_redraw(app.window);
}

static inline ScrollOffset
normalize_scroll_offset(ScrollOffset offset, int32_t absolute_unit)
{
    int32_t count;

    if (offset.fractional_part < 0)
    {
        count = (offset.fractional_part - (absolute_unit - 1)) / absolute_unit;
    }
    else
    {
        count = offset.fractional_part / absolute_unit;
    }

    offset.integer_part += count;
    offset.fractional_part -= count * absolute_unit;

    CuiAssert((offset.fractional_part >= 0) && (offset.fractional_part < absolute_unit));

    return offset;
}

static CuiPoint
update_scroll_handle(ScrollOffset scroll_offset, int32_t scroll_handle_min, int32_t scroll_handle_max,
                     int32_t scroll_handle_min_size, int64_t view_size, int64_t content_size, int64_t absolute_unit)
{
    CuiPoint result = { 0, 0 }; // those are min and max of the scroll handle

    if (content_size > view_size)
    {
        int64_t scroll_bar_size = scroll_handle_max - scroll_handle_min;

        int32_t scroll_handle_size = cui_max_int32(scroll_handle_min_size, cui_min_int32((int32_t) ((view_size * scroll_bar_size) / content_size), (int32_t) view_size));

        int64_t offset = (int64_t) scroll_offset.integer_part * absolute_unit + (int64_t) scroll_offset.fractional_part;
        int32_t scroll_handle_offset = (int32_t) (scroll_bar_size - (int64_t) scroll_handle_size) * offset / (content_size - view_size);

        result.x = scroll_handle_min + scroll_handle_offset;
        result.y = result.x + scroll_handle_size;
    }

    return result;
}

static CuiPoint
limit_scroll_offset(ScrollOffset *scroll_offset, int32_t scroll_handle_min, int32_t scroll_handle_max,
                    int32_t scroll_handle_min_size, int32_t view_size, int32_t absolute_unit, int32_t count)
{
    ScrollOffset max_scroll_offset;
    max_scroll_offset.integer_part = count;
    max_scroll_offset.fractional_part = -view_size;

    max_scroll_offset = normalize_scroll_offset(max_scroll_offset, absolute_unit);

    if (max_scroll_offset.integer_part < 0)
    {
        max_scroll_offset.integer_part = 0;
        max_scroll_offset.fractional_part = 0;
    }

    if ((scroll_offset->integer_part > max_scroll_offset.integer_part) ||
        ((scroll_offset->integer_part == max_scroll_offset.integer_part) &&
         (scroll_offset->fractional_part > max_scroll_offset.fractional_part)))
    {
        *scroll_offset = max_scroll_offset;
    }

    if (scroll_offset->integer_part < 0)
    {
        scroll_offset->integer_part = 0;
        scroll_offset->fractional_part = 0;
    }

    int64_t view_height = (int64_t) view_size;
    int64_t content_height = (int64_t) count * (int64_t) absolute_unit;

    return update_scroll_handle(*scroll_offset, scroll_handle_min, scroll_handle_max,
                                scroll_handle_min_size, view_height, content_height, absolute_unit);
}

static void
list_view_limit_scroll_offset(ListView *list_view)
{
    CuiAssert(list_view->base.window);
    CuiWindow *window = list_view->base.window;

    int32_t line_height = cui_window_get_font_line_height(window, app.list_view_font);
    int32_t row_height = line_height + 2 * list_view->px6 + list_view->px1;

    int32_t count = app.message_count;

    if (app.filter_checkbox.value)
    {
        count = app.filter_item_count;
    }

    CuiRect scroll_handle_bound = list_view->scroll_bar_rect;
    scroll_handle_bound.min.x += list_view->px2;
#if CUI_PLATFORM_MACOS
    scroll_handle_bound.max.x -= list_view->px2;
#endif

    CuiPoint scroll_handle = limit_scroll_offset(&list_view->scroll_offset, scroll_handle_bound.min.y, scroll_handle_bound.max.y,
                                                 list_view->px16, cui_rect_get_height(list_view->list_rect), row_height, count);

    scroll_handle_bound.min.y = scroll_handle.x;
    scroll_handle_bound.max.y = scroll_handle.y;

    list_view->scroll_handle_rect = scroll_handle_bound;
}

static void
list_view_set_ui_scale(CuiWidget *widget, float ui_scale)
{
    ListView *list_view = CuiContainerOf(widget, ListView, base);

    list_view->px1  = lroundf(ui_scale *  1.0f);
    list_view->px2  = lroundf(ui_scale *  2.0f);
    list_view->px4  = lroundf(ui_scale *  4.0f);
    list_view->px6  = lroundf(ui_scale *  6.0f);
    list_view->px8  = lroundf(ui_scale *  8.0f);
    list_view->px12 = lroundf(ui_scale * 12.0f);
    list_view->px16 = lroundf(ui_scale * 16.0f);
}

static CuiPoint
list_view_get_preferred_size(CuiWidget *widget)
{
    (void) widget;
    return cui_make_point(0, 0);
}

static void
list_view_layout(CuiWidget *widget, CuiRect rect)
{
    ListView *list_view = CuiContainerOf(widget, ListView, base);

    CuiAssert(widget->window);
    CuiWindow *window = widget->window;

    list_view->list_rect = rect;
    list_view->header_rect = rect;
    list_view->scroll_bar_rect = rect;

    // TODO: use font attached to widget
    int32_t line_height = cui_window_get_font_line_height(window, app.list_view_font);
    int32_t header_height = line_height + 2 * list_view->px4 + list_view->px1;
#if CUI_PLATFORM_MACOS
    int32_t scroll_bar_width = 2 * list_view->px2 + list_view->px12;
#else
    int32_t scroll_bar_width = list_view->px2 + list_view->px12;
#endif

    list_view->list_rect.max.x = cui_max_int32(list_view->list_rect.max.x - scroll_bar_width, list_view->list_rect.min.x);
    list_view->list_rect.min.y = cui_min_int32(list_view->list_rect.min.y + header_height, list_view->list_rect.max.y);
    list_view->header_rect.max.y = list_view->list_rect.min.y;
    list_view->scroll_bar_rect.min.x = list_view->list_rect.max.x;
    list_view->scroll_bar_rect.min.y = list_view->list_rect.min.y;

    list_view_limit_scroll_offset(list_view);
}

static void
list_view_draw(CuiWidget *widget, CuiGraphicsContext *ctx, const CuiColorTheme *color_theme)
{
    (void) color_theme;

    ListView *list_view = CuiContainerOf(widget, ListView, base);

    CuiColor background_color = CuiHexColorLiteral(0xFF191C28);

    CuiRect list_rect = list_view->list_rect;
    CuiRect header_rect = list_view->header_rect;

    int32_t timestamp_content_width = (int32_t) ceilf(cui_window_get_string_width(widget->window, app.list_view_font, CuiStringLiteral("9999999.999")));
    int32_t interface_content_width = (int32_t) ceilf(cui_window_get_string_width(widget->window, app.list_view_font, CuiStringLiteral("MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM")));
    int32_t id_content_width = (int32_t) ceilf(cui_window_get_string_width(widget->window, app.list_view_font, CuiStringLiteral("9999999999")));

    int32_t timestamp_column_width = timestamp_content_width + 2 * list_view->px8;
    int32_t interface_column_width = interface_content_width + 2 * list_view->px8;
    int32_t id_column_width = id_content_width + 2 * list_view->px8;
    int32_t message_column_width = cui_rect_get_width(header_rect) - (timestamp_column_width + interface_column_width + id_column_width + 3 * list_view->px1);

    int32_t line_height = cui_window_get_font_line_height(widget->window, app.list_view_font);
    float baseline_offset = cui_window_get_font_baseline_offset(widget->window, app.list_view_font);

    float header_baseline = (float) list_view->px4 + baseline_offset;

    float row_baseline = (float) list_view->px6 + baseline_offset;

    cui_draw_fill_rect(ctx, cui_make_rect(header_rect.min.x, header_rect.min.y, header_rect.max.x, header_rect.max.y - list_view->px1), CuiHexColor(0xFF1E2332));
    cui_draw_fill_rect(ctx, cui_make_rect(header_rect.min.x, header_rect.max.y - list_view->px1, header_rect.max.x, header_rect.max.y), color_theme->default_border);

    int32_t x = header_rect.min.x;
    int32_t y = header_rect.min.y;

    float w;

    w = cui_window_get_string_width(widget->window, app.list_view_font, CuiStringLiteral("Timestamp"));
    cui_draw_fill_string(ctx, app.list_view_font, (float) x + 0.5f * ((float) timestamp_column_width - w), (float) y + header_baseline, CuiStringLiteral("Timestamp"), CuiHexColor(0xFFafb7c4));
    x += timestamp_column_width;

    cui_draw_fill_rect(ctx, cui_make_rect(x, header_rect.min.y, x + list_view->px1, header_rect.max.y), color_theme->default_border);
    x += list_view->px1;

    w = cui_window_get_string_width(widget->window, app.list_view_font, CuiStringLiteral("Interface"));
    cui_draw_fill_string(ctx, app.list_view_font, (float) x + 0.5f * ((float) interface_column_width - w), (float) y + header_baseline, CuiStringLiteral("Interface"), CuiHexColor(0xFFafb7c4));
    x += interface_column_width;

    cui_draw_fill_rect(ctx, cui_make_rect(x, header_rect.min.y, x + list_view->px1, header_rect.max.y), color_theme->default_border);
    x += list_view->px1;

    w = cui_window_get_string_width(widget->window, app.list_view_font, CuiStringLiteral("Id"));
    cui_draw_fill_string(ctx, app.list_view_font, (float) x + 0.5f * ((float) id_column_width - w), (float) y + header_baseline, CuiStringLiteral("Id"), CuiHexColor(0xFFafb7c4));
    x += id_column_width;

    cui_draw_fill_rect(ctx, cui_make_rect(x, header_rect.min.y, x + list_view->px1, header_rect.max.y), color_theme->default_border);
    x += list_view->px1;

    w = cui_window_get_string_width(widget->window, app.list_view_font, CuiStringLiteral("Message"));
    cui_draw_fill_string(ctx, app.list_view_font, (float) x + 0.5f * ((float) message_column_width - w), (float) y + header_baseline, CuiStringLiteral("Message"), CuiHexColor(0xFFafb7c4));
    x += message_column_width;

    cui_draw_fill_rect(ctx, list_rect, background_color);
    cui_draw_fill_rect(ctx, list_view->scroll_bar_rect, color_theme->default_border);
    cui_draw_fill_rect(ctx, cui_make_rect(list_view->scroll_bar_rect.min.x + list_view->px2,
                                          list_view->scroll_bar_rect.min.y,
#if CUI_PLATFORM_MACOS
                                          list_view->scroll_bar_rect.max.x - list_view->px2,
#else
                                          list_view->scroll_bar_rect.max.x,
#endif
                                          list_view->scroll_bar_rect.max.y), background_color);
    cui_draw_fill_rounded_rect_1(ctx, list_view->scroll_handle_rect, (float) list_view->px6, CuiHexColor(0x3FAFB7C4));

    CuiRect prev_clip = cui_draw_set_clip_rect(ctx, list_rect);

    int32_t row_height = line_height + 2 * list_view->px6 + list_view->px1;

    x = list_rect.min.x;
    y = list_rect.min.y - list_view->scroll_offset.fractional_part;

    bool filter = app.filter_checkbox.value || filter_is_empty();

    uint32_t count = app.message_count;
    uint32_t index = list_view->scroll_offset.integer_part;
    uint32_t filter_index = 0;

    if (filter)
    {
        count = app.filter_item_count;
    }
    else
    {
        // TODO: binary search
        while ((filter_index < app.filter_item_count) &&
               (app.filter_items[filter_index].message_index < index))
        {
            filter_index += 1;
        }
    }

    while (index < count)
    {
        CuiTemporaryMemory temp_memory = cui_begin_temporary_memory(&app.temporary_memory);

        x = list_rect.min.x;

        CuiColor text_color = CuiHexColor(0xFFAFB7C4);
        CuiColor character_color = CuiHexColor(0xFF333846);

        Message *message;

        if (filter)
        {
            FilterItem *filter_item = app.filter_items + index;
            CuiAssert(filter_item->message_index < app.message_count);
            message = app.messages + filter_item->message_index;
        }
        else
        {
            message = app.messages + index;

            if ((filter_index < app.filter_item_count) &&
                (app.filter_items[filter_index].message_index == index))
            {
                cui_draw_fill_rect(ctx, cui_make_rect(list_rect.min.x, y, list_rect.max.x, y + row_height), blue_background);
                text_color = CuiHexColor(0xFFFFFFFF);
                character_color = blue_foreground;
                filter_index += 1;
            }
        }

        CuiString timestamp_str = cui_sprint(&app.temporary_memory, CuiStringLiteral("%u.%03u"), (uint32_t) (message->timestamp_us / 1000), (uint32_t) (message->timestamp_us % 1000));

        w = cui_window_get_string_width(widget->window, app.list_view_font, timestamp_str);
        cui_draw_fill_string(ctx, app.list_view_font, (float) (x + list_view->px8) + ((float) timestamp_content_width - w), (float) y + row_baseline, timestamp_str, text_color);
        x += timestamp_column_width;

        x += list_view->px1;

        w = cui_window_get_string_width(widget->window, app.list_view_font, message->interface_name);
        cui_draw_fill_string(ctx, app.list_view_font, (float) (x + list_view->px8) + ((float) interface_content_width - w), (float) y + row_baseline, message->interface_name, text_color);
        x += interface_column_width;

        x += list_view->px1;

        w = cui_window_get_string_width(widget->window, app.list_view_font, app.id_character);
        cui_draw_fill_string(ctx, app.list_view_font, (float) x - 0.5f * w, (float) y + row_baseline, app.id_character, character_color);

        CuiString id_str = cui_sprint(&app.temporary_memory, CuiStringLiteral("%u"), message->id);

        cui_draw_fill_string(ctx, app.list_view_font, (float) (x + list_view->px8), (float) y + row_baseline, id_str, text_color);
        x += id_column_width;

        x += list_view->px1;

        w = cui_window_get_string_width(widget->window, app.list_view_font, CuiStringLiteral("."));
        cui_draw_fill_string(ctx, app.list_view_font, (float) x - 0.5f * w, (float) y + row_baseline, CuiStringLiteral("."), character_color);

        cui_draw_fill_string(ctx, app.list_view_font, (float) (x + list_view->px8), (float) y + row_baseline, message->message_name, text_color);
        float sub_x = (float) (x + list_view->px8);
        sub_x += cui_window_get_string_width(widget->window, app.list_view_font, message->message_name);

        sub_x += (float) list_view->px2;

        cui_draw_fill_string(ctx, app.list_view_font, sub_x, (float) y + row_baseline, message->str, cui_make_color(1.0f, 1.0f, 1.0f, 1.0f));

        y += row_height;

        cui_draw_fill_rect(ctx, cui_make_rect(list_rect.min.x, y - list_view->px1, list_rect.max.x, y), CuiHexColor(0xFF1E2332));

        cui_end_temporary_memory(temp_memory);

        index += 1;

        if (y >= list_rect.max.y)
        {
            break;
        }
    }

    cui_draw_set_clip_rect(ctx, prev_clip);
}

static bool
list_view_handle_event(CuiWidget *widget, CuiEventType event_type)
{
    bool result = false;

    ListView *list_view = CuiContainerOf(widget, ListView, base);

    CuiAssert(widget->window);
    CuiWindow *window = widget->window;

    switch (event_type)
    {
        case CUI_EVENT_TYPE_MOUSE_DRAG:
        {
            if (list_view->scrolling)
            {
                int32_t count;
                bool filter = app.filter_checkbox.value;

                if (filter)
                {
                    count = app.filter_item_count;
                }
                else
                {
                    count = app.message_count;
                }

                int32_t line_height = cui_window_get_font_line_height(window, app.list_view_font);
                int32_t row_height = line_height + 2 * list_view->px6 + list_view->px1;

                int64_t view_height = cui_rect_get_height(list_view->list_rect);
                int64_t content_height = (int64_t) row_height * (int64_t) count;

                if (content_height > view_height)
                {
                    int32_t scroll_handle_height = cui_rect_get_height(list_view->scroll_handle_rect);
                    int64_t scroll_bar_height = cui_rect_get_height(list_view->scroll_bar_rect);

                    CuiPoint mouse = cui_window_get_mouse_position(window);

                    int32_t scroll_handle_offset = mouse.y + list_view->mouse_offset_y - list_view->scroll_bar_rect.min.y;

                    int64_t scroll_offset = ((content_height - view_height) * (int64_t) scroll_handle_offset) / (scroll_bar_height - scroll_handle_height);

                    if (scroll_offset < 0)
                    {
                        scroll_offset = 0;
                    }

                    list_view->scroll_offset.integer_part = (int32_t) (scroll_offset / (int64_t) row_height);
                    list_view->scroll_offset.fractional_part = (int32_t) (scroll_offset - (list_view->scroll_offset.integer_part * row_height));
                }
                else
                {
                    list_view->scroll_offset.integer_part = 0;
                    list_view->scroll_offset.fractional_part = 0;
                }

                list_view_limit_scroll_offset(list_view);

                cui_window_request_redraw(window);
            }
        } break;

        case CUI_EVENT_TYPE_LEFT_DOWN:
        {
            cui_window_set_pressed(window, widget);
            cui_window_set_focused(window, widget);

            CuiPoint mouse = cui_window_get_mouse_position(window);

            if (cui_rect_has_point_inside(list_view->scroll_handle_rect, mouse))
            {
                list_view->scrolling = true;
                list_view->mouse_offset_y = list_view->scroll_handle_rect.min.y - mouse.y;
            }

            result = true;
        } break;

        case CUI_EVENT_TYPE_LEFT_UP:
        {
            list_view->scrolling = false;
        } break;

        case CUI_EVENT_TYPE_DOUBLE_CLICK:
        {
            cui_window_set_pressed(window, widget);
            result = true;
        } break;

        case CUI_EVENT_TYPE_MOUSE_WHEEL:
        {
            float wheel_dy = cui_window_get_wheel_dy(window);

            if (wheel_dy != 0.0f)
            {
                int32_t line_height = cui_window_get_font_line_height(widget->window, app.list_view_font);
                int32_t row_height = line_height + 2 * list_view->px6 + list_view->px1;

                bool is_precise_scrolling = cui_window_is_precise_scrolling(window);

                float delta = wheel_dy;

                if (!is_precise_scrolling)
                {
                    delta *= (float) line_height;
                }

                list_view->scroll_offset.fractional_part = lroundf((float) list_view->scroll_offset.fractional_part - delta);
                list_view->scroll_offset = normalize_scroll_offset(list_view->scroll_offset, row_height);

                list_view_limit_scroll_offset(list_view);

                cui_window_request_redraw(window);
            }

            result = true;
        }

        case CUI_EVENT_TYPE_KEY_DOWN:
        {
            if (cui_window_event_is_ctrl_down(window) || cui_window_event_is_command_down(window))
            {
                uint32_t codepoint = cui_window_event_get_codepoint(window);

                switch (codepoint)
                {
                    case '0':
                    {
                        reset_font_size();
                    } break;

                    case '+':
                    {
                        increase_font_size();
                    } break;

                    case '-':
                    {
                        decrease_font_size();
                    } break;
                }
            }
        } break;

        default:
        {
        } break;
    }

    return result;
}

static void
graph_view_limit_scroll_offset(GraphView *graph_view)
{
    int32_t bar_width = graph_view->px4 + graph_view->px1;

    int32_t count = app.filter_item_count;

    CuiRect scroll_handle_bound = graph_view->scroll_bar_rect;
    scroll_handle_bound.min.x += graph_view->px1;
    scroll_handle_bound.min.y += graph_view->px1;
    scroll_handle_bound.max.x -= graph_view->px1;
    scroll_handle_bound.max.y -= graph_view->px1;

    CuiPoint scroll_handle = limit_scroll_offset(&graph_view->scroll_offset, scroll_handle_bound.min.x, scroll_handle_bound.max.x,
                                                 graph_view->px16, cui_rect_get_width(graph_view->graph_rect), bar_width, count);

    scroll_handle_bound.min.x = scroll_handle.x;
    scroll_handle_bound.max.x = scroll_handle.y;

    graph_view->scroll_handle_rect = scroll_handle_bound;
}

static void
graph_view_set_ui_scale(CuiWidget *widget, float ui_scale)
{
    GraphView *graph_view = CuiContainerOf(widget, GraphView, base);

    graph_view->px1  = lroundf(ui_scale *  1.0f);
    graph_view->px2  = lroundf(ui_scale *  2.0f);
    graph_view->px4  = lroundf(ui_scale *  4.0f);
    graph_view->px16 = lroundf(ui_scale * 16.0f);
}

static CuiPoint
graph_view_get_preferred_size(CuiWidget *widget)
{
    (void) widget;
    return cui_make_point(0, lroundf(widget->ui_scale * 75.0f));
}

static void
graph_view_layout(CuiWidget *widget, CuiRect rect)
{
    GraphView *graph_view = CuiContainerOf(widget, GraphView, base);

    graph_view->graph_rect = rect;
    graph_view->scroll_bar_rect = rect;
    graph_view->scroll_handle_rect = rect;

    int32_t px5 = graph_view->px4 + graph_view->px1;
    int32_t scroll_bar_height = 2 * px5;

    graph_view->scroll_bar_rect.min.y = graph_view->scroll_bar_rect.max.y - scroll_bar_height;
    graph_view->scroll_handle_rect.min.x += graph_view->px1;
    graph_view->scroll_handle_rect.min.y = graph_view->scroll_bar_rect.min.y + graph_view->px1;
    graph_view->scroll_handle_rect.max.x -= graph_view->px1;
    graph_view->scroll_handle_rect.max.y -= graph_view->px1;

    graph_view->graph_rect.min.x = cui_min_int32(graph_view->graph_rect.min.x + px5, graph_view->graph_rect.max.x);
    graph_view->graph_rect.max.x = cui_max_int32(graph_view->graph_rect.max.x - px5, graph_view->graph_rect.min.x);
    graph_view->graph_rect.max.y = cui_max_int32(graph_view->graph_rect.max.y - scroll_bar_height - graph_view->px4, graph_view->graph_rect.min.y);

    graph_view_limit_scroll_offset(graph_view);
}

static void
graph_view_draw(CuiWidget *widget, CuiGraphicsContext *ctx, const CuiColorTheme *color_theme)
{
    GraphView *graph_view = CuiContainerOf(widget, GraphView, base);

    CuiRect rect = graph_view->graph_rect;

    int32_t view_height = cui_rect_get_height(rect);

    CuiRect prev_clip = cui_draw_set_clip_rect(ctx, rect);

    int32_t x = rect.min.x;

    for (uint32_t i = graph_view->scroll_offset.integer_part + 1; i < app.filter_item_count; i += 1)
    {
        FilterItem *filter_item = app.filter_items + i;

        CuiColor color = red_foreground;

        if (filter_item->time_delta < 100) // 0.1 ms
        {
            color = CuiHexColor(0xFFAAAAAA);
        }
        else if (filter_item->time_delta < 9000) // 8.333 ms
        {
            color = blue_foreground;
        }
        else if (filter_item->time_delta < 17000) // 16.666 ms
        {
            color = green_foreground;
        }
        else if (filter_item->time_delta < 34000) // 33.333 ms
        {
            color = yellow_foreground;
        }

        int height = view_height;

        if (filter_item->time_delta <= 50000)
        {
            height = graph_view->px1 + (view_height * filter_item->time_delta) / 50000;
        }

        cui_draw_fill_rect(ctx, cui_make_rect(x, rect.max.y - height, x + graph_view->px4, rect.max.y), color);

        x += graph_view->px4 + graph_view->px1;

        if (x >= rect.max.x)
        {
            break;
        }
    }

    int32_t fps30_y = rect.max.y - (graph_view->px1 + (view_height * 33333) / 50000);
    CuiRect fps30_rect = cui_make_rect(rect.min.x, fps30_y - graph_view->px1, rect.max.x, fps30_y);
    cui_draw_fill_rect(ctx, fps30_rect, CuiHexColor(0x3FC22630));

    int32_t fps60_y = rect.max.y - (graph_view->px1 + (view_height * 16666) / 50000);
    CuiRect fps60_rect = cui_make_rect(rect.min.x, fps60_y - graph_view->px1, rect.max.x, fps60_y);
    cui_draw_fill_rect(ctx, fps60_rect, CuiHexColor(0x3FD5D84D));

    int32_t fps120_y = rect.max.y - (graph_view->px1 + (view_height * 8333) / 50000);
    CuiRect fps120_rect = cui_make_rect(rect.min.x, fps120_y - graph_view->px1, rect.max.x, fps120_y);
    cui_draw_fill_rect(ctx, fps120_rect, CuiHexColor(0x3F33B97B));

    cui_draw_fill_shadow(ctx, rect.min.x, rect.min.y + graph_view->px4, rect.max.x, graph_view->px4, CUI_DIRECTION_SOUTH, color_theme->window_titlebar_background);

    cui_draw_set_clip_rect(ctx, widget->rect);

    if (cui_rect_get_width(graph_view->scroll_handle_rect) > 0)
    {
        cui_draw_fill_rounded_rect_1(ctx, graph_view->scroll_bar_rect, (float) (graph_view->px4 + graph_view->px1), color_theme->default_border);
        cui_draw_fill_rounded_rect_1(ctx, graph_view->scroll_handle_rect, (float) graph_view->px4, color_theme->window_titlebar_background);
    }

    cui_draw_set_clip_rect(ctx, prev_clip);
}

static bool
graph_view_handle_event(CuiWidget *widget, CuiEventType event_type)
{
    bool result = false;

    GraphView *graph_view = CuiContainerOf(widget, GraphView, base);

    CuiAssert(widget->window);
    CuiWindow *window = widget->window;

    switch (event_type)
    {
        case CUI_EVENT_TYPE_MOUSE_DRAG:
        {
            if (graph_view->scrolling)
            {
                int32_t count = app.filter_item_count;

                int32_t bar_width = graph_view->px4 + graph_view->px1;

                int64_t view_width = cui_rect_get_width(graph_view->graph_rect);
                int64_t content_width = (int64_t) bar_width * (int64_t) count;

                if (content_width > view_width)
                {
                    int32_t scroll_handle_width = cui_rect_get_width(graph_view->scroll_handle_rect);

                    CuiRect scroll_handle_bound = graph_view->scroll_bar_rect;
                    scroll_handle_bound.min.x += graph_view->px1;
                    scroll_handle_bound.min.y += graph_view->px1;
                    scroll_handle_bound.max.x -= graph_view->px1;
                    scroll_handle_bound.max.y -= graph_view->px1;

                    int64_t scroll_bar_width = cui_rect_get_width(scroll_handle_bound);

                    CuiPoint mouse = cui_window_get_mouse_position(window);

                    int32_t scroll_handle_offset = mouse.x + graph_view->mouse_offset_x - graph_view->scroll_bar_rect.min.x;

                    int64_t scroll_offset = ((content_width - view_width) * (int64_t) scroll_handle_offset) / (scroll_bar_width - scroll_handle_width);

                    if (scroll_offset < 0)
                    {
                        scroll_offset = 0;
                    }

                    graph_view->scroll_offset.integer_part = (int32_t) (scroll_offset / (int64_t) bar_width);
                    graph_view->scroll_offset.fractional_part = (int32_t) (scroll_offset - (graph_view->scroll_offset.integer_part * bar_width));
                }
                else
                {
                    graph_view->scroll_offset.integer_part = 0;
                    graph_view->scroll_offset.fractional_part = 0;
                }

                graph_view_limit_scroll_offset(graph_view);

                cui_window_request_redraw(window);
            }
        } break;

        case CUI_EVENT_TYPE_LEFT_DOWN:
        {
            cui_window_set_pressed(window, widget);
            cui_window_set_focused(window, widget);

            CuiPoint mouse = cui_window_get_mouse_position(window);

            if (cui_rect_has_point_inside(graph_view->scroll_handle_rect, mouse))
            {
                graph_view->scrolling = true;
                graph_view->mouse_offset_x = graph_view->scroll_handle_rect.min.x - mouse.x;
            }

            result = true;
        } break;

        case CUI_EVENT_TYPE_LEFT_UP:
        {
            graph_view->scrolling = false;
        } break;

        case CUI_EVENT_TYPE_DOUBLE_CLICK:
        {
            cui_window_set_pressed(window, widget);
            result = true;
        } break;

        case CUI_EVENT_TYPE_KEY_DOWN:
        {
            if (cui_window_event_is_ctrl_down(window) || cui_window_event_is_command_down(window))
            {
                uint32_t codepoint = cui_window_event_get_codepoint(window);

                switch (codepoint)
                {
                    case '0':
                    {
                        reset_font_size();
                    } break;

                    case '+':
                    {
                        increase_font_size();
                    } break;

                    case '-':
                    {
                        decrease_font_size();
                    } break;
                }
            }
        } break;

        default:
        {
        } break;
    }

    return result;
}

static void
apply_filter(void)
{
    app.filter_item_count = 0;

    for (uint32_t message_index = 0; message_index < app.message_count; message_index += 1)
    {
        Message *message = app.messages + message_index;

        // TODO: change to contains
        if ((app.filter.interface_name.count > 0) && !cui_string_starts_with(message->interface_name, app.filter.interface_name))
        {
            continue;
        }

        // TODO: change to contains
        if ((app.filter.message_name.count > 0) && !cui_string_starts_with(message->message_name, app.filter.message_name))
        {
            continue;
        }

        if ((app.filter.id > 0) && (message->id != app.filter.id))
        {
            continue;
        }

        uint32_t time_delta = 0;

        if (app.filter_item_count > 0)
        {
            Message *prev_message = app.messages + app.filter_items[app.filter_item_count - 1].message_index;

            if (message->timestamp_us > prev_message->timestamp_us)
            {
                time_delta = (uint32_t) (message->timestamp_us - prev_message->timestamp_us);
            }
        }

        app.filter_items[app.filter_item_count].message_index = message_index;
        app.filter_items[app.filter_item_count].time_delta = time_delta;
        app.filter_item_count += 1;
    }

    list_view_limit_scroll_offset(&app.list_view);
    graph_view_limit_scroll_offset(&app.graph_view);
}

static inline void
skip_spaces(CuiString *str)
{
    while ((str->count > 0) && (str->data[0] == ' '))
    {
        cui_string_advance(str, 1);
    }
}

static inline bool
is_identifier_character(uint32_t c)
{
    return ((c >= '0') && (c <= '9')) ||
           ((c >= 'a') && (c <= 'z')) ||
           ((c >= 'A') && (c <= 'Z')) ||
            (c == '_');
}

static inline CuiString
parse_identifier(CuiString *str)
{
    CuiString result = cui_make_string(str->data, 0);

    while ((result.count < str->count) && is_identifier_character(str->data[result.count]))
    {
        result.count += 1;
    }

    cui_string_advance(str, result.count);

    return result;
}

static inline bool
parse_in_between(CuiString *str, CuiString *result, uint8_t left_char, uint8_t right_char)
{
    result->count = 0;
    result->data = 0;

    int64_t index = 0;

    if ((index < str->count) && (str->data[index] == left_char))
    {
        index += 1;

        int64_t start_index = index;
        int64_t end_index;

        if ((index < str->count) && (str->data[index] != right_char))
        {
            for (;; index += 1)
            {
                if (index >= str->count)
                {
                    return false;
                }

                if (str->data[index] == right_char)
                {
                    end_index = index;
                    index += 1;

                    if ((index < str->count) && (str->data[index] == ' '))
                    {
                        index += 1;
                        break;
                    }
                    else
                    {
                        return false;
                    }
                }
            }

            result->data = str->data + start_index;
            result->count = end_index - start_index;

            str->data += index;
            str->count -= index;

            return true;
        }
    }

    return false;
}

static void
on_filter_action(CuiWidget *widget)
{
    CuiAssert(widget->window);
    CuiWindow *window = widget->window;

    list_view_limit_scroll_offset(&app.list_view);

    cui_window_request_redraw(window);
}

static void
on_input_action(CuiWidget *widget)
{
    CuiString value = cui_string_trim(cui_widget_get_textinput_value(widget));

    CuiString interface_name = parse_identifier(&value);
    CuiString message_name = { 0 };
    uint32_t id = 0;

    if ((value.count > 0) && ((value.data[0] == '#') || (value.data[0] == '@')))
    {
        cui_string_advance(&value, 1);
        // TODO: use custom parsing function
        id = cui_string_parse_int32_advance(&value);
    }

    if ((value.count > 0) && (value.data[0] == '.'))
    {
        cui_string_advance(&value, 1);
        message_name = parse_identifier(&value);
    }

    app.filter.interface_name = interface_name;
    app.filter.message_name = message_name;
    app.filter.id = id;

    apply_filter();
}

static void
load_wayland_file(CuiString wayland_filename)
{
    CuiFile *file = cui_platform_file_open(&app.temporary_memory, wayland_filename, CUI_FILE_MODE_READ);

    if (file)
    {
        cui_widget_set_textinput_value(&app.filter_input, CuiStringLiteral(""));
        app.filter.interface_name.count = 0;
        app.filter.message_name.count = 0;
        app.filter.id = 0;

        if (app.root_widget)
        {
            cui_arena_clear(&app.title_arena);
            cui_window_set_title(app.window, cui_sprint(&app.title_arena, CuiStringLiteral("wl_sherlock - %S"), wayland_filename));
        }

        if (app.message_allocated)
        {
            cui_platform_deallocate(app.messages, app.message_allocated * sizeof(*app.messages));
            app.message_allocated = 0;
            app.messages = 0;
        }

        if (app.filter_item_allocated)
        {
            cui_platform_deallocate(app.filter_items, app.filter_item_allocated * sizeof(*app.filter_items));
            app.filter_item_allocated = 0;
            app.filter_items = 0;
        }

        if (app.file_content.count > 0)
        {
            cui_platform_deallocate(app.file_content.data, app.file_content.count);
        }

        uint64_t file_size = cui_platform_file_get_size(file);
        char *buffer = (char *) cui_platform_allocate(file_size);

        cui_platform_file_read(file, buffer, 0, file_size);
        cui_platform_file_close(file);

        app.file_content = cui_make_string(buffer, file_size);

        uint32_t line_count = 1;

        for (int64_t index = 0; index < app.file_content.count; index += 1)
        {
            if (app.file_content.data[index] == '\n')
            {
                line_count += 1;
            }
        }

        app.message_allocated = line_count;
        app.message_count = 0;
        app.messages = (Message *) cui_platform_allocate(app.message_allocated * sizeof(*app.messages));

        app.filter_item_allocated = line_count;
        app.filter_item_count = 0;
        app.filter_items = (FilterItem *) cui_platform_allocate(app.filter_item_allocated * sizeof(*app.filter_items));

        uint32_t at_count = 0;
        uint32_t hash_count = 0;

        uint64_t last_timestamp = 0;

        CuiString cursor = app.file_content;

        while (cursor.count > 0)
        {
            CuiString str = cui_string_trim(cui_string_get_next_line(&cursor));

            if ((str.count < 1) || (str.data[0] != '[') || (str.data[str.count - 1] != ')'))
            {
                continue;
            }

            CuiString connection_name = { 0 };
            CuiString queue_name = { 0 };

            if (!str.count || (str.data[0] != '['))
            {
                continue;
            }

            cui_string_advance(&str, 1);
            skip_spaces(&str);

            // TODO: use custom version to handle failure case
            int32_t timestamp_ms = cui_string_parse_int32_advance(&str);

            if (!str.count || (str.data[0] != '.'))
            {
                continue;
            }

            cui_string_advance(&str, 1);

            // TODO: use custom version to handle failure case
            int32_t timestamp_us = cui_string_parse_int32_advance(&str);

            if (!str.count || (str.data[0] != ']'))
            {
                continue;
            }

            cui_string_advance(&str, 1);
            skip_spaces(&str);

            if ((str.count > 0) && (str.data[0] == '{'))
            {
                if (!parse_in_between(&str, &queue_name, '{', '}'))
                {
                    continue;
                }
            }

            skip_spaces(&str);

            if (cui_string_starts_with(str, CuiStringLiteral("->")))
            {
                cui_string_advance(&str, CuiStringLiteral("->").count);
                skip_spaces(&str);
            }

            CuiString interface_name = parse_identifier(&str);

            if (cui_string_starts_with(str, CuiStringLiteral("#")))
            {
                hash_count += 1;
                cui_string_advance(&str, 1);
            }
            else if (cui_string_starts_with(str, CuiStringLiteral("@")))
            {
                at_count += 1;
                cui_string_advance(&str, 1);
            }
            else
            {
                continue;
            }

            // TODO: use custom version to handle failure case
            int32_t id = cui_string_parse_int32_advance(&str);

            if (cui_string_starts_with(str, CuiStringLiteral(".")))
            {
                cui_string_advance(&str, 1);
            }
            else
            {
                continue;
            }

            uint64_t timestamp = ((uint64_t) timestamp_ms * 1000) + (uint64_t) timestamp_us;

            CuiString message_name = parse_identifier(&str);

            uint32_t message_index = app.message_count;
            app.message_count += 1;

            app.messages[message_index].str = str;
            app.messages[message_index].connection_name = connection_name;
            app.messages[message_index].queue_name = queue_name;
            app.messages[message_index].interface_name = interface_name;
            app.messages[message_index].id = id;
            app.messages[message_index].message_name = message_name;
            app.messages[message_index].timestamp_us = timestamp;

            uint32_t time_delta = 0;

            if (app.filter_item_count > 0)
            {
                time_delta = (uint32_t) (timestamp - last_timestamp);
            }

            app.filter_items[app.filter_item_count].message_index = message_index;
            app.filter_items[app.filter_item_count].time_delta = time_delta;
            app.filter_item_count += 1;

            last_timestamp = timestamp;
        }

        if (at_count > hash_count)
        {
            app.id_character = CuiStringLiteral("@");
        }
        else
        {
            app.id_character = CuiStringLiteral("#");
        }

        if (!app.file_loaded)
        {
            if (app.root_widget)
            {
                cui_widget_insert_before(&app.bottom_container,
                                         cui_widget_get_first_child(&app.bottom_container),
                                         &app.info_panel);
                cui_widget_layout(&app.bottom_container, app.bottom_container.rect);
                cui_window_request_redraw(app.window);
            }

            app.file_loaded = true;
        }
    }
}

static void
on_open_file_action(CuiWidget *widget)
{
    (void) widget;

    CuiTemporaryMemory temp_memory = cui_begin_temporary_memory(&app.temporary_memory);
    CuiTemporaryMemory temp_widget_memory = cui_begin_temporary_memory(&app.widget_arena);

    CuiString *filenames = 0;
    cui_array_init(filenames, 4, &app.widget_arena);

    if (cui_platform_open_file_dialog(&app.temporary_memory, &app.widget_arena, &filenames, false, true, false))
    {
        if (cui_array_count(filenames) > 0)
        {
            load_wayland_file(filenames[0]);
            cui_widget_layout(&app.list_view.base, app.list_view.base.rect);
            cui_widget_layout(&app.graph_view.base, app.graph_view.base.rect);
        }
    }

    cui_end_temporary_memory(temp_widget_memory);
    cui_end_temporary_memory(temp_memory);
}

static inline CuiWidget *
create_widget(CuiArena *arena, uint32_t type)
{
    CuiWidget *widget = cui_alloc_type(arena, CuiWidget, CuiDefaultAllocationParams());

    cui_widget_init(widget, type);

    return widget;
}

static void
create_top_row(CuiWidget *parent, CuiArena *arena)
{
    CuiWidget *top_container = create_widget(arena, CUI_WIDGET_TYPE_BOX);

    cui_widget_set_main_axis(top_container, CUI_AXIS_X);
    cui_widget_set_x_axis_gravity(top_container, CUI_GRAVITY_END);
    cui_widget_add_flags(top_container, CUI_WIDGET_FLAG_DRAW_BACKGROUND);
    cui_widget_set_padding(top_container, 8.0f, 8.0f, 8.0f, 8.0f);
    cui_widget_set_border_width(top_container, 0.0f, 0.0f, 1.0f, 0.0f);

    top_container->color_normal_background = CUI_COLOR_WINDOW_TITLEBAR_BACKGROUND;

    cui_widget_append_child(parent, top_container);

    cui_widget_init(&app.filter_checkbox, CUI_WIDGET_TYPE_CHECKBOX);
    cui_widget_set_label(&app.filter_checkbox, CuiStringLiteral("Filter"));
    cui_widget_set_padding(&app.filter_checkbox, 0.0f, 4.0f, 0.0f, 12.0f);
    cui_widget_set_inline_padding(&app.filter_checkbox, 8.0f);
    cui_widget_set_font(&app.filter_checkbox, app.list_view_font);

    app.filter_checkbox.on_action = on_filter_action;

    cui_widget_append_child(top_container, &app.filter_checkbox);

    CuiWidget *left_container = create_widget(arena, CUI_WIDGET_TYPE_BOX);

    cui_widget_set_main_axis(left_container, CUI_AXIS_X);
    cui_widget_set_x_axis_gravity(left_container, CUI_GRAVITY_START);
    cui_widget_set_inline_padding(left_container, 12.0f);

    cui_widget_append_child(top_container, left_container);

    cui_widget_init(&app.open_file_button, CUI_WIDGET_TYPE_BUTTON);

    cui_widget_set_font(&app.open_file_button, app.list_view_font);
    cui_widget_set_label(&app.open_file_button, CuiStringLiteral("Open File"));
    cui_widget_set_box_shadow(&app.open_file_button, 0.0f, 0.0f, 0.0f);
    cui_widget_set_border_width(&app.open_file_button, 1.0f, 1.0f, 1.0f, 1.0f);

    app.open_file_button.on_action = on_open_file_action;

    cui_widget_append_child(left_container, &app.open_file_button);

    cui_widget_init(&app.filter_input, CUI_WIDGET_TYPE_TEXTINPUT);

    cui_widget_set_icon(&app.filter_input, CUI_ICON_SEARCH_12);
    cui_widget_set_border_radius(&app.filter_input, 2.0f, 2.0f, 2.0f, 2.0f);
    cui_widget_set_font(&app.filter_input, app.list_view_font);
    cui_widget_set_label(&app.filter_input, CuiStringLiteral("Filter messages..."));
    cui_widget_set_textinput_buffer(&app.filter_input, cui_alloc(arena, CuiKiB(1), CuiDefaultAllocationParams()), CuiKiB(1));

    app.filter_input.on_action = on_input_action;

    cui_widget_append_child(left_container, &app.filter_input);
}

static void
create_list_view(CuiWidget *parent)
{
    cui_widget_init(&app.list_view.base, WIDGET_TYPE_LIST_VIEW);
    CuiWidgetInitCustomFunctions(&app.list_view.base, list_view_);
    cui_widget_append_child(parent, &app.list_view.base);
}

static void
create_info_panel(CuiWidget *parent)
{
    cui_widget_init(&app.info_panel, CUI_WIDGET_TYPE_BOX);

    cui_widget_set_main_axis(&app.info_panel, CUI_AXIS_Y);
    cui_widget_set_y_axis_gravity(&app.info_panel, CUI_GRAVITY_START);
    cui_widget_add_flags(&app.info_panel, CUI_WIDGET_FLAG_DRAW_BACKGROUND);
#if CUI_PLATFORM_MACOS
    cui_widget_set_padding(&app.info_panel, 4.0f, 8.0f, 6.0f, 8.0f);
#else
    cui_widget_set_padding(&app.info_panel, 4.0f, 8.0f, 4.0f, 8.0f);
#endif
    cui_widget_set_border_width(&app.info_panel, 1.0f, 0.0f, 0.0f, 0.0f);

    app.info_panel.color_normal_background = CUI_COLOR_WINDOW_TITLEBAR_BACKGROUND;

    if (app.file_loaded)
    {
        cui_widget_append_child(parent, &app.info_panel);
    }

    cui_widget_init(&app.graph_view.base, WIDGET_TYPE_GRAPH_VIEW);
    CuiWidgetInitCustomFunctions(&app.graph_view.base, graph_view_);
    cui_widget_append_child(&app.info_panel, &app.graph_view.base);
}

static void
create_user_interface(CuiWindow *window, CuiArena *arena)
{
    app.root_widget = create_widget(arena, CUI_WIDGET_TYPE_BOX);

    cui_widget_set_main_axis(app.root_widget, CUI_AXIS_Y);
    cui_widget_set_y_axis_gravity(app.root_widget, CUI_GRAVITY_START);

    create_top_row(app.root_widget, arena);

    cui_widget_init(&app.bottom_container, CUI_WIDGET_TYPE_BOX);

    cui_widget_set_main_axis(&app.bottom_container, CUI_AXIS_Y);
    cui_widget_set_y_axis_gravity(&app.bottom_container, CUI_GRAVITY_END);

    cui_widget_append_child(app.root_widget, &app.bottom_container);

    create_info_panel(&app.bottom_container);
    create_list_view(&app.bottom_container);

    cui_window_set_root_widget(window, app.root_widget);
}

CUI_PLATFORM_MAIN
{
    if (!CUI_PLATFORM_INIT)
    {
        return -1;
    }

    cui_arena_allocate(&app.temporary_memory, CuiMiB(2));
    cui_arena_allocate(&app.widget_arena, CuiMiB(4));
    cui_arena_allocate(&app.title_arena, CuiKiB(1));

    CuiString wayland_filename = { 0 };

    CuiString *files_to_open = cui_get_files_to_open();
    CuiString *arguments = cui_get_command_line_arguments();

    if (cui_array_count(files_to_open) > 0)
    {
        wayland_filename = files_to_open[0];
    }
    else if (cui_array_count(arguments) > 0)
    {
        wayland_filename = arguments[0];
    }

    if (wayland_filename.count && cui_platform_file_exists(&app.temporary_memory, wayland_filename))
    {
        load_wayland_file(wayland_filename);
    }

    app.window = cui_window_create(0);

    if (app.file_loaded)
    {
        cui_arena_clear(&app.title_arena);
        cui_window_set_title(app.window, cui_sprint(&app.title_arena, CuiStringLiteral("wl_sherlock - %S"), wayland_filename));
    }
    else
    {
        cui_window_set_title(app.window, CuiStringLiteral("wl_sherlock"));
    }

    // cui_window_resize(app.window, lroundf(cui_window_get_ui_scale(app.window) * 900),
    //                               lroundf(cui_window_get_ui_scale(app.window) * 700));

    cui_window_set_color_theme(app.window, &color_theme);

    set_default_font_size();

    app.list_view_font = cui_window_find_font(app.window,
                                              // android
                                              cui_make_sized_font_spec(CuiStringLiteral("CutiveMono"),            app.font_size, app.line_height),
                                              // all
                                              cui_make_sized_font_spec(CuiStringLiteral("JetBrainsMono-Regular"), app.font_size, app.line_height),
                                              cui_make_sized_font_spec(CuiStringLiteral("FiraCode-Regular"),      app.font_size, app.line_height),
                                              cui_make_sized_font_spec(CuiStringLiteral("CascadiaCode"),          app.font_size, app.line_height),
                                              cui_make_sized_font_spec(CuiStringLiteral("Courier New Bold"),      app.font_size, app.line_height),
                                              cui_make_sized_font_spec(CuiStringLiteral("consola"),               app.font_size, app.line_height),
                                              cui_make_sized_font_spec(CuiStringLiteral("DejaVuSansMono"),        app.font_size, app.line_height),
                                              // emoji fonts
                                              cui_make_sized_font_spec(CuiStringLiteral("Twemoji.Mozilla"),       app.font_size, app.line_height),
                                              cui_make_sized_font_spec(CuiStringLiteral("TwemojiMozilla"),        app.font_size, app.line_height),
                                              cui_make_sized_font_spec(CuiStringLiteral("seguiemj"),              app.font_size, app.line_height));


    create_user_interface(app.window, &app.widget_arena);

    cui_window_show(app.window);

    return cui_main_loop();
}
