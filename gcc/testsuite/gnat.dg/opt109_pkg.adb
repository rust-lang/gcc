package body Opt109_Pkg is

   function F (Value : String) return Rec is
   begin
      if Value'Length > Max then
         return Result : Rec (Is_Small => False) do
            Result.B.Data := new String'(Value);
         end return;
      else
         return Result : Rec (Is_Small => True) do
            Result.S.Data (Value'Length + 1 .. Max) := (others => ' ');
         end return;
      end if;
   end;

end Opt109_Pkg;
