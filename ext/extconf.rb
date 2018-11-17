# frozen_string_literal: true

require 'mkmf'

have_func('fmemopen')
have_func('open_memstream')
create_header
create_makefile 'lz77/konami'
