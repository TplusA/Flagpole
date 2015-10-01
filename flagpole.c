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

static int process_command_line(int argc, char *argv[],
                                const char **rootxml_file, const char **xml_dir)
{
    static const char standard_rootxml_file[] = "flagpole.xml";
    static const char standard_xml_dir[] = ".";

    if(argc < 1 || argc > 3)
        return -1;

    *xml_dir      = (argc >= 2) ? argv[1] : standard_xml_dir;
    *rootxml_file = (argc >= 3) ? argv[2] : standard_rootxml_file;

    return 0;
}

int main(int argc, char *argv[])
{
    const char *rootxml_file;
    const char *xml_dir;

    if(process_command_line(argc, argv, &rootxml_file, &xml_dir) < 0)
    {
        fprintf(stderr, "Usage: %s [XML path] [root XML file]\n", argv[0]);
        return EXIT_FAILURE;
    }

    GError *error = NULL;
    GUPnPContext *context = gupnp_context_new(NULL, "eth1", 0, &error);

    if(error)
    {
        fprintf(stderr, "Failed creating GUPnP context: %s\n", error->message);
        g_error_free(error);
        return EXIT_FAILURE;
    }

    GUPnPRootDevice *dev =
        gupnp_root_device_new(context, rootxml_file, xml_dir);

    if(dev == NULL)
    {
        fprintf(stderr, "Failed creating UPnP root device\n");
        g_object_unref(context);
        return EXIT_FAILURE;
    }

    gupnp_root_device_set_available(dev, TRUE);

    GMainLoop *main_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(main_loop);

    g_main_loop_unref(main_loop);
    g_object_unref(dev);
    g_object_unref(context);

    return EXIT_SUCCESS;
}
