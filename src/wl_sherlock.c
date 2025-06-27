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
static const CuiColor green_background  = CuiHexColorLiteral(0xFF2C5948);
static const CuiColor green_foreground  = CuiHexColorLiteral(0xFF33B97B);
// static const CuiColor red_background    = CuiHexColorLiteral(0xFF4E2630);
static const CuiColor red_foreground    = CuiHexColorLiteral(0xFFC22630);
// static const CuiColor yellow_background = CuiHexColorLiteral(0xFF46431F);
static const CuiColor yellow_foreground = CuiHexColorLiteral(0xFFD5D84D);
static const CuiColor pink_background   = CuiHexColorLiteral(0xFF391A3E);
// static const CuiColor pink_foreground   = CuiHexColorLiteral(0xFFAF3CC3);
static const CuiColor pink_foreground   = CuiHexColorLiteral(0xFFC344D9);

static const int32_t SCROLL_ROW_OFFSET = 2;

typedef struct
{
    int32_t integer_part;
    int32_t fractional_part;
} ScrollOffset;

typedef enum
{
    ARGUMENT_TYPE_NIL     = 0,
    ARGUMENT_TYPE_INTEGER = 1,
    ARGUMENT_TYPE_FIXED   = 2,
    ARGUMENT_TYPE_FD      = 3,
    ARGUMENT_TYPE_STRING  = 4,
    ARGUMENT_TYPE_ARRAY   = 5,
    ARGUMENT_TYPE_OBJECT  = 6,
    ARGUMENT_TYPE_NEW_ID  = 7,
} ArgumentType;

typedef struct
{
    ArgumentType type;

    union
    {
        int64_t i;
        int32_t f;
        int32_t fd;
        uint32_t id;
        uint32_t count;
        CuiString str;
    } value;

    CuiString interface_name;
    CuiString label;
    CuiString value_str;
} Argument;

typedef enum
{
    MESSAGE_TYPE_PLAIN   = 0,
    MESSAGE_TYPE_WAYLAND = 1,
} MessageType;

typedef struct
{
    MessageType type;
    CuiString connection_name;
    CuiString queue_name;
    CuiString interface_name;
    CuiString message_name;
    uint32_t id;

    bool sent;

    uint32_t argument_count;
    Argument *arguments;

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

    bool is_filtered;
    bool show_non_wayland_messages;

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

    CuiArena message_arena;

    uint32_t message_allocated;
    uint32_t message_count;
    Message *messages;

    uint32_t wayland_message_allocated;
    uint32_t wayland_message_count;
    uint32_t *wayland_messages;

    uint32_t filtered_message_allocated;
    uint32_t filtered_message_count;
    uint32_t *filtered_messages;

    uint32_t filtered_wayland_message_allocated;
    uint32_t filtered_wayland_message_count;
    FilterItem *filtered_wayland_messages;

    Filter filter;
    uint32_t filtered_item_index;

    CuiString id_character;

    CuiWindow *window;
    CuiWidget *root_widget;

    float font_size;
    float line_height;
    CuiFontId list_view_font;

    CuiWidget open_file_button;
    CuiWidget filter_input;
    CuiWidget filter_checkbox;
    CuiWidget non_wayland_messages_checkbox;
    CuiWidget bottom_container;
    CuiWidget info_panel;
    ListView list_view;
    GraphView graph_view;
} Application;

static Application app;

typedef void (*ArgumentFormatFunc)(Argument *dst, Argument *src, CuiString label);

typedef struct
{
    ArgumentType type;
    CuiString label;
    ArgumentFormatFunc format_func;
} ArgumentSpec;

typedef struct MessageSpec MessageSpec;

typedef void (*MessageFormatFunc)(Message *message, uint32_t argument_count, Argument *arguments, MessageSpec *message_spec);

struct MessageSpec
{
    CuiString interface_name;
    CuiString message_name;
    MessageFormatFunc message_format_func;
    uint32_t argument_count;
    ArgumentSpec *arguments;
};

static inline bool
message_matches_signature(uint32_t argument_count, Argument *arguments, MessageSpec *message_spec)
{
    if (argument_count != message_spec->argument_count)
    {
        return false;
    }

    for (uint32_t i = 0; i < message_spec->argument_count; i += 1)
    {
        ArgumentSpec *argument_spec = message_spec->arguments + i;

        CuiAssert(argument_spec->type != ARGUMENT_TYPE_NIL);

        if (arguments[i].type == argument_spec->type)
        {
            continue;
        }

        // TODO: ARGUMENT_TYPE_NEW_ID can also be nil
        if (((argument_spec->type == ARGUMENT_TYPE_STRING) ||
             (argument_spec->type == ARGUMENT_TYPE_OBJECT)) &&
             (argument_spec->type == ARGUMENT_TYPE_NIL))
        {
            continue;
        }

        return false;
    }

    return true;
}

static void
format_argument(Argument *dst, Argument *src, CuiString label)
{
    dst->type = src->type;
    dst->interface_name = src->interface_name;
    dst->label = label;
    dst->value = src->value;

    CuiTemporaryMemory temp_memory = cui_begin_temporary_memory(&app.temporary_memory);

    CuiStringBuilder string_builder;
    cui_string_builder_init(&string_builder, &app.temporary_memory);

    switch (dst->type)
    {
        case ARGUMENT_TYPE_NIL:
        {
            dst->value_str = CuiStringLiteral("nil");
        } break;

        case ARGUMENT_TYPE_INTEGER:
        {
            cui_string_builder_print(&string_builder, CuiStringLiteral("%ld"), dst->value.i);
            dst->value_str = cui_string_builder_to_string(&string_builder, &app.message_arena);
        } break;

        case ARGUMENT_TYPE_FIXED:
        {
            if (dst->value.f >= 0)
            {
                cui_string_builder_print(&string_builder, CuiStringLiteral("%d.%08u"),
                                         (dst->value.f / 256), (uint32_t) (390625 * (dst->value.f % 256)));
            }
            else
            {
                cui_string_builder_print(&string_builder, CuiStringLiteral("-%d.%08u"),
                                         (dst->value.f / -256), (uint32_t) (-390625 * (dst->value.f % 256)));
            }
            dst->value_str = cui_string_builder_to_string(&string_builder, &app.message_arena);
        } break;

        case ARGUMENT_TYPE_FD:
        {
            cui_string_builder_print(&string_builder, CuiStringLiteral("fd %d"), dst->value.fd);
            dst->value_str = cui_string_builder_to_string(&string_builder, &app.message_arena);
        } break;

        case ARGUMENT_TYPE_STRING:
        {
            cui_string_builder_print(&string_builder, CuiStringLiteral("\"%S\""), dst->value.str);
            dst->value_str = cui_string_builder_to_string(&string_builder, &app.message_arena);
        } break;

        case ARGUMENT_TYPE_ARRAY:
        {
            cui_string_builder_print(&string_builder, CuiStringLiteral("array[%u]"), dst->value.count);
            dst->value_str = cui_string_builder_to_string(&string_builder, &app.message_arena);
        } break;

        case ARGUMENT_TYPE_OBJECT:
        {
            cui_string_builder_print(&string_builder, CuiStringLiteral("%S#%u"), dst->interface_name, dst->value.id);
            dst->value_str = cui_string_builder_to_string(&string_builder, &app.message_arena);
        } break;

        case ARGUMENT_TYPE_NEW_ID:
        {
            cui_string_builder_print(&string_builder, CuiStringLiteral("new id %S#%u"), dst->interface_name, dst->value.id);
            dst->value_str = cui_string_builder_to_string(&string_builder, &app.message_arena);
        } break;
    }

    cui_end_temporary_memory(temp_memory);
}

