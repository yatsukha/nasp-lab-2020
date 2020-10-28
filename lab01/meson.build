project('nasp', 'c', 'cpp')

executable(
  'lab01.out',
  sources: [
    'src/main.cpp'
  ],
  include_directories: [  
    'include'
  ],
  override_options: ['warning_level=3', 'werror=true', 'cpp_std=c++17']
)

test('rb tree properties test', executable(
  'lab01.test.out',
  sources: [
    'tst/src/main.cpp'
  ],
  include_directories: [
    'include',
    'tst/include'
  ],
  override_options: ['warning_level=3', 'werror=true', 'cpp_std=c++17']
))