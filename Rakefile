require 'bundler/gem_tasks'
require 'rspec/core/rake_task'
require 'rake/extensiontask'

RSpec::Core::RakeTask.new(:spec)

Rake::ExtensionTask.new do |ext|
  ext.name = 'fswatch'               
  ext.ext_dir = 'ext/fswatch'        
  ext.lib_dir = 'lib/fswatch'                                                               
end

task :default => [:compile, :spec]
