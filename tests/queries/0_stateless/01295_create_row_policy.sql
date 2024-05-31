-- Tags: no-parallel

DROP ROW POLICY IF EXISTS p1_01295, p2_01295, p3_01295, p4_01295, p5_01295, p6_01295, p7_01295, p8_01295, p9_01295, p10_01295 ON db.table;
DROP ROW POLICY IF EXISTS p2_01295_renamed ON db.table;
DROP ROW POLICY IF EXISTS p3_01295 ON db.table, db2.table2;
DROP ROW POLICY IF EXISTS p4_01295 ON db.table, p5_01295 ON db2.table2;
DROP USER IF EXISTS u1_01295;
DROP ROLE IF EXISTS r1_01295;

SELECT '-- default';
CREATE ROW POLICY p1_01295 ON db.table;
SHOW CREATE ROW POLICY p1_01295 ON db.table;

SELECT '-- same as default';
CREATE ROW POLICY p2_01295 ON db.table USING NONE TO NONE;
CREATE POLICY p3_01295 ON db.table;
SHOW CREATE POLICY p2_01295 ON db.table;
SHOW CREATE ROW POLICY p3_01295 ON db.table;

SELECT '-- rename';
ALTER ROW POLICY p2_01295 ON db.table RENAME TO 'p2_01295_renamed';
SHOW CREATE ROW POLICY p2_01295 ON db.table; -- { serverError UNKNOWN_ROW_POLICY } -- Policy not found
SHOW CREATE ROW POLICY p2_01295_renamed ON db.table;
DROP ROW POLICY p1_01295, p2_01295_renamed, p3_01295 ON db.table;

SELECT '-- filter';
CREATE ROW POLICY p1_01295 ON db.table USING a<b AND c>d;
CREATE ROW POLICY p2_01295 ON db.table USING id=currentUser() AS RESTRICTIVE;
CREATE ROW POLICY p3_01295 ON db.table USING 1 AS PERMISSIVE;
SHOW CREATE POLICY p1_01295 ON db.table;
SHOW CREATE POLICY p2_01295 ON db.table;
SHOW CREATE POLICY p3_01295 ON db.table;
ALTER ROW POLICY p1_01295 ON db.table FOR SELECT USING 0 AS RESTRICTIVE;
SHOW CREATE POLICY p1_01295 ON db.table;
DROP ROW POLICY p1_01295, p2_01295, p3_01295 ON db.table;

SELECT '-- to roles';
CREATE ROLE r1_01295;
CREATE USER u1_01295;
CREATE POLICY p1_01295 ON db.table TO NONE;
CREATE POLICY p2_01295 ON db.table TO ALL;
CREATE POLICY p3_01295 ON db.table TO r1_01295;
CREATE POLICY p4_01295 ON db.table TO u1_01295;
CREATE POLICY p5_01295 ON db.table TO r1_01295, u1_01295;
CREATE POLICY p6_01295 ON db.table TO ALL EXCEPT r1_01295;
CREATE POLICY p7_01295 ON db.table TO ALL EXCEPT r1_01295, u1_01295;
SHOW CREATE POLICY p1_01295 ON db.table;
SHOW CREATE POLICY p2_01295 ON db.table;
SHOW CREATE POLICY p3_01295 ON db.table;
SHOW CREATE POLICY p4_01295 ON db.table;
SHOW CREATE POLICY p5_01295 ON db.table;
SHOW CREATE POLICY p6_01295 ON db.table;
SHOW CREATE POLICY p7_01295 ON db.table;
ALTER POLICY p1_01295 ON db.table TO u1_01295;
ALTER POLICY p2_01295 ON db.table TO NONE;
SHOW CREATE POLICY p1_01295 ON db.table;
SHOW CREATE POLICY p2_01295 ON db.table;
DROP POLICY p1_01295, p2_01295, p3_01295, p4_01295, p5_01295, p6_01295, p7_01295 ON db.table;

SELECT '-- multiple policies in one command';
CREATE ROW POLICY p1_01295, p2_01295 ON db.table USING 1;
CREATE ROW POLICY p3_01295 ON db.table, db2.table2 TO u1_01295;
CREATE ROW POLICY p4_01295 ON db.table, p5_01295 ON db2.table2 USING a=b;
SHOW CREATE POLICY p1_01295, p2_01295 ON db.table;
SHOW CREATE POLICY p3_01295 ON db.table, db2.table2;
SHOW CREATE POLICY p4_01295 ON db.table, p5_01295 ON db2.table2;
ALTER POLICY p1_01295, p2_01295 ON db.table TO ALL;
SHOW CREATE POLICY p1_01295, p2_01295 ON db.table;
DROP POLICY p1_01295, p2_01295 ON db.table;
DROP POLICY p3_01295 ON db.table, db2.table2;
DROP POLICY p4_01295 ON db.table, p5_01295 ON db2.table2;

SELECT '-- system.row_policies';
CREATE ROW POLICY p1_01295 ON db.table USING a<b AND c>d;
CREATE ROW POLICY p2_01295 ON db.table USING id=currentUser() AS RESTRICTIVE TO u1_01295;
CREATE ROW POLICY p3_01295 ON db.table USING 1 AS PERMISSIVE TO ALL EXCEPT r1_01295;
SELECT name, short_name, database, table, storage, select_filter, is_restrictive, apply_to_all, apply_to_list, apply_to_except from system.row_policies WHERE short_name LIKE 'p%\_01295' ORDER BY name;
DROP ROW POLICY p1_01295, p2_01295, p3_01295 ON db.table;

DROP ROLE r1_01295;
DROP USER u1_01295;
