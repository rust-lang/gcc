       *> { dg-do run }
       *> { dg-output-file "group2/Numeric-display_sign_encoding_sanity_check.out" }
       IDENTIFICATION   DIVISION.
       PROGRAM-ID.      prog.
       DATA             DIVISION.
       WORKING-STORAGE  SECTION.
       01 RESULT0 PIC  999V999.
       01 RESULT1 PIC S999V999 leading .
       01 RESULT2 PIC S999V999 trailing .
       01 RESULT3 PIC S999V999 leading separate .
       01 RESULT4 PIC S999V999 trailing separate.
       PROCEDURE        DIVISION.
           move  123.456 to RESULT0 DISPLAY RESULT0
           move  123.456 to RESULT1 DISPLAY RESULT1
           move  123.456 to RESULT2 DISPLAY RESULT2
           move  123.456 to RESULT3 DISPLAY RESULT3
           move  123.456 to RESULT4 DISPLAY RESULT4
           move -123.456 to RESULT1 DISPLAY RESULT1
           move -123.456 to RESULT2 DISPLAY RESULT2
           move -123.456 to RESULT3 DISPLAY RESULT3
           move -123.456 to RESULT4 DISPLAY RESULT4
           STOP RUN.

