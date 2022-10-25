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
data structures). If it is an _AppleSingle_ file containing a data fork, it will also create
a file with the same name as the original AppleSingle file into the current directory, in
which it will write the data fork of the file.


License
-------

	Copyright 2022 by Uli Kusterer.
	
	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.
	
	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:
	
	   1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	
	   2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	
	   3. This notice may not be removed or altered from any source
	   distribution.
