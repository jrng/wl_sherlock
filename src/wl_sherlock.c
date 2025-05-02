#include <cui.h>

static const uint32_t WIDGET_TYPE_LIST_VIEW = CUI_WIDGET_TYPE_CUSTOM + 0;

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
    /* default_button_normal_background     */ CuiHexColorLiteral(0xFF2F333D),
    /* default_button_normal_box_shadow     */ CuiHexColorLiteral(0x3F000000),
    /* default_button_normal_border         */ CuiHexColorLiteral(0xFF1E1E1E),
    /* default_button_normal_text           */ CuiHexColorLiteral(0xFFD7DAE0),
    /* default_button_normal_icon           */ CuiHexColorLiteral(0xFFB7BAC0),
    /* default_textinput_normal_background  */ CuiHexColorLiteral(0xFF1a1e2d),
    /* default_textinput_normal_box_shadow  */ CuiHexColorLiteral(0x3F000000),
    /* default_textinput_normal_border      */ CuiHexColorLiteral(0xFF151721),
    /* default_textinput_normal_text        */ CuiHexColorLiteral(0xFFafb7c4),
    /* default_textinput_normal_placeholder */ CuiHexColorLiteral(0xFF333846),
    /* default_textinput_normal_icon        */ CuiHexColorLiteral(0xFFB7BAC0),
};

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
    int32_t px16;

    ScrollOffset scroll_offset;
} ListView;

typedef struct
{
    uint32_t message_index;
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

    uint32_t message_allocated;
    uint32_t message_count;
    Message *messages;

    uint32_t filter_item_allocated;
    uint32_t filter_item_count;
    FilterItem *filter_items;

    Filter filter;

    CuiString id_character;

    CuiWindow *window;

    CuiFontId list_view_font;

    ListView list_view;

    CuiWidget *filter_input;
} Application;

static Application app;

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

static void
limit_scroll_offset(ScrollOffset *offset, int32_t count)
{
    if (offset->integer_part < 0)
    {
        offset->integer_part = 0;
        offset->fractional_part = 0;
    }

    int32_t max_scroll_offset = count - 1;

    if (max_scroll_offset < 0)
    {
        max_scroll_offset = 0;
    }

    if (offset->integer_part >= max_scroll_offset)
    {
        offset->integer_part = max_scroll_offset;
        offset->fractional_part = 0;
    }
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
    (void) widget;
    (void) rect;
}

