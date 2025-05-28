
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "zafran.h"


/* Maximum sizes for various progress bar components. */
#define ZF_LINE_MAX           512
#define ZF_PREFIX_MAX         32
#define ZF_BAR_MAX            128
#define ZF_PERCENTAGE_MAX     8
#define ZF_ETA_MAX            16
#define ZF_CURRENT_STRING_MAX 16
#define ZF_TOTAL_STRING_MAX   16
#define ZF_SPEED_STRING_MAX   16
#define ZF_ELAPSED_STRING_MAX 32


/* Unit conversion constants */
#define ZF_KB ( 1024.0 )
#define ZF_MB ( 1024.0 * 1024.0 )


/* Default display values */
#define ZF_DEFAULT_NCOLS      100
#define ZF_DEFAULT_BAR_FORMAT "default"


/* Shorthand for fflush() */
#define ZF_FLUSH fflush


#define ZF_UNUSED( ptr ) ( (void)ptr )


/* Terminal cursor control macros */
/* #define ZF_MOVE_CURSOR_UP( n )   printf( "\033[%dA", ( n ) ) */
/* #define ZF_MOVE_CURSOR_DOWN( n ) printf( "\033[%dB", ( n ) ) */
/* #define ZF_CLEAR_LINE            printf( "\033[2K\r" ) */


/* Returns the current UNIX timestamp. */
static inline size_t zf_now() {

    /* ignore clang-format. */

    return (size_t)time( NULL );
}


/* Formats the estimated time remaining (ETA) and writes it into the buffer. */
static inline void zf_format_eta( zf_bar *bar, char *buf, size_t size ) {

    if ( bar->current == 0 ) {

        snprintf( buf, size, "00h:00m:00s" );
        return;
    }

    double elapsed = zf_now() - bar->_start_time;

    if ( elapsed == 0 ) {
        elapsed = 1;
    }

    double speed = bar->current / elapsed;

    if ( speed == 0 ) {

        snprintf( buf, size, "00h:00m:00s" );
        return;
    }

    size_t eta = (size_t)( ( (double)( bar->total - bar->current ) / speed ) + 0.5 );

    int hrs    = (int)( eta / 3600 );
    int mins   = (int)( ( eta % 3600 ) / 60 );
    int secs   = (int)( eta % 60 );

    snprintf( buf, size, "%02dh:%02dm:%02ds", hrs, mins, secs );
}


/* Generates a string representation of the progress bar using filled and unfilled characters. */
static inline void zf_format_bar_string( zf_bar *bar, char *buf, double ratio, size_t size ) {

    ZF_UNUSED( size );

    int filled = (int)( bar->ncols * ratio );
    int empty  = bar->ncols - filled;

    for ( int i = 0; i < filled; i++ ) {
        buf[i] = bar->fill_chr;
    }
    for ( int i = 0; i < empty; i++ ) {
        buf[filled + i] = bar->unfilled_chr;
    }

    buf[filled + empty] = '\0';
}


/* Formats the progress percentage as a string. */
static inline void zf_format_percentage( char *buf, double ratio, size_t size ) {

    /* ignore clang-format. */

    snprintf( buf, size, "%3.0f%%", ratio * 100.0 );
}


/* Formats a byte size value into a human-readable KB or MB string. */
static inline void zf_format_size( char *buf, size_t size, double bytes ) {

    if ( bytes >= ZF_MB ) {
        snprintf( buf, size, "%.2fMB/s", bytes / ZF_MB );
    } else if ( bytes >= ZF_KB ) {
        snprintf( buf, size, "%.2fKB/s", bytes / ZF_KB );
    } else {
        snprintf( buf, size, "%.2fB/s", bytes );
    }
}


