10  REM This program calculates the factorial of a number
20  PRINT "Enter number >"
30  INPUT NUM
40  LET FACT = 1
50  IF NUM < 0 THEN PRINT "error in input!"
80  FOR I = 1 TO NUM
90     LET  FACT = FACT * I
100 NEXT I
110 PRINT "Factorial of the number=",FACT
120 END    