static void
list_view_draw(CuiWidget *widget, CuiGraphicsContext *ctx, const CuiColorTheme *color_theme)
{
    (void) color_theme;

    ListView *list_view = CuiContainerOf(widget, ListView, base);

    CuiColor background_color = CuiHexColorLiteral(0xFF191C28);

    float timestamp_content_width = ceilf(cui_window_get_string_width(widget->window, app.list_view_font, CuiStringLiteral("9999999.999")));
    float interface_content_width = roundf(widget->ui_scale * 230.0f);
    float id_content_width = ceilf(cui_window_get_string_width(widget->window, app.list_view_font, CuiStringLiteral("9999999999")));
    float message_content_width = roundf(widget->ui_scale * 300.0f);

    float timestamp_column_width = timestamp_content_width + 2 * list_view->px8;
    float interface_column_width = interface_content_width + 2 * list_view->px8;
    float id_column_width = id_content_width + 2 * list_view->px8;
    float message_column_width = message_content_width + 2 * list_view->px8;

    int32_t line_height = cui_window_get_font_line_height(widget->window, app.list_view_font);
    float baseline_offset = cui_window_get_font_baseline_offset(widget->window, app.list_view_font);

    float header_baseline = (float) list_view->px4 + baseline_offset;

    float row_baseline = (float) list_view->px6 + baseline_offset;

    CuiRect list_rect = widget->rect;
    CuiRect header_rect = widget->rect;

    list_rect.min.y = cui_min_int32(list_rect.min.y + (line_height + 2 * list_view->px4 + list_view->px1), list_rect.max.y);
    header_rect.max.y = list_rect.min.y;

    cui_draw_fill_rect(ctx, cui_make_rect(header_rect.min.x, header_rect.min.y, header_rect.max.x, header_rect.max.y - list_view->px1), CuiHexColor(0xFF1E2332));
    cui_draw_fill_rect(ctx, cui_make_rect(header_rect.min.x, header_rect.max.y - list_view->px1, header_rect.max.x, header_rect.max.y), color_theme->default_border);

    float x = header_rect.min.x;
    float y = header_rect.min.y + header_baseline;

    float w;

    w = cui_window_get_string_width(widget->window, app.list_view_font, CuiStringLiteral("Timestamp"));
    cui_draw_fill_string(ctx, app.list_view_font, x + 0.5f * (timestamp_column_width - w), y, CuiStringLiteral("Timestamp"), CuiHexColor(0xFFafb7c4));
    x += timestamp_column_width;

    cui_draw_fill_rect(ctx, cui_make_rect(x, header_rect.min.y, x + list_view->px1, header_rect.max.y), color_theme->default_border);
    x += list_view->px1;

    w = cui_window_get_string_width(widget->window, app.list_view_font, CuiStringLiteral("Interface"));
    cui_draw_fill_string(ctx, app.list_view_font, x + 0.5f * (interface_column_width - w), y, CuiStringLiteral("Interface"), CuiHexColor(0xFFafb7c4));
    x += interface_column_width;

    cui_draw_fill_rect(ctx, cui_make_rect(x, header_rect.min.y, x + list_view->px1, header_rect.max.y), color_theme->default_border);
    x += list_view->px1;

    w = cui_window_get_string_width(widget->window, app.list_view_font, CuiStringLiteral("Id"));
    cui_draw_fill_string(ctx, app.list_view_font, x + 0.5f * (id_column_width - w), y, CuiStringLiteral("Id"), CuiHexColor(0xFFafb7c4));
    x += id_column_width;

    cui_draw_fill_rect(ctx, cui_make_rect(x, header_rect.min.y, x + list_view->px1, header_rect.max.y), color_theme->default_border);
    x += list_view->px1;

    w = cui_window_get_string_width(widget->window, app.list_view_font, CuiStringLiteral("Message"));
    cui_draw_fill_string(ctx, app.list_view_font, x + 0.5f * (message_column_width - w), y, CuiStringLiteral("Message"), CuiHexColor(0xFFafb7c4));
    x += message_column_width;

    cui_draw_fill_rect(ctx, list_rect, background_color);

    CuiRect prev_clip = cui_draw_set_clip_rect(ctx, list_rect);

    int32_t row_height = line_height + 2 * list_view->px6 + list_view->px1;

    float row_advance = (float) row_height;

    x = list_rect.min.x;
    y = (float) (list_rect.min.y - list_view->scroll_offset.fractional_part);

    uint32_t index = list_view->scroll_offset.integer_part;

    while (index < app.filter_item_count)
    {
        CuiTemporaryMemory temp_memory = cui_begin_temporary_memory(&app.temporary_memory);

        FilterItem *filter_item = app.filter_items + index;
        CuiAssert(filter_item->message_index < app.message_count);
        Message *message = app.messages + filter_item->message_index;

        x = list_rect.min.x;

        CuiString timestamp_str = cui_sprint(&app.temporary_memory, CuiStringLiteral("%u.%03u"), (uint32_t) (message->timestamp_us / 1000), (uint32_t) (message->timestamp_us % 1000));

        w = cui_window_get_string_width(widget->window, app.list_view_font, timestamp_str);
        cui_draw_fill_string(ctx, app.list_view_font, x + list_view->px8 + (timestamp_content_width - w), y + row_baseline, timestamp_str, CuiHexColor(0xFF333846));
        x += timestamp_column_width;

        x += list_view->px1;

        w = cui_window_get_string_width(widget->window, app.list_view_font, message->interface_name);
        cui_draw_fill_string(ctx, app.list_view_font, x + list_view->px8 + (interface_content_width - w), y + row_baseline, message->interface_name, CuiHexColor(0xFFafb7c4));
        x += interface_column_width;

        x += list_view->px1;

        w = cui_window_get_string_width(widget->window, app.list_view_font, app.id_character);
        cui_draw_fill_string(ctx, app.list_view_font, x - 0.5f * w, y + row_baseline, app.id_character, CuiHexColor(0xFF333846));

        CuiString id_str = cui_sprint(&app.temporary_memory, CuiStringLiteral("%u"), message->id);

        cui_draw_fill_string(ctx, app.list_view_font, x + list_view->px8, y + row_baseline, id_str, CuiHexColor(0xFFafb7c4));
        x += id_column_width;

        x += list_view->px1;

        w = cui_window_get_string_width(widget->window, app.list_view_font, CuiStringLiteral("."));
        cui_draw_fill_string(ctx, app.list_view_font, x - 0.5f * w, y + row_baseline, CuiStringLiteral("."), CuiHexColor(0xFF333846));

        cui_draw_fill_string(ctx, app.list_view_font, x + list_view->px8, y + row_baseline, message->message_name, CuiHexColor(0xFFafb7c4));
        x += list_view->px8 + cui_window_get_string_width(widget->window, app.list_view_font, message->message_name);

        x += list_view->px2;

        cui_draw_fill_string(ctx, app.list_view_font, x, y + row_baseline, message->str, cui_make_color(1.0f, 1.0f, 1.0f, 1.0f));

        y += row_advance;

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
        case CUI_EVENT_TYPE_LEFT_DOWN:
        {
            cui_window_set_pressed(window, widget);
            cui_window_set_focused(window, widget);
            result = true;
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

                limit_scroll_offset(&list_view->scroll_offset, app.filter_item_count);

                cui_window_request_redraw(window);
            }

            result = true;
        }

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

        app.filter_items[app.filter_item_count].message_index = message_index;
        app.filter_item_count += 1;
    }

    limit_scroll_offset(&app.list_view.scroll_offset, app.filter_item_count);
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
    cui_widget_set_y_axis_gravity(top_container, CUI_GRAVITY_END);
    cui_widget_add_flags(top_container, CUI_WIDGET_FLAG_DRAW_BACKGROUND);
    cui_widget_set_padding(top_container, 8.0f, 8.0f, 8.0f, 8.0f);
    cui_widget_set_border_width(top_container, 0.0f, 0.0f, 1.0f, 0.0f);

    top_container->color_normal_background = CUI_COLOR_WINDOW_TITLEBAR_BACKGROUND;

    cui_widget_append_child(parent, top_container);

    CuiWidget *right_container = create_widget(arena, CUI_WIDGET_TYPE_BOX);

    cui_widget_append_child(top_container, right_container);

    app.filter_input = create_widget(arena, CUI_WIDGET_TYPE_TEXTINPUT);

    cui_widget_set_icon(app.filter_input, CUI_ICON_SEARCH_12);
    cui_widget_set_border_radius(app.filter_input, 2.0f, 2.0f, 2.0f, 2.0f);
    cui_widget_set_font(app.filter_input, app.list_view_font);
    cui_widget_set_label(app.filter_input, CuiStringLiteral("Filter messages..."));
    cui_widget_set_textinput_buffer(app.filter_input, cui_alloc(arena, CuiKiB(1), CuiDefaultAllocationParams()), CuiKiB(1));

    app.filter_input->on_action = on_input_action;

    cui_widget_append_child(top_container, app.filter_input);
}

