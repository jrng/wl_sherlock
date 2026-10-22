// C_MAKE_COMPILER_FLAGS = "-std=c99 -Wall -Wextra -pedantic"
#define C_MAKE_IMPLEMENTATION
#include "src/c_make.h"

static void
build(const char *output_path, Architecture target_architecture)
{
    const char *cui_root_path = "";
    ConfigValue cui_root_path_value = config_get("cui_root_path");

    if (cui_root_path_value.is_valid)
    {
        cui_root_path = cui_root_path_value.val;
    }
    else
    {
#if PLATFORM_WINDOWS
        cui_root_path = "C:\\cui";
#else
        const char *HOME = getenv("HOME");

        if (HOME)
        {
            cui_root_path = c_string_path_concat(HOME, "opt", "cui");
        }
#endif
    }

    Command command = { 0 };

    const char *target_c_compiler = get_target_c_compiler();

    command_append(&command, target_c_compiler);
    command_append_command_line(&command, get_target_c_flags());
    command_append_default_compiler_flags(&command, get_build_type());

    if (compiler_is_msvc(target_c_compiler))
    {
        command_append(&command, "-MT", "-Gm-", "-GR-", "-EHa-", "-Oi", "-GS-", "-Gs10000000");
    }
    else
    {
        command_append(&command, "-std=c99");
    }

    switch (get_build_type())
    {
        case BuildTypeDebug:
        {
            if (!compiler_is_msvc(target_c_compiler))
            {
                command_append(&command, "-Wall", "-Wextra", "-pedantic");
            }

            command_append(&command, "-DCUI_DEBUG_BUILD=1");
        } break;

        case BuildTypeRelDebug:
        {
            command_append(&command, "-DCUI_DEBUG_BUILD=1");
        } break;

        case BuildTypeRelease:
        {
        } break;
    }

    if (get_target_platform() == PlatformWindows)
    {
        command_append(&command, c_string_concat("-I", cui_root_path));
    }
    else
    {
        command_append(&command, c_string_concat("-I", c_string_path_concat(cui_root_path, "include")));
    }

    if (get_target_platform() == PlatformMacOs)
    {
        if (target_architecture == ArchitectureAmd64)
        {
            command_append(&command, "-target", "x86_64-apple-macos10.14");
        }
        else if (target_architecture == ArchitectureAarch64)
        {
            command_append(&command, "-target", "arm64-apple-macos11");
        }
    }

    command_append_output_executable(&command, output_path, get_target_platform());
    command_append(&command, c_string_path_concat(get_source_path(), "src", "wl_sherlock.c"));

    if (get_target_platform() == PlatformWindows)
    {
        command_append(&command, c_string_path_concat(get_build_path(), "wl_sherlock.res"));
    }

    command_append_default_linker_flags(&command, target_architecture);

    switch (get_target_platform())
    {
        case PlatformAndroid: break;
        case PlatformFreeBsd: break;

        case PlatformWindows:
        {
            command_append(&command, "-stack:0x100000,0x100000", "-subsystem:windows,6.0", "-opt:ref", "-incremental:no");
            command_append(&command, c_string_concat("-libpath:\"", cui_root_path, "\""));
            command_append(&command, "user32.lib", "gdi32.lib", "shell32.lib", "uxtheme.lib", "comdlg32.lib");
            command_append(&command, "d3d11.lib", "dxguid.lib", "d3dcompiler.lib");
            command_append(&command, "cui.lib");
        } break;

        case PlatformLinux:
        {
            command_append(&command, c_string_concat("-L", c_string_path_concat(cui_root_path, "lib64")));
            command_append(&command, "-lcui");
            command_append(&command, "-lm", "-pthread");
            command_append(&command, "-lX11", "-lXext", "-lXrandr");
            command_append(&command, "-lwayland-client", "-lwayland-cursor", "-lxkbcommon", "-lwayland-egl");
            command_append(&command, "-lEGL", "-lGLESv2");
        } break;

        case PlatformMacOs:
        {
            command_append(&command, c_string_concat("-L", c_string_path_concat(cui_root_path, "lib64")));
            command_append(&command, "-lcui");
            command_append(&command, "-framework", "AppKit");
            command_append(&command, "-framework", "Quartz");
            command_append(&command, "-framework", "Metal");
        } break;

        case PlatformWeb: break;
    }

    c_make_log(LogLevelInfo, "compile 'wl_sherlock'\n");
    command_run(command);
}

C_MAKE_INFO(commands_info, configs_info)
{
    add_default_info(commands_info, configs_info);
}

