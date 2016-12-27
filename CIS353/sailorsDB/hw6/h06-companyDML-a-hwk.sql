-- File: companyDML-a-solution 
-- SQL/DML HOMEWORK (on the COMPANY database)
/*
Every query is worth 2 point. There is no partial credit for a
partially working query - think of this hwk as a large program and each query is a small part of the program.
--
IMPORTANT SPECIFICATIONS
--
(A)
-- Download the script file company.sql and use it to create your COMPANY database.
-- Dowlnoad the file companyDBinstance; it is provided for your convenience when checking the results of your queries.
(B)
Implement the queries below by ***editing this file*** to include
your name and your SQL code in the indicated places.   
--
(C)
IMPORTANT:
-- Don't use views
-- Don't use inline queries in the FROM clause - see our class notes.
--
(D)
After you have written the SQL code in the appropriate places:
** Run this file (from the command line in sqlplus).
** Print the resulting spooled file (companyDML-a.out) and submit the printout in class on the due date.
--
**** Note: you can use Apex to develop the individual queries. However, you ***MUST*** run this file from the command line as just explained above and then submit a printout of the spooled file. Submitting a printout of the webpage resulting from Apex will *NOT* be accepted.
--
*/
-- Please don't remove the SET ECHO command below.
SPOOL company2DML-a.out
SET ECHO ON
-- ---------------------------------------------------------------
-- 
-- Name: < ***** Isaac Smith ***** >
--
-- ------------------------------------------------------------
-- NULL AND SUBSTRINGS -------------------------------
--
/*(10A)
Find the ssn and last name of every employee who doesn't have a  supervisor, or his last name contains at least two occurences of the letter 'a'. Sort the results by ssn.
*/

SELECT ssn, lname 
FROM employee
WHERE super_ssn IS NULL
OR lname LIKE '%a%a%';

--
-- JOINING 3 TABLES ------------------------------
-- 
/*(11A)
For every employee who works more than 30 hours on any project: Find the ssn, lname, project number, project name, and numer of hours. Sort the results by ssn.
*/

SELECT e.ssn, e.lname, w.hours, w.pno
FROM employee e, works_on w, project p
WHERE w.hours > 30 
AND e.ssn = w.essn
AND w.pno = p.pnumber
ORDER BY SSN; 

--
-- JOINING 3 TABLES ---------------------------
--
/*(12A)
Write a query that consists of one block only.
For every employee who works on a project that is not controlled by the department he works for: Find the employee's lname, the department he works for, the project number that he works on, and the number of the department that controls that project. Sort the results by lname.
*/

SELECT e.lname, e.dno, w.pno, p.dnum 
FROM employee e, works_on w, project p
where e.dno != p.dnum
AND w.pno = p.pnumber
AND e.ssn = w.essn
ORDER BY e.lname;

--
-- JOINING 4 TABLES -------------------------
--
/*(13A)
For every employee who works for more than 20 hours on any project that is located in the same location as his department: Find the ssn, lname, project number, project location, department number, and department location.Sort the results by lname
*/

SELECT e.ssn, e.lname, p.pnumber, p.plocation, p.dnum, dl.dlocation
FROM employee e, project p, works_on w, dept_locations dl
WHERE  w.hours > 20
AND e.ssn = w.essn
AND p.plocation = dl.dlocation
AND w.PNO = p.pnumber
ORDER BY e.lname;

--
-- <<< Your SQL code goes here >>>
-- SELF JOIN -------------------------------------------
-- 
/*(14A)
Write a query that consists of one block only.
For every employee whose salary is less than 70% of his immediate supervisor's salary: Find his ssn, lname, salary; and his supervisor's ssn, lname, and salary. Sort the results by ssn.  
*/

SELECT e.ssn, e.lname, e.salary
FROM employee e, employee s
WHERE e.super_ssn = s.ssn
AND e.salary < (s.salary * .7)
ORDER BY e.ssn;

--
-- USING MORE THAN ONE RANGE VARIABLE ON ONE TABLE -------------------
--
/*(15A)
For projects located in Houston: Find pairs of last names such that the two employees in the pair work on the same project. Remove duplicates. Sort the result by the lname in the left column in the result. 
*/

SELECT DISTINCT E.LNAME, E2.LNAME
FROM EMPLOYEE E, EMPLOYEE E2, PROJECT P, WORKS_ON W, WORKS_ON W2
WHERE E.SSN != E2.SSN
AND E.SSN = W.ESSN
AND E2.SSN = W2.ESSN
AND W.PNO = P.PNUMBER
AND w2.PNO = P.PNUMBER
And P.PLOCATION LIKE 'Houston'
ORDER BY E.LNAME;

--
------------------------------------
--
/*(16A) Hint: A NULL in the hours column should be considered as zero hours.
Find the ssn, lname, and the total number of hours worked on projects for every employee whose total is less than 40 hours. Sort the result by lname
*/ 

SELECT e.ssn, e.lname, sum(w.hours)
FROM employee e, works_on w
WHERE e.ssn = w.essn 
GROUP BY e.ssn, e.lname
HAVING sum(w.hours) < 40 
ORDER BY e.lname;

--
------------------------------------
-- 
/*(17A)
For every project that has more than 2 employees working on it: Find the project number, project name, number of employees working on it, and the total number of hours worked by all employees on that project. Sort the results by project number.
*/ 

SELECT p.pnumber,p.pname, count(*) empWorking, SUM(w.hours)
FROM project p, works_on w
WHERE w.pno = p.pnumber
AND pno IN 
	  (SELECT pno 
	  FROM works_on 
	  GROUP BY pno 
	  HAVING count(*) > 2)
GROUP BY pnumber,pname;


-- 
-- CORRELATED SUBQUERY --------------------------------
--
/*(18A)
For every employee who has the highest salary in his department: Find the dno, ssn, lname, and salary . Sort the results by department number.
*/

SELECT dno, ssn, lname, salary 
FROM Employee e
WHERE e.salary = ANY (SELECT MAX(salary) FROM employee f
		      WHERE e.dno = f.dno);

--
-- NON-CORRELATED SUBQUERY -------------------------------
--
/*(19A)
For every employee who does not work on any project that is located in Houston: Find the ssn and lname. Sort the results by lname
*/

SELECT ssn, lname
FROM employee
WHERE not exists((select pnumber from project where plocation = 'Houston')
INTERSECT
(select pno from works_on where essn = ssn))
ORDER BY lname;

--
-- DIVISION ---------------------------------------------
--
/*(20A) Hint: This is a DIVISION query
For every employee who works on every project that is located in Stafford: Find the ssn and lname. Sort the results by lname
*/

SELECT ssn, lname
FROM employee
WHERE not exists 
((SELECT pnumber FROM project WHERE plocation = 'Stafford')
MINUS
(SELECT pno FROM works_on WHERE essn = ssn))
ORDER BY lname;

--
SET ECHO OFF
SPOOL OFF


