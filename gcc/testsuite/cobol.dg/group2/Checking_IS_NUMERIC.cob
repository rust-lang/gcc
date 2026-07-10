       *> { dg-do run }
       *> { dg-options "-dialect mf" }
       *> { dg-output-file "group2/Checking_IS_NUMERIC.out" }
        identification              division.
        program-id.                 prog.
        data                        division.
        working-storage             section.
            01 var0 pic xxxx.
            01 var1 redefines var0 pic 9999.
            01 var2 redefines var0 pic s9999 trailing.
            01 var3 redefines var0 pic s9999 leading.
            01 var4 redefines var0 pic s999  trailing separate.
            01 var5 redefines var0 pic s999  leading separate.
        procedure                   division.
            move "1234" to var0 display var0
            if var1 is not numeric display "Failure 1-1" space var0 end-if
            if var2 is not numeric display "Failure 1-2" space var0 end-if
            if var3 is not numeric display "Failure 1-3" space var0 end-if
            if var4 is     numeric display "Failure 1-4" space var0 end-if
            if var5 is     numeric display "Failure 1-5" space var0 end-if
            move "12a4" to var0 display var0
            if var1 is     numeric display "Failure 2-1" space var0 end-if
            if var2 is     numeric display "Failure 2-2" space var0 end-if
            if var3 is     numeric display "Failure 2-3" space var0 end-if
            if var4 is     numeric display "Failure 2-4" space var0 end-if
            if var5 is     numeric display "Failure 2-5" space var0 end-if
            move "+123" to var0 display var0
            if var1 is     numeric display "Failure 3-1" space var0 end-if
            if var2 is     numeric display "Failure 3-2" space var0 end-if
            if var3 is     numeric display "Failure 3-3" space var0 end-if
            if var4 is     numeric display "Failure 3-4" space var0 end-if
            if var5 is not numeric display "Failure 3-5" space var0 end-if
            move "123-" to var0 display var0
            if var1 is     numeric display "Failure 4-1" space var0 end-if
            if var2 is     numeric display "Failure 4-2" space var0 end-if
            if var3 is     numeric display "Failure 4-3" space var0 end-if
            if var4 is not numeric display "Failure 4-4" space var0 end-if
            if var5 is     numeric display "Failure 4-5" space var0 end-if
            move -123 to var2 display var2
            move var2(1:) to var0(1:)
            if var1 is     numeric display "Failure 5-1" space var0 end-if
            if var2 is not numeric display "Failure 5-2" space var0 end-if
            if var3 is     numeric display "Failure 5-3" space var0 end-if
            if var4 is     numeric display "Failure 5-4" space var0 end-if
            if var5 is     numeric display "Failure 5-5" space var0 end-if
            move -123 to var3 display var3
            move var2(1:) to var0(1:)
            if var1 is     numeric display "Failure 6-1" space var0 end-if
            if var2 is     numeric display "Failure 6-2" space var0 end-if
            if var3 is not numeric display "Failure 6-3" space var0 end-if
            if var4 is     numeric display "Failure 6-4" space var0 end-if
            if var5 is     numeric display "Failure 6-5" space var0 end-if
            goback.
        end program                 prog.


