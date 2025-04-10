------------------------------------------------------------------------------
--                                                                          --
--                         GNAT RUN-TIME COMPONENTS                         --
--                                                                          --
--                 A D A . S T R I N G S . U N B O U N D E D                --
--                                                                          --
--                                 B o d y                                  --
--                                                                          --
--          Copyright (C) 1992-2025, Free Software Foundation, Inc.         --
--                                                                          --
-- GNAT is free software;  you can  redistribute it  and/or modify it under --
-- terms of the  GNU General Public License as published  by the Free Soft- --
-- ware  Foundation;  either version 3,  or (at your option) any later ver- --
-- sion.  GNAT is distributed in the hope that it will be useful, but WITH- --
-- OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY --
-- or FITNESS FOR A PARTICULAR PURPOSE.                                     --
--                                                                          --
-- As a special exception under Section 7 of GPL version 3, you are granted --
-- additional permissions described in the GCC Runtime Library Exception,   --
-- version 3.1, as published by the Free Software Foundation.               --
--                                                                          --
-- You should have received a copy of the GNU General Public License and    --
-- a copy of the GCC Runtime Library Exception along with this program;     --
-- see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see    --
-- <http://www.gnu.org/licenses/>.                                          --
--                                                                          --
-- GNAT was originally developed  by the GNAT team at  New York University. --
-- Extensive contributions were provided by Ada Core Technologies Inc.      --
--                                                                          --
------------------------------------------------------------------------------

with Ada.Unchecked_Deallocation;

