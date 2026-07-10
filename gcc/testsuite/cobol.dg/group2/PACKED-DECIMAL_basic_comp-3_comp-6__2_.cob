       *> { dg-do run }
       *> { dg-options "-dialect mf" }
       *> { dg-output-file "group2/PACKED-DECIMAL_basic_comp-3_comp-6__2_.out" }
        identification      division.
        program-id.         prog.
        data                division.
        working-storage     section.
          01 var01      pic  99v99     comp-3       value  43.21 .
          01 var02      pic s99v99     comp-3       value  43.21 .
          01 var03      pic s99v99     comp-3       value -43.21 .
          01 var04      pic  99v99     comp-6       value  43.21 .
        procedure           division.
        display length of var01 space "0x" function hex-of(var01) space space var01
        display length of var02 space "0x" function hex-of(var02) space var02
        display length of var03 space "0x" function hex-of(var03) space var03
        display length of var04 "   " "0x" function hex-of(var04) space space var04
        move  12.34 to var01
        move  12.34 to var02
        move -12.34 to var03
        move  12.34 to var04
        display function length(var01) space "0x" function hex-of(var01) space space var01
        display function length(var02) space "0x" function hex-of(var02) space var02
        display function length(var03) space "0x" function hex-of(var03) space var03
        display function length(var04) "   " "0x" function hex-of(var04) space space var04
        goback.
        end program         prog.

