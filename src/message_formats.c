static CuiString wl_display__error__arguments[] = {
    CuiStringConstant("object_id"),
    CuiStringConstant("code"),
    CuiStringConstant("message"),
};

static MessageSpec wl_display__error__spec = {
    .argument_count = CuiArrayCount(wl_display__error__arguments),
    .arguments      = wl_display__error__arguments,
};

static CuiString wl_registry__global__arguments[] = {
    CuiStringConstant("name"),
    CuiStringConstant("interface"),
    CuiStringConstant("version"),
};

static MessageSpec wl_registry__global__spec = {
    .argument_count = CuiArrayCount(wl_registry__global__arguments),
    .arguments      = wl_registry__global__arguments,
};

static CuiString wl_shm__format__arguments[] = {
    { 0, 0 },
};

static MessageSpec wl_shm__format__spec = {
    .argument_count = CuiArrayCount(wl_shm__format__arguments),
    .arguments      = wl_shm__format__arguments,
};

static CuiString wl_shm_pool__create_buffer__arguments[] = {
    { 0, 0 },
    CuiStringConstant("offset"),
    CuiStringConstant("width"),
    CuiStringConstant("height"),
    CuiStringConstant("stride"),
    CuiStringConstant("format"),
};

static MessageSpec wl_shm_pool__create_buffer__spec = {
    .argument_count = CuiArrayCount(wl_shm_pool__create_buffer__arguments),
    .arguments      = wl_shm_pool__create_buffer__arguments,
};

static CuiString zwp_linux_dmabuf_v1__format__arguments[] = {
    { 0, 0 },
};

static MessageSpec zwp_linux_dmabuf_v1__format__spec = {
    .argument_count = CuiArrayCount(zwp_linux_dmabuf_v1__format__arguments),
    .arguments      = zwp_linux_dmabuf_v1__format__arguments,
};

static CuiString zwp_linux_dmabuf_v1__modifier__arguments[] = {
    CuiStringConstant("format"),
    CuiStringConstant("modifier_hi"),
    CuiStringConstant("modifier_lo"),
};

static MessageSpec zwp_linux_dmabuf_v1__modifier__spec = {
    .argument_count = CuiArrayCount(zwp_linux_dmabuf_v1__modifier__arguments),
    .arguments      = zwp_linux_dmabuf_v1__modifier__arguments,
};

static CuiString zwp_linux_buffer_params_v1__add__arguments[] = {
    { 0, 0 },
    CuiStringConstant("plane_idx"),
    CuiStringConstant("offset"),
    CuiStringConstant("stride"),
    CuiStringConstant("modifier_hi"),
    CuiStringConstant("modifier_lo"),
};

static MessageSpec zwp_linux_buffer_params_v1__add__spec = {
    .argument_count = CuiArrayCount(zwp_linux_buffer_params_v1__add__arguments),
    .arguments      = zwp_linux_buffer_params_v1__add__arguments,
};

static CuiString zwp_linux_buffer_params_v1__create__arguments[] = {
    CuiStringConstant("width"),
    CuiStringConstant("height"),
    CuiStringConstant("format"),
    CuiStringConstant("flags"),
};

static MessageSpec zwp_linux_buffer_params_v1__create__spec = {
    .argument_count = CuiArrayCount(zwp_linux_buffer_params_v1__create__arguments),
    .arguments      = zwp_linux_buffer_params_v1__create__arguments,
};

static CuiString zwp_linux_buffer_params_v1__create_immed__arguments[] = {
    { 0, 0 },
    CuiStringConstant("width"),
    CuiStringConstant("height"),
    CuiStringConstant("format"),
    CuiStringConstant("flags"),
};

static MessageSpec zwp_linux_buffer_params_v1__create_immed__spec = {
    .argument_count = CuiArrayCount(zwp_linux_buffer_params_v1__create_immed__arguments),
    .arguments      = zwp_linux_buffer_params_v1__create_immed__arguments,
};

