panels_list += cappletname
desktop = 'gnome-@0@-panel.desktop'.format(cappletname)

desktop_in = configure_file(
  input: desktop + '.in.in',
  output: desktop + '.in',
  configuration: desktop_conf
)

custom_target(
  desktop,
  input: desktop_in,
  output: desktop,
  command: intltool_desktop_cmd,
  install: true,
  install_dir: control_center_desktopdir
)

sources = files(
  'cc-region-panel.c',
  'cc-format-chooser.c',
  'cc-ibus-utils.c',
  'cc-input-chooser.c',
  'cc-input-options.c'
)

resource_data = files(
  'format-chooser.ui',
  'input-chooser.ui',
  'input-options.ui',
  'region.ui'
)

sources += gnome.compile_resources(
  'cc-' + cappletname + '-resources',
  cappletname + '.gresource.xml',
  c_name: 'cc_' + cappletname,
  dependencies: resource_data,
  export: true
)

deps = common_deps + [
  accounts_dep,
  gnome_desktop_dep,
  libgd_dep,
  liblanguage_dep,
  polkit_gobject_dep
]

if enable_ibus
  deps += ibus_dep
endif

panels_libs += static_library(
  cappletname,
  sources: sources,
  include_directories: top_inc,
  dependencies: deps,
  c_args: cflags
)