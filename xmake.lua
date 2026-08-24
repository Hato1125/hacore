set_project('hacore')
set_version('0.1.0')

set_languages('c++26')
add_rules('mode.debug', 'mode.release')

add_requires('glaze 7.9.0')

target('hacore')
  set_kind('static')
  add_files('src/*.cc')
  add_packages('glaze')
  add_includedirs('src', {public = true})

target('play')
  set_kind('binary')
  add_files('play/*.cc')
  add_deps('hacore')
