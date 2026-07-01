with System;

package Opt109_Pkg is

   Max : constant Natural := 7;
   subtype Small_String_Size is Natural range 0 .. Max;

   type Small_String is record
      Is_Small : Boolean;
      Length   : Small_String_Size;
      Data     : aliased String (1 .. Max);
   end record;

   type Big_String_Access is access all String;

   type Big_String is record
      Data : Big_String_Access := null;
   end record;

   for Big_String use record
      Data at 0 range 0 .. System.Word_Size - 1;
   end record;

   type Rec (Is_Small : Boolean := True) is record
      case Is_Small is
         when True  => S : Small_String;
         when False => B : Big_String;
      end case;
   end record with Unchecked_Union;

   function F (Value : String) return Rec;

end Opt109_Pkg;
