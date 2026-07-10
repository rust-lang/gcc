       *> { dg-do run }
       *> { dg-output-file "group2/Non-numeric_data_in_packed_decimal.out" }
        identification   division.
        program-id.      prog2.
        data             division.
        working-storage  section.
        01 x.
          03 x-num      pic 9(06) packed-decimal value 123.
          03 x-bin redefines x-num binary-char.
        procedure        division.
           if x-num numeric
              display "Starts off numeric"
              end-display
           move 255 to x-bin
           if x-num numeric
              display "bad prog"
              end-display
           end-if
           display "test over"
           end-display
           goback.

