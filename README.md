cic_bgworker
============

Make CREATE INDEX CONCURRENTLY use background workers instead of requiring a client connection

To build it, just do this:

    make
    make installcheck
    make install

If you encounter an error such as:

    "Makefile", line 8: Need an operator

You need to use GNU make, which may well be installed on your system as
`gmake`:

    gmake
    gmake install
    gmake installcheck

If you encounter an error such as:

    make: pg_config: Command not found

Be sure that you have `pg_config` installed and in your path. If you used a
package management system such as RPM to install PostgreSQL, be sure that the
`-devel` package is also installed. If necessary tell the build process where
to find it:

    env PG_CONFIG=/path/to/pg_config make && make installcheck && make install

If you encounter an error such as:

    ERROR:  must be owner of database regression

You need to run the test suite using a database super user, as distinct from the
`root` user:

    make installcheck PGUSER=postgres

Once cic_bgworker is installed, you can add it to a database.

    CREATE EXTENSION cic_bgworker;

Dependencies
------------
The `cic_bgworker` data type has no dependencies other than PostgreSQL.

Copyright and License
---------------------

Copyright (c) 2022 <david@fetter.org>.

