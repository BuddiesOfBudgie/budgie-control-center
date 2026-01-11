/* cc-introduction-panel.h */

#pragma once

#include <shell/cc-panel.h>

G_BEGIN_DECLS

#define CC_TYPE_INTRODUCTION_PANEL (cc_introduction_panel_get_type ())
G_DECLARE_FINAL_TYPE (CcIntroductionPanel, cc_introduction_panel, CC, INTRODUCTION_PANEL, CcPanel)

/* Function to get desktop apps for search integration */
GPtrArray * cc_introduction_panel_get_desktop_apps (CcIntroductionPanel *self);

G_END_DECLS
