#!/usr/bin/env coffee

child_process = require "child_process"
fs = require "fs"

s = process.argv[2]

child = child_process.execFile "hashjson", [],
  stdio: 'pipe',
  (err, stdout, stderr) ->
    if err?
      console.error stderr
    else
      j = JSON.parse stdout
      j.string = s
      console.log j

child.stdin.write s
child.stdin.end()
