DAN'S APPLEDOUBLE
-----------------

## What is it?

A command line tool that can extract data from AppleDouble (and probably AppleSingle)
files.

This is useful if you have stored Mac files on FAT32-formatted floppy disks or in
macOS X ZIP archives and want to (further) extract the data from them on a non-Mac
operating system.

## How to use it?

Just point it at the second file containing the AppleDouble data, e.g.:

```
dansappledouble "/Volumes/DOSDISK/._Art Bits"
dansappledouble "~/UnpackedZipArchive/__MACOSX/._Art Bits"
```

It will then write an `FInfo.json` file containing the MacOS type code and creator code of
the file to the current directory. It will also write a `TYPE_123_Name.bin` file for each
resource containing its raw data (where 123 would be the resource ID number). Finally, it
will write an `FInfo.bin` file containing the raw file info data (`FInfo` and `FXInfo`
data structures).
