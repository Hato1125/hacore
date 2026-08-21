set_project('hacore')
set_version('0.1.0')

set_languages('c++26')
add_rules('mode.debug', 'mode.release')

target('hacore')
  set_kind('static')
  add_files('src/*.cc')
  add_includedirs('src', {public = true})
