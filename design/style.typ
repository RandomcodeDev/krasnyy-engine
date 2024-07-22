= Engine coding style
The engine is written in C++03 with no STL and no C runtime. This comes with some advantages, but plenty of disadvantages
as well.
#table(
  columns: 2,
  [*Pros*], [*Cons*],
  [Full control over nearly everything], [Have to implement everything],
  [_Very_ portable, everything that can possibly run the engine has a C++03 compiler], [Missing really nice things like `constexpr` and range-based for loops, and almost no libraries support C89/C++03],
  [Only the exact functionality needed is implemented], [Features not built on existing support code take longer],
)

== Types
`public/base/types.h` defines short type names largely based on Rust's type names. Sizes should use the signed `ssize` to make
calculation errors easier to see, and the fact that it's a size already gives the indication it can't be negative. Any other
type can be unsigned, and for certain things like `operator new()` where using `usize` is required, that's fine too.

== Comments
Comments should explain what code does. At the top of a file that implements something complex, or the main header for a whole
component, explain the overall design of the component, any important choices and the reasoning, and whatever limitations exist.
Additionally, when functions are complex, add comments explaining what's happening/why it's happening. The memory manager in
`base/memory.cpp` is the best example of commenting things so far.

== Headers
Public headers (ones visible to any component) should include as few headers as possible, and forward declare types where needed.
In `.cpp` files, all headers for the things used should be included, not just ones that happen to include the right things. Private
headers are more free to include things, and have references to globals inside components, like `base/base.h`.

== Standard library replacement
Because the C runtime and STL aren't used, there are some replacements for the commonly used stuff, and there are also common
utility functions implemented on top of these, like automatically allocating a buffer for snprintf.

In terms of replacements for the CRT, `base.h` has `Base_Alloc`, `Base_MemSet`, `Base_MemCopy`, and `Base_MemCompare`, and
`basicstr.h` has `Base_StrFormat`, `Base_StrCopy`, `Base_StrClone`, and `Base_StrCompare`. They work basically just like
`malloc`, `memset`, `memcpy`/`memmove`, `memcmp`, `snprintf`, `strcopy`, `strdup`, and `strcmp`, but because this is still C++,
they're overloaded and have behaviour controlled by parameters, which makes them more convenient to use. `Base_MemSet`,
`Base_MemCopy`, and `Base_MemCompare` (and the string functions implemented on top of them) also make use of SIMD where possible.

There's not many fancy containers yet, but `CVector<T>` defined in `vector.h` is a working implementation of a dynamic array.
Additionally, there's `CLinkedList<T>`, which is used for the free list in the memory allocator, and offers significant user
control over the nodes for exactly that reason.

== Assertions and error handling
Assertions are mainly for scenarios that shouldn't happen; don't use them for general error handling. If a piece of memory _must_
be allocated successfully, like in `operator new()` where the standard technically requires that it not return `nullptr` (even though
the standard isn't as relevant for the engine), then you can use an assert. For anything else, prefer using functions that return
`bool`, and return `false` when an error happens. When an unrecoverable error happens, use `Base_Quit` (or `Base_QuitSafe` in functions
where avoiding memory allocation is important, such as the memory manager where it would recurse) to kill the engine and show the user
an error message.