package body Ada.Strings.Unbounded is

   use Ada.Strings.Maps;

   procedure Non_Inlined_Append
     (Source   : in out Unbounded_String;
      New_Item : Unbounded_String);

   procedure Non_Inlined_Append
     (Source   : in out Unbounded_String;
      New_Item : String);

   procedure Non_Inlined_Append
      (Source   : in out Unbounded_String;
       New_Item : Character);
   --  Non_Inlined_Append are part of the respective Append method that
   --  should not be inlined. The idea is that the code of Append is inlined.
   --  In order to make inlining efficient it is better to have the inlined
   --  code as small as possible. Thus most common cases are inlined and less
   --  common cases are deferred in these functions.

   Growth_Factor : constant := 2;
   --  The growth factor controls how much extra space is allocated when
   --  we have to increase the size of an allocated unbounded string. By
   --  allocating extra space, we avoid the need to reallocate on every
   --  append, particularly important when a string is built up by repeated
   --  append operations of small pieces. This is expressed as a factor so
   --  2 means add 1/2 of the length of the string as growth space.

   Min_Mul_Alloc : constant := Standard'Maximum_Alignment;
   --  Allocation will be done by a multiple of Min_Mul_Alloc. This causes
   --  no memory loss as most (all?) malloc implementations are obliged to
   --  align the returned memory on the maximum alignment as malloc does not
   --  know the target alignment.

   function Aligned_Max_Length
     (Required_Length : Natural;
      Reserved_Length : Natural) return Natural;
   --  Returns recommended length of the shared string which is greater or
   --  equal to specified required length and desired reserved length.
   --  Calculation takes into account alignment of the allocated memory
   --  segments to use memory effectively by Append/Insert/etc operations.

   function Sum (Left, Right : Natural) return Natural with Inline;
   --  Returns summary of Left and Right, raise Constraint_Error on overflow

   function Mul (Left, Right : Natural) return Natural with Inline;
   --  Returns multiplication of Left and Right, raise Constraint_Error on
   --  overflow

   ---------
   -- "&" --
   ---------

   function "&"
     (Left  : Unbounded_String;
      Right : Unbounded_String) return Unbounded_String
   is
      LR : constant Shared_String_Access := Left.Reference;
      RR : constant Shared_String_Access := Right.Reference;
      DL : constant Natural := Sum (LR.Last, RR.Last);
      DR : Shared_String_Access;

   begin
      --  Result is an empty string, reuse shared empty string

      if DL = 0 then
         DR := Empty_Shared_String'Access;

      --  Left string is empty, return Right string

      elsif LR.Last = 0 then
         Reference (RR);
         DR := RR;

      --  Right string is empty, return Left string

      elsif RR.Last = 0 then
         Reference (LR);
         DR := LR;

      --  Otherwise, allocate new shared string and fill data

      else
         DR := Allocate (DL);
         DR.Data (1 .. LR.Last) := LR.Data (1 .. LR.Last);
         DR.Data (LR.Last + 1 .. DL) := RR.Data (1 .. RR.Last);
         DR.Last := DL;
      end if;

      return (AF.Controlled with Reference => DR);
   end "&";

   function "&"
     (Left  : Unbounded_String;
      Right : String) return Unbounded_String
   is
      LR : constant Shared_String_Access := Left.Reference;
      DL : constant Natural := Sum (LR.Last, Right'Length);
      DR : Shared_String_Access;

   begin
      --  Result is an empty string, reuse shared empty string

      if DL = 0 then
         DR := Empty_Shared_String'Access;

      --  Right is an empty string, return Left string

      elsif Right'Length = 0 then
         Reference (LR);
         DR := LR;

      --  Otherwise, allocate new shared string and fill it

      else
         DR := Allocate (DL);
         DR.Data (1 .. LR.Last) := LR.Data (1 .. LR.Last);
         DR.Data (LR.Last + 1 .. DL) := Right;
         DR.Last := DL;
      end if;

      return (AF.Controlled with Reference => DR);
   end "&";

   function "&"
     (Left  : String;
      Right : Unbounded_String) return Unbounded_String
   is
      RR : constant Shared_String_Access := Right.Reference;
      DL : constant Natural := Sum (Left'Length, RR.Last);
      DR : Shared_String_Access;

   begin
      --  Result is an empty string, reuse shared one

      if DL = 0 then
         DR := Empty_Shared_String'Access;

      --  Left is empty string, return Right string

      elsif Left'Length = 0 then
         Reference (RR);
         DR := RR;

      --  Otherwise, allocate new shared string and fill it

      else
         DR := Allocate (DL);
         DR.Data (1 .. Left'Length) := Left;

         if Left'Length < Natural'Last then
            DR.Data (Left'Length + 1 .. DL) := RR.Data (1 .. RR.Last);
         end if;

         DR.Last := DL;
      end if;

      return (AF.Controlled with Reference => DR);
   end "&";

   function "&"
     (Left  : Unbounded_String;
      Right : Character) return Unbounded_String
   is
      LR : constant Shared_String_Access := Left.Reference;
      DL : constant Natural := Sum (LR.Last, 1);
      DR : Shared_String_Access;

   begin
      DR := Allocate (DL);
      DR.Data (1 .. LR.Last) := LR.Data (1 .. LR.Last);
      DR.Data (DL) := Right;
      DR.Last := DL;

      return (AF.Controlled with Reference => DR);
   end "&";

   function "&"
     (Left  : Character;
      Right : Unbounded_String) return Unbounded_String
   is
      RR : constant Shared_String_Access := Right.Reference;
      DL : constant Natural := Sum (1, RR.Last);
      DR : Shared_String_Access;

   begin
      DR := Allocate (DL);
      DR.Data (1) := Left;
      DR.Data (2 .. DL) := RR.Data (1 .. RR.Last);
      DR.Last := DL;

      return (AF.Controlled with Reference => DR);
   end "&";

   ---------
   -- "*" --
   ---------

   function "*"
     (Left  : Natural;
      Right : Character) return Unbounded_String
   is
      DR : Shared_String_Access;

   begin
      --  Result is an empty string, reuse shared empty string

      if Left = 0 then
         DR := Empty_Shared_String'Access;

      --  Otherwise, allocate new shared string and fill it

      else
         DR := Allocate (Left);

         for J in 1 .. Left loop
            DR.Data (J) := Right;
         end loop;

         DR.Last := Left;
      end if;

      return (AF.Controlled with Reference => DR);
   end "*";

   function "*"
     (Left  : Natural;
      Right : String) return Unbounded_String
   is
      DL : constant Natural := Mul (Left, Right'Length);
      DR : Shared_String_Access;
      K  : Natural;

   begin
      --  Result is an empty string, reuse shared empty string

      if DL = 0 then
         DR := Empty_Shared_String'Access;

      --  Otherwise, allocate new shared string and fill it

      else
         DR := Allocate (DL);
         K := 0;

         for J in 1 .. Left loop
            DR.Data (K + 1 .. K + Right'Length) := Right;
            K := K + Right'Length;
         end loop;

         DR.Last := DL;
      end if;

      return (AF.Controlled with Reference => DR);
   end "*";

   function "*"
     (Left  : Natural;
      Right : Unbounded_String) return Unbounded_String
   is
      RR : constant Shared_String_Access := Right.Reference;
      DL : constant Natural := Mul (Left, RR.Last);
      DR : Shared_String_Access;
      K  : Natural;

   begin
      --  Result is an empty string, reuse shared empty string

      if DL = 0 then
         DR := Empty_Shared_String'Access;

      --  Coefficient is one, just return string itself

      elsif Left = 1 then
         Reference (RR);
         DR := RR;

      --  Otherwise, allocate new shared string and fill it

      else
         DR := Allocate (DL);
         K := 0;

         for J in 1 .. Left loop
            DR.Data (K + 1 .. K + RR.Last) := RR.Data (1 .. RR.Last);
            K := K + RR.Last;
         end loop;

         DR.Last := DL;
      end if;

      return (AF.Controlled with Reference => DR);
   end "*";

   ---------
   -- "<" --
   ---------

   function "<"
     (Left  : Unbounded_String;
      Right : Unbounded_String) return Boolean
   is
      LR : constant Shared_String_Access := Left.Reference;
      RR : constant Shared_String_Access := Right.Reference;
   begin
      return LR.Data (1 .. LR.Last) < RR.Data (1 .. RR.Last);
   end "<";

   function "<"
     (Left  : Unbounded_String;
      Right : String) return Boolean
   is
      LR : constant Shared_String_Access := Left.Reference;
   begin
      return LR.Data (1 .. LR.Last) < Right;
   end "<";

   function "<"
     (Left  : String;
      Right : Unbounded_String) return Boolean
   is
      RR : constant Shared_String_Access := Right.Reference;
   begin
      return Left < RR.Data (1 .. RR.Last);
   end "<";

   ----------
   -- "<=" --
   ----------

   function "<="
     (Left  : Unbounded_String;
      Right : Unbounded_String) return Boolean
   is
      LR : constant Shared_String_Access := Left.Reference;
      RR : constant Shared_String_Access := Right.Reference;

   begin
      --  LR = RR means two strings shares shared string, thus they are equal

      return LR = RR or else LR.Data (1 .. LR.Last) <= RR.Data (1 .. RR.Last);
   end "<=";

   function "<="
     (Left  : Unbounded_String;
      Right : String) return Boolean
   is
      LR : constant Shared_String_Access := Left.Reference;
   begin
      return LR.Data (1 .. LR.Last) <= Right;
   end "<=";

   function "<="
     (Left  : String;
      Right : Unbounded_String) return Boolean
   is
      RR : constant Shared_String_Access := Right.Reference;
   begin
      return Left <= RR.Data (1 .. RR.Last);
   end "<=";

   ---------
   -- "=" --
   ---------

   function "="
     (Left  : Unbounded_String;
      Right : Unbounded_String) return Boolean
   is
      LR : constant Shared_String_Access := Left.Reference;
      RR : constant Shared_String_Access := Right.Reference;

   begin
      return LR = RR or else LR.Data (1 .. LR.Last) = RR.Data (1 .. RR.Last);
      --  LR = RR means two strings shares shared string, thus they are equal
   end "=";

   function "="
     (Left  : Unbounded_String;
      Right : String) return Boolean
   is
      LR : constant Shared_String_Access := Left.Reference;
   begin
      return LR.Data (1 .. LR.Last) = Right;
   end "=";

   function "="
     (Left  : String;
      Right : Unbounded_String) return Boolean
   is
      RR : constant Shared_String_Access := Right.Reference;
   begin
      return Left = RR.Data (1 .. RR.Last);
   end "=";

   ---------
   -- ">" --
   ---------

   function ">"
     (Left  : Unbounded_String;
      Right : Unbounded_String) return Boolean
   is
      LR : constant Shared_String_Access := Left.Reference;
      RR : constant Shared_String_Access := Right.Reference;
   begin
      return LR.Data (1 .. LR.Last) > RR.Data (1 .. RR.Last);
   end ">";

   function ">"
     (Left  : Unbounded_String;
      Right : String) return Boolean
   is
      LR : constant Shared_String_Access := Left.Reference;
   begin
      return LR.Data (1 .. LR.Last) > Right;
   end ">";

   function ">"
     (Left  : String;
      Right : Unbounded_String) return Boolean
   is
      RR : constant Shared_String_Access := Right.Reference;
   begin
      return Left > RR.Data (1 .. RR.Last);
   end ">";

   ----------
   -- ">=" --
   ----------

   function ">="
     (Left  : Unbounded_String;
      Right : Unbounded_String) return Boolean
   is
      LR : constant Shared_String_Access := Left.Reference;
      RR : constant Shared_String_Access := Right.Reference;

   begin
      --  LR = RR means two strings shares shared string, thus they are equal

      return LR = RR or else LR.Data (1 .. LR.Last) >= RR.Data (1 .. RR.Last);
   end ">=";

   function ">="
     (Left  : Unbounded_String;
      Right : String) return Boolean
   is
      LR : constant Shared_String_Access := Left.Reference;
   begin
      return LR.Data (1 .. LR.Last) >= Right;
   end ">=";

   function ">="
     (Left  : String;
      Right : Unbounded_String) return Boolean
   is
      RR : constant Shared_String_Access := Right.Reference;
   begin
      return Left >= RR.Data (1 .. RR.Last);
   end ">=";

   ------------
   -- Adjust --
   ------------

   procedure Adjust (Object : in out Unbounded_String) is
   begin
      Reference (Object.Reference);
   end Adjust;

   ------------------------
   -- Aligned_Max_Length --
   ------------------------

   function Aligned_Max_Length
     (Required_Length : Natural;
      Reserved_Length : Natural) return Natural
   is
      Static_Size : constant Natural :=
                      Empty_Shared_String'Size / Standard'Storage_Unit;
      --  Total size of all Shared_String static components
   begin
      if Required_Length > Natural'Last - Static_Size - Reserved_Length then
         --  Total requested length is larger than maximum possible length.
         --  Use of Static_Size needed to avoid overflows in expression to
         --  compute aligned length.
         return Natural'Last;

      else
         return
           ((Static_Size + Required_Length + Reserved_Length - 1)
              / Min_Mul_Alloc + 2) * Min_Mul_Alloc - Static_Size;
      end if;
   end Aligned_Max_Length;

   --------------
   -- Allocate --
   --------------

   function Allocate
     (Required_Length : Natural;
      Reserved_Length : Natural := 0) return not null Shared_String_Access
   is
   begin
      --  Empty string requested, return shared empty string

      if Required_Length = 0 then
         return Empty_Shared_String'Access;

      --  Otherwise, allocate requested space (and probably some more room)

      else
         return
           new Shared_String
                 (Aligned_Max_Length (Required_Length, Reserved_Length));
      end if;
   end Allocate;

   ------------
   -- Append --
   ------------

   procedure Append
     (Source   : in out Unbounded_String;
      New_Item : Unbounded_String)
   is
      pragma Suppress (All_Checks);
      --  Suppress checks as they are redundant with the checks done in that
      --  function.

      SR  : constant Shared_String_Access := Source.Reference;
      NR  : constant Shared_String_Access := New_Item.Reference;

   begin
      --  Source is an empty string, reuse New_Item data

      if SR.Last = 0 then
         Reference (NR);
         Source.Reference := NR;
         Unreference (SR);

      --  New_Item is empty string, nothing to do

      elsif NR.Last = 0 then
         null;

      --  Try to reuse existing shared string

      elsif System.Atomic_Counters.Is_One (SR.Counter)
         and then NR.Last <= SR.Max_Length
         and then SR.Max_Length - NR.Last >= SR.Last
      then
         SR.Data (SR.Last + 1 .. SR.Last + NR.Last) := NR.Data (1 .. NR.Last);
         SR.Last := SR.Last + NR.Last;

      --  Otherwise, allocate new one and fill it

      else
         Non_Inlined_Append (Source, New_Item);
      end if;
   end Append;

   procedure Append
     (Source   : in out Unbounded_String;
      New_Item : String)
   is
      pragma Suppress (All_Checks);
      --  Suppress checks as they are redundant with the checks done in that
      --  function.

      New_Item_Length : constant Natural := New_Item'Length;
      SR : constant Shared_String_Access := Source.Reference;
   begin

      if New_Item'Length = 0 then
         --  New_Item is an empty string, nothing to do
         null;

      elsif System.Atomic_Counters.Is_One (SR.Counter)
         --  The following test checks in fact that
         --  SR.Max_Length >= SR.Last + New_Item_Length without causing
         --  overflow.
         and then New_Item_Length <= SR.Max_Length
         and then SR.Max_Length - New_Item_Length >= SR.Last
      then
         --  Try to reuse existing shared string
         SR.Data (SR.Last + 1 .. SR.Last + New_Item_Length) := New_Item;
         SR.Last := SR.Last + New_Item_Length;

      else
         --  Otherwise, allocate new one and fill it. Deferring the worst case
         --  into a separate non-inlined function ensure that inlined Append
         --  code size remains short and thus efficient.
         Non_Inlined_Append (Source, New_Item);
      end if;
   end Append;

   procedure Append
     (Source   : in out Unbounded_String;
      New_Item : Character)
   is
      pragma Suppress (All_Checks);
      --  Suppress checks as they are redundant with the checks done in that
      --  function.

      SR : constant Shared_String_Access := Source.Reference;
   begin
      if System.Atomic_Counters.Is_One (SR.Counter)
         and then SR.Max_Length > SR.Last
      then
         --  Try to reuse existing shared string
         SR.Data (SR.Last + 1) := New_Item;
         SR.Last := SR.Last + 1;

      else
         --  Otherwise, allocate new one and fill it. Deferring the worst case
         --  into a separate non-inlined function ensure that inlined Append
         --  code size remains short and thus efficient.
         Non_Inlined_Append (Source, New_Item);
      end if;
   end Append;

   -------------------
   -- Can_Be_Reused --
   -------------------

   function Can_Be_Reused
     (Item   : not null Shared_String_Access;
      Length : Natural) return Boolean
   is
   begin
      return
        System.Atomic_Counters.Is_One (Item.Counter)
          and then Item.Max_Length >= Length
          and then Item.Max_Length <=
                     Aligned_Max_Length (Length, Length / Growth_Factor);
   end Can_Be_Reused;

   -----------
   -- Count --
   -----------

   function Count
     (Source  : Unbounded_String;
      Pattern : String;
      Mapping : Maps.Character_Mapping := Maps.Identity) return Natural
   is
      SR : constant Shared_String_Access := Source.Reference;
   begin
      return Search.Count (SR.Data (1 .. SR.Last), Pattern, Mapping);
   end Count;

   function Count
     (Source  : Unbounded_String;
      Pattern : String;
      Mapping : Maps.Character_Mapping_Function) return Natural
   is
      SR : constant Shared_String_Access := Source.Reference;
   begin
      return Search.Count (SR.Data (1 .. SR.Last), Pattern, Mapping);
   end Count;

   function Count
     (Source : Unbounded_String;
      Set    : Maps.Character_Set) return Natural
   is
      SR : constant Shared_String_Access := Source.Reference;
   begin
      return Search.Count (SR.Data (1 .. SR.Last), Set);
   end Count;

   ------------
   -- Delete --
   ------------

   function Delete
     (Source  : Unbounded_String;
      From    : Positive;
      Through : Natural) return Unbounded_String
   is
      SR : constant Shared_String_Access := Source.Reference;
      DL : Natural;
      DR : Shared_String_Access;

   begin
      --  Empty slice is deleted, use the same shared string

      if From > Through then
         Reference (SR);
         DR := SR;

      --  From is too large

      elsif From - 1 > SR.Last then
         raise Index_Error;

      --  Compute size of the result

      else
         DL := SR.Last - (Natural'Min (SR.Last, Through) - From + 1);

         --  Result is an empty string, reuse shared empty string

         if DL = 0 then
            DR := Empty_Shared_String'Access;

         --  Otherwise, allocate new shared string and fill it

         else
            DR := Allocate (DL);
            DR.Data (1 .. From - 1) := SR.Data (1 .. From - 1);

            if Through < Natural'Last then
               DR.Data (From .. DL) := SR.Data (Through + 1 .. SR.Last);
            end if;

            DR.Last := DL;
         end if;
      end if;

      return (AF.Controlled with Reference => DR);
   end Delete;

   procedure Delete
     (Source  : in out Unbounded_String;
      From    : Positive;
      Through : Natural)
   is
      SR : constant Shared_String_Access := Source.Reference;
      DL : Natural;
      DR : Shared_String_Access;

   begin
      --  Nothing changed, return

      if From > Through then
         null;

      --  From is too large

      elsif From - 1 > SR.Last then
         raise Index_Error;

      else
         DL := SR.Last - (Natural'Min (SR.Last, Through) - From + 1);

         --  Result is empty, reuse shared empty string

         if DL = 0 then
            Source.Reference := Empty_Shared_String'Access;
            Unreference (SR);

         --  Try to reuse existing shared string

         elsif Can_Be_Reused (SR, DL) then
            if Through < Natural'Last then
               SR.Data (From .. DL) := SR.Data (Through + 1 .. SR.Last);
            end if;

            SR.Last := DL;

         --  Otherwise, allocate new shared string

         else
            DR := Allocate (DL);
            DR.Data (1 .. From - 1) := SR.Data (1 .. From - 1);

            if Through < Natural'Last then
               DR.Data (From .. DL) := SR.Data (Through + 1 .. SR.Last);
            end if;

            DR.Last := DL;
            Source.Reference := DR;
            Unreference (SR);
         end if;
      end if;
   end Delete;

   -------------
   -- Element --
   -------------

   function Element
     (Source : Unbounded_String;
      Index  : Positive) return Character
   is
      pragma Suppress (All_Checks);
      SR : constant Shared_String_Access := Source.Reference;
   begin
      if Index <= SR.Last and then Index > 0 then
         return SR.Data (Index);
      else
         raise Index_Error;
      end if;
   end Element;

   --------------
   -- Finalize --
   --------------

   procedure Finalize (Object : in out Unbounded_String) is
      SR : constant not null Shared_String_Access := Object.Reference;
   begin
      if SR /= Null_Unbounded_String.Reference then

         --  The same controlled object can be finalized several times for
         --  some reason. As per 7.6.1(24) this should have no ill effect,
         --  so we need to add a guard for the case of finalizing the same
         --  object twice.

         --  We set the Object to the empty string so there will be no ill
         --  effects if a program references an already-finalized object.

         Object.Reference := Null_Unbounded_String.Reference;
         Unreference (SR);
      end if;
   end Finalize;

   ----------------
   -- Find_Token --
   ----------------

   procedure Find_Token
     (Source : Unbounded_String;
      Set    : Maps.Character_Set;
      From   : Positive;
      Test   : Strings.Membership;
      First  : out Positive;
      Last   : out Natural)
   is
      SR : constant Shared_String_Access := Source.Reference;
   begin
      Search.Find_Token (SR.Data (From .. SR.Last), Set, Test, First, Last);
   end Find_Token;

   procedure Find_Token
     (Source : Unbounded_String;
      Set    : Maps.Character_Set;
      Test   : Strings.Membership;
      First  : out Positive;
      Last   : out Natural)
   is
      SR : constant Shared_String_Access := Source.Reference;
   begin
      Search.Find_Token (SR.Data (1 .. SR.Last), Set, Test, First, Last);
   end Find_Token;

   ----------
   -- Free --
   ----------

   procedure Free (X : in out String_Access) is
      procedure Deallocate is
        new Ada.Unchecked_Deallocation (String, String_Access);
   begin
      Deallocate (X);
   end Free;

   ----------
   -- Head --
   ----------

   function Head
     (Source : Unbounded_String;
      Count  : Natural;
      Pad    : Character := Space) return Unbounded_String
   is
      SR : constant Shared_String_Access := Source.Reference;
      DR : Shared_String_Access;

   begin
      --  Result is empty, reuse shared empty string

      if Count = 0 then
         DR := Empty_Shared_String'Access;

      --  Length of the string is the same as requested, reuse source shared
      --  string.

      elsif Count = SR.Last then
         Reference (SR);
         DR := SR;

      --  Otherwise, allocate new shared string and fill it

      else
         DR := Allocate (Count);

         --  Length of the source string is more than requested, copy
         --  corresponding slice.

         if Count < SR.Last then
            DR.Data (1 .. Count) := SR.Data (1 .. Count);

         --  Length of the source string is less than requested, copy all
         --  contents and fill others by Pad character.

         else
            DR.Data (1 .. SR.Last) := SR.Data (1 .. SR.Last);

            for J in SR.Last + 1 .. Count loop
               DR.Data (J) := Pad;
            end loop;
         end if;

         DR.Last := Count;
      end if;

      return (AF.Controlled with Reference => DR);
   end Head;

   procedure Head
     (Source : in out Unbounded_String;
      Count  : Natural;
      Pad    : Character := Space)
   is
      SR : constant Shared_String_Access := Source.Reference;
      DR : Shared_String_Access;

   begin
      --  Result is empty, reuse empty shared string

      if Count = 0 then
         Source.Reference := Empty_Shared_String'Access;
         Unreference (SR);

      --  Result is same as source string, reuse source shared string

      elsif Count = SR.Last then
         null;

      --  Try to reuse existing shared string

      elsif Can_Be_Reused (SR, Count) then
         if Count > SR.Last then
            for J in SR.Last + 1 .. Count loop
               SR.Data (J) := Pad;
            end loop;
         end if;

         SR.Last := Count;

      --  Otherwise, allocate new shared string and fill it

      else
         DR := Allocate (Count);

         --  Length of the source string is greater than requested, copy
         --  corresponding slice.

         if Count < SR.Last then
            DR.Data (1 .. Count) := SR.Data (1 .. Count);

         --  Length of the source string is less than requested, copy all
         --  existing data and fill remaining positions with Pad characters.

         else
            DR.Data (1 .. SR.Last) := SR.Data (1 .. SR.Last);

            for J in SR.Last + 1 .. Count loop
               DR.Data (J) := Pad;
            end loop;
         end if;

         DR.Last := Count;
         Source.Reference := DR;
         Unreference (SR);
      end if;
   end Head;

   -----------
   -- Index --
   -----------

   function Index
     (Source  : Unbounded_String;
      Pattern : String;
      Going   : Strings.Direction := Strings.Forward;
      Mapping : Maps.Character_Mapping := Maps.Identity) return Natural
   is
      SR : constant Shared_String_Access := Source.Reference;
   begin
      return Search.Index (SR.Data (1 .. SR.Last), Pattern, Going, Mapping);
   end Index;

   function Index
     (Source  : Unbounded_String;
      Pattern : String;
      Going   : Direction := Forward;
      Mapping : Maps.Character_Mapping_Function) return Natural
   is
      SR : constant Shared_String_Access := Source.Reference;
   begin
      return Search.Index (SR.Data (1 .. SR.Last), Pattern, Going, Mapping);
   end Index;

   function Index
     (Source : Unbounded_String;
      Set    : Maps.Character_Set;
      Test   : Strings.Membership := Strings.Inside;
      Going  : Strings.Direction  := Strings.Forward) return Natural
   is
      SR : constant Shared_String_Access := Source.Reference;
   begin
      return Search.Index (SR.Data (1 .. SR.Last), Set, Test, Going);
   end Index;

   function Index
     (Source  : Unbounded_String;
      Pattern : String;
      From    : Positive;
      Going   : Direction := Forward;
      Mapping : Maps.Character_Mapping := Maps.Identity) return Natural
   is
      SR : constant Shared_String_Access := Source.Reference;
   begin
      return Search.Index
        (SR.Data (1 .. SR.Last), Pattern, From, Going, Mapping);
   end Index;

   function Index
     (Source  : Unbounded_String;
      Pattern : String;
      From    : Positive;
      Going   : Direction := Forward;
      Mapping : Maps.Character_Mapping_Function) return Natural
   is
      SR : constant Shared_String_Access := Source.Reference;
   begin
      return Search.Index
        (SR.Data (1 .. SR.Last), Pattern, From, Going, Mapping);
   end Index;

   function Index
     (Source  : Unbounded_String;
      Set     : Maps.Character_Set;
      From    : Positive;
      Test    : Membership := Inside;
      Going   : Direction := Forward) return Natural
   is
      SR : constant Shared_String_Access := Source.Reference;
   begin
      return Search.Index (SR.Data (1 .. SR.Last), Set, From, Test, Going);
   end Index;

   ---------------------
   -- Index_Non_Blank --
   ---------------------

   function Index_Non_Blank
     (Source : Unbounded_String;
      Going  : Strings.Direction := Strings.Forward) return Natural
   is
      SR : constant Shared_String_Access := Source.Reference;
   begin
      return Search.Index_Non_Blank (SR.Data (1 .. SR.Last), Going);
   end Index_Non_Blank;

   function Index_Non_Blank
     (Source : Unbounded_String;
      From   : Positive;
      Going  : Direction := Forward) return Natural
   is
      SR : constant Shared_String_Access := Source.Reference;
   begin
      return Search.Index_Non_Blank (SR.Data (1 .. SR.Last), From, Going);
   end Index_Non_Blank;

   ----------------
   -- Initialize --
   ----------------

   procedure Initialize (Object : in out Unbounded_String) is
   begin
      Reference (Object.Reference);
   end Initialize;

   ------------
   -- Insert --
   ------------

   function Insert
     (Source   : Unbounded_String;
      Before   : Positive;
      New_Item : String) return Unbounded_String
   is
      SR : constant Shared_String_Access := Source.Reference;
      DL : constant Natural := SR.Last + New_Item'Length;
      DR : Shared_String_Access;

   begin
      --  Check index first

      if Before - 1 > SR.Last then
         raise Index_Error;
      end if;

      --  Result is empty, reuse empty shared string

      if DL = 0 then
         DR := Empty_Shared_String'Access;

      --  Inserted string is empty, reuse source shared string

      elsif New_Item'Length = 0 then
         Reference (SR);
         DR := SR;

      --  Otherwise, allocate new shared string and fill it

      else
         DR := Allocate (DL, DL / Growth_Factor);
         DR.Data (1 .. Before - 1) := SR.Data (1 .. Before - 1);
         DR.Data (Before .. Before - 1 + New_Item'Length) := New_Item;

         if Before <= SR.Last then
            DR.Data (Before + New_Item'Length .. DL) :=
              SR.Data (Before .. SR.Last);
         end if;

         DR.Last := DL;
      end if;

      return (AF.Controlled with Reference => DR);
   end Insert;

   procedure Insert
     (Source   : in out Unbounded_String;
      Before   : Positive;
      New_Item : String)
   is
      SR : constant Shared_String_Access := Source.Reference;
      DL : constant Natural              := SR.Last + New_Item'Length;
      DR : Shared_String_Access;

   begin
      --  Check bounds

      if Before - 1 > SR.Last then
         raise Index_Error;
      end if;

      --  Result is empty string, reuse empty shared string

      if DL = 0 then
         Source.Reference := Empty_Shared_String'Access;
         Unreference (SR);

      --  Inserted string is empty, nothing to do

      elsif New_Item'Length = 0 then
         null;

      --  Try to reuse existing shared string first

      elsif Can_Be_Reused (SR, DL) then
         if Before <= SR.Last then
            SR.Data (Before + New_Item'Length .. DL) :=
              SR.Data (Before .. SR.Last);
         end if;

         SR.Data (Before .. Before - 1 + New_Item'Length) := New_Item;
         SR.Last := DL;

      --  Otherwise, allocate new shared string and fill it

      else
         DR := Allocate (DL, DL / Growth_Factor);
         DR.Data (1 .. Before - 1) := SR.Data (1 .. Before - 1);
         DR.Data (Before .. Before - 1 + New_Item'Length) := New_Item;

         if Before <= SR.Last then
            DR.Data (Before + New_Item'Length .. DL) :=
              SR.Data (Before .. SR.Last);
         end if;

         DR.Last := DL;
         Source.Reference := DR;
         Unreference (SR);
      end if;
   end Insert;

   ------------
   -- Length --
   ------------

   function Length (Source : Unbounded_String) return Natural is
   begin
      return Source.Reference.Last;
   end Length;

   ---------
   -- Mul --
   ---------

   function Mul (Left, Right : Natural) return Natural is
      pragma Unsuppress (Overflow_Check);
   begin
      return Left * Right;
   end Mul;

   ------------------------
   -- Non_Inlined_Append --
   ------------------------

   procedure Non_Inlined_Append
       (Source   : in out Unbounded_String;
        New_Item : Unbounded_String)
   is
      SR : constant Shared_String_Access := Source.Reference;
      NR  : constant Shared_String_Access := New_Item.Reference;
      DL : constant Natural := Sum (SR.Last, NR.Last);
      DR : Shared_String_Access;
   begin
      DR := Allocate (DL, DL / Growth_Factor);
      DR.Data (1 .. SR.Last) := SR.Data (1 .. SR.Last);
      DR.Data (SR.Last + 1 .. DL) := NR.Data (1 .. NR.Last);
      DR.Last := DL;
      Source.Reference := DR;
      Unreference (SR);
   end Non_Inlined_Append;

   procedure Non_Inlined_Append
     (Source   : in out Unbounded_String;
      New_Item : String)
   is
      SR : constant Shared_String_Access := Source.Reference;
      DL : constant Natural := Sum (SR.Last, New_Item'Length);
      DR : Shared_String_Access;
   begin
      DR := Allocate (DL, DL / Growth_Factor);
      DR.Data (1 .. SR.Last) := SR.Data (1 .. SR.Last);
      DR.Data (SR.Last + 1 .. DL) := New_Item;
      DR.Last := DL;
      Source.Reference := DR;
      Unreference (SR);
   end Non_Inlined_Append;

   procedure Non_Inlined_Append
      (Source   : in out Unbounded_String;
       New_Item : Character)
   is
      SR : constant Shared_String_Access := Source.Reference;
   begin
      if SR.Last = Natural'Last then
         raise Constraint_Error;
      else
         declare
            DL : constant Natural := SR.Last + 1;
            DR : Shared_String_Access;
         begin
            DR := Allocate (DL, DL / Growth_Factor);
            DR.Data (1 .. SR.Last) := SR.Data (1 .. SR.Last);
            DR.Data (DL) := New_Item;
            DR.Last := DL;
            Source.Reference := DR;
            Unreference (SR);
         end;
      end if;
   end Non_Inlined_Append;

   ---------------
   -- Overwrite --
   ---------------

   function Overwrite
     (Source   : Unbounded_String;
      Position : Positive;
      New_Item : String) return Unbounded_String
   is
      SR : constant Shared_String_Access := Source.Reference;
      DL : Natural;
      DR : Shared_String_Access;

   begin
      --  Check bounds

      if Position - 1 > SR.Last then
         raise Index_Error;
      end if;

      DL := Integer'Max (SR.Last, Sum (Position - 1, New_Item'Length));

      --  Result is empty string, reuse empty shared string

      if DL = 0 then
         DR := Empty_Shared_String'Access;

      --  Result is same as source string, reuse source shared string

      elsif New_Item'Length = 0 then
         Reference (SR);
         DR := SR;

      --  Otherwise, allocate new shared string and fill it

      else
         DR := Allocate (DL);
         DR.Data (1 .. Position - 1) := SR.Data (1 .. Position - 1);
         DR.Data (Position .. Position - 1 + New_Item'Length) := New_Item;

         if Position <= SR.Last - New_Item'Length then
            DR.Data (Position + New_Item'Length .. DL) :=
              SR.Data (Position + New_Item'Length .. SR.Last);
         end if;

         DR.Last := DL;
      end if;

      return (AF.Controlled with Reference => DR);
   end Overwrite;

   procedure Overwrite
     (Source    : in out Unbounded_String;
      Position  : Positive;
      New_Item  : String)
   is
      SR : constant Shared_String_Access := Source.Reference;
      DL : Natural;
      DR : Shared_String_Access;

   begin
      --  Bounds check

      if Position - 1 > SR.Last then
         raise Index_Error;
      end if;

      DL := Integer'Max (SR.Last, Sum (Position - 1, New_Item'Length));

      --  Result is empty string, reuse empty shared string

      if DL = 0 then
         Source.Reference := Empty_Shared_String'Access;
         Unreference (SR);

      --  String unchanged, nothing to do

      elsif New_Item'Length = 0 then
         null;

      --  Try to reuse existing shared string

      elsif Can_Be_Reused (SR, DL) then
         SR.Data (Position .. Position - 1 + New_Item'Length) := New_Item;
         SR.Last := DL;

      --  Otherwise allocate new shared string and fill it

      else
         DR := Allocate (DL);
         DR.Data (1 .. Position - 1) := SR.Data (1 .. Position - 1);
         DR.Data (Position .. Position - 1 + New_Item'Length) := New_Item;

         if Position <= SR.Last - New_Item'Length then
            DR.Data (Position + New_Item'Length .. DL) :=
              SR.Data (Position + New_Item'Length .. SR.Last);
         end if;

         DR.Last := DL;
         Source.Reference := DR;
         Unreference (SR);
      end if;
   end Overwrite;

   ---------------
   -- Put_Image --
   ---------------

   procedure Put_Image
     (S : in out Ada.Strings.Text_Buffers.Root_Buffer_Type'Class;
      V : Unbounded_String) is
   begin
      String'Put_Image (S, To_String (V));
   end Put_Image;

   ---------------
   -- Reference --
   ---------------

   procedure Reference (Item : not null Shared_String_Access) is
   begin
      if Item = Empty_Shared_String'Access then
         return;
      end if;

      System.Atomic_Counters.Increment (Item.Counter);
   end Reference;

   ---------------------
   -- Replace_Element --
   ---------------------

   procedure Replace_Element
     (Source : in out Unbounded_String;
      Index  : Positive;
      By     : Character)
   is
      SR : constant Shared_String_Access := Source.Reference;
      DR : Shared_String_Access;

   begin
      --  Bounds check

      if Index <= SR.Last then

         --  Try to reuse existing shared string

         if Can_Be_Reused (SR, SR.Last) then
            SR.Data (Index) := By;

         --  Otherwise allocate new shared string and fill it

         else
            DR := Allocate (SR.Last);
            DR.Data (1 .. SR.Last) := SR.Data (1 .. SR.Last);
            DR.Data (Index) := By;
            DR.Last := SR.Last;
            Source.Reference := DR;
            Unreference (SR);
         end if;

      else
         raise Index_Error;
      end if;
   end Replace_Element;

   -------------------
   -- Replace_Slice --
   -------------------

   function Replace_Slice
     (Source : Unbounded_String;
      Low    : Positive;
      High   : Natural;
      By     : String) return Unbounded_String
   is
      SR : constant Shared_String_Access := Source.Reference;
      DL : Natural;
      DR : Shared_String_Access;

   begin
      --  Check bounds

      if Low - 1 > SR.Last then
         raise Index_Error;
      end if;

      --  Do replace operation when removed slice is not empty

      if High >= Low then
         DL := Sum (Low - 1 + Integer'Max (SR.Last - High, 0), By'Length);
         --  This is the number of characters remaining in the string after
         --  replacing the slice.

         --  Result is empty string, reuse empty shared string

         if DL = 0 then
            DR := Empty_Shared_String'Access;

         --  Otherwise allocate new shared string and fill it

         else
            DR := Allocate (DL);
            DR.Data (1 .. Low - 1) := SR.Data (1 .. Low - 1);
            DR.Data (Low .. Low - 1 + By'Length) := By;

            if High < SR.Last then
               DR.Data (Low + By'Length .. DL) :=
                 SR.Data (High + 1 .. SR.Last);
            end if;

            DR.Last := DL;
         end if;

         return (AF.Controlled with Reference => DR);

      --  Otherwise just insert string

      else
         return Insert (Source, Low, By);
      end if;
   end Replace_Slice;

   procedure Replace_Slice
     (Source : in out Unbounded_String;
      Low    : Positive;
      High   : Natural;
      By     : String)
   is
      SR : constant Shared_String_Access := Source.Reference;
      DL : Natural;
      DR : Shared_String_Access;

   begin
      --  Bounds check

      if Low - 1 > SR.Last then
         raise Index_Error;
      end if;

      --  Do replace operation only when replaced slice is not empty

      if High >= Low then
         DL := Sum (Low - 1 + Integer'Max (SR.Last - High, 0), By'Length);
         --  This is the number of characters remaining in the string after
         --  replacing the slice.

         --  Result is empty string, reuse empty shared string

         if DL = 0 then
            Source.Reference := Empty_Shared_String'Access;
            Unreference (SR);

         --  Try to reuse existing shared string

         elsif Can_Be_Reused (SR, DL) then
            if High < SR.Last then
               SR.Data (Low + By'Length .. DL) :=
                 SR.Data (High + 1 .. SR.Last);
            end if;

            SR.Data (Low .. Low - 1 + By'Length) := By;
            SR.Last := DL;

         --  Otherwise allocate new shared string and fill it

         else
            DR := Allocate (DL);
            DR.Data (1 .. Low - 1) := SR.Data (1 .. Low - 1);
            DR.Data (Low .. Low - 1 + By'Length) := By;

            if High < SR.Last then
               DR.Data (Low + By'Length .. DL) :=
                 SR.Data (High + 1 .. SR.Last);
            end if;

            DR.Last := DL;
            Source.Reference := DR;
            Unreference (SR);
         end if;

      --  Otherwise just insert item

      else
         Insert (Source, Low, By);
      end if;
   end Replace_Slice;

   --------------------------
   -- Set_Unbounded_String --
   --------------------------

   procedure Set_Unbounded_String
     (Target : out Unbounded_String;
      Source : String)
   is
      TR : constant Shared_String_Access := Target.Reference;
      DR : Shared_String_Access;

   begin
      --  In case of empty string, reuse empty shared string

      if Source'Length = 0 then
         Target.Reference := Empty_Shared_String'Access;

      else
         --  Try to reuse existing shared string

         if Can_Be_Reused (TR, Source'Length) then
            Reference (TR);
            DR := TR;

         --  Otherwise allocate new shared string

         else
            DR := Allocate (Source'Length);
            Target.Reference := DR;
         end if;

         DR.Data (1 .. Source'Length) := Source;
         DR.Last := Source'Length;
      end if;

      Unreference (TR);
   end Set_Unbounded_String;

   -----------
   -- Slice --
   -----------

   function Slice
     (Source : Unbounded_String;
      Low    : Positive;
      High   : Natural) return String
   is
      SR : constant Shared_String_Access := Source.Reference;

   begin
      --  Note: test of High > Length is in accordance with AI95-00128

      if Low - 1 > SR.Last or else High > SR.Last then
         raise Index_Error;

      else
         return SR.Data (Low .. High);
      end if;
   end Slice;

   ---------
   -- Sum --
   ---------

   function Sum (Left, Right : Natural) return Natural is
      pragma Unsuppress (Overflow_Check);
   begin
      return Left + Right;
   end Sum;

   ----------
   -- Tail --
   ----------

   function Tail
     (Source : Unbounded_String;
      Count  : Natural;
      Pad    : Character := Space) return Unbounded_String
   is
      SR : constant Shared_String_Access := Source.Reference;
      DR : Shared_String_Access;

   begin
      --  For empty result reuse empty shared string

      if Count = 0 then
         DR := Empty_Shared_String'Access;

      --  Result is whole source string, reuse source shared string

      elsif Count = SR.Last then
         Reference (SR);
         DR := SR;

      --  Otherwise allocate new shared string and fill it

      else
         DR := Allocate (Count);

         if Count < SR.Last then
            DR.Data (1 .. Count) := SR.Data (SR.Last - Count + 1 .. SR.Last);

         else
            for J in 1 .. Count - SR.Last loop
               DR.Data (J) := Pad;
            end loop;

            DR.Data (Count - SR.Last + 1 .. Count) := SR.Data (1 .. SR.Last);
         end if;

         DR.Last := Count;
      end if;

      return (AF.Controlled with Reference => DR);
   end Tail;

   procedure Tail
     (Source : in out Unbounded_String;
      Count  : Natural;
      Pad    : Character := Space)
   is
      SR : constant Shared_String_Access := Source.Reference;
      DR : Shared_String_Access;

      procedure Common
        (SR    : Shared_String_Access;
         DR    : Shared_String_Access;
         Count : Natural);
      --  Common code of tail computation. SR/DR can point to the same object

      ------------
      -- Common --
      ------------

      procedure Common
        (SR    : Shared_String_Access;
         DR    : Shared_String_Access;
         Count : Natural) is
      begin
         if Count < SR.Last then
            DR.Data (1 .. Count) := SR.Data (SR.Last - Count + 1 .. SR.Last);

         else
            DR.Data (Count - SR.Last + 1 .. Count) := SR.Data (1 .. SR.Last);

            for J in 1 .. Count - SR.Last loop
               DR.Data (J) := Pad;
            end loop;
         end if;

         DR.Last := Count;
      end Common;

   begin
      --  Result is empty string, reuse empty shared string

      if Count = 0 then
         Source.Reference := Empty_Shared_String'Access;
         Unreference (SR);

      --  Length of the result is the same as length of the source string,
      --  reuse source shared string.

      elsif Count = SR.Last then
         null;

      --  Try to reuse existing shared string

      elsif Can_Be_Reused (SR, Count) then
         Common (SR, SR, Count);

      --  Otherwise allocate new shared string and fill it

      else
         DR := Allocate (Count);
         Common (SR, DR, Count);
         Source.Reference := DR;
         Unreference (SR);
      end if;
   end Tail;

   ---------------
   -- To_String --
   ---------------

   function To_String (Source : Unbounded_String) return String is
   begin
      return Source.Reference.Data (1 .. Source.Reference.Last);
   end To_String;

   -------------------------
   -- To_Unbounded_String --
   -------------------------

   function To_Unbounded_String (Source : String) return Unbounded_String is
      DR : Shared_String_Access;

   begin
      if Source'Length = 0 then
         DR := Empty_Shared_String'Access;

      else
         DR := Allocate (Source'Length);
         DR.Data (1 .. Source'Length) := Source;
         DR.Last := Source'Length;
      end if;

      return (AF.Controlled with Reference => DR);
   end To_Unbounded_String;

   function To_Unbounded_String (Length : Natural) return Unbounded_String is
      DR : Shared_String_Access;

   begin
      if Length = 0 then
         DR := Empty_Shared_String'Access;

      else
         DR := Allocate (Length);
         DR.Last := Length;
      end if;

      return (AF.Controlled with Reference => DR);
   end To_Unbounded_String;

   ---------------
   -- Translate --
   ---------------

   function Translate
     (Source  : Unbounded_String;
      Mapping : Maps.Character_Mapping) return Unbounded_String
   is
      SR : constant Shared_String_Access := Source.Reference;
      DR : Shared_String_Access;

   begin
      --  Nothing to translate, reuse empty shared string

      if SR.Last = 0 then
         DR := Empty_Shared_String'Access;

      --  Otherwise, allocate new shared string and fill it

      else
         DR := Allocate (SR.Last);

         for J in 1 .. SR.Last loop
            DR.Data (J) := Value (Mapping, SR.Data (J));
         end loop;

         DR.Last := SR.Last;
      end if;

      return (AF.Controlled with Reference => DR);
   end Translate;

   procedure Translate
     (Source  : in out Unbounded_String;
      Mapping : Maps.Character_Mapping)
   is
      SR : constant Shared_String_Access := Source.Reference;
      DR : Shared_String_Access;

   begin
      --  Nothing to translate

      if SR.Last = 0 then
         null;

      --  Try to reuse shared string

      elsif Can_Be_Reused (SR, SR.Last) then
         for J in 1 .. SR.Last loop
            SR.Data (J) := Value (Mapping, SR.Data (J));
         end loop;

      --  Otherwise, allocate new shared string

      else
         DR := Allocate (SR.Last);

         for J in 1 .. SR.Last loop
            DR.Data (J) := Value (Mapping, SR.Data (J));
         end loop;

         DR.Last := SR.Last;
         Source.Reference := DR;
         Unreference (SR);
      end if;
   end Translate;

   function Translate
     (Source  : Unbounded_String;
      Mapping : Maps.Character_Mapping_Function) return Unbounded_String
   is
      SR : constant Shared_String_Access := Source.Reference;
      DR : Shared_String_Access;

   begin
      --  Nothing to translate, reuse empty shared string

      if SR.Last = 0 then
         DR := Empty_Shared_String'Access;

      --  Otherwise, allocate new shared string and fill it

      else
         DR := Allocate (SR.Last);

         for J in 1 .. SR.Last loop
            DR.Data (J) := Mapping.all (SR.Data (J));
         end loop;

         DR.Last := SR.Last;
      end if;

      return (AF.Controlled with Reference => DR);

   exception
      when others =>
         Unreference (DR);

         raise;
   end Translate;

   procedure Translate
     (Source  : in out Unbounded_String;
      Mapping : Maps.Character_Mapping_Function)
   is
      SR : constant Shared_String_Access := Source.Reference;
      DR : Shared_String_Access;

   begin
      --  Nothing to translate

      if SR.Last = 0 then
         null;

      --  Try to reuse shared string

      elsif Can_Be_Reused (SR, SR.Last) then
         for J in 1 .. SR.Last loop
            SR.Data (J) := Mapping.all (SR.Data (J));
         end loop;

      --  Otherwise allocate new shared string and fill it

      else
         DR := Allocate (SR.Last);

         for J in 1 .. SR.Last loop
            DR.Data (J) := Mapping.all (SR.Data (J));
         end loop;

         DR.Last := SR.Last;
         Source.Reference := DR;
         Unreference (SR);
      end if;

   exception
      when others =>
         if DR /= null then
            Unreference (DR);
         end if;

         raise;
   end Translate;

   ----------
   -- Trim --
   ----------

   function Trim
     (Source : Unbounded_String;
      Side   : Trim_End) return Unbounded_String
   is
      SR   : constant Shared_String_Access := Source.Reference;
      DL   : Natural;
      DR   : Shared_String_Access;
      Low  : Natural;
      High : Natural;

   begin
      Low := Index_Non_Blank (Source, Forward);

      --  All blanks, reuse empty shared string

      if Low = 0 then
         DR := Empty_Shared_String'Access;

      else
         case Side is
            when Left =>
               High := SR.Last;
               DL   := SR.Last - Low + 1;

            when Right =>
               Low  := 1;
               High := Index_Non_Blank (Source, Backward);
               DL   := High;

            when Both =>
               High := Index_Non_Blank (Source, Backward);
               DL   := High - Low + 1;
         end case;

         --  Length of the result is the same as length of the source string,
         --  reuse source shared string.

         if DL = SR.Last then
            Reference (SR);
            DR := SR;

         --  Otherwise, allocate new shared string

         else
            DR := Allocate (DL);
            DR.Data (1 .. DL) := SR.Data (Low .. High);
            DR.Last := DL;
         end if;
      end if;

      return (AF.Controlled with Reference => DR);
   end Trim;

   procedure Trim
     (Source : in out Unbounded_String;
      Side   : Trim_End)
   is
      SR   : constant Shared_String_Access := Source.Reference;
      DL   : Natural;
      DR   : Shared_String_Access;
      Low  : Natural;
      High : Natural;

   begin
      Low := Index_Non_Blank (Source, Forward);

      --  All blanks, reuse empty shared string

      if Low = 0 then
         Source.Reference := Empty_Shared_String'Access;
         Unreference (SR);

      else
         case Side is
            when Left =>
               High := SR.Last;
               DL   := SR.Last - Low + 1;

            when Right =>
               Low  := 1;
               High := Index_Non_Blank (Source, Backward);
               DL   := High;

            when Both =>
               High := Index_Non_Blank (Source, Backward);
               DL   := High - Low + 1;
         end case;

         --  Length of the result is the same as length of the source string,
         --  nothing to do.

         if DL = SR.Last then
            null;

         --  Try to reuse existing shared string

         elsif Can_Be_Reused (SR, DL) then
            SR.Data (1 .. DL) := SR.Data (Low .. High);
            SR.Last := DL;

         --  Otherwise, allocate new shared string

         else
            DR := Allocate (DL);
            DR.Data (1 .. DL) := SR.Data (Low .. High);
            DR.Last := DL;
            Source.Reference := DR;
            Unreference (SR);
         end if;
      end if;
   end Trim;

   function Trim
     (Source : Unbounded_String;
      Left   : Maps.Character_Set;
      Right  : Maps.Character_Set) return Unbounded_String
   is
      SR   : constant Shared_String_Access := Source.Reference;
      DL   : Natural;
      DR   : Shared_String_Access;
      Low  : Natural;
      High : Natural;

   begin
      Low := Index (Source, Left, Outside, Forward);

      --  Source includes only characters from Left set, reuse empty shared
      --  string.

      if Low = 0 then
         DR := Empty_Shared_String'Access;

      else
         High := Index (Source, Right, Outside, Backward);
         DL   := Integer'Max (0, High - Low + 1);

         --  Source includes only characters from Right set or result string
         --  is empty, reuse empty shared string.

         if High = 0 or else DL = 0 then
            DR := Empty_Shared_String'Access;

         --  Otherwise, allocate new shared string and fill it

         else
            DR := Allocate (DL);
            DR.Data (1 .. DL) := SR.Data (Low .. High);
            DR.Last := DL;
         end if;
      end if;

      return (AF.Controlled with Reference => DR);
   end Trim;

   procedure Trim
     (Source : in out Unbounded_String;
      Left   : Maps.Character_Set;
      Right  : Maps.Character_Set)
   is
      SR   : constant Shared_String_Access := Source.Reference;
      DL   : Natural;
      DR   : Shared_String_Access;
      Low  : Natural;
      High : Natural;

   begin
      Low := Index (Source, Left, Outside, Forward);

      --  Source includes only characters from Left set, reuse empty shared
      --  string.

      if Low = 0 then
         Source.Reference := Empty_Shared_String'Access;
         Unreference (SR);

      else
         High := Index (Source, Right, Outside, Backward);
         DL   := Integer'Max (0, High - Low + 1);

         --  Source includes only characters from Right set or result string
         --  is empty, reuse empty shared string.

         if High = 0 or else DL = 0 then
            Source.Reference := Empty_Shared_String'Access;
            Unreference (SR);

         --  Try to reuse existing shared string

         elsif Can_Be_Reused (SR, DL) then
            SR.Data (1 .. DL) := SR.Data (Low .. High);
            SR.Last := DL;

         --  Otherwise, allocate new shared string and fill it

         else
            DR := Allocate (DL);
            DR.Data (1 .. DL) := SR.Data (Low .. High);
            DR.Last := DL;
            Source.Reference := DR;
            Unreference (SR);
         end if;
      end if;
   end Trim;

   ---------------------
   -- Unbounded_Slice --
   ---------------------

   function Unbounded_Slice
     (Source : Unbounded_String;
      Low    : Positive;
      High   : Natural) return Unbounded_String
   is
      SR : constant Shared_String_Access := Source.Reference;
      DL : Natural;
      DR : Shared_String_Access;

   begin
      --  Check bounds

      if Low - 1 > SR.Last or else High > SR.Last then
         raise Index_Error;

      --  Result is empty slice, reuse empty shared string

      elsif Low > High then
         DR := Empty_Shared_String'Access;

      --  Otherwise, allocate new shared string and fill it

      else
         DL := High - Low + 1;
         DR := Allocate (DL);
         DR.Data (1 .. DL) := SR.Data (Low .. High);
         DR.Last := DL;
      end if;

      return (AF.Controlled with Reference => DR);
   end Unbounded_Slice;

   procedure Unbounded_Slice
     (Source : Unbounded_String;
      Target : out Unbounded_String;
      Low    : Positive;
      High   : Natural)
   is
      SR : constant Shared_String_Access := Source.Reference;
      TR : constant Shared_String_Access := Target.Reference;
      DL : Natural;
      DR : Shared_String_Access;

   begin
      --  Check bounds

      if Low - 1 > SR.Last or else High > SR.Last then
         raise Index_Error;

      --  Result is empty slice, reuse empty shared string

      elsif Low > High then
         Target.Reference := Empty_Shared_String'Access;
         Unreference (TR);

      else
         DL := High - Low + 1;

         --  Try to reuse existing shared string

         if Can_Be_Reused (TR, DL) then
            TR.Data (1 .. DL) := SR.Data (Low .. High);
            TR.Last := DL;

         --  Otherwise, allocate new shared string and fill it

         else
            DR := Allocate (DL);
            DR.Data (1 .. DL) := SR.Data (Low .. High);
            DR.Last := DL;
            Target.Reference := DR;
            Unreference (TR);
         end if;
      end if;
   end Unbounded_Slice;

   -----------------
   -- Unreference --
   -----------------

   procedure Unreference (Item : not null Shared_String_Access) is

      procedure Free is
        new Ada.Unchecked_Deallocation (Shared_String, Shared_String_Access);

      Aux : Shared_String_Access := Item;

   begin
      if Aux = Empty_Shared_String'Access then
         return;
      end if;

      if System.Atomic_Counters.Decrement (Aux.Counter) then
         Free (Aux);
      end if;
   end Unreference;

end Ada.Strings.Unbounded;