static CuiString wayland_buffer_backend__format__arguments[] = {
    { 0, 0 },
};

static MessageSpec wayland_buffer_backend__format__spec = {
    .argument_count = CuiArrayCount(wayland_buffer_backend__format__arguments),
    .arguments      = wayland_buffer_backend__format__arguments,
};

static CuiString wayland_buffer_backend__create_buffer__arguments[] = {
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    CuiStringConstant("width"),
    CuiStringConstant("height"),
    CuiStringConstant("format"),
    CuiStringConstant("stride"),
};

static MessageSpec wayland_buffer_backend__create_buffer__spec = {
    .argument_count = CuiArrayCount(wayland_buffer_backend__create_buffer__arguments),
    .arguments      = wayland_buffer_backend__create_buffer__arguments,
};

// This only lists labels for messages that have more than 1
// argument as non or single argument messages are pretty self-explanatory.

static MessageFormatRoutine message_format_routines[] = {
    {
        .interface_name = CuiStringConstant("wl_display"),
        .message_name   = CuiStringConstant("error"),
        .func           = default_message_format_func,
        .data           = &wl_display__error__spec,
    },
    {
        .interface_name = CuiStringConstant("wl_registry"),
        .message_name   = CuiStringConstant("global"),
        .func           = default_message_format_func,
        .data           = &wl_registry__global__spec,
    },
    {
        .interface_name = CuiStringConstant("wl_shm"),
        .message_name   = CuiStringConstant("format"),
        .func           = wl_shm__format__format_func,
        .data           = &wl_shm__format__spec,
    },
    {
        .interface_name = CuiStringConstant("wl_shm_pool"),
        .message_name   = CuiStringConstant("create_buffer"),
        .func           = wl_shm_pool__create_buffer__format_func,
        .data           = &wl_shm_pool__create_buffer__spec,
    },
    {
        .interface_name = CuiStringConstant("zwp_linux_dmabuf_v1"),
        .message_name   = CuiStringConstant("format"),
        .func           = zwp_linux_dmabuf_v1__format__format_func,
        .data           = &zwp_linux_dmabuf_v1__format__spec,
    },
    {
        .interface_name = CuiStringConstant("zwp_linux_dmabuf_v1"),
        .message_name   = CuiStringConstant("modifier"),
        .func           = zwp_linux_dmabuf_v1__modifier__format_func,
        .data           = &zwp_linux_dmabuf_v1__modifier__spec,
    },
    {
        .interface_name = CuiStringConstant("zwp_linux_buffer_params_v1"),
        .message_name   = CuiStringConstant("add"),
        .func           = zwp_linux_buffer_params_v1__add__format_func,
        .data           = &zwp_linux_buffer_params_v1__add__spec,
    },
    {
        .interface_name = CuiStringConstant("zwp_linux_buffer_params_v1"),
        .message_name   = CuiStringConstant("create"),
        .func           = zwp_linux_buffer_params_v1__create__format_func,
        .data           = &zwp_linux_buffer_params_v1__create__spec,
    },
    {
        .interface_name = CuiStringConstant("zwp_linux_buffer_params_v1"),
        .message_name   = CuiStringConstant("create_immed"),
        .func           = zwp_linux_buffer_params_v1__create_immed__format_func,
        .data           = &zwp_linux_buffer_params_v1__create_immed__spec,
    },
    {
        .interface_name = CuiStringConstant("wayland_buffer_backend"),
        .message_name   = CuiStringConstant("format"),
        .func           = wayland_buffer_backend__format__format_func,
        .data           = &wayland_buffer_backend__format__spec,
    },
    {
        .interface_name = CuiStringConstant("wayland_buffer_backend"),
        .message_name   = CuiStringConstant("create_buffer"),
        .func           = wayland_buffer_backend__create_buffer__format_func,
        .data           = &wayland_buffer_backend__create_buffer__spec,
    },
};
