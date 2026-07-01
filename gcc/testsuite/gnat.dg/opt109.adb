-- { dg-do run }
-- { dg-options "-O" }

with Opt109_Pkg; use Opt109_Pkg;

procedure Opt109 is
  S : constant String := "Hello World!";
  R : constant Rec := F (S);
begin
  if R.B.Data.all /= S then
    raise Program_Error;
  end if;
end;
