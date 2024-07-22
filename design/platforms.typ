= Platforms
The engine will support at least Windows and Linux. All desktop platforms will use Steam, all others will use the platform's official store.
#table(
  columns: 3,
  [*Platform*], [*Toolchain*], [*Graphics API(s)*],
  [Windows], [MSVC, GDK], [DirectX 12, Vulkan, OpenGL],
  [Linux], [LLVM], [Vulkan, OpenGL],
  [Xbox Series X|S], [MSVC, GDKX], [DirectX 12],
  [PlayStation 5], [LLVM, PS5 SDK], [GNM],
  [Nintendo Switch/Switch 2], [LLVM, Switch SDK], [Vulkan]
)
These platforms may be supported purely out of personal interest:
#table(
  columns: 4,
  [*Platform*], [*Toolchain*], [*Graphics API(s)*], [*Notes*],
  [Xbox 360], [Ancient MSVC], [DirectX 9], [The engine builds, but somehow the XEXs don't have export tables, meaning it doesn't run],
  [PlayStation 3], [Ancient GCC, possibly modern LLVM], [GCM, OpenGL], [Haven't tried this very hard yet, it's probably possible],
  [PlayStation Portable], [GCC], [OpenGL], [Crashes in homebrew startup code, seems like current firmware doesn't support how syscalls are used in it],
  [Bare metal x86], [LLVM], [Software renderer], [This would take a lot of engineering and probably not be worth it]
)

== x86
On x86-based platforms, SIMD is dynamically detected at startup using `cpuid`, so old CPUs still work but modern ones can be used
fully. Unfortunately, this does reduce the opportunities for optimization on 32-bit, but that's fine because that isn't going to
be the main build that people get.

== Windows
On Windows, the engine does some pretty crazy things. For one, it directly uses `ntdll.dll` instead of `kernel32.dll`, which is
mostly a matter of preference. Another thing is that `Base.dll` manually imports system functions from `ntdll.dll` and `user32.dll`
and exports them for other modules, in addition to exporting `*_Available` functions. This all lets it run on ancient versions,
but then dynamically importing useful functions from newer versions when they're available. The manual importing works by having
function pointers and exporting forwarder functions that call them as the real names that `ntdll.dll` or whatever else exports, and
then also having functions that check whether the function pointers are null or not. It even avoids having an import table at all by
using the PEB to get `ntdll.dll`'s base address, parsing it, and finding `LdrGetProcedureAddress`, and then using that to get other
functions normally.

