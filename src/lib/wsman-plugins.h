/*******************************************************************************
 * Copyright (C) 2004-2006 Intel Corp. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  - Neither the name of Intel Corp. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL Intel Corp. OR THE CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

/**
 * @author Anas Nashif 
 */


#ifndef _WSMAN_PLUGINS_H
#define _WSMAN_PLUGINS_H

#include <gmodule.h>


//GList* scan_files_in_dir (const char *dir, int (*select)(const struct dirent *));

typedef	enum _WsManPluginError
{
    PLUGIN_ERROR_OK,
    PLUGIN_ERROR_NOTLOADED,
    PLUGIN_ERROR_SYMBOLSNOTFOUND,
    PLUGIN_ERROR_INITFAILED,
    PLUGIN_ERROR_BADPARMS,
} WsManPluginError;

typedef struct _WsManPlugin
{
    GModule *p_handle;
    gchar *p_name;
    int  started;	/* Flag successfuly initialized */
    void *data;	/* user data */
    /* Get plugin description */
    //gchar	*(*get_description)(void);
    /* Get the menu under whcih the plugin should go */
    // glong	(*get_version)(void);
    /* Init Module */
    gboolean (*init)( GModule *self, void **data);
    /* Clean-up */
    void (*cleanup)( GModule *self, void *data);
    /* Activation */
    //void (*activate)( GModule *self, void *data);

    void (*get_endpoints)( GModule *self, void *data);
    WsDispatchInterfaceInfo *interface;

} WsManPlugin;

gboolean wsman_plugins_load(WsManListenerH *listener);
gboolean wsman_plugins_unload(WsManListenerH *listener);

#endif /* _WSMAN_PLUGINS_H */