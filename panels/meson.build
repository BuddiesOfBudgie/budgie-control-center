subdir('common')

panels = [
  'applications',
  'background',
  'camera',
  'color',
  'datetime',
  'default-apps',
  'diagnostics',
  'info-overview',
  'keyboard',
  'introduction',
  'location',
  'lock',
  'microphone',
  'mouse',
  'multitasking',
  'night-light',
  'notifications',
  'power',
  'printers',
  'region',
  'removable-media',
  'sharing',
  'sound',
  'universal-access',
  'usage',
  'user-accounts',
  'wwan',
]

if host_is_linux
  panels += ['network']
endif

if host_is_linux_not_s390
  if enable_bluetooth
    panels += ['bluetooth']
  endif

  panels += [
    'thunderbolt',
    'wacom'
  ]
endif

panels_list = []
panels_libs = []
foreach cappletname: panels
  cflags = [
    '-DG_LOG_DOMAIN="@0@-cc-panel"'.format(cappletname),
    '-DPANEL_ID="@0@"'.format(cappletname)
  ]

  subdir(cappletname)
endforeach
