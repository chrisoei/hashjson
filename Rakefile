require 'json'
require 'yaml'


def version_tag
  @version_tag ||= `git describe --always --tags`.chomp
end

desc "build"
task :build do
  puts `cd src ; make clean ; make`
end

desc "test"
task :test => :build do
  (1..4).each do |n|
    j = JSON.parse(`./src/hashjson test_vectors/vector#{n}.dat`)
    r = YAML.load_file("test_vectors/vector#{n}.yml")
    r['version'] = "hashjson-#{version_tag}"
    raise "Mismatch" unless r == j
  end
end

task :default => :test