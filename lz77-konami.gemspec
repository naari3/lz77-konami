# frozen_string_literal: true

lib = File.expand_path('lib', __dir__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'lz77/konami/version'

Gem::Specification.new do |spec|
  spec.name          = 'lz77-konami'
  spec.version       = LZ77::Konami::VERSION
  spec.authors       = ['naari3']
  spec.email         = ['naari.named@gmail.com']

  spec.summary       = 'support konami compression'
  spec.description   = spec.summary
  spec.homepage      = 'http://gitlab.com/naari3/lz77-konami'
  spec.extensions    = %w[ext/lz77/konami/extconf.rb]

  # Specify which files should be added to the gem when it is released.
  # The `git ls-files -z` loads the files in the RubyGem that have been added into git.
  spec.files = Dir.chdir(File.expand_path(__dir__)) do
    `git ls-files -z`.split("\x0").reject { |f| f.match(%r{^(test|spec|features)/}) }
  end
  spec.bindir        = 'exe'
  spec.executables   = spec.files.grep(%r{^exe/}) { |f| File.basename(f) }
  spec.require_paths = ['lib']

  spec.add_development_dependency 'bundler', '~> 1.16'
  spec.add_development_dependency 'rake', '~> 12.3'
end
