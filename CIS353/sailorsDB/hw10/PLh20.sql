------------------------------------------------------------------------------
-- File PLh20.sql
-- Author: Isaac Smith
------------------------------------------------------------------------------
SET SERVEROUTPUT ON
SET VERIFY OFF
------------------------------------------------------------------------------
ACCEPT rateDecrement NUMBER PROMPT 'Enter the rate decrement: '
ACCEPT allowedMinRate NUMBER PROMPT 'Enter the allowed min rate: '
DECLARE 
   sr     boats%ROWTYPE; 
   CURSOR sCursor IS 
	  (SELECT bid, bname, color, rate, length, logkeeper
          FROM boats) 
          MINUS
	  (SELECT B.bid, B.bname, B.color, B.rate, B.length, B.logkeeper
	   FROM boats B, reservations R
	   WHERE B.bid = R.bid); 
BEGIN
   OPEN sCursor; 
   LOOP 
-- Fetch the qualifying rows one by one 
      FETCH sCursor INTO sr; 
      EXIT WHEN sCursor%NOTFOUND; 
-- Print the sailor' old record
      DBMS_OUTPUT.PUT_LINE ('+++++ old row: '||sr.bid||' ' 
             ||sr.bname||sr.color||' '||sr.rate||' '||sr.length||'  '||sr.logkeeper);  
      sr.rate := sr.rate - &rateDecrement; 
-- A nested block 
      DECLARE  
         belowAllowedMin EXCEPTION; 
      BEGIN   
         IF   sr.rate < &allowedMinRate  
         THEN RAISE belowAllowedMin; 
         ELSE UPDATE boats 
              SET rate = sr.rate 
              WHERE boats.bid = sr.bid; 
-- Print the sailor' new record
             DBMS_OUTPUT.PUT_LINE ('+++++ new row: '||sr.bid||' ' 
                  ||sr.bname||sr.color||' '||sr.rate||' '||sr.length||'  '||sr.logkeeper);     
         END IF; 
      EXCEPTION 
        WHEN belowAllowedMin THEN 
           DBMS_OUTPUT.PUT_LINE('+++++ Update rejected: '|| 
               'The new rating would have been: '|| sr.rate); 
        WHEN OTHERS THEN 
           DBMS_OUTPUT.PUT_LINE('+++++ update rejected: ' || 
                                   SQLCODE||'...'||SQLERRM); 
      END; 
-- end of the nested block 
END LOOP; 
   COMMIT; 
   CLOSE sCursor; 
END;
/ 
