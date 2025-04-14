#include "shell/cc-keyfile-search.h"
#include <gio/gio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <ctype.h>

// Private module variables
static GKeyFile *keyfile = NULL;
// The list of window managers supported - these are the process names
static const gchar* window_manager_processes[] = {
    "labwc", NULL
};

// Function to check if a process is running
static gboolean is_process_running(const gchar *process_name) {
    g_autofree gchar *command = g_strdup_printf("pgrep -u %d %s", getuid(), process_name);
    gboolean result = FALSE;
    g_autoptr(GError) error = NULL;
    g_autofree gchar *output = NULL;

    if (g_spawn_command_line_sync(command, &output, NULL, NULL, &error)) {
        if (output && strlen(output) > 0) {
            result = TRUE;
        }
    } else {
        g_warning("Error checking process: %s", error->message);
    }

    return result;
}

// Public function to initialize the keyfile
gboolean initialize_keyfile(void) {
    g_autoptr(GError) error = NULL;
    const gchar * const * data_dirs = g_get_system_data_dirs ();

    // Check for running window manager processes and update the keyfile path
    const gchar **process = window_manager_processes;
    gboolean process_found = FALSE;
    while (*process) {
        if (is_process_running(*process)) {
            for (int i = 0; data_dirs[i] != NULL; i++) {
                g_autofree gchar *prefixed_file_path = g_strdup_printf("%s_keyfile.ini", *process);
                g_autofree gchar *dir_path = g_build_filename (data_dirs[i], "budgie-control-center", "keyfile", prefixed_file_path, NULL);
                g_debug("looking for %s", dir_path);

                struct stat buffer;
                if (stat(dir_path, &buffer) == 0) {
                    g_debug("found file");
                    keyfile = g_key_file_new();
                    if (g_key_file_load_from_file(keyfile, dir_path, G_KEY_FILE_NONE, &error)) {
                        g_debug("loaded keyfile");
                        process_found = TRUE;
                        break;
                    }
                    g_key_file_unref(keyfile);
                    keyfile = NULL;
                }
            }
        }
        process++;
    }

    if (!process_found) {
        g_warning("No window manager process found. Keyfile initialization failed.");
        return FALSE;
    }

    return TRUE;
}

// Public function to cleanup the keyfile
void cleanup_keyfile(void) {
    g_clear_object(&keyfile);
}

// Convenience function to confirm if the search for the object should be declared visible
// this is done by exclusion - only explicitly hide if a match is found
gboolean search_keyfile_visible(const gchar *group, const gchar *search_string) {
    gboolean returnval = FALSE;

    if (!keyfile) {
        g_warning("Keyfile is not initialized.");
        return TRUE;
    }

    g_autoptr(GError) error = NULL;

    if (g_key_file_has_group(keyfile, group)) {
        if (g_key_file_has_key(keyfile, group, search_string, &error)) {
            g_autofree gchar *value = g_key_file_get_string(keyfile, group, search_string, &error);
            if (g_strcmp0(value, "true") == 0) {
                g_debug("found key %s with value %s", search_string, value);
                returnval = TRUE;
            }
        } else {
            g_debug("Search string '%s' not found in group '%s'.", search_string, group);
            returnval = TRUE;
        }
    } else {
        g_debug("Group '%s' not found in keyfile.", group);
        returnval = TRUE;
    }

    return returnval;
}
