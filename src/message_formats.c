static ArgumentSpec wl_display__error__arguments[] = {
    { .type = ARGUMENT_TYPE_OBJECT , .label = CuiStringConstant("object_id"), .format_func = 0 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("code")     , .format_func = 0 },
    { .type = ARGUMENT_TYPE_STRING , .label = CuiStringConstant("message")  , .format_func = 0 },
};

static ArgumentSpec wl_registry__global__arguments[] = {
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("name")     , .format_func = 0 },
    { .type = ARGUMENT_TYPE_STRING , .label = CuiStringConstant("interface"), .format_func = 0 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("version")  , .format_func = 0 },
};

static ArgumentSpec wl_shm__format__arguments[] = {
    { .type = ARGUMENT_TYPE_INTEGER, .label = { 0, 0 }, .format_func = wl_shm_format_format },
};

static ArgumentSpec wl_shm_pool__create_buffer__arguments[] = {
    { .type = ARGUMENT_TYPE_NEW_ID , .label = { 0, 0 }                   , .format_func = 0                    },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("offset"), .format_func = 0                    },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("width") , .format_func = 0                    },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("height"), .format_func = 0                    },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("stride"), .format_func = 0                    },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("format"), .format_func = wl_shm_format_format },
};

static ArgumentSpec wl_drm__format__arguments[] = {
    { .type = ARGUMENT_TYPE_INTEGER, .label = { 0, 0 }, .format_func = drm_format_format },
};

static ArgumentSpec wl_drm__create_buffer__arguments[] = {
    { .type = ARGUMENT_TYPE_NEW_ID , .label = { 0, 0 }                   , .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("name")  , .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("width") , .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("height"), .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("stride"), .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("format"), .format_func = drm_format_format },
};

static ArgumentSpec wl_drm__create_planar_buffer__arguments[] = {
    { .type = ARGUMENT_TYPE_NEW_ID , .label = { 0, 0 }                    , .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("name")   , .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("width")  , .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("height") , .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("format") , .format_func = drm_format_format },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("offset0"), .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("stride0"), .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("offset1"), .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("stride1"), .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("offset2"), .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("stride2"), .format_func = 0                 },
};

static ArgumentSpec wl_drm__create_prime_buffer__arguments[] = {
    { .type = ARGUMENT_TYPE_NEW_ID , .label = { 0, 0 }                    , .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("name")   , .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("width")  , .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("height") , .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("format") , .format_func = drm_format_format },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("offset0"), .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("stride0"), .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("offset1"), .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("stride1"), .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("offset2"), .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("stride2"), .format_func = 0                 },
};

static ArgumentSpec zwp_linux_dmabuf_v1__format__arguments[] = {
    { .type = ARGUMENT_TYPE_INTEGER, .label = { 0, 0 }, .format_func = drm_format_format },
};

static ArgumentSpec zwp_linux_dmabuf_v1__modifier__arguments[] = {
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("format")     , .format_func = 0 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("modifier_hi"), .format_func = 0 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("modifier_lo"), .format_func = 0 },
};

static ArgumentSpec zwp_linux_buffer_params_v1__add__arguments[] = {
    { .type = ARGUMENT_TYPE_FD     , .label = { 0, 0 }                        , .format_func = 0 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("plane_idx")  , .format_func = 0 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("offset")     , .format_func = 0 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("stride")     , .format_func = 0 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("modifier_hi"), .format_func = 0 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("modifier_lo"), .format_func = 0 },
};

static ArgumentSpec zwp_linux_buffer_params_v1__create__arguments[] = {
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("width") , .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("height"), .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("format"), .format_func = drm_format_format },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("flags") , .format_func = 0                 },
};

static ArgumentSpec zwp_linux_buffer_params_v1__create_immed__arguments[] = {
    { .type = ARGUMENT_TYPE_NEW_ID , .label = { 0, 0 }                   , .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("width") , .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("height"), .format_func = 0                 },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("format"), .format_func = drm_format_format },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("flags") , .format_func = 0                 },
};

static ArgumentSpec wayland_buffer_backend__format__arguments[] = {
    { .type = ARGUMENT_TYPE_INTEGER, .label = { 0, 0 }, .format_func = wayland_buffer_backend_format_format },
};

static ArgumentSpec wayland_buffer_backend__create_buffer__arguments[] = {
    { .type = ARGUMENT_TYPE_NEW_ID , .label = { 0, 0 }                   , .format_func = 0                                    },
    { .type = ARGUMENT_TYPE_FD     , .label = { 0, 0 }                   , .format_func = 0                                    },
    { .type = ARGUMENT_TYPE_FD     , .label = { 0, 0 }                   , .format_func = 0                                    },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("width") , .format_func = 0                                    },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("height"), .format_func = 0                                    },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("format"), .format_func = wayland_buffer_backend_format_format },
    { .type = ARGUMENT_TYPE_INTEGER, .label = CuiStringConstant("stride"), .format_func = 0                                    },
};

// This only lists labels for messages that have more than 1
// argument as non or single argument messages are pretty self-explanatory.

