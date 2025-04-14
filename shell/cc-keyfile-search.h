#ifndef KEYFILE_SEARCH_H
#define KEYFILE_SEARCH_H

#include <glib.h>

// Public function to initialize the keyfile
gboolean initialize_keyfile(void);

// Public function to cleanup the keyfile
void cleanup_keyfile(void);

// Public function to search keyfile pair
GString* search_keyfile_pair(const gchar *group, const gchar *default_string, const gchar *search_string);
gboolean search_keyfile_visible(const gchar *group, const gchar *search_string);

#endif // KEYFILE_SEARCH_H