C_MAKE_ENTRY(command, argument_count, arguments)
{
    if (strings_are_equal(command, COMMAND_SETUP))
    {
        ConfigValue cui_root_path_value = config_get("cui_root_path");

        if (!cui_root_path_value.is_valid)
        {
#if PLATFORM_WINDOWS
            config_set("cui_root_path", "C:\\cui");
#else
            const char *HOME = getenv("HOME");

            if (HOME)
            {
                config_set("cui_root_path", c_string_path_concat(HOME, "opt", "cui"));
            }
#endif
        }
    }
    else if (strings_are_equal(command, COMMAND_BUILD))
    {
        if (get_target_platform() == PlatformWindows)
        {
            const char *resource_compiler = get_executable("windows_rc_executable", "rc.exe");

            Command cmd = { 0 };

            command_append(&cmd, resource_compiler, "-nologo", "-fo",
                           c_string_path_concat(get_build_path(), "wl_sherlock.res"),
                           c_string_path_concat(get_source_path(), "src", "wl_sherlock.rc"));

            c_make_log(LogLevelInfo, "generate 'wl_sherlock.res'\n");
            command_run_and_reset_and_wait(&cmd);
        }

        if (get_target_platform() == PlatformMacOs)
        {
            create_directory_recursively(c_string_path_concat(get_build_path(), "wl_sherlock.app", "Contents", "MacOS"));
            create_directory_recursively(c_string_path_concat(get_build_path(), "wl_sherlock.app", "Contents", "Resources"));

            if (get_host_architecture() == ArchitectureAarch64)
            {
                build(c_string_path_concat(get_build_path(), "wl_sherlock-arm64"), ArchitectureAarch64);
                build(c_string_path_concat(get_build_path(), "wl_sherlock-x86_64"), ArchitectureAmd64);

                process_wait_for_all();

                Command cmd = { 0 };

                command_append(&cmd, "lipo", "-create", "-output",
                               c_string_path_concat(get_build_path(), "wl_sherlock.app", "Contents", "MacOS", "wl_sherlock"),
                               c_string_path_concat(get_build_path(), "wl_sherlock-arm64"),
                               c_string_path_concat(get_build_path(), "wl_sherlock-x86_64"));

                c_make_log(LogLevelInfo, "generate 'wl_sherlock'\n");
                command_run_and_reset(&cmd);
            }
            else
            {
                c_make_log(LogLevelWarning, "This machine can't compile for aarch64.\n");
                build(c_string_path_concat(get_build_path(), "wl_sherlock.app", "Contents", "MacOS", "wl_sherlock"), get_target_architecture());
            }

            copy_file(c_string_path_concat(get_source_path(), "src", "wl_sherlock.Info.plist"),
                      c_string_path_concat(get_build_path(), "wl_sherlock.app", "Contents", "Info.plist"));

            create_directory(c_string_path_concat(get_build_path(), "wl_sherlock.iconset"));

            copy_file(c_string_path_concat(get_source_path(), "icon", "icon_512x512@2.png"),
                      c_string_path_concat(get_build_path(), "wl_sherlock.iconset", "icon_512x512@2.png"));
            copy_file(c_string_path_concat(get_source_path(), "icon", "icon_256x256@2.png"),
                      c_string_path_concat(get_build_path(), "wl_sherlock.iconset", "icon_256x256@2.png"));
            copy_file(c_string_path_concat(get_source_path(), "icon", "icon_128x128@2.png"),
                      c_string_path_concat(get_build_path(), "wl_sherlock.iconset", "icon_128x128@2.png"));
            copy_file(c_string_path_concat(get_source_path(), "icon", "icon_32x32@2.png"),
                      c_string_path_concat(get_build_path(), "wl_sherlock.iconset", "icon_32x32@2.png"));
            copy_file(c_string_path_concat(get_source_path(), "icon", "icon_16x16@2.png"),
                      c_string_path_concat(get_build_path(), "wl_sherlock.iconset", "icon_16x16@2.png"));
            copy_file(c_string_path_concat(get_source_path(), "icon", "icon_512x512.png"),
                      c_string_path_concat(get_build_path(), "wl_sherlock.iconset", "icon_512x512.png"));
            copy_file(c_string_path_concat(get_source_path(), "icon", "icon_256x256.png"),
                      c_string_path_concat(get_build_path(), "wl_sherlock.iconset", "icon_256x256.png"));
            copy_file(c_string_path_concat(get_source_path(), "icon", "icon_128x128.png"),
                      c_string_path_concat(get_build_path(), "wl_sherlock.iconset", "icon_128x128.png"));
            copy_file(c_string_path_concat(get_source_path(), "icon", "icon_32x32.png"),
                      c_string_path_concat(get_build_path(), "wl_sherlock.iconset", "icon_32x32.png"));
            copy_file(c_string_path_concat(get_source_path(), "icon", "icon_16x16.png"),
                      c_string_path_concat(get_build_path(), "wl_sherlock.iconset", "icon_16x16.png"));

            Command cmd = { 0 };

            command_append(&cmd, "actool",
                           "--compile", c_string_path_concat(get_build_path(), "wl_sherlock.app", "Contents", "Resources"),
                           "--output-format", "human-readable-text",
                           "--app-icon", "wl_sherlock", "--minimum-deployment-target", "26.0", "--platform", "macosx",
                           "--output-partial-info-plist", c_string_path_concat(get_build_path(), "wl_sherlock.partial.plist"),
                           c_string_path_concat(get_source_path(), "icon", "wl_sherlock.icon"));

            c_make_log(LogLevelInfo, "generate 'Assets.car'\n");
            command_run_and_reset(&cmd);

            process_wait_for_all();

            command_append(&cmd, "iconutil", "-c", "icns", "-o",
                           c_string_path_concat(get_build_path(), "wl_sherlock.app", "Contents", "Resources", "wl_sherlock.icns"),
                           c_string_path_concat(get_build_path(), "wl_sherlock.iconset"));

            c_make_log(LogLevelInfo, "generate 'wl_sherlock.icns'\n");
            command_run_and_reset(&cmd);
        }
        else
        {
            build(c_string_path_concat(get_build_path(), "wl_sherlock"), get_target_architecture());
        }
    }
    else
    {
        handle_default_commands(command, argument_count, arguments);
    }
}
