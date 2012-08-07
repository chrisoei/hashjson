create table checksum (
  filename varchar,
  adler32 char(8),
  crc32 char(8),
  md5 char(32),
  sha1 char(40),
  sha256 char(64),
  sha512 char(128),
  ripemd160 char(40),
  size integer,
  dts char(19) default current_timestamp);

create table annotation (
  md5 char(32),
  sha1 char(40),
  sha256 char(64),
  comment varchar default null,
  dts char(19) default current_timestamp);
