SPOOL ddl.out
SET ECHO ON
/*
Homework on SQL/DDL
Author: Isaac Smith
*/
--
-- IMPORTANT: use the names IC1, IC2, etc. as given below.
-- --------------------------------------------------------------------
DROP TABLE Employee CASCADE CONSTRAINTS;
DROP TABLE Dependent CASCADE CONSTRAINTS;
--
CREATE TABLE Employee
(
id INTEGER PRIMARY KEY,
name CHAR(10) NOT NULL,
rank CHAR(10) NOT NULL,
salary INTEGER NOT NULL,
/*
IC1: The rank is one of: 'DB guru', 'DB expert', or 'DB rookie'
*/
CONSTRAINT IC1 CHECK (rank in ('DB guru', 'DB expert', 'DB rookie')),
/*
IC2: The salary of a 'DB guru' is above 200.
*/
CONSTRAINT IC2 CHECK((rank = 'DB guru' and salary > 200) or rank <> 'DB guru'),
/*
IC3: The salary of a 'DB expert' is between 80 and 220 (inclusive).
*/
CONSTRAINT IC3 CHECK((rank = 'DB expert' and salary > 80 and salary < 220)or rank <> 'DB expert'),
/*
IC4: The salary of a 'DB rookie' is less than 100.
*/
CONSTRAINT IC4 CHECK((rank = 'DB rookie' and salary < 100) or rank <> 'DB rookie')
);
--
--
CREATE TABLE Dependent
(
empID INTEGER,
dependentName CHAR(20) NOT NULL,
relationship CHAR(20) NOT NULL,
PRIMARY KEY (empID, dependentName),
/*
IC5: empID must refer to an employee in the company. Also:
if an employee is deleted then his/her dependents must be deleted.
IMPORTANT: DO NOT declare this IC as DEFERRABLE.
*/
CONSTRAINT IC5 FOREIGN KEY (empid)
				REFERENCES Employee(id)
				ON DELETE CASCADE
);
--
-- ----------------------------------------------------------------
-- TESTING THE SCHEMA
-- ----------------------------------------------------------------
INSERT INTO Employee VALUES (10, 'Gray', 'DB guru', 240);
INSERT INTO Employee VALUES (20, 'Garland', 'DB guru', 190);
INSERT INTO Employee VALUES (30, 'Edison', 'DB expert', 210);
INSERT INTO Employee VALUES (40, 'Eckerd', 'DB expert', 70);
INSERT INTO Employee VALUES (50, 'Roberts', 'DB rookie', 110);
INSERT INTO Employee VALUES (60, 'Rush', 'DB rookie', 90);
SELECT * from Employee;
-- ----------------------------------------------------------------
INSERT INTO Dependent VALUES (10, 'Grace', 'daughter');
INSERT INTO Dependent VALUES (10, 'George', 'son');
INSERT INTO Dependent VALUES (60, 'Reba', 'daughter');
INSERT INTO Dependent VALUES (15, 'Dustin', 'son');
SELECT * FROM Dependent;
--
DELETE FROM Employee WHERE id = 10;
SELECT * From Employee;
SELECT * FROM Dependent;
--
SET ECHO OFF
SPOOL OFF
