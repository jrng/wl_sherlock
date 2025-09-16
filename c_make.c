// C_MAKE_COMPILER_FLAGS = "-std=c99 -Wall -Wextra -pedantic"
#define C_MAKE_IMPLEMENTATION
#include "src/c_make.h"

static void
build(const char *output_path, CMakeArchitecture target_architecture)
{
    const char *cui_root_path = "";
    CMakeConfigValue cui_root_path_value = c_make_config_get("cui_root_path");

    if (cui_root_path_value.is_valid)
    {
        cui_root_path = cui_root_path_value.val;
    }
    else
    {
#if C_MAKE_PLATFORM_WINDOWS
        cui_root_path = "C:\\cui";
#else
        const char *HOME = getenv("HOME");

        if (HOME)
        {
            cui_root_path = c_make_c_string_path_concat(HOME, "opt", "cui");
        }
#endif
    }

    CMakeCommand command = { 0 };

    const char *target_c_compiler = c_make_get_target_c_compiler();

    c_make_command_append(&command, target_c_compiler);
    c_make_command_append_command_line(&command, c_make_get_target_c_flags());
    c_make_command_append_default_compiler_flags(&command, c_make_get_build_type());

    if (c_make_compiler_is_msvc(target_c_compiler))
    {
        c_make_command_append(&command, "-MT", "-Gm-", "-GR-", "-EHa-", "-Oi", "-GS-", "-Gs10000000");
    }
    else
    {
        c_make_command_append(&command, "-std=c99");
    }

    switch (c_make_get_build_type())
    {
        case CMakeBuildTypeDebug:
        {
            if (!c_make_compiler_is_msvc(target_c_compiler))
            {
                c_make_command_append(&command, "-Wall", "-Wextra", "-pedantic");
            }

            c_make_command_append(&command, "-DCUI_DEBUG_BUILD=1");
        } break;

        case CMakeBuildTypeRelDebug:
        {
            c_make_command_append(&command, "-DCUI_DEBUG_BUILD=1");
        } break;

        case CMakeBuildTypeRelease:
        {
        } break;
    }

    if (c_make_get_target_platform() == CMakePlatformWindows)
    {
        c_make_command_append(&command, c_make_c_string_concat("-I", cui_root_path));
    }
    else
    {
        c_make_command_append(&command, c_make_c_string_concat("-I", c_make_c_string_path_concat(cui_root_path, "include")));
    }

    if (c_make_get_target_platform() == CMakePlatformMacOs)
    {
        if (target_architecture == CMakeArchitectureAmd64)
        {
            c_make_command_append(&command, "-target", "x86_64-apple-macos10.14");
        }
        else if (target_architecture == CMakeArchitectureAarch64)
        {
            c_make_command_append(&command, "-target", "arm64-apple-macos11");
        }
    }

    c_make_command_append_output_executable(&command, output_path, c_make_get_target_platform());
    c_make_command_append(&command, c_make_c_string_path_concat(c_make_get_source_path(), "src", "wl_sherlock.c"));

    if (c_make_get_target_platform() == CMakePlatformWindows)
    {
        c_make_command_append(&command, c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.res"));
    }

    c_make_command_append_default_linker_flags(&command, target_architecture);

    switch (c_make_get_target_platform())
    {
        case CMakePlatformAndroid: break;
        case CMakePlatformFreeBsd: break;

        case CMakePlatformWindows:
        {
            c_make_command_append(&command, "-stack:0x100000,0x100000", "-subsystem:windows,6.0", "-opt:ref", "-incremental:no");
            c_make_command_append(&command, c_make_c_string_concat("-libpath:\"", cui_root_path, "\""));
            c_make_command_append(&command, "user32.lib", "gdi32.lib", "shell32.lib", "uxtheme.lib", "comdlg32.lib");
            c_make_command_append(&command, "d3d11.lib", "dxguid.lib", "d3dcompiler.lib");
            c_make_command_append(&command, "cui.lib");
        } break;

        case CMakePlatformLinux:
        {
            c_make_command_append(&command, c_make_c_string_concat("-L", c_make_c_string_path_concat(cui_root_path, "lib64")));
            c_make_command_append(&command, "-lcui");
            c_make_command_append(&command, "-lm", "-pthread");
            c_make_command_append(&command, "-lX11", "-lXext", "-lXrandr");
            c_make_command_append(&command, "-lwayland-client", "-lwayland-cursor", "-lxkbcommon", "-lwayland-egl");
            c_make_command_append(&command, "-lEGL", "-lGLESv2");
        } break;

        case CMakePlatformMacOs:
        {
            c_make_command_append(&command, c_make_c_string_concat("-L", c_make_c_string_path_concat(cui_root_path, "lib64")));
            c_make_command_append(&command, "-lcui");
            c_make_command_append(&command, "-framework", "AppKit");
            c_make_command_append(&command, "-framework", "Quartz");
            c_make_command_append(&command, "-framework", "Metal");
        } break;

        case CMakePlatformWeb: break;
    }

    c_make_log(CMakeLogLevelInfo, "compile 'wl_sherlock'\n");
    c_make_command_run(command);
}

C_MAKE_ENTRY()
{
    switch (c_make_target)
    {
        case CMakeTargetSetup:
        {
            CMakeConfigValue cui_root_path_value = c_make_config_get("cui_root_path");

            if (!cui_root_path_value.is_valid)
            {
#if C_MAKE_PLATFORM_WINDOWS
                c_make_config_set("cui_root_path", "C:\\cui");
#else
                const char *HOME = getenv("HOME");

                if (HOME)
                {
                    c_make_config_set("cui_root_path", c_make_c_string_path_concat(HOME, "opt", "cui"));
                }
#endif
            }
        } break;

        case CMakeTargetBuild:
        {
            if (c_make_get_target_platform() == CMakePlatformWindows)
            {
                const char *resource_compiler = c_make_get_executable("windows_rc_executable", "rc.exe");

                CMakeCommand command = { 0 };

                c_make_command_append(&command, resource_compiler, "-nologo", "-fo",
                                      c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.res"),
                                      c_make_c_string_path_concat(c_make_get_source_path(), "src", "wl_sherlock.rc"));

                c_make_log(CMakeLogLevelInfo, "generate 'wl_sherlock.res'\n");
                c_make_command_run_and_reset(&command);

                c_make_process_wait_for_all();
            }

            if (c_make_get_target_platform() == CMakePlatformMacOs)
            {
                c_make_create_directory_recursively(c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.app", "Contents", "MacOS"));
                c_make_create_directory_recursively(c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.app", "Contents", "Resources"));

                if (c_make_get_host_architecture() == CMakeArchitectureAarch64)
                {
                    build(c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock-arm64"), CMakeArchitectureAarch64);
                    build(c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock-x86_64"), CMakeArchitectureAmd64);

                    c_make_process_wait_for_all();

                    CMakeCommand command = { 0 };

                    c_make_command_append(&command, "lipo", "-create", "-output", 
                                          c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.app", "Contents", "MacOS", "wl_sherlock"),
                                          c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock-arm64"),
                                          c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock-x86_64"));

                    c_make_log(CMakeLogLevelInfo, "generate 'wl_sherlock'\n");
                    c_make_command_run_and_reset(&command);
                }
                else
                {
                    c_make_log(CMakeLogLevelWarning, "This machine can't compile for aarch64.\n");
                    build(c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.app", "Contents", "MacOS", "wl_sherlock"), c_make_get_target_architecture());
                }

                c_make_copy_file(c_make_c_string_path_concat(c_make_get_source_path(), "src", "wl_sherlock.Info.plist"),
                                 c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.app", "Contents", "Info.plist"));

                c_make_create_directory(c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.iconset"));

                c_make_copy_file(c_make_c_string_path_concat(c_make_get_source_path(), "icon", "icon_512x512@2.png"),
                                 c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.iconset", "icon_512x512@2.png"));
                c_make_copy_file(c_make_c_string_path_concat(c_make_get_source_path(), "icon", "icon_256x256@2.png"),
                                 c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.iconset", "icon_256x256@2.png"));
                c_make_copy_file(c_make_c_string_path_concat(c_make_get_source_path(), "icon", "icon_128x128@2.png"),
                                 c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.iconset", "icon_128x128@2.png"));
                c_make_copy_file(c_make_c_string_path_concat(c_make_get_source_path(), "icon", "icon_32x32@2.png"),
                                 c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.iconset", "icon_32x32@2.png"));
                c_make_copy_file(c_make_c_string_path_concat(c_make_get_source_path(), "icon", "icon_16x16@2.png"),
                                 c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.iconset", "icon_16x16@2.png"));
                c_make_copy_file(c_make_c_string_path_concat(c_make_get_source_path(), "icon", "icon_512x512.png"),
                                 c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.iconset", "icon_512x512.png"));
                c_make_copy_file(c_make_c_string_path_concat(c_make_get_source_path(), "icon", "icon_256x256.png"),
                                 c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.iconset", "icon_256x256.png"));
                c_make_copy_file(c_make_c_string_path_concat(c_make_get_source_path(), "icon", "icon_128x128.png"),
                                 c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.iconset", "icon_128x128.png"));
                c_make_copy_file(c_make_c_string_path_concat(c_make_get_source_path(), "icon", "icon_32x32.png"),
                                 c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.iconset", "icon_32x32.png"));
                c_make_copy_file(c_make_c_string_path_concat(c_make_get_source_path(), "icon", "icon_16x16.png"),
                                 c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.iconset", "icon_16x16.png"));

                CMakeCommand command = { 0 };

                c_make_command_append(&command, "actool",
                                      "--compile", c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.app", "Contents", "Resources"),
                                      "--app-icon", "wl_sherlock", "--minimum-deployment-target", "26.0", "--platform", "macosx",
                                      "--output-partial-info-plist", c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.partial.plist"),
                                      c_make_c_string_path_concat(c_make_get_source_path(), "icon", "wl_sherlock.icon"));

                c_make_log(CMakeLogLevelInfo, "generate 'Assets.car'\n");
                c_make_command_run_and_reset(&command);

                c_make_process_wait_for_all();

                c_make_command_append(&command, "iconutil", "-c", "icns", "-o",
                                      c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.app", "Contents", "Resources", "wl_sherlock.icns"),
                                      c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock.iconset"));

                c_make_log(CMakeLogLevelInfo, "generate 'wl_sherlock.icns'\n");
                c_make_command_run_and_reset(&command);
            }
            else
            {
                build(c_make_c_string_path_concat(c_make_get_build_path(), "wl_sherlock"), c_make_get_target_architecture());
            }
        } break;

        case CMakeTargetInstall:
        {
        } break;
    }
}