static void
wl_shm_format_format(Argument *dst, Argument *src, CuiString label)
{
    CuiAssert(src->type == ARGUMENT_TYPE_INTEGER);

    dst->type = src->type;
    dst->interface_name = src->interface_name;
    dst->label = label;
    dst->value = src->value;

    CuiTemporaryMemory temp_memory = cui_begin_temporary_memory(&app.temporary_memory);

    CuiStringBuilder string_builder;
    cui_string_builder_init(&string_builder, &app.temporary_memory);

    uint32_t format = (uint32_t) dst->value.i;

    switch (format)
    {
        case 0: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_ARGB8888");                      break;
        case 1: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_XRGB8888");                      break;
        case 0x20203843: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_C8");                   break;
        case 0x38424752: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_RGB332");               break;
        case 0x38524742: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_BGR233");               break;
        case 0x32315258: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_XRGB4444");             break;
        case 0x32314258: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_XBGR4444");             break;
        case 0x32315852: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_RGBX4444");             break;
        case 0x32315842: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_BGRX4444");             break;
        case 0x32315241: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_ARGB4444");             break;
        case 0x32314241: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_ABGR4444");             break;
        case 0x32314152: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_RGBA4444");             break;
        case 0x32314142: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_BGRA4444");             break;
        case 0x35315258: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_XRGB1555");             break;
        case 0x35314258: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_XBGR1555");             break;
        case 0x35315852: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_RGBX5551");             break;
        case 0x35315842: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_BGRX5551");             break;
        case 0x35315241: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_ARGB1555");             break;
        case 0x35314241: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_ABGR1555");             break;
        case 0x35314152: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_RGBA5551");             break;
        case 0x35314142: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_BGRA5551");             break;
        case 0x36314752: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_RGB565");               break;
        case 0x36314742: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_BGR565");               break;
        case 0x34324752: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_RGB888");               break;
        case 0x34324742: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_BGR888");               break;
        case 0x34324258: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_XBGR8888");             break;
        case 0x34325852: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_RGBX8888");             break;
        case 0x34325842: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_BGRX8888");             break;
        case 0x34324241: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_ABGR8888");             break;
        case 0x34324152: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_RGBA8888");             break;
        case 0x34324142: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_BGRA8888");             break;
        case 0x30335258: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_XRGB2101010");          break;
        case 0x30334258: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_XBGR2101010");          break;
        case 0x30335852: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_RGBX1010102");          break;
        case 0x30335842: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_BGRX1010102");          break;
        case 0x30335241: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_ARGB2101010");          break;
        case 0x30334241: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_ABGR2101010");          break;
        case 0x30334152: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_RGBA1010102");          break;
        case 0x30334142: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_BGRA1010102");          break;
        case 0x56595559: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_YUYV");                 break;
        case 0x55595659: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_YVYU");                 break;
        case 0x59565955: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_UYVY");                 break;
        case 0x59555956: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_VYUY");                 break;
        case 0x56555941: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_AYUV");                 break;
        case 0x3231564e: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_NV12");                 break;
        case 0x3132564e: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_NV21");                 break;
        case 0x3631564e: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_NV16");                 break;
        case 0x3136564e: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_NV61");                 break;
        case 0x39565559: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_YUV410");               break;
        case 0x39555659: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_YVU410");               break;
        case 0x31315559: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_YUV411");               break;
        case 0x31315659: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_YVU411");               break;
        case 0x32315559: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_YUV420");               break;
        case 0x32315659: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_YVU420");               break;
        case 0x36315559: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_YUV422");               break;
        case 0x36315659: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_YVU422");               break;
        case 0x34325559: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_YUV444");               break;
        case 0x34325659: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_YVU444");               break;
        case 0x20203852: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_R8");                   break;
        case 0x20363152: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_R16");                  break;
        case 0x38384752: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_RG88");                 break;
        case 0x38385247: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_GR88");                 break;
        case 0x32334752: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_RG1616");               break;
        case 0x32335247: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_GR1616");               break;
        case 0x48345258: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_XRGB16161616F");        break;
        case 0x48344258: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_XBGR16161616F");        break;
        case 0x48345241: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_ARGB16161616F");        break;
        case 0x48344241: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_ABGR16161616F");        break;
        case 0x56555958: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_XYUV8888");             break;
        case 0x34325556: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_VUY888");               break;
        case 0x30335556: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_VUY101010");            break;
        case 0x30313259: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_Y210");                 break;
        case 0x32313259: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_Y212");                 break;
        case 0x36313259: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_Y216");                 break;
        case 0x30313459: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_Y410");                 break;
        case 0x32313459: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_Y412");                 break;
        case 0x36313459: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_Y416");                 break;
        case 0x30335658: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_XVYU2101010");          break;
        case 0x36335658: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_XVYU12_16161616");      break;
        case 0x38345658: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_XVYU16161616");         break;
        case 0x304c3059: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_Y0L0");                 break;
        case 0x304c3058: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_X0L0");                 break;
        case 0x324c3059: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_Y0L2");                 break;
        case 0x324c3058: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_X0L2");                 break;
        case 0x38305559: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_YUV420_8BIT");          break;
        case 0x30315559: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_YUV420_10BIT");         break;
        case 0x38415258: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_XRGB8888_A8");          break;
        case 0x38414258: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_XBGR8888_A8");          break;
        case 0x38415852: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_RGBX8888_A8");          break;
        case 0x38415842: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_BGRX8888_A8");          break;
        case 0x38413852: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_RGB888_A8");            break;
        case 0x38413842: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_BGR888_A8");            break;
        case 0x38413552: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_RGB565_A8");            break;
        case 0x38413542: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_BGR565_A8");            break;
        case 0x3432564e: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_NV24");                 break;
        case 0x3234564e: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_NV42");                 break;
        case 0x30313250: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_P210");                 break;
        case 0x30313050: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_P010");                 break;
        case 0x32313050: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_P012");                 break;
        case 0x36313050: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_P016");                 break;
        case 0x30314241: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_AXBXGXRX106106106106"); break;
        case 0x3531564e: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_NV15");                 break;
        case 0x30313451: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_Q410");                 break;
        case 0x31303451: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_Q401");                 break;
        case 0x38345258: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_XRGB16161616");         break;
        case 0x38344258: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_XBGR16161616");         break;
        case 0x38345241: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_ARGB16161616");         break;
        case 0x38344241: dst->value_str = CuiStringLiteral("WL_SHM_FORMAT_ABGR16161616");         break;

        default:
        {
            cui_string_builder_print(&string_builder, CuiStringLiteral("%u"), format);
            dst->value_str = cui_string_builder_to_string(&string_builder, &app.message_arena);
        } break;
    }

    cui_end_temporary_memory(temp_memory);
}

