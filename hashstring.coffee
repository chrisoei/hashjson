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
      j =
        string: s
        hash: JSON.parse stdout
      console.log j

child.stdin.write s
child.stdin.end()
