# Hache

## What is Hache?

Hache is an experimental in-memory key-value datastore much like Memcached or Redis.

![alt text][cli-img]

## Usage

### Installation

Use the makefile, which compiles Hache with GCC. 

### On the command line

The first word of your command specifies the module that handles it. The subcommands are:

`service`: The service module, which handles starting and stopping Hache databases.

`shell`: The shell module. Pass `host` and `port` as flags to connect to a particular service. The default behaviour is to connect to localhost:7070.

### Syntax

Both the shell and the Hache TCP protocol require the same syntax. The basic commands are:

`GET [key]`: Get the value associated with a particular key.

`SET [key] [value]`: Set the value for some key. This will overwrite the current value if one exists.

`DELETE [key]`: Delete a key-value pair from the database.

### Networking

By default Hache will listen on port 7070 when you run `hache service start`.


## Technical info

### Database implementation

Hache uses a simple hash-table implementation based on an in-memory array which is periodically written to disk. We use a hashing algorithm designed to (i) use simple and fast bitwise ops where possible, while (ii) be responsive to key length and bit values where possible rather than calculations based only on the hash value in order to increase uniformity of distribution.


[cli-img]: https://github.com/samziz/hache/raw/master/misc/cli-img.png "Command line screenshot"