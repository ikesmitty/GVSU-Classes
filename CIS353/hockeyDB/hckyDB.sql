--SQL File
/*
CIS 353 - Database Design Project
Daniel DeRuiter
Chad Mayo
Hayden Miedema
Douglas Money
Isaac Smith
*/
SET SERVEROUTPUT ON SIZE 1000000;
SET LINESIZE 50000;
set pagesize 50000;
set long 50000;
SET LIN[ESIZE] 200;
SPOOL hdb.out
--
-- Drop the tables (in case they already exist)
--
DROP TABLE player CASCADE CONSTRAINTS;
DROP TABLE team CASCADE CONSTRAINTS;
DROP TABLE arena CASCADE CONSTRAINTS;
DROP TABLE sponsor CASCADE CONSTRAINTS;
DROP TABLE injury_history CASCADE CONSTRAINTS;
DROP TABLE injury CASCADE CONSTRAINTS;
DROP TABLE company CASCADE CONSTRAINTS;
--
-- Create the tables
--
--
CREATE TABLE  team
(
city        varchar(256),
tName            varchar(256) PRIMARY KEY,
owner        varchar(256),
coach        varchar(256),
gm        varchar(256)
);
--
CREATE TABLE player
(
ssn            varchar(9),
age        int,
position        varchar(256),
name         varchar(256),
pNumber     int,
teamName    varchar(256),
--IC1
 PRIMARY KEY (ssn), 
--IC1b:ssn length must be 4
CONSTRAINT IC1b CHECK (length(ssn) = 4),
--IC3:
CONSTRAINT IC3 CHECK (position IN ('C','LW','RW','D','G')),
--IC5
CONSTRAINT IC5 FOREIGN KEY (teamName) REFERENCES team(tName) Deferrable initially deferred
);
--
CREATE TABLE  arena
(
arenaName    varchar(256) PRIMARY KEY,
capacity    int,
tName       varchar(256),
CONSTRAINT IC6 FOREIGN KEY (tName) REFERENCES team(tName) Deferrable initially deferred
);
--
CREATE TABLE  company
(
cName    varchar(256)    PRIMARY KEY,
country    varchar(256)
);
--
CREATE TABLE injury_history
(
pssn                   varchar(9),
injuryID               int,
dateInjured            date,
gamesMissedDueToInjury int,
PRIMARY KEY(pssn,injuryID,dateInjured),
--IC2:
CONSTRAINT IC2 FOREIGN KEY (pssn) REFERENCES player(ssn) Deferrable initially deferred
);
--
CREATE TABLE injury
(
injuryID int,
injuryType varchar(256),
primary key (injuryID)
);
--
CREATE TABLE  sponsor
(
tName               varchar(256)    NOT NULL,
cName              varchar(256)    NOT NULL,
annualContribution int,
PRIMARY KEY(tName,cName)
);
--
--
alter session set  NLS_DATE_FORMAT = 'YYYY-MM-DD';
--
--
--Player Inserts
--
--Red Wings
insert into player values(1423, 37, 'C', 'Pavel Datsyuk', 13,'Red Wings');
insert into player values(5673, 34, 'D', 'Niklas Kronwall', 55,'Red Wings');
insert into player values(9875, 31, 'G', 'Jimmy Howard', 35, 'Red Wings');
--Bruins
insert into player values(1236, 38, 'D', 'Zdeno Chara', 33,'Bruins');
insert into player values(7254, 30, 'RW', 'Loui Eriksson', 21,'Bruins');
insert into player values(1726, 28, 'G', 'Tuukka Rask', 40,'Bruins');
--Canadiens
insert into player values(2745, 26, 'D', 'P.K. Subban', 76,'Canadiens');
insert into player values(8356, 27, 'LW', 'Max Pacioretty', 67,'Canadiens');
insert into player values(1935, 28, 'G', 'Carey Price', 31,'Canadiens');
--Rangers
insert into player values(4529, 27, 'LW', 'Carl Hagelin', 62,'Rangers');
insert into player values(3333, 31, 'LW', 'Rick Nash', 61,'Rangers');
insert into player values(8213, 33, 'G', 'Henrik Lundqvist', 32,'Rangers');
--Maple Leafs
insert into player values(9112, 26, 'LW', 'James Van Riemsdyk', 21,'Maple Leafs');
insert into player values(6533, 30, 'C', 'Dion Phaneuf', 3,'Maple Leafs');
insert into player values(2903, 27, 'G', 'James Reimer', 35,'Maple Leafs');
--Blackhawks
insert into player values(5681, 27, 'RW', 'Patrick Kane', 88,'Blackhawks');
insert into player values(8499, 27, 'C', 'Jonathan Toews', 18,'Blackhawks');
insert into player values(2235, 30, 'G', 'Corey Crawford', 50,'Blackhawks');
--
--Team Inserts
--insert into team values(city,teamName,owner, coach, gm)
--
insert into team values('Detroit','Red Wings','Mike Ilitch', 'Jeff Blashill', 'Ken Holland');
insert into team values('Boston', 'Bruins','Jeremy Jacobs', 'Claude Julien', 'Don Sweeney');
insert into team values('Montreal','Canadiens', 'Geoff Molson', 'Michel Therrien', 'Marc Bergevin');
insert into team values('Toronto', 'Maple Leafs','Larry Tanenbaum', 'Mike Babcock', 'Lou Lamoriello');
insert into team values('New York','Rangers', 'James Dolan', 'Alain Vigneault', 'Jeff Gorton');
insert into team values('Chicago', 'Blackhawks','Rocky Wirtz', 'Joel Quenneville','Stan Bowman');
--
--Arena Inserts
--
insert into arena values('Joe Louis Arena', 20066,'Red Wings');
insert into arena values('TD Garden', 17565, 'Bruins');
insert into arena values('Bell Centre', 21273,'Canadiens');
insert into arena values('Air Canada Centre', 19800,'Maple Leafs');
insert into arena values('Madison Square Garden', 18200,'Rangers');
insert into arena values('United Center', 20500,'Blackhawks');  
--
--Company (sponsor) Inserts
--
insert into company values('Little Caesars Pizza', 'United States');
insert into company values('Samuel Adams Beer', 'United States');
insert into company values('Imperial Oil', 'Canada');
insert into company values('Tim Hortons', 'Canada');
insert into company values('JPMorgan Chase', 'United States');
insert into company values('Radisson Hotels', 'United States');
--
--Sponsor Inserts
--
insert into sponsor values('Red Wings', 'Little Caesars Pizza', 10000);
insert into sponsor values('Bruins', 'Samuel Adams Beer', 5000);
insert into sponsor values('Canadiens', 'Imperial Oil', 4000);
insert into sponsor values('Maple Leafs', 'Tim Hortons', 15000);
insert into sponsor values('Blackhawks', 'Radisson Hotels', 12000);
--
--Injury Inserts
--
insert into injury values(001, 'Sprain');
insert into injury values(002, 'Break');
insert into injury values(003, 'Strain');
insert into injury values(004, 'Concussion');
insert into injury values(005, 'Muscle Tear');
insert into injury values(006, 'Laceration');
insert into injury values(007, 'DEATH');
--
--Injury History
--
insert into injury_history values(1423, 001, '2015-01-29', 20);
insert into injury_history values(1236, 003, '2014-12-20', 9);
insert into injury_history values(1726, 002, '2014-11-19', 50);
insert into injury_history values(3333, 004, '2015-02-12', 13);
insert into injury_history values(8499, 005, '2014-03-14', 42);
insert into injury_history values(5681, 007, '2014-10-25', 100);
COMMIT;
--
--<Print tables>
--
SELECT * FROM player;
SELECT * FROM team;
SELECT * FROM company;
SELECT * FROM injury;
SELECT * FROM sponsor;
SELECT * FROM injury_history;
SELECT * FROM arena;
--
--
--
--<SQL QUERIES>
--
-- Joining 4 table
--
-- For all players that have suffered an injury find the player name,
-- the team that he plays on, the city the team is in, the type of
-- injury that occurred and the date the player was injured
--
SELECT p.name, p.teamname, t.city, i.injuryType, h.dateInjured
FROM player p, team t, injury i, injury_history h
WHERE p.ssn = h.pssn
AND p.teamName = t.tName
AND h.injuryID = i.injuryID;
--
--
--Q02 Aggregation (AVG)
--Find the average age of all players
SELECT avg(age) AS avAGE
FROM PLAYER;
--
--
--Q03 Counting
--Find and display the number of players on each team
--
SELECT t.tName, COUNT(*)
FROM Team t, Player p
WHERE p.teamname = t.tName
GROUP BY t.tName;
--
--
--Q04 MINUS
-- What is the ssn of all players who do not have a recorded injury
--

SELECT p.ssn, p.name 
FROM player p, 
(SELECT ssn
FROM player
MINUS
SELECT pssn
FROM injury_history) p2
WHERE p.ssn = p2.ssn;

--
--Q05 SELF JOIN
-- What players are the same age?
--
--
--
SELECT DISTINCT p1.ssn,p1.name, p1.age
FROM player p1, player p2
WHERE p1.age = p2.age
AND p1.ssn != p2.ssn
ORDER BY p1.age;
--
--Q06 A non-correlated subquery
--Find the name and ssn of players who have suffered an injury
--
SELECT p.name, p.ssn
FROM player p
WHERE p.ssn IN (SELECT i.pssn
                FROM injury_history i);
--
--
--Q07
--Retrieve the name and ssn of any player whos team receives an above --average sponsor contribution
--
SELECT name, teamname
FROM player p, sponsor s
WHERE p.teamName = s.tName
AND s.annualContribution > (select AVG(annualcontribution)
                            from sponsor);
--
--
--
--Q08 A relational DIVISION query.
-- Find all of the players who have missed less than 15 games from a 
-- single injury.
--

--
--Q09
SPOOL off