static void
drm_format_format(Argument *dst, Argument *src, CuiString label)
{
    CuiAssert(src->type == ARGUMENT_TYPE_INTEGER);

    dst->type = src->type;
    dst->interface_name = src->interface_name;
    dst->label = label;
    dst->value = src->value;

    CuiTemporaryMemory temp_memory = cui_begin_temporary_memory(&app.temporary_memory);

    CuiStringBuilder string_builder;
    cui_string_builder_init(&string_builder, &app.temporary_memory);

    uint32_t drm_format = (uint32_t) dst->value.i;

    switch (drm_format)
    {
        case 0x00000000: dst->value_str = CuiStringLiteral("DRM_FORMAT_INVALID");              break;
        case 0x20203143: dst->value_str = CuiStringLiteral("DRM_FORMAT_C1");                   break;
        case 0x20203243: dst->value_str = CuiStringLiteral("DRM_FORMAT_C2");                   break;
        case 0x20203443: dst->value_str = CuiStringLiteral("DRM_FORMAT_C4");                   break;
        case 0x20203843: dst->value_str = CuiStringLiteral("DRM_FORMAT_C8");                   break;
        case 0x20203144: dst->value_str = CuiStringLiteral("DRM_FORMAT_D1");                   break;
        case 0x20203244: dst->value_str = CuiStringLiteral("DRM_FORMAT_D2");                   break;
        case 0x20203444: dst->value_str = CuiStringLiteral("DRM_FORMAT_D4");                   break;
        case 0x20203844: dst->value_str = CuiStringLiteral("DRM_FORMAT_D8");                   break;
        case 0x20203152: dst->value_str = CuiStringLiteral("DRM_FORMAT_R1");                   break;
        case 0x20203252: dst->value_str = CuiStringLiteral("DRM_FORMAT_R2");                   break;
        case 0x20203452: dst->value_str = CuiStringLiteral("DRM_FORMAT_R4");                   break;
        case 0x20203852: dst->value_str = CuiStringLiteral("DRM_FORMAT_R8");                   break;
        case 0x20303152: dst->value_str = CuiStringLiteral("DRM_FORMAT_R10");                  break;
        case 0x20323152: dst->value_str = CuiStringLiteral("DRM_FORMAT_R12");                  break;
        case 0x20363152: dst->value_str = CuiStringLiteral("DRM_FORMAT_R16");                  break;
        case 0x38384752: dst->value_str = CuiStringLiteral("DRM_FORMAT_RG88");                 break;
        case 0x38385247: dst->value_str = CuiStringLiteral("DRM_FORMAT_GR88");                 break;
        case 0x32334752: dst->value_str = CuiStringLiteral("DRM_FORMAT_RG1616");               break;
        case 0x32335247: dst->value_str = CuiStringLiteral("DRM_FORMAT_GR1616");               break;
        case 0x38424752: dst->value_str = CuiStringLiteral("DRM_FORMAT_RGB332");               break;
        case 0x38524742: dst->value_str = CuiStringLiteral("DRM_FORMAT_BGR233");               break;
        case 0x32315258: dst->value_str = CuiStringLiteral("DRM_FORMAT_XRGB4444");             break;
        case 0x32314258: dst->value_str = CuiStringLiteral("DRM_FORMAT_XBGR4444");             break;
        case 0x32315852: dst->value_str = CuiStringLiteral("DRM_FORMAT_RGBX4444");             break;
        case 0x32315842: dst->value_str = CuiStringLiteral("DRM_FORMAT_BGRX4444");             break;
        case 0x32315241: dst->value_str = CuiStringLiteral("DRM_FORMAT_ARGB4444");             break;
        case 0x32314241: dst->value_str = CuiStringLiteral("DRM_FORMAT_ABGR4444");             break;
        case 0x32314152: dst->value_str = CuiStringLiteral("DRM_FORMAT_RGBA4444");             break;
        case 0x32314142: dst->value_str = CuiStringLiteral("DRM_FORMAT_BGRA4444");             break;
        case 0x35315258: dst->value_str = CuiStringLiteral("DRM_FORMAT_XRGB1555");             break;
        case 0x35314258: dst->value_str = CuiStringLiteral("DRM_FORMAT_XBGR1555");             break;
        case 0x35315852: dst->value_str = CuiStringLiteral("DRM_FORMAT_RGBX5551");             break;
        case 0x35315842: dst->value_str = CuiStringLiteral("DRM_FORMAT_BGRX5551");             break;
        case 0x35315241: dst->value_str = CuiStringLiteral("DRM_FORMAT_ARGB1555");             break;
        case 0x35314241: dst->value_str = CuiStringLiteral("DRM_FORMAT_ABGR1555");             break;
        case 0x35314152: dst->value_str = CuiStringLiteral("DRM_FORMAT_RGBA5551");             break;
        case 0x35314142: dst->value_str = CuiStringLiteral("DRM_FORMAT_BGRA5551");             break;
        case 0x36314752: dst->value_str = CuiStringLiteral("DRM_FORMAT_RGB565");               break;
        case 0x36314742: dst->value_str = CuiStringLiteral("DRM_FORMAT_BGR565");               break;
        case 0x34324752: dst->value_str = CuiStringLiteral("DRM_FORMAT_RGB888");               break;
        case 0x34324742: dst->value_str = CuiStringLiteral("DRM_FORMAT_BGR888");               break;
        case 0x34325258: dst->value_str = CuiStringLiteral("DRM_FORMAT_XRGB8888");             break;
        case 0x34324258: dst->value_str = CuiStringLiteral("DRM_FORMAT_XBGR8888");             break;
        case 0x34325852: dst->value_str = CuiStringLiteral("DRM_FORMAT_RGBX8888");             break;
        case 0x34325842: dst->value_str = CuiStringLiteral("DRM_FORMAT_BGRX8888");             break;
        case 0x34325241: dst->value_str = CuiStringLiteral("DRM_FORMAT_ARGB8888");             break;
        case 0x34324241: dst->value_str = CuiStringLiteral("DRM_FORMAT_ABGR8888");             break;
        case 0x34324152: dst->value_str = CuiStringLiteral("DRM_FORMAT_RGBA8888");             break;
        case 0x34324142: dst->value_str = CuiStringLiteral("DRM_FORMAT_BGRA8888");             break;
        case 0x30335258: dst->value_str = CuiStringLiteral("DRM_FORMAT_XRGB2101010");          break;
        case 0x30334258: dst->value_str = CuiStringLiteral("DRM_FORMAT_XBGR2101010");          break;
        case 0x30335852: dst->value_str = CuiStringLiteral("DRM_FORMAT_RGBX1010102");          break;
        case 0x30335842: dst->value_str = CuiStringLiteral("DRM_FORMAT_BGRX1010102");          break;
        case 0x30335241: dst->value_str = CuiStringLiteral("DRM_FORMAT_ARGB2101010");          break;
        case 0x30334241: dst->value_str = CuiStringLiteral("DRM_FORMAT_ABGR2101010");          break;
        case 0x30334152: dst->value_str = CuiStringLiteral("DRM_FORMAT_RGBA1010102");          break;
        case 0x30334142: dst->value_str = CuiStringLiteral("DRM_FORMAT_BGRA1010102");          break;
        case 0x38345258: dst->value_str = CuiStringLiteral("DRM_FORMAT_XRGB16161616");         break;
        case 0x38344258: dst->value_str = CuiStringLiteral("DRM_FORMAT_XBGR16161616");         break;
        case 0x38345241: dst->value_str = CuiStringLiteral("DRM_FORMAT_ARGB16161616");         break;
        case 0x38344241: dst->value_str = CuiStringLiteral("DRM_FORMAT_ABGR16161616");         break;
        case 0x48345258: dst->value_str = CuiStringLiteral("DRM_FORMAT_XRGB16161616F");        break;
        case 0x48344258: dst->value_str = CuiStringLiteral("DRM_FORMAT_XBGR16161616F");        break;
        case 0x48345241: dst->value_str = CuiStringLiteral("DRM_FORMAT_ARGB16161616F");        break;
        case 0x48344241: dst->value_str = CuiStringLiteral("DRM_FORMAT_ABGR16161616F");        break;
        case 0x30314241: dst->value_str = CuiStringLiteral("DRM_FORMAT_AXBXGXRX106106106106"); break;
        case 0x56595559: dst->value_str = CuiStringLiteral("DRM_FORMAT_YUYV");                 break;
        case 0x55595659: dst->value_str = CuiStringLiteral("DRM_FORMAT_YVYU");                 break;
        case 0x59565955: dst->value_str = CuiStringLiteral("DRM_FORMAT_UYVY");                 break;
        case 0x59555956: dst->value_str = CuiStringLiteral("DRM_FORMAT_VYUY");                 break;
        case 0x56555941: dst->value_str = CuiStringLiteral("DRM_FORMAT_AYUV");                 break;
        case 0x59555641: dst->value_str = CuiStringLiteral("DRM_FORMAT_AVUY8888");             break;
        case 0x56555958: dst->value_str = CuiStringLiteral("DRM_FORMAT_XYUV8888");             break;
        case 0x59555658: dst->value_str = CuiStringLiteral("DRM_FORMAT_XVUY8888");             break;
        case 0x34325556: dst->value_str = CuiStringLiteral("DRM_FORMAT_VUY888");               break;
        case 0x30335556: dst->value_str = CuiStringLiteral("DRM_FORMAT_VUY101010");            break;
        case 0x30313259: dst->value_str = CuiStringLiteral("DRM_FORMAT_Y210");                 break;
        case 0x32313259: dst->value_str = CuiStringLiteral("DRM_FORMAT_Y212");                 break;
        case 0x36313259: dst->value_str = CuiStringLiteral("DRM_FORMAT_Y216");                 break;
        case 0x30313459: dst->value_str = CuiStringLiteral("DRM_FORMAT_Y410");                 break;
        case 0x32313459: dst->value_str = CuiStringLiteral("DRM_FORMAT_Y412");                 break;
        case 0x36313459: dst->value_str = CuiStringLiteral("DRM_FORMAT_Y416");                 break;
        case 0x30335658: dst->value_str = CuiStringLiteral("DRM_FORMAT_XVYU2101010");          break;
        case 0x36335658: dst->value_str = CuiStringLiteral("DRM_FORMAT_XVYU12_16161616");      break;
        case 0x38345658: dst->value_str = CuiStringLiteral("DRM_FORMAT_XVYU16161616");         break;
        case 0x304C3059: dst->value_str = CuiStringLiteral("DRM_FORMAT_Y0L0");                 break;
        case 0x304C3058: dst->value_str = CuiStringLiteral("DRM_FORMAT_X0L0");                 break;
        case 0x324C3059: dst->value_str = CuiStringLiteral("DRM_FORMAT_Y0L2");                 break;
        case 0x324C3058: dst->value_str = CuiStringLiteral("DRM_FORMAT_X0L2");                 break;
        case 0x38305559: dst->value_str = CuiStringLiteral("DRM_FORMAT_YUV420_8BIT");          break;
        case 0x30315559: dst->value_str = CuiStringLiteral("DRM_FORMAT_YUV420_10BIT");         break;
        case 0x38415258: dst->value_str = CuiStringLiteral("DRM_FORMAT_XRGB8888_A8");          break;
        case 0x38414258: dst->value_str = CuiStringLiteral("DRM_FORMAT_XBGR8888_A8");          break;
        case 0x38415852: dst->value_str = CuiStringLiteral("DRM_FORMAT_RGBX8888_A8");          break;
        case 0x38415842: dst->value_str = CuiStringLiteral("DRM_FORMAT_BGRX8888_A8");          break;
        case 0x38413852: dst->value_str = CuiStringLiteral("DRM_FORMAT_RGB888_A8");            break;
        case 0x38413842: dst->value_str = CuiStringLiteral("DRM_FORMAT_BGR888_A8");            break;
        case 0x38413552: dst->value_str = CuiStringLiteral("DRM_FORMAT_RGB565_A8");            break;
        case 0x38413542: dst->value_str = CuiStringLiteral("DRM_FORMAT_BGR565_A8");            break;
        case 0x3231564E: dst->value_str = CuiStringLiteral("DRM_FORMAT_NV12");                 break;
        case 0x3132564E: dst->value_str = CuiStringLiteral("DRM_FORMAT_NV21");                 break;
        case 0x3631564E: dst->value_str = CuiStringLiteral("DRM_FORMAT_NV16");                 break;
        case 0x3136564E: dst->value_str = CuiStringLiteral("DRM_FORMAT_NV61");                 break;
        case 0x3432564E: dst->value_str = CuiStringLiteral("DRM_FORMAT_NV24");                 break;
        case 0x3234564E: dst->value_str = CuiStringLiteral("DRM_FORMAT_NV42");                 break;
        case 0x3531564E: dst->value_str = CuiStringLiteral("DRM_FORMAT_NV15");                 break;
        case 0x3032564E: dst->value_str = CuiStringLiteral("DRM_FORMAT_NV20");                 break;
        case 0x3033564E: dst->value_str = CuiStringLiteral("DRM_FORMAT_NV30");                 break;
        case 0x30313250: dst->value_str = CuiStringLiteral("DRM_FORMAT_P210");                 break;
        case 0x30313050: dst->value_str = CuiStringLiteral("DRM_FORMAT_P010");                 break;
        case 0x32313050: dst->value_str = CuiStringLiteral("DRM_FORMAT_P012");                 break;
        case 0x36313050: dst->value_str = CuiStringLiteral("DRM_FORMAT_P016");                 break;
        case 0x30333050: dst->value_str = CuiStringLiteral("DRM_FORMAT_P030");                 break;
        case 0x30313451: dst->value_str = CuiStringLiteral("DRM_FORMAT_Q410");                 break;
        case 0x31303451: dst->value_str = CuiStringLiteral("DRM_FORMAT_Q401");                 break;
        case 0x39565559: dst->value_str = CuiStringLiteral("DRM_FORMAT_YUV410");               break;
        case 0x39555659: dst->value_str = CuiStringLiteral("DRM_FORMAT_YVU410");               break;
        case 0x31315559: dst->value_str = CuiStringLiteral("DRM_FORMAT_YUV411");               break;
        case 0x31315659: dst->value_str = CuiStringLiteral("DRM_FORMAT_YVU411");               break;
        case 0x32315559: dst->value_str = CuiStringLiteral("DRM_FORMAT_YUV420");               break;
        case 0x32315659: dst->value_str = CuiStringLiteral("DRM_FORMAT_YVU420");               break;
        case 0x36315559: dst->value_str = CuiStringLiteral("DRM_FORMAT_YUV422");               break;
        case 0x36315659: dst->value_str = CuiStringLiteral("DRM_FORMAT_YVU422");               break;
        case 0x34325559: dst->value_str = CuiStringLiteral("DRM_FORMAT_YUV444");               break;
        case 0x34325659: dst->value_str = CuiStringLiteral("DRM_FORMAT_YVU444");               break;

        default:
        {
            cui_string_builder_print(&string_builder, CuiStringLiteral("0x%08X"), drm_format);
            dst->value_str = cui_string_builder_to_string(&string_builder, &app.message_arena);
        } break;
    }

    cui_end_temporary_memory(temp_memory);
}

