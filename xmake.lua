add_rules("mode.debug", "mode.release", "plugin.vsxmake.autoupdate")

set_warnings("everything")

set_languages("gnu11", "cxx03")

add_defines("_CRT_SECURE_NO_WARNINGS")
add_defines("_POSIX_C_SOURCE=200809")

if is_plat("windows") then
	add_defines("KR_WIN32")
elseif is_plat("gdk") then
	add_defines("KR_WIN32", "KR_GDK")
elseif is_plat("gdkx") then
	add_defines("KR_WIN32", "KR_GDK", "KR_GDKX")
elseif is_plat("linux") then
	add_defines("KR_LINUX", "KR_UNIX")
elseif is_plat("freebsd") then
	add_defines("KR_FREEBSD", "KR_UNIX")
elseif is_plat("switch", "switchhb") then
	add_defines("KR_SWITCH", "KR_UNIX")
elseif is_plat("psp", "ps3") then
    add_defines("KR_PLAYSTATION", "KR_UNIX", "KR_CONSOLE_HOMEBREW", "KR_LEGACY_GRAPHICS")
    if is_plat("psp") then
        add_defines("KR_PSP")
    elseif is_plat("ps3") then
        add_defines("KR_PS3")
        ps3_add_settings(support_root, deps_root)
    end
elseif is_plat("xbox360") then
    add_defines("KR_WIN32", "KR_XBOX360")
end

if is_mode("debug") then
	add_defines("KR_DEBUG")
elseif is_mode("release") then
	add_defines("KR_RELEASE")
end

target("Platform")
	

	set_group("Support")
target_end()

