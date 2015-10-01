/*
 * Copyright (C) 2015  T+A elektroakustik GmbH & Co. KG
 *
 * This file is part of Flagpole.
 *
 * Flagpole is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 3 as
 * published by the Free Software Foundation.
 *
 * Flagpole is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Flagpole.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libgupnp/gupnp.h>
#include <stdlib.h>

struct DeviceData
{
    const char *rootxml_file;
    const char *xml_dir;
};

static void context_available(GUPnPContextManager *context_manager,
                              GUPnPContext *context, gpointer user_data)
{
    const struct DeviceData *data = user_data;

    GUPnPRootDevice *dev =
        gupnp_root_device_new(context, data->rootxml_file, data->xml_dir);

    if(dev == NULL)
        fprintf(stderr, "Failed creating UPnP root device\n");
    else
    {
        fprintf(stderr, "Announcing UPnP root device on %s\n",
                gupnp_context_get_host_ip(context));

        gupnp_root_device_set_available(dev, TRUE);
        gupnp_context_manager_manage_root_device(context_manager, dev);
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
