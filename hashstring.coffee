#!/usr/bin/env coffee

{exec} = require "child_process"

s = process.argv[2]

exec "echo -n #{s}|hashjson", (err, stdout, stderr) ->
  j = JSON.parse stdout
  console.log j

