# kvdb

### What is kvdb?

KVDB is an experimental in-memory key-value datastore much like Memcached or Redis, but using a linked list rather than a tree structure to store your data. This means there's no individual limit on the size of your keys except for the size of your database, and makes it easier to allocate memory efficiently.

### Installation

Use the makefile, which compiles kvdb using the gcc compiler. 

### Syntax

The syntax is incredibly simple:

**GET** [key]
**SET** [key] [value]
**DELETE** [key]