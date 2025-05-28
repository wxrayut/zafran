
#ifndef _ZAFRAN_H
#define _ZAFRAN_H

#include <stdio.h>  // FILE, fflush
#include <stddef.h> // size_t


/* ZAFRAN_API_FUNC: Defines the export visibility of functions for shared libraries.
                    On Windows, uses __declspec(dllexport/dllimport). On GCC, uses __attribute__((visibility)). */
#if defined( _WIN32 ) || defined( _WIN64 )
#    if defined( ZAFRAN_EXPORTS )
#        define ZAFRAN_API_FUNC( type ) __declspec( dllexport ) type
#    else
#        define ZAFRAN_API_FUNC( type ) type
#    endif
#else
#    if defined( __GNUC__ )
#        define ZAFRAN_API_FUNC( type ) __attribute__( ( visibility( "default" ) ) ) type
#    else
#        define ZAFRAN_API_FUNC( type ) type
#    endif
#endif


/* Structure representing a progress bar. */
typedef struct zf_bar {
    size_t      total;   /* Total progress value. */
    size_t      current; /* Current progress. */
    const char *prefix;  /* Text displayed before the bar. */
    const char *suffix;  /* Text displayed after the bar. */

    int ncols;           /* Width of the bar in columns. */
    int show_percent;    /* Show percentage (1 = yes, 0 = no). */
    int show_eta;        /* Show estimated time remaining (1 = yes, 0 = no). */
    /* int use_color; */

    char fill_chr;     /* Character used to fill completed progress. */
    char unfilled_chr; /* Character used for remaining progress. */

    char *bar_format;  /* The bar format. */

    FILE *stream;      /* Output stream for printing the bar. */

    /* int thread_safe; */

    /* Internal state */
    size_t _start_time;               /* Start timestamp.*/
    size_t _end_time;                 /* End timestamp (used when finished). */
    /* size_t _last_rendered_tick; */ /* Last tick that triggered a render. */

} zf_bar;


/*
 * Initializes a zf_bar instance with a given total.
 *
 * @param bar: Pointer to the zf_bar structure to initialize.
 * @param total: The total progress count.
 */
ZAFRAN_API_FUNC( void ) zf_init( zf_bar *bar, size_t total );

/*
 * Sets the prefix string displayed before the progress bar.
 *
 * @param bar: Pointer to the zf_bar structure.
 * @param prefix: Null-terminated string to be used as prefix.
 */
ZAFRAN_API_FUNC( void ) zf_set_prefix( zf_bar *bar, const char *prefix );

/*
 * Sets the number of columns (width) for the progress bar.
 *
 * @param bar: Pointer to the zf_bar structure.
 * @param ncols: Desired width in characters.
 */
ZAFRAN_API_FUNC( void ) zf_set_ncols( zf_bar *bar, int ncols );

/*
 * Sets the characters used for the filled and unfilled parts of the bar.
 *
 * @param bar: Pointer to the zf_bar structure.
 * @param fill_chr: Character for completed progress.
 * @param unfilled_chr: Character for remaining progress.
 */
ZAFRAN_API_FUNC( void ) zf_set_fill_char( zf_bar *bar, char fill_chr, char unfilled_chr );

/*
 * Sets a custom format string for the progress bar.
 *
 * @param bar: Pointer to the zf_bar structure.
 * @param bar_format: Format style string: "default" or "download".
 */
ZAFRAN_API_FUNC( void ) zf_set_bar_format( zf_bar *bar, char *bar_format );

/* */
/* ZAFRAN_API_FUNC( void ) zf_set_thread_safe( zf_bar *bar, int enabled ); */

/*
 * Updates the progress bar to a new current value and re-renders.
 *
 * @param bar: Pointer to the zf_bar structure.
 * @param current: New progress value to set.
 */
ZAFRAN_API_FUNC( void ) zf_update( zf_bar *bar, size_t current );

/*
 * Updates the prefix text while retaining other settings.
 *
 * @param bar: Pointer to the zf_bar structure.
 * @param prefix: New prefix string to use.
 */
ZAFRAN_API_FUNC( void ) zf_update_prefix( zf_bar *bar, const char *prefix );

/*
 * Updates the suffix text while retaining other settings.
 *
 * @param bar: Pointer to the zf_bar structure.
 * @param suffix: New suffix string to use.
 */
ZAFRAN_API_FUNC( void ) zf_update_suffix( zf_bar *bar, const char *suffix );

/*
 * Marks the progress bar as finished, optionally updating prefix and suffix.
 *
 * @param bar: Pointer to the zf_bar structure.
 * @param prefix: Optional final prefix string (can be NULL to keep current).
 * @param suffix: Optional final suffix string (can be NULL to keep current).
 */
ZAFRAN_API_FUNC( void ) zf_finished( zf_bar *bar, const char *prefix, const char *suffix );

#endif /* _ZAFRAN_H */
