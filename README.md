## Zafran - A dumb progress bar for C 🚀

<div align="center">
    <img src="https://github.com/wxrayut/zafran/blob/main/assets/demo.gif?raw=true">
</div>
<br>

**Zafran** is a progress bar library written in pure C — for people who just want to show progress in the terminal without crying over dependencies or weird frameworks. maybe try this stupid library.

<p align="center">
    <strong>If you like it, don't forget to ⭐ star this repo!</strong>
</p>

### ✨ Features

- You can see it in: **[demo.c](https://github.com/wxrayut/zafran/blob/main/demo.c)** 🤷

### 📦 Installation

You can drop the source files directly into your C project, or build it as a static library.

### 🔧 Build as a library

```bash
make lib
```

This produces `libzafran[.dll, .so, .dylib]` in the `build/` directory.

### 🚀 Quick Usage

```c
#include "zafran.h"

int main( int argc, char *argv[] ) {

    zf_bar bar;

    zf_init( &bar, 100 );

    /* Set various visual styles */
    zf_set_prefix( &bar, "Working" );
    zf_set_ncols( &bar, 100 );
    zf_set_fill_char( &bar, '=', ' ' );

    /* Simulate progress */
    for ( size_t i = 0; i < bar.total; i++ ) {

        zf_update( &bar, i );
    }

    /* Finish with custom message */
    zf_finished( &bar, "✅ Completed ", "Ok, done!" );

    return 0;
}
```

#### 📄 License

[MIT License](https://github.com/wxrayut/zafran/blob/main/LICENSE) — do whatever you want. Credit is cool, but optional.
