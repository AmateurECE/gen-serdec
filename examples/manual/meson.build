###############################################################################
# NAME:             meson.build
#
# AUTHOR:           Ethan D. Twardy <ethan.twardy@gmail.com>
#
# DESCRIPTION:      Build script for manual example.
#
# CREATED:          09/11/2022
#
# LAST EDITED:      09/11/2022
#
# Copyright 2022, Ethan D. Twardy
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
###

project('manual-example', 'c', version: '0.1.0',
        default_options: [
          'buildtype=debug',
          'debug=true',
          'optimization=0',
          'warning_level=3',
        ])

add_global_arguments('--std=c17', language: 'c')
add_global_arguments('-D_POSIX_C_SOURCE=200809L', language: 'c')

libyaml = dependency('yaml-0.1')

executable(
  'manual-example',
  sources: [
    'foo.c',
    'main.c'
  ],
  include_directories: ['../../include/'],
  dependencies: [libyaml],
)

###############################################################################