static bool
drm_format_modifier_format(Argument *dst, Argument *src, CuiString label, CuiString label0, CuiString label1)
{
    Argument *dst0 = dst;
    Argument *dst1 = dst + 1;

    Argument *src0 = src;
    Argument *src1 = src + 1;

    CuiAssert((src0->type == ARGUMENT_TYPE_INTEGER) && (src1->type == ARGUMENT_TYPE_INTEGER));

    CuiTemporaryMemory temp_memory = cui_begin_temporary_memory(&app.temporary_memory);

    CuiStringBuilder string_builder;
    cui_string_builder_init(&string_builder, &app.temporary_memory);

    bool found_modifier = true;
    uint64_t drm_format_modifier = ((uint64_t) src0->value.i << 32) | (uint64_t) src1->value.i;

    CuiString value_str = cui_make_string(0, 0);

    switch (drm_format_modifier)
    {
        case 0x00FFFFFFFFFFFFFF: value_str = CuiStringLiteral("DRM_FORMAT_MOD_INVALID");                          break;
        case 0x0000000000000000: value_str = CuiStringLiteral("DRM_FORMAT_MOD_LINEAR");                           break;
        case 0x0100000000000001: value_str = CuiStringLiteral("I915_FORMAT_MOD_X_TILED");                         break;
        case 0x0100000000000002: value_str = CuiStringLiteral("I915_FORMAT_MOD_Y_TILED");                         break;
        case 0x0100000000000003: value_str = CuiStringLiteral("I915_FORMAT_MOD_Yf_TILED");                        break;
        case 0x0100000000000004: value_str = CuiStringLiteral("I915_FORMAT_MOD_Y_TILED_CCS");                     break;
        case 0x0100000000000005: value_str = CuiStringLiteral("I915_FORMAT_MOD_Yf_TILED_CCS");                    break;
        case 0x0100000000000006: value_str = CuiStringLiteral("I915_FORMAT_MOD_Y_TILED_GEN12_RC_CCS");            break;
        case 0x0100000000000007: value_str = CuiStringLiteral("I915_FORMAT_MOD_Y_TILED_GEN12_MC_CCS");            break;
        case 0x0100000000000008: value_str = CuiStringLiteral("I915_FORMAT_MOD_Y_TILED_GEN12_RC_CCS_CC");         break;
        case 0x0100000000000009: value_str = CuiStringLiteral("I915_FORMAT_MOD_4_TILED");                         break;
        case 0x010000000000000A: value_str = CuiStringLiteral("I915_FORMAT_MOD_4_TILED_DG2_RC_CCS");              break;
        case 0x010000000000000B: value_str = CuiStringLiteral("I915_FORMAT_MOD_4_TILED_DG2_MC_CCS");              break;
        case 0x010000000000000C: value_str = CuiStringLiteral("I915_FORMAT_MOD_4_TILED_DG2_RC_CCS_CC");           break;
        case 0x010000000000000D: value_str = CuiStringLiteral("I915_FORMAT_MOD_4_TILED_MTL_RC_CCS");              break;
        case 0x010000000000000E: value_str = CuiStringLiteral("I915_FORMAT_MOD_4_TILED_MTL_MC_CCS");              break;
        case 0x010000000000000F: value_str = CuiStringLiteral("I915_FORMAT_MOD_4_TILED_MTL_RC_CCS_CC");           break;
        case 0x0400000000000001: value_str = CuiStringLiteral("DRM_FORMAT_MOD_SAMSUNG_64_32_TILE");               break;
        case 0x0400000000000002: value_str = CuiStringLiteral("DRM_FORMAT_MOD_SAMSUNG_16_16_TILE");               break;
        case 0x0500000000000001: value_str = CuiStringLiteral("DRM_FORMAT_MOD_QCOM_COMPRESSED");                  break;
        case 0x0500000000000003: value_str = CuiStringLiteral("DRM_FORMAT_MOD_QCOM_TILED3");                      break;
        case 0x0500000000000002: value_str = CuiStringLiteral("DRM_FORMAT_MOD_QCOM_TILED2");                      break;
        case 0x0500000000000004: value_str = CuiStringLiteral("DRM_FORMAT_MOD_QCOM_TIGHT");                       break;
        case 0x0500000000000008: value_str = CuiStringLiteral("DRM_FORMAT_MOD_QCOM_TILE");                        break;
        case 0x0500000000000010: value_str = CuiStringLiteral("DRM_FORMAT_MOD_QTI_SECURE");                       break;
        case 0x0600000000000001: value_str = CuiStringLiteral("DRM_FORMAT_MOD_VIVANTE_TILED");                    break;
        case 0x0600000000000002: value_str = CuiStringLiteral("DRM_FORMAT_MOD_VIVANTE_SUPER_TILED");              break;
        case 0x0600000000000003: value_str = CuiStringLiteral("DRM_FORMAT_MOD_VIVANTE_SPLIT_TILED");              break;
        case 0x0600000000000004: value_str = CuiStringLiteral("DRM_FORMAT_MOD_VIVANTE_SPLIT_SUPER_TILED");        break;
        case 0x0300000000000001: value_str = CuiStringLiteral("DRM_FORMAT_MOD_NVIDIA_TEGRA_TILED");               break;
        case 0x0300000000000010: value_str = CuiStringLiteral("DRM_FORMAT_MOD_NVIDIA_16BX2_BLOCK_ONE_GOB");       break;
        case 0x0300000000000011: value_str = CuiStringLiteral("DRM_FORMAT_MOD_NVIDIA_16BX2_BLOCK_TWO_GOB");       break;
        case 0x0300000000000012: value_str = CuiStringLiteral("DRM_FORMAT_MOD_NVIDIA_16BX2_BLOCK_FOUR_GOB");      break;
        case 0x0300000000000013: value_str = CuiStringLiteral("DRM_FORMAT_MOD_NVIDIA_16BX2_BLOCK_EIGHT_GOB");     break;
        case 0x0300000000000014: value_str = CuiStringLiteral("DRM_FORMAT_MOD_NVIDIA_16BX2_BLOCK_SIXTEEN_GOB");   break;
        case 0x0300000000000015: value_str = CuiStringLiteral("DRM_FORMAT_MOD_NVIDIA_16BX2_BLOCK_THIRTYTWO_GOB"); break;
        case 0x0700000000000001: value_str = CuiStringLiteral("DRM_FORMAT_MOD_BROADCOM_VC4_T_TILED");             break;
        case 0x0700000000000002: value_str = CuiStringLiteral("DRM_FORMAT_MOD_BROADCOM_SAND32");                  break;
        case 0x0700000000000003: value_str = CuiStringLiteral("DRM_FORMAT_MOD_BROADCOM_SAND64");                  break;
        case 0x0700000000000004: value_str = CuiStringLiteral("DRM_FORMAT_MOD_BROADCOM_SAND128");                 break;
        case 0x0700000000000005: value_str = CuiStringLiteral("DRM_FORMAT_MOD_BROADCOM_SAND256");                 break;
        case 0x0700000000000006: value_str = CuiStringLiteral("DRM_FORMAT_MOD_BROADCOM_UIF");                     break;
        case 0x0810000000000001: value_str = CuiStringLiteral("DRM_FORMAT_MOD_ARM_16X16_BLOCK_U_INTERLEAVED");    break;
        case 0x0900000000000001: value_str = CuiStringLiteral("DRM_FORMAT_MOD_ALLWINNER_TILED");                  break;

        default:
        {
            found_modifier = false;

            dst0->type = src0->type;
            dst0->interface_name = src0->interface_name;
            dst0->label = label0;
            dst0->value = src0->value;

            cui_string_builder_print(&string_builder, CuiStringLiteral("0x%08X"), (uint32_t) dst0->value.i);
            dst0->value_str = cui_string_builder_to_string(&string_builder, &app.message_arena);

            dst1->type = src1->type;
            dst1->interface_name = src1->interface_name;
            dst1->label = label1;
            dst1->value = src1->value;

            cui_string_builder_init(&string_builder, &app.temporary_memory);
            cui_string_builder_print(&string_builder, CuiStringLiteral("0x%08X"), (uint32_t) dst1->value.i);
            dst1->value_str = cui_string_builder_to_string(&string_builder, &app.message_arena);
        } break;
    }

    cui_end_temporary_memory(temp_memory);

    if (found_modifier)
    {
        dst0->type = ARGUMENT_TYPE_INTEGER;
        dst0->interface_name = cui_make_string(0, 0);
        dst0->label = label;
        dst0->value.i = 0;
        dst0->value_str = value_str;
    }

    return found_modifier;
}

