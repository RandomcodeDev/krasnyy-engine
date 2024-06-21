package("mimalloc_local")
    set_homepage("https://github.com/microsoft/mimalloc")
    set_description("mimalloc (pronounced 'me-malloc') is a general purpose allocator with excellent performance characteristics.")
    set_license("MIT")

	add_includedirs("include")

    set_urls("https://github.com/microsoft/mimalloc/archive/v$(version).zip")
    add_versions("2.1.2", "86281c918921c1007945a8a31e5ad6ae9af77e510abfec20d000dd05d15123c7")
    add_versions("2.0.7", "ddb32937aabddedd0d3a57bf68158d4e53ecf9e051618df3331a67182b8b0508")
    add_versions("2.0.6", "23e7443d0b4d7aa945779ea8a806e4e109c0ed62d740953d3656cddea7e04cf8")
    add_versions("2.0.5", "e8d4e031123e82081325a5131ac57d954f5123b6a13653a6d984cbc3b8488bd9")
    add_versions("2.0.3", "8e5f0b74fdafab09e8853415700a9ade4d62d5f56cd43f54adf02580ceda86c1")
    add_versions("2.0.2", "6ccba822e251b8d10f8a63d5d7767bc0cbfae689756a4047cdf3d1e4a9fd33d0")
    add_versions("2.0.1", "59c1fe79933e0ac9837a9ca4d954e4887dccd80a84281a6f849681b89a8b8876")

    add_versions("1.7.7", "d51a5b8f3bc6800a0b2fc46830ce67b4d31b12f4e4550ff80cf394d5a88fead8")
    add_versions("1.7.6", "bae56f8ebdcd43da83b52610d7f1c1602ea8d3798d906825defa5c40ad2eb560")
    add_versions("1.7.3", "8319eca4a114dce5f897a4cb7d945bce22d915b4b262adb861cd7ac68fa3e848")
    add_versions("1.7.2", "2c432e44803d9f4f017323be705f194db5d1452f9a60e38896605e7cfe8b332f")
    add_versions("1.7.1", "dc3219066b4fd50c7f23d60c13fa15ae269a2b10b7dd45b046d5c52c9addb477")
    add_versions("1.7.0", "13f3c82bca3a95233c5e29adb5675ab2b772f0ade23184d822079578c9d6c698")
    add_versions("1.6.7", "5a12aac020650876615a2ce3dd8adc8b208cdcee4d9e6bcfc33b3fbe307f0dbf")

    add_configs("secure", {description = "Use a secured version of mimalloc", default = true, type = "boolean"})
    add_configs("rltgenrandom", {description = "Use a RtlGenRandom instead of BCrypt", default = true, type = "boolean"})

    if is_plat("linux") then
        add_syslinks("pthread")
    elseif is_plat("android") then
        add_syslinks("atomic")
    end

    on_install("macosx", "gdk", "gdkx", "windows", "linux", "android", "mingw", function (package)
        os.cp(path.join(package:scriptdir(), "port", "xmake.lua"), "xmake.lua")
        import("package.tools.xmake").install(package, configs)
    end)

    on_test(function (package)
        assert(package:has_cfuncs("mi_malloc", {includes = "mimalloc.h"}))
    end)
package_end()