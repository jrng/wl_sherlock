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
        .interface_name = CuiStringConstant("zwp_linux_buffer_params_v1"),
        .message_name   = CuiStringConstant("create_immed"),
        .func           = zwp_linux_buffer_params_v1__create_immed__format_func,
        .data           = &zwp_linux_buffer_params_v1__create_immed__spec,
    },
};
