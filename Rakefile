# frozen_string_literal: true

require 'bundler/gem_tasks'
require 'rake/extensiontask'

task default: :spec

Rake::ExtensionTask.new 'lz77/konami' do |ext|
  ext.lib_dir = 'lib/lz77/konami'
end
