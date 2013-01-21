require 'json'
require 'yaml'


def version_tag
  @version_tag ||= `git describe --always --tags`.chomp
end

def cflags
  [
    '-O3',
    ' -Isrc',
    '-D_FILE_OFFSET_BITS=64',
    '-D_LARGEFILE_SOURCE',
    "-DHASHJSON_VERSION=\\\"#{version_tag}\\\"",
    '-Wno-deprecated-declarations'
  ].join(' ')
end

def libs
  [
    '-lssl',
    '-lcrypto',
    '-lz',
    '-lpthread',
    '-ldl'
  ].join(' ')
end

file 'hashjson' => [ 'src/hashjson.c', 'src/cko_types.c', 'src/cko_stomach.c', 'src/keccak.c' ] do
  sh %{ gcc #{cflags} -o hashjson src/hashjson.c src/cko_types.c src/cko_stomach.c src/keccak.c #{libs} }
end

file 'hashyaml' => [ 'src/hashyaml.c', 'src/cko_types.c', 'src/cko_stomach.c', 'src/keccak.c' ] do
  sh %{ gcc #{cflags} -o hashyaml src/hashyaml.c src/cko_types.c src/cko_stomach.c src/keccak.c #{libs} }
end

desc "build"
task :build => [ 'hashjson', 'hashyaml' ]

task :clean do
  sh %{ rm -f hashjson hashyaml }
end

desc "test"
task :test => :build do
  (1..4).each do |n|
    j = JSON.parse(`./hashjson test_vectors/vector#{n}.dat`)
    r = YAML.load_file("test_vectors/vector#{n}.yml")
    r['version'] = "hashjson-#{version_tag}"
    raise "Mismatch" unless r == j
    y = YAML.load(`./hashyaml test_vectors/vector#{n}.dat`)
    r['version'] = "hashyaml-#{version_tag}"
    raise "Mismatch" unless r == y
  end
  puts "All tests pass."
end

task :default => :test