static void
create_list_view(CuiWidget *parent, CuiArena *arena)
{
    cui_widget_init(&app.list_view.base, WIDGET_TYPE_LIST_VIEW);
    CuiWidgetInitCustomFunctions(&app.list_view.base, list_view_);
    cui_widget_append_child(parent, &app.list_view.base);
}

static void
create_info_panel(CuiWidget *parent, CuiArena *arena)
{
    CuiWidget *status_container = create_widget(arena, CUI_WIDGET_TYPE_BOX);

    cui_widget_set_main_axis(status_container, CUI_AXIS_X);
    cui_widget_set_y_axis_gravity(status_container, CUI_GRAVITY_START);
    cui_widget_add_flags(status_container, CUI_WIDGET_FLAG_DRAW_BACKGROUND);
#if CUI_PLATFORM_MACOS
    cui_widget_set_padding(status_container, 4.0f, 8.0f, 6.0f, 8.0f);
#else
    cui_widget_set_padding(status_container, 4.0f, 8.0f, 4.0f, 8.0f);
#endif
    cui_widget_set_inline_padding(status_container, 16.0f);

    status_container->color_normal_background = CUI_COLOR_WINDOW_TITLEBAR_BACKGROUND;

    cui_widget_append_child(parent, status_container);

    // directory

    CuiWidget *directory_label = create_widget(arena, CUI_WIDGET_TYPE_LABEL);

    // cui_widget_set_label(directory_label, app.directory);

    cui_widget_append_child(status_container, directory_label);

    // file count

    CuiWidget *file_count_label = create_widget(arena, CUI_WIDGET_TYPE_LABEL);

    // cui_widget_set_label(file_count_label, cui_sprint(arena, CuiStringLiteral("%d files"), app.file_count));

    cui_widget_append_child(status_container, file_count_label);

    // folder count

    CuiWidget *folder_count_label = create_widget(arena, CUI_WIDGET_TYPE_LABEL);

    // cui_widget_set_label(folder_count_label, cui_sprint(arena, CuiStringLiteral("%d folders"), app.folder_count));

    cui_widget_append_child(status_container, folder_count_label);
}

