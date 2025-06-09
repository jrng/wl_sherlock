static CuiString wl_display__error__arguments[] = {
    CuiStringConstant("object_id"),
    CuiStringConstant("code"),
    CuiStringConstant("message"),
};

static CuiString wl_registry__global__arguments[] = {
    CuiStringConstant("name"),
    CuiStringConstant("interface"),
    CuiStringConstant("version"),
};

// This only lists labels for messages that have more than 1
// argument as non or single argument messages are pretty self-explanatory.

static MessageSpec message_specs[] = {
    {
        .interface_name = CuiStringConstant("wl_display"),
        .message_name   = CuiStringConstant("error"),
        .argument_count = CuiArrayCount(wl_display__error__arguments),
        .arguments      = wl_display__error__arguments,
    },
    {
        .interface_name = CuiStringConstant("wl_registry"),
        .message_name   = CuiStringConstant("global"),
        .argument_count = CuiArrayCount(wl_registry__global__arguments),
        .arguments      = wl_registry__global__arguments,
    },
};