static void
wayland_buffer_backend_format_format(Argument *dst, Argument *src, CuiString label)
{
    CuiAssert(src->type == ARGUMENT_TYPE_INTEGER);

    dst->type = src->type;
    dst->interface_name = src->interface_name;
    dst->label = label;
    dst->value = src->value;

    CuiTemporaryMemory temp_memory = cui_begin_temporary_memory(&app.temporary_memory);

    CuiStringBuilder string_builder;
    cui_string_builder_init(&string_builder, &app.temporary_memory);

    uint32_t format = (uint32_t) dst->value.i;

    // The wayland_buffer_backend protocol extension is a proprietary
    // wayland extension by qualcomm. There is no public reference for
    // the format ids passed over the messages of this protocol.
    // The pixel formats for these numbers are found by passing different
    // VK_FORMAT_* values to vkCreateSwapchainKHR and having a look at
    // the id getting passed to the create_buffer message in the wayland logs.
    // These are the results:
    //   VK_FORMAT_R5G6B5_UNORM_PACK16      -> 5
    //   VK_FORMAT_R8G8B8_SRGB              -> 7
    //   VK_FORMAT_R8G8B8_UNORM             -> 7
    //   VK_FORMAT_B8G8R8A8_UNORM           -> 8 (VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
    //   VK_FORMAT_R8G8B8A8_UNORM           -> 9 (VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
    //   VK_FORMAT_B8G8R8A8_UNORM           -> 10 (VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
    //   VK_FORMAT_B8G8R8A8_SRGB            -> 10
    //   VK_FORMAT_R8G8B8A8_UNORM           -> 11 (VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
    //   VK_FORMAT_R8G8B8A8_SRGB            -> 11
    //   VK_FORMAT_A2B10G10R10_UNORM_PACK32 -> 13
    //   VK_FORMAT_G8_B8R8_2PLANE_420_UNORM -> 14
    //   VK_FORMAT_B8G8R8G8_422_UNORM       -> 16
    //   VK_FORMAT_B5G6R5_UNORM_PACK16      -> 17

    switch (format)
    {
        case 5:  dst->value_str = CuiStringLiteral("5 ~ DRM_FORMAT_RGB565");       break;
        case 7:  dst->value_str = CuiStringLiteral("7 ~ DRM_FORMAT_BGR888");       break;
        case 8:  dst->value_str = CuiStringLiteral("8 ~ DRM_FORMAT_XRGB8888");     break;
        case 9:  dst->value_str = CuiStringLiteral("9 ~ DRM_FORMAT_XBGR8888");     break;
        case 10: dst->value_str = CuiStringLiteral("10 ~ DRM_FORMAT_ARGB8888");    break;
        case 11: dst->value_str = CuiStringLiteral("11 ~ DRM_FORMAT_ABGR8888");    break;
        case 12: dst->value_str = CuiStringLiteral("12 ???"); break;
        case 13: dst->value_str = CuiStringLiteral("13 ~ DRM_FORMAT_ABGR2101010"); break;
        case 14: dst->value_str = CuiStringLiteral("14 ??? (VK_FORMAT_G8_B8R8_2PLANE_420_UNORM)"); break;
        case 16: dst->value_str = CuiStringLiteral("16 ??? (VK_FORMAT_B8G8R8G8_422_UNORM)"); break;
        case 17: dst->value_str = CuiStringLiteral("17 ~ DRM_FORMAT_BGR565");      break;

        default:
        {
            cui_string_builder_print(&string_builder, CuiStringLiteral("%u"), format);
            dst->value_str = cui_string_builder_to_string(&string_builder, &app.message_arena);
        } break;
    }

    cui_end_temporary_memory(temp_memory);
}

static void
format_message(Message *message, uint32_t argument_count, Argument *arguments, MessageSpec *message_spec)
{
    CuiAssert(argument_count == message_spec->argument_count);

    message->argument_count = argument_count;
    message->arguments = cui_alloc_array(&app.message_arena, Argument, argument_count, CuiDefaultAllocationParams());

    for (uint32_t i = 0; i < argument_count; i += 1)
    {
        ArgumentSpec *argument_spec = message_spec->arguments + i;

        if (argument_spec->format_func)
        {
            argument_spec->format_func(message->arguments + i, arguments + i, argument_spec->label);
        }
        else
        {
            format_argument(message->arguments + i, arguments + i, argument_spec->label);
        }
    }
}

static void
format_message_without_spec(Message *message, uint32_t argument_count, Argument *arguments)
{
    message->argument_count = argument_count;
    message->arguments = cui_alloc_array(&app.message_arena, Argument, argument_count, CuiDefaultAllocationParams());

    for (uint32_t i = 0; i < argument_count; i += 1)
    {
        format_argument(message->arguments + i, arguments + i, cui_make_string(0, 0));
    }
}

static void
zwp_linux_dmabuf_v1__modifier__format_func(Message *message, uint32_t argument_count, Argument *arguments, MessageSpec *message_spec)
{
    message->argument_count = argument_count;
    message->arguments = cui_alloc_array(&app.message_arena, Argument, argument_count, CuiDefaultAllocationParams());

    drm_format_format(message->arguments + 0, arguments + 0, message_spec->arguments[0].label);

    if (drm_format_modifier_format(message->arguments + 1, arguments + 1, CuiStringLiteral("modifier"),
                                   CuiStringLiteral("modifier_hi"), CuiStringLiteral("modifier_lo")))
    {
        message->argument_count -= 1;
    }
}

static void
zwp_linux_buffer_params_v1__add__format_func(Message *message, uint32_t argument_count, Argument *arguments, MessageSpec *message_spec)
{
    message->argument_count = argument_count;
    message->arguments = cui_alloc_array(&app.message_arena, Argument, argument_count, CuiDefaultAllocationParams());

    format_argument(message->arguments + 0, arguments + 0, message_spec->arguments[0].label);
    format_argument(message->arguments + 1, arguments + 1, message_spec->arguments[1].label);
    format_argument(message->arguments + 2, arguments + 2, message_spec->arguments[2].label);
    format_argument(message->arguments + 3, arguments + 3, message_spec->arguments[3].label);

    if (drm_format_modifier_format(message->arguments + 4, arguments + 4, CuiStringLiteral("modifier"),
                                   CuiStringLiteral("modifier_hi"), CuiStringLiteral("modifier_lo")))
    {
        message->argument_count -= 1;
    }
}

#include "message_formats.c"

static inline bool
filter_is_empty(void)
{
    return (app.filter.id == 0) && (app.filter.interface_name.count == 0) && (app.filter.message_name.count == 0);
}

