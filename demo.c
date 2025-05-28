/*
 * demo.c - Simple demonstration of Zafran progress bar usage
 *
 * This file shows how to use the Zafran progress bar in a
 * basic sequential example.
 *
 */

#include "zafran.h"


#if defined( _WIN64 ) || defined( _WIN32 )
#    include <windows.h>
#    define ZF_SLEEP( ms ) Sleep( ms )
#else
#    include <unistd.h>
#    define ZF_SLEEP( ms ) usleep( ( ms ) * 1000 )
#endif


void zf_demo() {

    zf_bar bar;

    zf_init( &bar, 100 );

    /* Set various visual styles */
    zf_set_prefix( &bar, "Working" );
    zf_set_ncols( &bar, 50 );
    zf_set_fill_char( &bar, '=', ' ' );
    zf_set_bar_format( &bar, "default" );

    /* Simulate progress */
    for ( size_t i = 0; i < bar.total; i++ ) {

        zf_update( &bar, i );
        ZF_SLEEP( 50 );
    }

    /* Finish with custom message */
    zf_finished( &bar, "✅ Completed ", "Ok, done!" );


    /* Change prefix and suffix mid-progress. */
    zf_set_prefix( &bar, "Preparing" );
    zf_set_fill_char( &bar, '$', '.' );
    zf_set_ncols( &bar, 50 );
    zf_set_bar_format( &bar, "default" );

    for ( size_t i = 0; i < bar.total; i++ ) {

        if ( i == 10 ) {
            zf_update_prefix( &bar, "Uploading" );
            zf_update_suffix( &bar, "(slow...)" );
        } else if ( i == 20 ) {
            zf_update_prefix( &bar, "Finalizing" );
            zf_update_suffix( &bar, "(almost there)" );
        }

        zf_update( &bar, i );
        ZF_SLEEP( 60 );
    }

    /* Finish with custom message */
    zf_finished( &bar, "✅ Done      ", "Ok, done!" );


    zf_init( &bar, 1000 );

    /* Try with a "download" format */
    zf_set_prefix( &bar, "Downloading" );
    zf_set_fill_char( &bar, '#', '.' );
    zf_set_ncols( &bar, 50 );
    zf_set_bar_format( &bar, "download" );
    zf_update_suffix( &bar, "📦" );

    for ( size_t i = 0; i < bar.total; i++ ) {

        zf_update( &bar, i );
        ZF_SLEEP( 30 );
    }

    /* Finish with custom message */
    zf_finished( &bar, "✅ Downloaded", "✔️" );
}


int main( void ) {

    zf_demo();

    return 0;
}
