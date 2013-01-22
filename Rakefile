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

file 'test_vectors/vector5.dat' do
  File.open('test_vectors/vector5.dat', 'w:US_ASCII') do |f| 
    1000000.times { f.write('a') } 
  end
end

file 'test_vectors/vector6.dat' do
  File.open('test_vectors/vector6.dat', 'w:US_ASCII') do |f| 
    16777216.times do
      f.write('abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno') } 
    end
  end
end

desc "build"
task :build => [ 'hashjson', 'hashyaml', 'test_vectors/vector5.dat' ]

task :clean do
  sh %{ rm -f hashjson hashyaml }
end

desc "test"
task :test => :build do
  (1..5).each do |n|
    j = JSON.parse(`./hashjson test_vectors/vector#{n}.dat`)
    r = YAML.load_file("test_vectors/vector#{n}.yml")
    r['version'] = "hashjson-#{version_tag}"
    raise "Mismatch" unless r == j
    y = YAML.load(`./hashyaml test_vectors/vector#{n}.dat`)
    r['version'] = "hashyaml-#{version_tag}"
    raise "Mismatch" unless r == y
  end

  j = JSON.parse(`./hashjson /dev/null`)
  r = YAML.load_file("test_vectors/vector0.yml")
  r['version'] = "hashjson-#{version_tag}"
  raise "Mismatch: hashjson" unless r == j
  y = YAML.load(`./hashyaml /dev/null`)
  r['version'] = "hashyaml-#{version_tag}"
  raise "Mismatch: hashyaml" unless r == y

  puts "All tests pass."
end

task :default => :test