static inline int32_t
get_item_count(ListView *list_view)
{
    if (list_view->is_filtered)
    {
        if (list_view->show_non_wayland_messages)
        {
            return app.filtered_message_count;
        }
        else
        {
            return app.filtered_wayland_message_count;
        }
    }
    else
    {
        if (list_view->show_non_wayland_messages)
        {
            return app.message_count;
        }
        else
        {
            return app.wayland_message_count;
        }
    }
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

    int32_t count = get_item_count(list_view);

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

    int32_t space_w = (int32_t) ceilf(cui_window_get_string_width(widget->window, app.list_view_font, CuiStringLiteral(" ")));
    int32_t type_rect_w = (int32_t) ceilf(widget->ui_scale * app.font_size);

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

    uint32_t count = get_item_count(list_view);
    uint32_t index = list_view->scroll_offset.integer_part;
    uint32_t filtered_index = 0;

    bool is_empty = filter_is_empty();

    if (!list_view->is_filtered)
    {
        uint32_t message_index = 0;

        if (list_view->show_non_wayland_messages)
        {
            message_index = index;
        }
        else if (index < app.wayland_message_count)
        {
            message_index = app.wayland_messages[index];
        }

        while ((filtered_index < app.filtered_wayland_message_count) &&
               (app.filtered_wayland_messages[filtered_index].message_index < message_index))
        {
            filtered_index += 1;
        }
    }

    float message_x1 = (float) list_rect.max.x - cui_window_get_string_width(widget->window, app.list_view_font, CuiStringLiteral(")"));

    while (index < count)
    {
        CuiTemporaryMemory temp_memory = cui_begin_temporary_memory(&app.temporary_memory);

        x = list_rect.min.x;

        CuiColor text_color = CuiHexColor(0xFFAFB7C4);
        CuiColor character_color = CuiHexColor(0xFF333846);

        Message *message;

        if (list_view->is_filtered)
        {
            if (list_view->show_non_wayland_messages)
            {
                message = app.messages + app.filtered_messages[index];
            }
            else
            {
                message = app.messages + app.filtered_wayland_messages[index].message_index;
            }
        }
        else
        {
            uint32_t message_index;

            if (list_view->show_non_wayland_messages)
            {
                message_index = index;
            }
            else
            {
                message_index = app.wayland_messages[index];
            }

            message = app.messages + message_index;

            if ((filtered_index < app.filtered_wayland_message_count) &&
                (app.filtered_wayland_messages[filtered_index].message_index == message_index))
            {
                if (!is_empty)
                {
                    cui_draw_fill_rect(ctx, cui_make_rect(list_rect.min.x, y, list_rect.max.x, y + row_height), blue_background);
                    text_color = CuiHexColor(0xFFFFFFFF);
                    character_color = blue_foreground;
                }

                filtered_index += 1;
            }
        }

        if (message->type == MESSAGE_TYPE_WAYLAND)
        {
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

            x += space_w;

            int32_t type_rect_y = y + (row_height - type_rect_w) / 2;
            CuiRect type_rect = cui_make_rect(x, type_rect_y, x + type_rect_w, type_rect_y + type_rect_w);

            if (message->sent)
            {
                cui_draw_fill_rounded_rect_1(ctx, type_rect, (float) list_view->px2, pink_background);
                w = cui_window_get_string_width(widget->window, app.list_view_font, CuiStringLiteral("R"));
                cui_draw_fill_string(ctx, app.list_view_font, (float) x + 0.5f * ((float) type_rect_w - w), (float) y + row_baseline, CuiStringLiteral("R"), pink_foreground);
            }
            else
            {
                cui_draw_fill_rounded_rect_1(ctx, type_rect, (float) list_view->px2, green_background);
                w = cui_window_get_string_width(widget->window, app.list_view_font, CuiStringLiteral("E"));
                cui_draw_fill_string(ctx, app.list_view_font, (float) x + 0.5f * ((float) type_rect_w - w), (float) y + row_baseline, CuiStringLiteral("E"), green_foreground);
            }

            x += type_rect_w + space_w;

            float sub_x = (float) x;

            sub_x += cui_draw_fill_string(ctx, app.list_view_font, sub_x, (float) y + row_baseline, message->message_name, text_color);
            sub_x += (float) list_view->px2;

            sub_x += cui_draw_fill_string(ctx, app.list_view_font, sub_x, (float) y + row_baseline, CuiStringLiteral("("), cui_make_color(1.0f, 1.0f, 1.0f, 1.0f));

            float width_with_labels = 0.0f;

            for (uint32_t i = 0; i < message->argument_count; i += 1)
            {
                Argument *argument = message->arguments + i;

                if (i > 0)
                {
                    width_with_labels += cui_window_get_string_width(widget->window, app.list_view_font, CuiStringLiteral(", "));
                }

                if (argument->label.count)
                {
                    width_with_labels += cui_window_get_string_width(widget->window, app.list_view_font, argument->label);
                    width_with_labels += cui_window_get_string_width(widget->window, app.list_view_font, CuiStringLiteral(": "));
                }

                width_with_labels += cui_window_get_string_width(widget->window, app.list_view_font, argument->value_str);
            }

            bool draw_with_labels = false;
            float remaining_width = message_x1 - sub_x;

            if (width_with_labels <= remaining_width)
            {
                draw_with_labels = true;
            }

            for (uint32_t i = 0; i < message->argument_count; i += 1)
            {
                Argument *argument = message->arguments + i;

                if (i > 0)
                {
                    sub_x += cui_draw_fill_string(ctx, app.list_view_font, sub_x, (float) y + row_baseline, CuiStringLiteral(", "), cui_make_color(1.0f, 1.0f, 1.0f, 1.0f));
                }

                if (draw_with_labels && argument->label.count)
                {
                    sub_x += cui_draw_fill_string(ctx, app.list_view_font, sub_x, (float) y + row_baseline, argument->label, text_color);
                    sub_x += cui_draw_fill_string(ctx, app.list_view_font, sub_x, (float) y + row_baseline, CuiStringLiteral(": "), text_color);
                }

                sub_x += cui_draw_fill_string(ctx, app.list_view_font, sub_x, (float) y + row_baseline, argument->value_str, cui_make_color(1.0f, 1.0f, 1.0f, 1.0f));
            }

            sub_x += cui_draw_fill_string(ctx, app.list_view_font, sub_x, (float) y + row_baseline, CuiStringLiteral(")"), cui_make_color(1.0f, 1.0f, 1.0f, 1.0f));
        }
        else
        {
            CuiAssert(message->type == MESSAGE_TYPE_PLAIN);
            cui_draw_fill_string(ctx, app.list_view_font, (float) (x + list_view->px8), (float) y + row_baseline, message->message_name, character_color);
            // cui_draw_fill_string(ctx, app.list_view_font, (float) (x + list_view->px8), (float) y + row_baseline, message->message_name, color_theme->window_outline);
        }

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
                int32_t count = get_item_count(list_view);

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
graph_view_limit_scroll_offset(GraphView *graph_view)
{
    int32_t bar_width = graph_view->px4 + graph_view->px1;

    int32_t count = app.filtered_wayland_message_count;

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

    for (uint32_t i = graph_view->scroll_offset.integer_part + 1; i < app.filtered_wayland_message_count; i += 1)
    {
        FilterItem *filter_item = app.filtered_wayland_messages + i;

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
                int32_t count = app.filtered_wayland_message_count;

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
    app.filtered_message_count = 0;
    app.filtered_wayland_message_count = 0;

    for (uint32_t message_index = 0; message_index < app.message_count; message_index += 1)
    {
        Message *message = app.messages + message_index;

        if (message->type == MESSAGE_TYPE_WAYLAND)
        {
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

            if (app.filtered_wayland_message_count > 0)
            {
                Message *prev_message = app.messages + app.filtered_wayland_messages[app.filtered_wayland_message_count - 1].message_index;

                CuiAssert(prev_message->type == MESSAGE_TYPE_WAYLAND);

                if (message->timestamp_us > prev_message->timestamp_us)
                {
                    time_delta = (uint32_t) (message->timestamp_us - prev_message->timestamp_us);
                }
            }

            app.filtered_wayland_messages[app.filtered_wayland_message_count].message_index = message_index;
            app.filtered_wayland_messages[app.filtered_wayland_message_count].time_delta = time_delta;
            app.filtered_wayland_message_count += 1;

            app.filtered_messages[app.filtered_message_count] = message_index;
            app.filtered_message_count += 1;
        }
        else
        {
            CuiAssert(message->type == MESSAGE_TYPE_PLAIN);

            app.filtered_messages[app.filtered_message_count] = message_index;
            app.filtered_message_count += 1;
        }
    }
}

static void
scroll_to_next_filtered_item(ListView *list_view)
{
    if (!list_view->is_filtered)
    {
        ScrollOffset scroll_offset = { 0, 0 };

        if (app.filtered_wayland_message_count > 0)
        {
            app.filtered_item_index += 1;

            if (app.filtered_item_index >= app.filtered_wayland_message_count)
            {
                app.filtered_item_index = 0;
            }

            uint32_t message_index = app.filtered_wayland_messages[app.filtered_item_index].message_index;

            if (list_view->show_non_wayland_messages)
            {
                scroll_offset.integer_part = message_index - SCROLL_ROW_OFFSET;
            }
            else
            {
                for (uint32_t i = 0; i < app.wayland_message_count; i += 1)
                {
                    if (app.wayland_messages[i] == message_index)
                    {
                        scroll_offset.integer_part = i - SCROLL_ROW_OFFSET;
                        break;
                    }
                }
            }
        }

        list_view->scroll_offset = scroll_offset;
        list_view_limit_scroll_offset(list_view);
    }
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
            (c == '_') || (c == '[') || (c == ']');
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
parse_integer(CuiString *str, int64_t *result)
{
    int64_t value = 0;
    int64_t index = 0;

    bool is_signed = false;

    if ((index < str->count) && (str->data[index] == '-'))
    {
        is_signed = true;
        index += 1;
    }

    if ((index >= str->count) || !cui_unicode_is_digit(str->data[index]))
    {
        return false;
    }

    while ((index < str->count) && cui_unicode_is_digit(str->data[index]))
    {
        value = (10 * value) + (str->data[index] - '0');
        index += 1;
    }

    if (is_signed)
    {
        value = -value;
    }

    cui_string_advance(str, index);
    *result = value;

    return true;
}

static inline bool
parse_fixed(CuiString *str, int32_t *result)
{
    int32_t a = 0;
    int32_t b = 0;
    int64_t index = 0;

    bool is_signed = false;

    if ((index < str->count) && (str->data[index] == '-'))
    {
        is_signed = true;
        index += 1;
    }

    if ((index >= str->count) || !cui_unicode_is_digit(str->data[index]))
    {
        return false;
    }

    while ((index < str->count) && cui_unicode_is_digit(str->data[index]))
    {
        a = (10 * a) + (str->data[index] - '0');
        index += 1;
    }

    if ((index >= str->count) || (str->data[index] != '.'))
    {
        return false;
    }

    index += 1;

    if ((index >= str->count) || !cui_unicode_is_digit(str->data[index]))
    {
        return false;
    }

    // TODO: parse exactly 8 digits, extend if needed
    while ((index < str->count) && cui_unicode_is_digit(str->data[index]))
    {
        b = (10 * b) + (str->data[index] - '0');
        index += 1;
    }

    if (is_signed)
    {
        *result = -(a * 256) - (b / 390625);
    }
    else
    {
        *result = (a * 256) + (b / 390625);
    }

    cui_string_advance(str, index);

    return true;
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

    app.list_view.is_filtered = app.filter_checkbox.value ? true : false;

    app.filtered_item_index = app.filtered_wayland_message_count;
    scroll_to_next_filtered_item(&app.list_view);
    list_view_limit_scroll_offset(&app.list_view);

    cui_window_request_redraw(window);
}

static void
on_non_wayland_messages_action(CuiWidget *widget)
{
    CuiAssert(widget->window);
    CuiWindow *window = widget->window;

    uint32_t message_index = 0;

    CuiAssert(app.list_view.scroll_offset.integer_part >= 0);
    uint32_t scroll_index = (uint32_t) app.list_view.scroll_offset.integer_part + SCROLL_ROW_OFFSET;

    if (app.list_view.is_filtered)
    {
        if (app.list_view.show_non_wayland_messages)
        {
            if (scroll_index < app.filtered_message_count)
            {
                message_index = app.filtered_messages[scroll_index];
            }
        }
        else
        {
            if (scroll_index < app.filtered_wayland_message_count)
            {
                message_index = app.filtered_wayland_messages[scroll_index].message_index;
            }
        }
    }
    else
    {
        if (app.list_view.show_non_wayland_messages)
        {
            if (scroll_index < app.message_count)
            {
                message_index = scroll_index;
            }
        }
        else
        {
            if (scroll_index < app.wayland_message_count)
            {
                message_index = app.wayland_messages[scroll_index];
            }
        }
    }

    app.list_view.show_non_wayland_messages = app.non_wayland_messages_checkbox.value ? true : false;

    if (app.list_view.is_filtered)
    {
        if (app.list_view.show_non_wayland_messages)
        {
            for (uint32_t i = 0; i < app.filtered_message_count; i += 1)
            {
                if (app.filtered_messages[i] >= message_index)
                {
                    app.list_view.scroll_offset.integer_part = (int32_t) i - SCROLL_ROW_OFFSET;
                    break;
                }
            }
        }
        else
        {
            for (uint32_t i = 0; i < app.filtered_wayland_message_count; i += 1)
            {
                if (app.filtered_wayland_messages[i].message_index >= message_index)
                {
                    app.list_view.scroll_offset.integer_part = (int32_t) i - SCROLL_ROW_OFFSET;
                    break;
                }
            }
        }
    }
    else
    {
        if (app.list_view.show_non_wayland_messages)
        {
            app.list_view.scroll_offset.integer_part = (int32_t) message_index - SCROLL_ROW_OFFSET;
        }
        else
        {
            for (uint32_t i = 0; i < app.wayland_message_count; i += 1)
            {
                if (app.wayland_messages[i] >= message_index)
                {
                    app.list_view.scroll_offset.integer_part = (int32_t) i - SCROLL_ROW_OFFSET;
                    break;
                }
            }
        }
    }

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

    if (app.file_loaded)
    {
        apply_filter();

        app.filtered_item_index = app.filtered_wayland_message_count;
        scroll_to_next_filtered_item(&app.list_view);
        list_view_limit_scroll_offset(&app.list_view);
        graph_view_limit_scroll_offset(&app.graph_view);
    }
}

static void
on_input_changed(CuiWidget *widget)
{
    CuiAssert(widget->window);
    CuiWindow *window = widget->window;

    scroll_to_next_filtered_item(&app.list_view);
    cui_window_request_redraw(window);
}

static bool
parse_argument(CuiString *str, Argument *argument)
{
    if (str->count == 0)
    {
        return false;
    }

    if (cui_unicode_is_digit(str->data[0]) || (str->data[0] == '-'))
    {
        int32_t fixed_value;
        int64_t integer_value;

        if (parse_fixed(str, &fixed_value))
        {
            argument->type = ARGUMENT_TYPE_FIXED;
            argument->value.f = fixed_value;
        }
        else if (parse_integer(str, &integer_value))
        {
            // TODO: check if the values are in range
            argument->type = ARGUMENT_TYPE_INTEGER;
            argument->value.i = integer_value;
        }
        else
        {
            return false;
        }
    }
    else if (str->data[0] == '"')
    {
        int64_t index = 1;
        int64_t start_index = index;

        while ((index < str->count) && (str->data[index] != '"'))
        {
            index += 1;
        }

        int64_t end_index = index;
        CuiString value = cui_make_string(str->data + start_index, end_index - start_index);

        if ((index < str->count) && (str->data[index] == '"'))
        {
            index += 1;
            cui_string_advance(str, index);

            argument->type = ARGUMENT_TYPE_STRING;
            argument->value.str = value;
        }
        else
        {
            return false;
        }
    }
    else if (cui_string_starts_with(*str, CuiStringLiteral("new id ")))
    {
        cui_string_advance(str, CuiStringLiteral("new id ").count);

        CuiString interf = parse_identifier(str);

        if (!cui_string_starts_with(*str, CuiStringLiteral("#")) &&
            !cui_string_starts_with(*str, CuiStringLiteral("@")))
        {
            return false;
        }

        cui_string_advance(str, 1);

        int64_t new_id;

        if (!parse_integer(str, &new_id) || (new_id <= 0) || (new_id > 0xFFFFFFFF))
        {
            return false;
        }

        argument->type = ARGUMENT_TYPE_NEW_ID;
        argument->interface_name = interf;
        argument->value.id = (uint32_t) new_id;
    }
    else if (cui_string_starts_with(*str, CuiStringLiteral("array")))
    {
        cui_string_advance(str, CuiStringLiteral("array").count);

        if (!cui_string_starts_with(*str, CuiStringLiteral("[")))
        {
            return false;
        }

        cui_string_advance(str, 1);

        int64_t count;

        if (!parse_integer(str, &count) || (count < 0) || (count > 0xFFFFFFFF))
        {
            return false;
        }

        if (!cui_string_starts_with(*str, CuiStringLiteral("]")))
        {
            return false;
        }

        cui_string_advance(str, 1);

        argument->type = ARGUMENT_TYPE_ARRAY;
        argument->value.count = (uint32_t) count;
    }
    else if (cui_string_starts_with(*str, CuiStringLiteral("fd ")))
    {
        cui_string_advance(str, CuiStringLiteral("fd ").count);

        int64_t fd;

        if (!parse_integer(str, &fd) || (fd < (int64_t) 0xFFFFFFFF80000000) || (fd > 0x7FFFFFFF))
        {
            return false;
        }

        argument->type = ARGUMENT_TYPE_FD;
        argument->value.fd = (int32_t) fd;
    }
    else if (cui_string_starts_with(*str, CuiStringLiteral("nil")))
    {
        cui_string_advance(str, CuiStringLiteral("nil").count);

        argument->type = ARGUMENT_TYPE_NIL;
    }
    else if (is_identifier_character(str->data[0]))
    {
        CuiString interf = parse_identifier(str);

        if (!cui_string_starts_with(*str, CuiStringLiteral("#")) &&
            !cui_string_starts_with(*str, CuiStringLiteral("@")))
        {
            return false;
        }

        cui_string_advance(str, 1);

        int64_t id;

        if (!parse_integer(str, &id) || (id <= 0) || (id > 0xFFFFFFFF))
        {
            return false;
        }

        argument->type = ARGUMENT_TYPE_OBJECT;
        argument->interface_name = interf;
        argument->value.id = (uint32_t) id;
    }
    else
    {
        return false;
    }

    return true;
}

static inline bool
parse_wayland_message(Message *message, CuiString str, uint32_t *at_count, uint32_t *hash_count)
{
    str = cui_string_trim(str);

    int64_t index = 0;

    while ((index < str.count) && (str.data[index] != '['))
    {
        index += 1;
    }

    cui_string_advance(&str, index);

    if ((str.count < 1) || (str.data[0] != '[') || (str.data[str.count - 1] != ')'))
    {
        return false;
    }

    CuiString connection_name = { 0 };
    CuiString queue_name = { 0 };

    if (!str.count || (str.data[0] != '['))
    {
        return false;
    }

    cui_string_advance(&str, 1);
    skip_spaces(&str);

    // TODO: use custom version to handle failure case
    int32_t timestamp_ms = cui_string_parse_int32_advance(&str);

    if (!str.count || (str.data[0] != '.'))
    {
        return false;
    }

    cui_string_advance(&str, 1);

    // TODO: use custom version to handle failure case
    int32_t timestamp_us = cui_string_parse_int32_advance(&str);

    if (!str.count || (str.data[0] != ']'))
    {
        return false;
    }

    cui_string_advance(&str, 1);
    skip_spaces(&str);

    if ((str.count > 0) && (str.data[0] == '{'))
    {
        if (!parse_in_between(&str, &queue_name, '{', '}'))
        {
            return false;
        }
    }

    skip_spaces(&str);

    if (cui_string_starts_with(str, CuiStringLiteral("discarded ")))
    {
        cui_string_advance(&str, CuiStringLiteral("discarded ").count);
        skip_spaces(&str);
    }

    bool sent = false;

    if (cui_string_starts_with(str, CuiStringLiteral("->")))
    {
        sent = true;
        cui_string_advance(&str, CuiStringLiteral("->").count);
        skip_spaces(&str);
    }

    CuiString interface_name = parse_identifier(&str);

    if (cui_string_starts_with(str, CuiStringLiteral("#")))
    {
        *hash_count += 1;
        cui_string_advance(&str, 1);
    }
    else if (cui_string_starts_with(str, CuiStringLiteral("@")))
    {
        *at_count += 1;
        cui_string_advance(&str, 1);
    }
    else
    {
        return false;
    }

    // TODO: use custom version to handle failure case
    int32_t id = cui_string_parse_int32_advance(&str);

    if (cui_string_starts_with(str, CuiStringLiteral(".")))
    {
        cui_string_advance(&str, 1);
    }
    else
    {
        return false;
    }

    uint64_t timestamp = ((uint64_t) timestamp_ms * 1000) + (uint64_t) timestamp_us;

    CuiString message_name = parse_identifier(&str);

    if (cui_string_starts_with(str, CuiStringLiteral("(")))
    {
        cui_string_advance(&str, 1);
    }
    else
    {
        return false;
    }

    uint32_t argument_count = 0;
    Argument arguments[16];

    while ((str.count > 0) && (str.data[0] != ')'))
    {
        if (argument_count >= CuiArrayCount(arguments))
        {
            fprintf(stderr, "error: message has more than %zu arguments.\n", CuiArrayCount(arguments));
            return false;
        }

        skip_spaces(&str);

        if (!parse_argument(&str, arguments + argument_count))
        {
            return false;
        }

        argument_count += 1;

        if (cui_string_starts_with(str, CuiStringLiteral(",")))
        {
            cui_string_advance(&str, 1);
        }
        else
        {
            break;
        }
    }

    if (cui_string_starts_with(str, CuiStringLiteral(")")))
    {
        cui_string_advance(&str, 1);
    }
    else
    {
        return false;
    }

    message->type = MESSAGE_TYPE_WAYLAND;
    message->connection_name = connection_name;
    message->queue_name = queue_name;
    message->interface_name = interface_name;
    message->id = id;
    message->sent = sent;
    message->message_name = message_name;
    message->timestamp_us = timestamp;

    MessageSpec *message_spec = 0;

    for (size_t k = 0; k < CuiArrayCount(message_specs); k += 1)
    {
        MessageSpec *msg_spec = message_specs + k;

        if (cui_string_equals(message->interface_name, msg_spec->interface_name) &&
            cui_string_equals(message->message_name, msg_spec->message_name))
        {
            message_spec = msg_spec;
            break;
        }
    }

    if (message_spec && message_matches_signature(argument_count, arguments, message_spec))
    {
        if (message_spec->message_format_func)
        {
            message_spec->message_format_func(message, argument_count, arguments, message_spec);
        }
        else
        {
            format_message(message, argument_count, arguments, message_spec);
        }
    }
    else
    {
        format_message_without_spec(message, argument_count, arguments);
    }

    return true;
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

        if (app.message_arena.capacity)
        {
            cui_arena_deallocate(&app.message_arena);
        }

        if (app.message_allocated)
        {
            cui_platform_deallocate(app.messages, app.message_allocated * sizeof(*app.messages));
            app.message_allocated = 0;
            app.messages = 0;
        }

        if (app.wayland_message_allocated)
        {
            cui_platform_deallocate(app.wayland_messages, app.wayland_message_allocated * sizeof(*app.wayland_messages));
            app.wayland_message_allocated = 0;
            app.wayland_messages = 0;
        }

        if (app.filtered_message_allocated)
        {
            cui_platform_deallocate(app.filtered_messages, app.filtered_message_allocated * sizeof(*app.filtered_messages));
            app.filtered_message_allocated = 0;
            app.filtered_messages = 0;
        }

        if (app.filtered_wayland_message_allocated)
        {
            cui_platform_deallocate(app.filtered_wayland_messages, app.filtered_wayland_message_allocated * sizeof(*app.filtered_wayland_messages));
            app.filtered_wayland_message_allocated = 0;
            app.filtered_wayland_messages = 0;
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

        cui_arena_allocate(&app.message_arena, line_count * 512);

        app.message_allocated = line_count;
        app.message_count = 0;
        app.messages = (Message *) cui_platform_allocate(app.message_allocated * sizeof(*app.messages));

        app.wayland_message_allocated = line_count;
        app.wayland_message_count = 0;
        app.wayland_messages = (uint32_t *) cui_platform_allocate(app.wayland_message_allocated * sizeof(*app.wayland_messages));

        app.filtered_message_allocated = line_count;
        app.filtered_message_count = 0;
        app.filtered_messages = (uint32_t *) cui_platform_allocate(app.filtered_message_allocated * sizeof(*app.filtered_messages));

        app.filtered_wayland_message_allocated = line_count;
        app.filtered_wayland_message_count = 0;
        app.filtered_wayland_messages = (FilterItem *) cui_platform_allocate(app.filtered_wayland_message_allocated * sizeof(*app.filtered_wayland_messages));

        uint32_t at_count = 0;
        uint32_t hash_count = 0;

        CuiString cursor = app.file_content;

        while (cursor.count > 0)
        {
            CuiString str = cui_string_get_next_line(&cursor);

            uint32_t message_index = app.message_count;
            Message *message = app.messages + message_index;
            app.message_count += 1;

            if (parse_wayland_message(message, str, &at_count, &hash_count))
            {
                app.wayland_messages[app.wayland_message_count] = message_index;
                app.wayland_message_count += 1;
            }
            else
            {
                message->type = MESSAGE_TYPE_PLAIN;
                message->message_name = str;
            }
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

        apply_filter();
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

    cui_widget_init(&app.non_wayland_messages_checkbox, CUI_WIDGET_TYPE_CHECKBOX);
    cui_widget_set_label(&app.non_wayland_messages_checkbox, CuiStringLiteral("Show non-wayland messages"));
    cui_widget_set_padding(&app.non_wayland_messages_checkbox, 0.0f, 4.0f, 0.0f, 12.0f);
    cui_widget_set_inline_padding(&app.non_wayland_messages_checkbox, 8.0f);
    cui_widget_set_font(&app.non_wayland_messages_checkbox, app.list_view_font);

    app.non_wayland_messages_checkbox.on_action = on_non_wayland_messages_action;

    cui_widget_append_child(top_container, &app.non_wayland_messages_checkbox);

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
    app.filter_input.on_changed = on_input_changed;

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
