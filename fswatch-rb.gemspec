
lib = File.expand_path("../lib", __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)

Gem::Specification.new do |spec|
  spec.name          = "fswatch-rb"
  spec.version       = '0.1.3'
  spec.authors       = ['Igor Yamolov']
  spec.email         = %w(clouster@yandex.ru)

  spec.summary       = %q{Ruby binding for libfswatch library}
  spec.description   = %q{Ruby library to watch filesystem changes}
  spec.homepage      = "https://github.com/t3hk0d3/fswatch-rb"
  spec.license       = "MIT"

  spec.files         = `git ls-files -z`.split("\x0").reject do |f|
    f.match(%r{^(test|spec|features)/})
  end
  spec.bindir        = "exe"
  spec.executables   = spec.files.grep(%r{^exe/}) { |f| File.basename(f) }
  spec.require_paths = ["lib"]
  spec.extensions    = %w[ext/fswatch/extconf.rb]

  spec.add_development_dependency "bundler", "~> 1.16"
  spec.add_development_dependency "rake", "~> 10.0"
  spec.add_development_dependency 'rake-compiler', '~> 1.0'
  spec.add_development_dependency "rspec", "~> 3.7"
end