static void
create_user_interface(CuiWindow *window, CuiArena *arena)
{
    CuiWidget *root_widget = create_widget(arena, CUI_WIDGET_TYPE_BOX);

    cui_widget_set_main_axis(root_widget, CUI_AXIS_Y);
    cui_widget_set_y_axis_gravity(root_widget, CUI_GRAVITY_START);

    create_top_row(root_widget, arena);

    CuiWidget *bottom_container = create_widget(arena, CUI_WIDGET_TYPE_BOX);

    cui_widget_set_main_axis(bottom_container, CUI_AXIS_Y);
    cui_widget_set_y_axis_gravity(bottom_container, CUI_GRAVITY_END);

    cui_widget_append_child(root_widget, bottom_container);

    // create_info_panel(bottom_container, arena);
    create_list_view(bottom_container, arena);

    cui_window_set_root_widget(window, root_widget);
}

static void
load_wayland_file(CuiString wayland_filename)
{
    CuiFile *file = cui_platform_file_open(&app.temporary_memory, wayland_filename, CUI_FILE_MODE_READ);

    if (file)
    {
        uint64_t file_size = cui_platform_file_get_size(file);
        char *buffer = (char *) cui_platform_allocate(file_size);

        cui_platform_file_read(file, buffer, 0, file_size);

        CuiString content = cui_make_string(buffer, file_size);

        uint32_t line_count = 1;

        for (int64_t index = 0; index < content.count; index += 1)
        {
            if (content.data[index] == '\n')
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

        CuiString cursor = content;

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

            CuiString message_name = parse_identifier(&str);

            uint32_t message_index = app.message_count;
            app.message_count += 1;

            app.messages[message_index].str = str;
            app.messages[message_index].connection_name = connection_name;
            app.messages[message_index].queue_name = queue_name;
            app.messages[message_index].interface_name = interface_name;
            app.messages[message_index].id = id;
            app.messages[message_index].message_name = message_name;
            app.messages[message_index].timestamp_us = ((uint64_t) timestamp_ms * 1000) + (uint64_t) timestamp_us;

            app.filter_items[app.filter_item_count].message_index = message_index;
            app.filter_item_count += 1;
        }

        if (at_count > hash_count)
        {
            app.id_character = CuiStringLiteral("@");
        }
        else
        {
            app.id_character = CuiStringLiteral("#");
        }

        // cui_platform_deallocate(buffer, file_size);
        cui_platform_file_close(file);
    }
}

CUI_PLATFORM_MAIN
{
    if (!CUI_PLATFORM_INIT)
    {
        return -1;
    }

    cui_arena_allocate(&app.temporary_memory, CuiMiB(2));
    cui_arena_allocate(&app.widget_arena, CuiMiB(4));

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

    cui_window_set_title(app.window, CuiStringLiteral("wl_sherlock"));
    // cui_window_resize(app.window, lroundf(cui_window_get_ui_scale(app.window) * 900),
    //                               lroundf(cui_window_get_ui_scale(app.window) * 700));

    cui_window_set_color_theme(app.window, &color_theme);

    float font_size = 14.0f;
    float line_height = 1.0f;

    app.list_view_font = cui_window_find_font(app.window,
                                              // android
                                              cui_make_sized_font_spec(CuiStringLiteral("CutiveMono"),            font_size, line_height),
                                              // all
                                              cui_make_sized_font_spec(CuiStringLiteral("JetBrainsMono-Regular"), font_size, line_height),
                                              cui_make_sized_font_spec(CuiStringLiteral("FiraCode-Regular"),      font_size, line_height),
                                              cui_make_sized_font_spec(CuiStringLiteral("CascadiaCode"),          font_size, line_height),
                                              cui_make_sized_font_spec(CuiStringLiteral("Courier New Bold"),      font_size, line_height),
                                              cui_make_sized_font_spec(CuiStringLiteral("consola"),               font_size, line_height),
                                              cui_make_sized_font_spec(CuiStringLiteral("DejaVuSansMono"),        font_size, line_height),
                                              // emoji fonts
                                              cui_make_sized_font_spec(CuiStringLiteral("Twemoji.Mozilla"),       font_size, line_height),
                                              cui_make_sized_font_spec(CuiStringLiteral("TwemojiMozilla"),        font_size, line_height),
                                              cui_make_sized_font_spec(CuiStringLiteral("seguiemj"),              font_size, line_height));


    create_user_interface(app.window, &app.widget_arena);

    cui_window_show(app.window);

    return cui_main_loop();
}