/* Formats elapsed time and average speed into human-readable strings. */
static inline void zf_format_progress_stats( const zf_bar *bar, char *elapsed_str, size_t elapsed_size, char *speed_str, size_t speed_size ) {

    size_t elapsed = zf_now() - bar->_start_time;
    double speed   = ( elapsed > 0 && bar->current > 0 ) ? (double)bar->current / elapsed : 0.0;

    snprintf( elapsed_str, elapsed_size, "%zus", elapsed );

    if ( speed >= ZF_MB ) {
        snprintf( speed_str, speed_size, "%.2fMB/s", speed / ZF_MB );
    } else {
        snprintf( speed_str, speed_size, "%.2fKB/s", speed / ZF_KB );
    }
}


/* Renders the default progress bar style. */
static void zf_render_default( zf_bar *bar, double ratio ) {

    char line[ZF_LINE_MAX];
    /* char prefix[ZF_PREFIX_MAX]; */
    char bar_string[ZF_BAR_MAX];
    char percentage[ZF_PERCENTAGE_MAX];
    char eta_string[ZF_ETA_MAX];

    zf_format_eta( bar, eta_string, sizeof( eta_string ) );
    zf_format_bar_string( bar, bar_string, ratio, sizeof( bar_string ) );
    zf_format_percentage( percentage, ratio, sizeof( percentage ) );

    if ( bar->prefix && *bar->prefix ) {
        snprintf( line, sizeof( line ), "\r%s: [%s] [%s | %.0f/%.0f] ETA: %s %s", bar->prefix, bar_string, percentage, (double)bar->current,
                  (double)bar->total, eta_string, bar->suffix ? bar->suffix : "" );
    } else {
        snprintf( line, sizeof( line ), "\r[%s] [%s | %.0f/%.0f] ETA: %s %s", bar_string, percentage, (double)bar->current, (double)bar->total,
                  eta_string, bar->suffix ? bar->suffix : "" );
    }

    fprintf( bar->stream, "%s", line );
}


/* Renders the "download" style progress bar with file size, speed, and ETA. */
static void zf_render_download( zf_bar *bar, double ratio ) {

    char line[ZF_LINE_MAX];
    /* char prefix[ZF_PREFIX_MAX]; */
    char bar_string[ZF_BAR_MAX];
    char percentage[ZF_PERCENTAGE_MAX];
    char eta_string[ZF_ETA_MAX];
    char size_current[ZF_CURRENT_STRING_MAX];
    char size_total[ZF_TOTAL_STRING_MAX];
    char speed_str[ZF_SPEED_STRING_MAX];
    char elapsed_str[ZF_ELAPSED_STRING_MAX];

    zf_format_bar_string( bar, bar_string, ratio, sizeof( bar_string ) );
    zf_format_percentage( percentage, ratio, sizeof( percentage ) );
    zf_format_eta( bar, eta_string, sizeof( eta_string ) );
    zf_format_size( size_current, sizeof( size_current ), (double)bar->current );
    zf_format_size( size_total, sizeof( size_total ), (double)bar->total );
    zf_format_progress_stats( bar, elapsed_str, sizeof( elapsed_str ), speed_str, sizeof( speed_str ) );

    if ( bar->prefix && *bar->prefix ) {
        snprintf( line, sizeof( line ), "\r%s: [%s] %s / %s [%s] in %s (~%s, %s) %s", bar->prefix, bar_string, size_current, size_total, percentage,
                  elapsed_str, eta_string, speed_str, bar->suffix ? bar->suffix : "" );
    } else {
        snprintf( line, sizeof( line ), "\r[%s] %s / %s [%s] in %s (~%s, %s) %s", bar_string, size_current, size_total, percentage, elapsed_str,
                  eta_string, speed_str, bar->suffix ? bar->suffix : "" );
    }

    fprintf( bar->stream, "%s", line );
}


/* Dispatches rendering to the appropriate format based on bar->bar_format.
   Falls back to default if format is unknown. */
static void zf_render( zf_bar *bar, double ratio, int filled, int empty ) {

    ZF_UNUSED( filled );
    ZF_UNUSED( empty );

    if ( !bar->bar_format || strcmp( bar->bar_format, "default" ) == 0 ) {
        zf_render_default( bar, ratio );
    } else if ( strcmp( bar->bar_format, "download" ) == 0 ) {
        zf_render_download( bar, ratio );
    } else {
        /* fallback */
        zf_render_default( bar, ratio );
    }

    ZF_FLUSH( bar->stream );
}


