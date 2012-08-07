#!/usr/bin/env coffee

child_process = require "child_process"

child_process.exec "dd if=/dev/zero bs=1000000 count=1000 | hashjson",
  (err, stdout, stderr) ->
    if err?
      console.error "hashjson returned error"
    else
      j = JSON.parse stdout
      reference =
        adler32: "48420001"
        crc32: "63f45742",
        md5: "e37115d4da0e187130ab645dee4f14ed",
        sha1: "1dd775261d7abab0b66910acc1d827a2c3799eaf",
        sha256: "bc17f06f9d9b5f6f79ca189a1772b1a3a38d6e40c45bec50f9c4f28144efddca",
        sha512: "7bb94181e3a1573cbc5b04da855646f2972a997d51c14c19f7b288afdb98b05bc67b691f402da20ea27b30e752f0e445d49f636171d90e736a03f6c6770842e3",
        ripemd160: "68fe3d73ef5ed9a85139728448a8c114c1f9c123",
        size: "1000000000"
      for h, v of reference
        unless j[h] is v
          console.error "Hash #{h} expected #{v} actual #{j[h]}"
