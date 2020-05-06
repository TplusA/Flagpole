/*
 * Copyright (C) 2015, 2017, 2019, 2020  T+A elektroakustik GmbH & Co. KG
 *
 * This file is part of Flagpole.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 */

#include <libgupnp/gupnp.h>
#include <stdlib.h>

static void redirect_request(SoupServer *server, SoupMessage *msg,
                             const char *path, GHashTable *query,
                             SoupClientContext *client, gpointer user_data)
{
    GUPnPContext *context = user_data;

    char buffer[4096];
    snprintf(buffer, sizeof(buffer),
             "http://%s%s",
             gssdp_client_get_host_ip(GSSDP_CLIENT(context)), path);

    soup_message_set_redirect(msg, SOUP_STATUS_MOVED_PERMANENTLY, buffer);
}

struct DeviceData
{
    const char *rootxml_file;
    const char *xml_dir;
};

static void context_available(GUPnPContextManager *context_manager,
                              GUPnPContext *context, gpointer user_data)
{
    const struct DeviceData *data = user_data;

    GError *error = NULL;
    GUPnPRootDevice *dev =
        gupnp_root_device_new(context, data->rootxml_file, data->xml_dir,
                              &error);

    if(dev == NULL)
    {
        fprintf(stderr, "Failed creating UPnP root device\n");

        if(error != NULL)
            fprintf(stderr, "Error %s, %d: %s",
                    g_quark_to_string(error->domain), error->code,
                    error->message);
    }
    else
    {
        fprintf(stderr, "Announcing UPnP root device on %s\n",
                gssdp_client_get_host_ip(GSSDP_CLIENT(context)));

        soup_server_add_handler(gupnp_context_get_server(context),
                                NULL, redirect_request, context, NULL);

        gupnp_root_device_set_available(dev, TRUE);
        gupnp_context_manager_manage_root_device(context_manager, dev);

        g_object_unref(dev);
    }
}

static int process_command_line(int argc, char *argv[],
                                struct DeviceData *data)
{
    static const char standard_rootxml_file[] = "flagpole.xml";
    static const char standard_xml_dir[] = ".";

    if(argc < 1 || argc > 3)
        return -1;

    data->xml_dir      = (argc >= 2) ? argv[1] : standard_xml_dir;
    data->rootxml_file = (argc >= 3) ? argv[2] : standard_rootxml_file;

    return 0;
}

int main(int argc, char *argv[])
{
    struct DeviceData data;

    if(process_command_line(argc, argv, &data) < 0)
    {
        fprintf(stderr, "Usage: %s [XML path] [root XML file]\n", argv[0]);
        return EXIT_FAILURE;
    }

    GUPnPContextManager *ctx_manager = gupnp_context_manager_create(0);

    if(ctx_manager == NULL)
    {
        fprintf(stderr, "Failed creating GUPnP context manager\n");
        return EXIT_FAILURE;
    }

    g_signal_connect(G_OBJECT(ctx_manager), "context-available",
                     (GCallback)context_available, &data);

    GMainLoop *main_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(main_loop);

    g_main_loop_unref(main_loop);
    g_object_unref(ctx_manager);

    return EXIT_SUCCESS;
}