ZAFRAN_API_FUNC( void ) zf_init( zf_bar *bar, size_t total ) {

    if ( total == 0 ) {
        return;
    }

    bar->total        = total;
    bar->current      = 0;

    bar->prefix       = NULL;
    bar->suffix       = NULL;

    bar->ncols        = ZF_DEFAULT_NCOLS;
    bar->show_percent = 1;
    bar->show_eta     = 1;
    /* bar->use_color    = 0; */

    bar->fill_chr     = '#';
    bar->unfilled_chr = '.';

    bar->bar_format   = ZF_DEFAULT_BAR_FORMAT;

    /* bar->thread_safe         = 0; */

    bar->stream       = stdout;

    /* bar->parent              = NULL; */
    /* bar->subbars             = NULL; */
    /* bar->n_subbars           = 0; */

    bar->_start_time  = (size_t)time( NULL );
    bar->_end_time    = 0;
    /* bar->_last_rendered_tick = 0; */
}


ZAFRAN_API_FUNC( void ) zf_set_prefix( zf_bar *bar, const char *prefix ) {

    if ( prefix == NULL ) {
        return;
    }

    bar->prefix = prefix;
}


ZAFRAN_API_FUNC( void ) zf_set_suffix( zf_bar *bar, const char *suffix ) {

    if ( suffix == NULL ) {
        return;
    }

    bar->suffix = suffix;
}


ZAFRAN_API_FUNC( void ) zf_set_ncols( zf_bar *bar, int ncols ) {

    if ( ncols == 0 || ncols > ZF_BAR_MAX ) {
        ncols = ZF_DEFAULT_NCOLS;
    }

    bar->ncols = ncols;
}


ZAFRAN_API_FUNC( void ) zf_set_fill_char( zf_bar *bar, char fill_chr, char unfilled_chr ) {

    if ( fill_chr ) {
        bar->fill_chr = fill_chr;
    }

    if ( !unfilled_chr ) {
        unfilled_chr = '.';
    }

    bar->unfilled_chr = unfilled_chr;
}


ZAFRAN_API_FUNC( void ) zf_set_bar_format( zf_bar *bar, char *bar_format ) {

    if ( bar_format == NULL ) {
        return;
    }

    bar->bar_format = bar_format;
}


/*
ZAFRAN_API_FUNC( void ) zf_set_thread_safe( zf_bar *bar, int enabled ) {

    if ( enabled != 1 ) {
        return;
    }

    bar->thread_safe = enabled;
}
*/


ZAFRAN_API_FUNC( void ) zf_update( zf_bar *bar, size_t current ) {

    bar->current = current;

    double ratio = (double)bar->current / bar->total;

    if ( ratio > 1.0 ) {
        ratio = 1.0;
    }

    int filled = (int)( bar->ncols * ratio );
    int empty  = (int)( bar->ncols - filled );

    zf_render( bar, ratio, filled, empty );
}


ZAFRAN_API_FUNC( void ) zf_update_prefix( zf_bar *bar, const char *prefix ) {

    if ( prefix == NULL ) {
        return;
    }

    bar->prefix = prefix;

    zf_update( bar, bar->current );
}


ZAFRAN_API_FUNC( void ) zf_update_suffix( zf_bar *bar, const char *suffix ) {

    if ( suffix == NULL ) {
        return;
    }

    bar->suffix = suffix;

    zf_update( bar, bar->current );
}


ZAFRAN_API_FUNC( void ) zf_finished( zf_bar *bar, const char *prefix, const char *suffix ) {

    if ( prefix ) {
        bar->prefix = prefix;
    }
    if ( suffix ) {
        bar->suffix = suffix;
    }

    zf_update( bar, bar->total );

    fputc( '\n', bar->stream );
}
