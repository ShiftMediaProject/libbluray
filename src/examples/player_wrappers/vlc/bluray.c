/*
 * Bluray stream playback
 * by Doom9 libbluray Team 2010
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_SYS_STAT_H
#   include <sys/stat.h>
#endif
#include <limits.h>
#include <dlfcn.h>
#include <vlc_common.h>
#include <vlc_plugin.h>
#include <vlc_access.h>
#include <vlc_messages.h>
#include <assert.h>

/*****************************************************************************
 * Module descriptor
 *****************************************************************************/
#define CACHING_TEXT N_("Caching value in ms")
#define CACHING_LONGTEXT N_( \
    "Caching value for BDs. This "\
    "value should be set in milliseconds." )
#define KEYCFG_TEXT N_("KeyDB file for libbluray")
#define KEYCFG_LONGTEXT N_("Filename for the keycfg.db file.")
#define DEBUGMASK_TEXT N_("BD Debug mask")
#define DEBUGMASK_LONGTEXT N_("Set up the debug mask value for libbluray")
static int  blurayOpen ( vlc_object_t * );
static void blurayClose( vlc_object_t * );

vlc_module_begin ()
    set_shortname( N_("BLURAY") )
    set_description( N_("Blu-Ray Disc (libbluray)") )
    set_category( CAT_INPUT )
    set_subcategory( SUBCAT_INPUT_ACCESS )
    add_integer( "bluray-caching", 1000, NULL,
        CACHING_TEXT, CACHING_LONGTEXT, true )
    add_file("keycfg-db", "/usr/local/share/bluray/keycfg.db", NULL, 
             KEYCFG_TEXT, KEYCFG_LONGTEXT, false);
    add_string("debug-mask", "0x0000", NULL,
	       DEBUGMASK_TEXT, DEBUGMASK_LONGTEXT, false);
    set_capability( "access", 10 )
    add_shortcut( "bluray" )
    add_shortcut( "file" )
    set_callbacks( blurayOpen, blurayClose )
vlc_module_end ()

typedef int (*fptr_int)();
typedef uint64_t (*fptr_uint64)();
typedef void* (*fptr_p_void)();


/*****************************************************************************
 * Local prototypes
 *****************************************************************************/

struct access_sys_t
{
    void *dl_handle;
    void *bluray;
    int i_bd_delay;
};

static ssize_t blurayRead   (access_t *, uint8_t *, size_t);
static block_t *blurayBlock (access_t *);
static int     bluraySeek   (access_t *, int64_t);
static int     blurayControl(access_t *, int, va_list);
static int     bluraySetTitle(access_t *p_access, int i_tile);

/*****************************************************************************
 * blurayOpen: module init function
 *****************************************************************************/
static int blurayOpen( vlc_object_t *object )
{
    char *keycfg = NULL;
    char *debug_mask = NULL;
    char *pos_title;
    int i_title = 0;
    access_t *p_access = (access_t*)object;
    access_sys_t *p_sys;
    char bd_path[512];

    msg_Warn( p_access, "blurayOpen :)" );
    if( !p_access->psz_path || !*p_access->psz_path ) {
        msg_Warn( p_access, "blurayOpen - not selected" );
        return VLC_EGENERIC;
    }

    var_Create( p_access, "bluray-caching", VLC_VAR_INTEGER|VLC_VAR_DOINHERIT );
    var_Create( p_access, "keycfg-db", VLC_VAR_STRING|VLC_VAR_DOINHERIT );
    var_Create( p_access, "debug-mask", VLC_VAR_STRING|VLC_VAR_DOINHERIT );
    

    keycfg = var_GetString(p_access, "keycfg-db");
    if ( keycfg == NULL ) {
        msg_Err( p_access, "read null string for keydb");
        return VLC_EGENERIC;
    }

    debug_mask = var_GetString(p_access, "debug-mask");
    if ( debug_mask ) {
        setenv("BD_DEBUG_MASK", debug_mask, 1);
    }

    /* init access fields */
    access_InitFields(p_access);

    /* register callback function for communication */
    ACCESS_SET_CALLBACKS(blurayRead, NULL, blurayControl, bluraySeek);

    p_access->p_sys = p_sys = malloc(sizeof(access_sys_t));
    if (!p_sys) {
        msg_Warn( p_access, "cannot alloc access_sys memory");
        return VLC_ENOMEM;
    }

    /* open libbluray handle */
    if ( !(p_sys->dl_handle = dlopen("libbluray.so", RTLD_LAZY))) {
        msg_Err( p_access, "library libbluray.so not found");
        return VLC_ENOMEM;
    }
 
    fptr_p_void fptr = dlsym(p_sys->dl_handle, "bd_open");

    /* store current bd_path */
    strncpy(bd_path, p_access->psz_path, sizeof bd_path);
    if ( (pos_title = strrchr(bd_path, ':')) ) {
        /* found character ':' for title information */
        pos_title[0] = '\0';
        i_title = atoi(pos_title + 1);
    }

    p_sys->bluray = fptr(bd_path, keycfg);
    if ( p_sys->bluray == NULL ) {
        return VLC_EGENERIC;
    }

    /* set start title number */
    if ( bluraySetTitle(p_access, i_title) != VLC_SUCCESS ) {
        return VLC_EGENERIC;
    }

    p_sys->i_bd_delay = var_GetInteger(p_access, "bluray-caching");

    return VLC_SUCCESS;
}



