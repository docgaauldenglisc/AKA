# AKA

A digital contact book for linux.

## Install 

To compile from source, first you need the following dependencies:

`gtk+-3.0`

`sqlite3`

`pcre` (specifically version 1, not 2)

`git`

`make`

`cmake` (version 3.10 or higher)

Then it is recommended that you create a **build** folder:

`mkdir build`

And then navigate inside of it:

`cd build`

Then, run **cmake** on the source directory: 

`cmake ../src`

After that, run the **make** command:

`make`

And then the binary will be compiled into the **build** directory.
