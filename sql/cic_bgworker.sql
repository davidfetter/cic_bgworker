/*
 * Author: <david@fetter.org>
 * Created at: 2022-06-09 00:27:18 -0700
 *
 */

--
-- This is a example code generated automatically
-- by pgxn-utils.

-- This is how you define a C function in PostgreSQL.
CREATE OR REPLACE FUNCTION cic_bgworker(text)
RETURNS text
AS 'cic_bgworker'
LANGUAGE C IMMUTABLE STRICT;

-- See more: http://www.postgresql.org/docs/current/static/xfunc-c.html