/*****************************************************************************
 * blurayClose: module destroy function
 *****************************************************************************/
static void blurayClose( vlc_object_t *object )
{
    access_t *p_access = (access_t*)object;
    access_sys_t *p_sys = p_access->p_sys;

    msg_Warn( p_access, "close bluray");
    if ( p_sys->bluray != NULL ) {
	fptr_p_void fptr = dlsym(p_sys->dl_handle, "bd_close");
        fptr(p_sys->bluray);
    }
    dlclose(p_sys->dl_handle);
}


/*****************************************************************************
 * bluraySetTitle: select new BD title
 *****************************************************************************/
static int bluraySetTitle(access_t *p_access, int i_title)
{
    access_sys_t *p_sys = p_access->p_sys;
    fptr_int fptr_i = dlsym(p_sys->dl_handle, "bd_select_title");
 
    /* select blue ray title */
    if ( fptr_i(p_access->p_sys->bluray, i_title) == 0 ) {
        msg_Err( p_access, "cannot select bd title '%d'", p_access->info.i_title);
        return VLC_EGENERIC;
    }
 
    /* read title length and init some values */
    fptr_uint64 fptr_ui64 = dlsym(p_sys->dl_handle, "bd_get_title_size");
    p_access->info.i_title = i_title;
    p_access->info.i_size = fptr_ui64(p_sys->bluray);
    p_access->info.i_pos = 0;
    p_access->info.b_eof = false;
    p_access->info.i_seekpoint = 0;

    return VLC_SUCCESS;
}


/*****************************************************************************
 * blurayControl: handle the controls
 *****************************************************************************/
static int blurayControl(access_t *p_access, int query, va_list args)
{
    access_sys_t *p_sys = p_access->p_sys;
    bool     *pb_bool;
    int64_t  *pi_64;
    uint32_t pos;

    switch (query) {
        case ACCESS_CAN_SEEK:
        case ACCESS_CAN_FASTSEEK: 
        case ACCESS_CAN_PAUSE:
             pb_bool = (bool*)va_arg( args, bool * );
             *pb_bool = true;
             break;

        case ACCESS_CAN_CONTROL_PACE:
            pb_bool = (bool*)va_arg( args, bool* );
            *pb_bool = true;
            break;

        case ACCESS_GET_PTS_DELAY:
            pi_64 = (int64_t*)va_arg( args, int64_t * );
            *pi_64 = p_sys->i_bd_delay;
            break;

        case ACCESS_SET_PAUSE_STATE:
            /* Nothing to do */
            msg_Warn( p_access, "set pause state");
            break;

        case ACCESS_SET_TITLE:

            break;

        case ACCESS_SET_SEEKPOINT:
            pos = va_arg( args, uint32_t );
            msg_Warn( p_access, "set seek position '%d'", pos);
            break;

        case ACCESS_GET_META:
            return VLC_EGENERIC;
            break;

        case ACCESS_GET_TITLE_INFO:
        case ACCESS_SET_PRIVATE_ID_STATE:
        case ACCESS_GET_CONTENT_TYPE:
            return VLC_EGENERIC;

        default:
            msg_Warn( p_access, "unimplemented query (%d) in control", query );
            return VLC_EGENERIC;
    }

    return VLC_SUCCESS;
}


/*****************************************************************************
 * bluraySeek: seek to the given position
 *****************************************************************************/
static int bluraySeek(access_t *p_access, int64_t position)
{
    access_sys_t *p_sys = p_access->p_sys;

    fptr_uint64 fptr_ui64 = dlsym(p_sys->dl_handle, "bd_seek");
    p_access->info.i_pos = fptr_ui64(p_sys->bluray, position);
    p_access->info.b_eof = false;

    return VLC_SUCCESS;
}


/*****************************************************************************
 * blurayRead: read BD data into buffer
 *****************************************************************************/
static ssize_t blurayRead(access_t *p_access, uint8_t *data, size_t size)
{
    access_sys_t *p_sys = p_access->p_sys;
    int nread;

    if (p_access->info.b_eof) {
        return 0;
    }

    /* read data into buffer with given length */
    fptr_int fptr_i = dlsym(p_sys->dl_handle, "bd_read");
    nread = fptr_i(p_sys->bluray, data, size);

    if( nread == 0 ) {
        p_access->info.b_eof = true;
    }
    else if( nread > 0 ) {
        p_access->info.i_pos += nread;
    }

    return nread;
}
  

