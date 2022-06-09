\set ECHO none
BEGIN;
\i sql/cic_bgworker.sql
\set ECHO all

-- You should write your tests

SELECT cic_bgworker('test');

ROLLBACK;
