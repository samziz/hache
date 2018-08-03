# Hache

Hache is an experimental in-memory key-value datastore much like Memcached or Redis.

![](https://github.com/samziz/hache/raw/master/misc/cli.png)


## Getting started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

You'll need a working C compiler. The makefile uses GCC by default but you can substitute another compiler by setting `COMPILER` (e.g. `make COMPILER=clang`) or by building manually. 

You don't need to manually link any libraries. Only POSIX and GNU extensions are expected to be available.

### Installing

• Clone this repo to a build directory.

• `cd` into your build directory.

• Run `make`.

To test if your installation is working, spin up a new local service with `hache service start`. This will listen by default on port 7070 unless you specify a different port with the `--port` option. You can access this database by shell with `hache shell start --host localhost --port 7070`. Try setting a key (`SET mykey myvalue`) and then retrieving it (`GET mykey`).


## Guide

### Syntax

Both the shell and the Hache TCP protocol require the same syntax. The basic commands are:

`GET [key]`: Get the value associated with a particular key.

`SET [key] [value]`: Set the value for some key. This will overwrite the current value if one exists.

`DELETE [key]`: Delete a key-value pair from the database.


## Technical info

### Database implementation

Hache uses a simple hash-table implementation based on an in-memory array which is periodically written to disk. We use a hashing algorithm designed to (i) use simple and fast bitwise ops where possible, while (ii) be responsive to key length and bit values where possible rather than calculations based only on the hash value in order to increase uniformity of distribution.


## Contributing

I welcome contributions on any outstanding issues. If you want to contribute anything else, you should raise an issue first to check if it's likely to be merged.


## License

This project is licensed under the GNU General Public License. See the [LICENSE.md](LICENSE.md) file for details.