static MessageSpec message_specs[] = {
    {
        .interface_name      = CuiStringConstant("wl_display"),
        .message_name        = CuiStringConstant("error"),
        .message_format_func = 0,
        .argument_count      = CuiArrayCount(wl_display__error__arguments),
        .arguments           = wl_display__error__arguments,
    },
    {
        .interface_name      = CuiStringConstant("wl_registry"),
        .message_name        = CuiStringConstant("global"),
        .message_format_func = 0,
        .argument_count      = CuiArrayCount(wl_registry__global__arguments),
        .arguments           = wl_registry__global__arguments,
    },
    {
        .interface_name      = CuiStringConstant("wl_shm"),
        .message_name        = CuiStringConstant("format"),
        .message_format_func = 0,
        .argument_count      = CuiArrayCount(wl_shm__format__arguments),
        .arguments           = wl_shm__format__arguments,
    },
    {
        .interface_name      = CuiStringConstant("wl_shm_pool"),
        .message_name        = CuiStringConstant("create_buffer"),
        .message_format_func = 0,
        .argument_count      = CuiArrayCount(wl_shm_pool__create_buffer__arguments),
        .arguments           = wl_shm_pool__create_buffer__arguments,
    },
    {
        .interface_name      = CuiStringConstant("wl_drm"),
        .message_name        = CuiStringConstant("format"),
        .message_format_func = 0,
        .argument_count      = CuiArrayCount(wl_drm__format__arguments),
        .arguments           = wl_drm__format__arguments,
    },
    {
        .interface_name      = CuiStringConstant("wl_drm"),
        .message_name        = CuiStringConstant("create_buffer"),
        .message_format_func = 0,
        .argument_count      = CuiArrayCount(wl_drm__create_buffer__arguments),
        .arguments           = wl_drm__create_buffer__arguments,
    },
    {
        .interface_name      = CuiStringConstant("wl_drm"),
        .message_name        = CuiStringConstant("create_planar_buffer"),
        .message_format_func = 0,
        .argument_count      = CuiArrayCount(wl_drm__create_planar_buffer__arguments),
        .arguments           = wl_drm__create_planar_buffer__arguments,
    },
    {
        .interface_name      = CuiStringConstant("wl_drm"),
        .message_name        = CuiStringConstant("create_prime_buffer"),
        .message_format_func = 0,
        .argument_count      = CuiArrayCount(wl_drm__create_prime_buffer__arguments),
        .arguments           = wl_drm__create_prime_buffer__arguments,
    },
    {
        .interface_name      = CuiStringConstant("zwp_linux_dmabuf_v1"),
        .message_name        = CuiStringConstant("format"),
        .message_format_func = 0,
        .argument_count      = CuiArrayCount(zwp_linux_dmabuf_v1__format__arguments),
        .arguments           = zwp_linux_dmabuf_v1__format__arguments,
    },
    {
        .interface_name      = CuiStringConstant("zwp_linux_dmabuf_v1"),
        .message_name        = CuiStringConstant("modifier"),
        .message_format_func = zwp_linux_dmabuf_v1__modifier__format_func,
        .argument_count      = CuiArrayCount(zwp_linux_dmabuf_v1__modifier__arguments),
        .arguments           = zwp_linux_dmabuf_v1__modifier__arguments,
    },
    {
        .interface_name      = CuiStringConstant("zwp_linux_buffer_params_v1"),
        .message_name        = CuiStringConstant("add"),
        .message_format_func = zwp_linux_buffer_params_v1__add__format_func,
        .argument_count      = CuiArrayCount(zwp_linux_buffer_params_v1__add__arguments),
        .arguments           = zwp_linux_buffer_params_v1__add__arguments,
    },
    {
        .interface_name      = CuiStringConstant("zwp_linux_buffer_params_v1"),
        .message_name        = CuiStringConstant("create"),
        .message_format_func = 0,
        .argument_count      = CuiArrayCount(zwp_linux_buffer_params_v1__create__arguments),
        .arguments           = zwp_linux_buffer_params_v1__create__arguments,
    },
    {
        .interface_name      = CuiStringConstant("zwp_linux_buffer_params_v1"),
        .message_name        = CuiStringConstant("create_immed"),
        .message_format_func = 0,
        .argument_count      = CuiArrayCount(zwp_linux_buffer_params_v1__create_immed__arguments),
        .arguments           = zwp_linux_buffer_params_v1__create_immed__arguments,
    },
    {
        .interface_name      = CuiStringConstant("wayland_buffer_backend"),
        .message_name        = CuiStringConstant("format"),
        .message_format_func = 0,
        .argument_count      = CuiArrayCount(wayland_buffer_backend__format__arguments),
        .arguments           = wayland_buffer_backend__format__arguments,
    },
    {
        .interface_name      = CuiStringConstant("wayland_buffer_backend"),
        .message_name        = CuiStringConstant("create_buffer"),
        .message_format_func = 0,
        .argument_count      = CuiArrayCount(wayland_buffer_backend__create_buffer__arguments),
        .arguments           = wayland_buffer_backend__create_buffer__arguments,
    },
};
