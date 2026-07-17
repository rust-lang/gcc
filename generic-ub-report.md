# GCC GENERIC tree codes: UB vs. defined behavior, as interpreted by GCC itself

**Batch 1** — arithmetic, pointer, shift, and division tree codes, plus five named edge cases.
Status: **Batch 1 complete** — all six stages done; every stage's citations spot-checked against the tree. Remaining ~230 tree codes await methodology sign-off before proceeding (see plan).

## 0. Methodology and provenance

Analyzed checkout: GCC **17.0.0 experimental** (master line), tree at `/home/bouanto/tests/gcc/gcc`, branch `fix/try-catch-refactor` (local changes confined to `gcc/jit/`; all middle-end evidence below reflects upstream master). All file references use this tree's paths and the modern `.cc` file spellings.

Analysis done on 2416b96f411c932a757767a593b09a6a01b63802 (upstream: ca3810f001338bb4d15da43160e017d453f234e8)

"UB as interpreted by GCC" is established here by three signals, in decreasing strength:

1. **Optimizer reliance** (Stage 3): a transform that would be unsound if the edge case occurred. This is the primary signal — it means GCC *acts* on the assumption.
2. **UBSan instrumentation** (Stage 2): the operation gets an opt-in runtime check — documented, high-confidence UB, but instrumentation alone doesn't prove the optimizer exploits it.
3. **Documentation** (Stage 1): `gcc/tree.def` comments and `gcc/doc/generic.texi`. Weakest signal; several real UB assumptions are undocumented, and some documented "undefined" cases are not exploited.

Every citation below is `file:line` in this checkout plus a verbatim quote; citations were spot-checked against the files after each research stage. Claims that failed verification were dropped or moved to §4 (flagged for manual review).

### 0.1 The flag machinery (Stage 4 background)

Overflow UB-ness is not decided per-pass; it is centralized in three predicates in `gcc/tree.h`, which every fold/range/loop pass queries:

- `gcc/tree.h:987` `TYPE_OVERFLOW_WRAPS (TYPE)` — true for pointer types iff `flag_wrapv_pointer`; for integral types iff unsigned or `flag_wrapv`.
- `gcc/tree.h:995` `TYPE_OVERFLOW_UNDEFINED (TYPE)` — the optimizer's license: *"We may optimize on the assumption that values in the type never overflow."* True for pointers iff `!flag_wrapv_pointer`; for integers iff signed `&& !flag_wrapv && !flag_trapv`.
- `gcc/tree.h:1007` `TYPE_OVERFLOW_SANITIZED (TYPE)` — true when `-fsanitize=signed-integer-overflow` is active and the type doesn't wrap; used to *suppress* UB-exploiting folds so the check isn't optimized away before ubsan sees it.

Consequences worth stating up front:

| Flag | Default | Effect on UB status |
|---|---|---|
| `-fwrapv` | off (`common.opt:3662`, no `Init`) | on: signed integer overflow becomes defined wrapping; `TYPE_OVERFLOW_UNDEFINED` goes false for integers |
| `-fwrapv-pointer` | off | on: pointer arithmetic wrap becomes defined |
| `-ftrapv` | off (`common.opt:3229`) | on: signed overflow is a defined trap; also kills `TYPE_OVERFLOW_UNDEFINED` |
| `-fstrict-overflow` | n/a | **no longer a real flag**: `common.opt:3118` — `Common` with no `Var`; "Negated as -fwrapv -fwrapv-pointer". Pure sugar in GCC 17. |
| `-fstrict-aliasing` | off at -O0/-O1, on at -O2+ (`common.opt:3114`, no `Init`; enabled via opts machinery) | off: TBAA-based disambiguation disabled |
| `-fdelete-null-pointer-checks` | `Init(-1)` = target-resolved (`common.opt:1409`); effectively on for mainstream targets, off where address 0 is valid (e.g. AVR) | off: "deref implies non-null" reasoning disabled |
| `-fsanitize=signed-integer-overflow` etc. | off | on: `TYPE_OVERFLOW_SANITIZED` suppresses the corresponding UB-exploiting folds |
| `-fnon-call-exceptions` | off | on: possibly-trapping memory ops/divisions become EH-relevant, restricting speculation and dead-code deletion |

## 1. Per-tree-code entries

Entry schema — (a) documented semantics; (b) UB verdict; (c) evidence (Stages 2–4); (d) confidence.

### 1.1 PLUS_EXPR / MINUS_EXPR / MULT_EXPR (signed integer case)

**(a) Documented.** `tree.def:719-722` defines them bare ("Simple arithmetic."). `generic.texi:1545-1546`: *"The behavior of these operations on signed arithmetic overflow is controlled by the `flag_wrapv` and `flag_trapv` variables."* So the documentation itself makes overflow semantics flag-dependent, not absolutely undefined.

**(b) Verdict.** **Conditionally UB**: signed overflow is UB iff `TYPE_OVERFLOW_UNDEFINED` holds (signed type, no `-fwrapv`, no `-ftrapv`); unsigned overflow is always defined wrapping (no unsigned-overflow sanitizer even exists, §2). Per-operation, `-fsanitize=signed-integer-overflow` suppresses the *folding* exploitation via `TYPE_OVERFLOW_SANITIZED` — but notably **not** the range/loop-analysis exploitation (see caveat below).

**(c) Evidence.**
- UBSan instruments exactly these codes with `IFN_UBSAN_CHECK_ADD/SUB/MUL` (§2, `ubsan.cc:1661-1695`).
- Loop iteration analysis — the strongest exploitation: `tree-ssa-loop-niter.cc:5347-5357` `nowrap_type_p`: *"Returns true if the arithmetics in TYPE can be assumed not to wrap"* — true for `ANY_INTEGRAL_TYPE_P && TYPE_OVERFLOW_UNDEFINED` (verified). Feeds `scev_probably_wraps_p` (`:5629`), `infer_loop_bounds_from_signedness` (`:4491-4520`, records non-wrapping IV bounds purely from signedness), and `number_of_iterations_ne_max` (`:915-925`, "exit reached before overflow" gated `TYPE_OVERFLOW_UNDEFINED && multiple_of_p`).
- Algebraic folding: `(t*u)/u → t` — `match.pd:1046-1049` gated `TYPE_OVERFLOW_UNDEFINED (type) && !TYPE_OVERFLOW_SANITIZED (type)` (verified); same gate family for `(t*u)/v` and `(t*u)/(t*v)` (`match.pd:1065/1079`). Comparison rewriting `X±C1 CMP Y±C2 → X CMP Y±(C2∓C1)`: `fold-const.cc:10129-10131` gated `TYPE_OVERFLOW_UNDEFINED`.
- Range computation: `range-op.cc:965-983` `value_range_with_overflow` — for non-wrapping types an overflowing bound *saturates*, and if both bounds overflow the same way *"the result is undefined"* (`r.set_undefined ()` — the empty range, i.e. "this can't happen"). `operator_mult` treats overflow as saturation only under `TYPE_OVERFLOW_UNDEFINED` (`range-op.cc:2286-2296`) and refuses to solve `0 = op1*N` for wrapping types (`:2252-2256`). Symbolic comparisons `NAME+CST` assume no overflow: `tree-vrp.cc:567-569`.
- Non-zero deduction: `fold-const.cc:15066-15085` `tree_binary_nonzero_p` — nonneg+nonzero can't wrap to 0, gated `TYPE_OVERFLOW_UNDEFINED`.
- IV optimization: `tree-ssa-loop-ivopts.cc:2771-2781` reassociates signed address arithmetic by converting to unsigned, licensed by `TYPE_OVERFLOW_UNDEFINED`.
- Care not to *introduce* UB: `fold-const.cc:11526-11532` (PR83269) blocks `(-A)-B → (-B)-A` when the rewrite could create overflow in a type where it's undefined — GCC treats UB as one-directional license (may assume absent; must not manufacture).

**Caveat (verified negative result):** `TYPE_OVERFLOW_SANITIZED` gates exist only in `match.pd`/`fold-const.cc`. `tree-ssa-loop-niter.cc`, `tree-scalar-evolution.cc`, `range-op.cc`, `tree-vrp.cc`, `tree-ssa-loop-ivopts.cc` contain zero occurrences — so even under `-fsanitize=signed-integer-overflow`, ranger/niter still assume signed IVs don't wrap.

**(d) Confidence.** High (multiple independent passes, all citations verified).

### 1.2 NEGATE_EXPR

**(a) Documented.** `generic.texi:1380-1381`: overflow behavior "controlled by the `flag_wrapv` and `flag_trapv` variables". Edge case: `-INT_MIN`.

**(b) Verdict.** **Conditionally UB** at `-INT_MIN`, same gating as §1.1 (`TYPE_OVERFLOW_UNDEFINED`); fold-level exploitation additionally suppressed by `TYPE_OVERFLOW_SANITIZED`.

**(c) Evidence.** `negate_expr_p` allows folding through negation for signed types via `fold-const.cc:395-396`: `case NEGATE_EXPR: return !TYPE_OVERFLOW_SANITIZED (type);` (verified) — i.e. `-(-x) → x` style rewrites assume `-INT_MIN` doesn't occur, disabled only when sanitizing. Constant negation refuses only literal `INT_MIN` (`may_negate_without_overflow_p`, `fold-const.cc:363`). `X / -X → -1` assumes `-X` doesn't overflow, gated `TYPE_OVERFLOW_UNDEFINED` (`match.pd:602-609`). UBSan lowers `NEGATE_EXPR` to `UBSAN_CHECK_SUB (0, u)` (`ubsan.cc:1705`, §2). In ranger, `operator_negate` delegates to `MINUS_EXPR (0 - X)` (`range-op.cc:4799-4802`), inheriting §1.1's saturation-under-UB semantics (agent-verified negative result: no negate-specific rule).

**(d) Confidence.** High.

### 1.3 ABS_EXPR (integer case) and ABSU_EXPR contrast

**(a) Documented.** `tree.def:790-794`: operand and result have the same type — so `ABS_EXPR(INT_MIN)` cannot be represented without overflow. `tree.def:796-799` / `generic.texi:1397-1400`: ABSU_EXPR returns the *unsigned* type "such that `ABSU_EXPR` of `TYPE_MIN` is well defined" — the documentation itself frames ABSU as the defined-at-INT_MIN variant, implying ABS at INT_MIN is the UB case.

**(b) Verdict.** `ABS_EXPR(INT_MIN)`: **conditionally UB** (`TYPE_OVERFLOW_UNDEFINED`); with `-fwrapv` it is defined to yield `INT_MIN` and the optimizer stops claiming `abs(x) >= 0`. `ABSU_EXPR`: **definitely defined** at `INT_MIN`, unconditionally.

**(c) Evidence.** `fold-const.cc:14497-14503` `tree_unary_nonnegative_warnv_p`: *"We can't return 1 if flag_wrapv is set because ABS_EXPR<INT_MIN> = INT_MIN"* — nonnegativity claimed only under `TYPE_OVERFLOW_UNDEFINED`. Ranger `operator_abs::wi_fold` `range-op.cc:4639-4647` (verified): if overflow is *not* undefined and `INT_MIN` is in the input range, the result goes `varying`; under UB-overflow the tight nonnegative range is kept; symmetric logic in `op1_range` (`:4719-4724`) re-adds the `INT_MIN` preimage only when overflow is defined. `(max(x,0) + max(-x,0)) → abs(x)` gated `TYPE_OVERFLOW_UNDEFINED` (`match.pd:517-519`). UBSan instruments ABS under `signed-integer-overflow` (`ubsan.cc:1709-1716`). **ABSU contrast** (agent-verified negative result): no `TYPE_OVERFLOW_*` macro appears in any ABSU path — `match.pd:212-219` narrows widened `abs` to `absu` unconditionally; `operator_absu::wi_fold` (`range-op.cc:4763-4778`) computes `wi::abs` into the unsigned type with no overflow test.

**(d) Confidence.** High.

### 1.4 POINTER_PLUS_EXPR

**(a) Documented.** `tree.def:724-726`: first operand a pointer, second a `sizetype` integer. `generic.texi:1522-1526` adds only that it's one of two binary ops allowed on pointers. **Nothing documented** about staying within an object or about wrapping — any UB status is purely emergent from pass behavior (Stage 3B) and ubsan's `pointer-overflow` check (Stage 2).

**(b) Verdict.** Two distinct UB facets with different gating:
1. **Wrapping past the address space: conditionally UB** — undefined iff `!flag_wrapv_pointer` (`TYPE_OVERFLOW_UNDEFINED` on pointer types, `tree.h:995-999`).
2. **Leaving the underlying object (cross-object arithmetic): unconditionally UB** — exploited by points-to and alias disambiguation with **no flag to turn it off**. No option makes `&a + huge_offset` landing in `b` defined.
Additionally, "nonzero offset ⇒ result non-null" reasoning is gated on `flag_delete_null_pointer_checks`.

**(c) Evidence.**
- Cross-object (unconditional): points-to treats `p + off` as pointing into *the same variable* `p` points to — `gimple-ssa-pta-constraints.cc:465-469` (verified): *"we include at least the last or the first field of the variable to represent reachability of off-bound addresses, in particular &object + 1, conservatively correct"* — one-past-end is modeled; farther is not. Offset-based disambiguation: `tree-ssa-alias.cc:2148-2155` (verified): *"the pointer base cannot validly point to an offset less than zero of the variable"* → a `MEM_REF [p + off]` is declared non-aliasing with a decl the offset would undershoot. `pointer-query.cc:884-899`: zero-based object offsets clamped to `[0, size]`, one-past-end allowed.
- No-wrap (flag-gated): `range-op-ptr.cc:361-366` (verified): result set non-zero when base or offset excludes 0, gated `!TYPE_OVERFLOW_WRAPS (type) && (flag_delete_null_pointer_checks || !wi::sign_mask (rh_ub))` — with an explicit 15-line comment (`:346-360`, verified) constructing the `-fno-delete-null-pointer-checks` counterexample (`&a[6] - 6` when `&a[0]` is address 0). Relation derivation (`LHS > OP1` from positive offset) at `range-op-ptr.cc:404-424`: *"Any overflow is considered UB and thus ignored"*, gated `TYPE_OVERFLOW_UNDEFINED`. Overflow-check fusion `(p+cst <= q) | (q+cst <= p)` gated `TYPE_OVERFLOW_UNDEFINED` (`match.pd:11590-11592`).
- Algebraic (unconditional, but wrap-agnostic): reassociation `(p +p o1) +p o2 → p +p (o1+o2)` (`match.pd:3232-3235`, verified); `p+a == p+b → a == b` (`match.pd:2986-2989`); `(p +p off) == p → off == 0` (`match.pd:3072-3077`).
- UBSan: `pointer-overflow` check on nonzero offsets (§2); zero-offset checks deleted.

**(d) Confidence.** High. (For the *scope* of "one past the end is OK, farther is not": high — stated directly in the PTA comment and pointer-query clamps.)

### 1.5 POINTER_DIFF_EXPR

**(a) Documented.** `tree.def:728-734`: *"Behavior is undefined if the difference does not fit in the result type."* (also `generic.texi:1528-1534`, which spells out the infinite-precision interpretation). One of only two batch-1 codes with UB stated in the IR definition itself. Note the raw difference is **not** divided by element size — the division is a separate `EXACT_DIV_EXPR` produced by the front end (`c/c-typeck.cc:5444-5446`, `cp/typeck.cc:7309`, both verified).

**(b) Verdict.** Cross-object difference: **unconditionally UB** (same-object assumption exploited, no gating flag). "Difference doesn't fit the signed result type": **documented UB**, but no transform was found that exploits *that specific* facet directly — treat as documented-but-thinly-evidenced (see §4).

**(c) Evidence.** `fold-const.cc:16225-16228` `ptr_difference_const`: `&e1 - &e2` folds to a constant only when both split to the *same* core object (`operand_equal_p (core1, core2)`). `match.pd:3341-3352` folds `(&a +p b) - (&a[1] +p c)` through `ptr_difference_const`; `p+a - p → a` and `(p+a)-(p+b) → a-b` cancel the base symbolically (`match.pd:2990-2996, 4196-4204`). Ranger: `range-op-ptr.cc:498-500` (verified): *"if op1 and op2 point to the same object, the diff is 0"* — `pt_invariant_p` collapses the range to zero, unsound if the two pointers could validly address different objects with different addresses.

**(d) Confidence.** High for the same-object facet; low/flagged for the "doesn't fit" facet (documentation only).

### 1.6 MEM_REF / INDIRECT_REF

**(a) Documented.** `INDIRECT_REF` `tree.def:485-486`: "C unary `*`" — GENERIC-only; gimplification lowers it to `MEM_REF`. **Ambiguity flag (Stage 1):** MEM_REF (`tree.def:1122-1130`) is defined in tree.def and usable in late GENERIC but is primarily the GIMPLE dereference form; both are in scope here since their semantics are shared. MEM_REF's second operand is a constant offset whose *type* carries the TBAA interpretation ("used for TBAA purposes", `tree.def:1124-1125`). Nothing documented about null or validity of the address.

**(b) Verdict.** Three facets:
1. **Null dereference: conditionally UB** — exploited iff `flag_delete_null_pointer_checks` (target-resolved default, §0.1) and the address space doesn't allow address 0.
2. **Out-of-object / wild dereference: UB**, but exploited *conservatively*: GCC uses it for path isolation and check deletion, yet does **not** freely speculate possibly-trapping loads (the trap model restrains hoisting).
3. **Type-punned access (TBAA violation): conditionally UB** — iff `flag_strict_aliasing` (on at -O2+).

**(c) Evidence.**
- Deref ⇒ non-null: `gimple-range-infer.cc:246-250` — every load/store's base pointer gets a non-zero range for the rest of the region, gated `flag_delete_null_pointer_checks` and `!targetm.addr_space.zero_address_valid` (`:66-74`). Whole nonnull-inference machinery hard-gated the same way: `gimple.cc:3216-3221` (verified): *"We can only assume that a pointer dereference will yield non-NULL if -fdelete-null-pointer-checks is enabled."*
- Path isolation: `gimple-ssa-isolate-paths.cc:288-299` replaces a statically-proven `*NULL` path with `__builtin_trap` — gated `flag_isolate_erroneous_paths_dereference`, enabled at -O2+ (`opts.cc:659`, verified).
- Trap model bounding speculation: `tree-eh.cc:2830-2873` `tree_could_trap_p` — `MEM_REF`/`INDIRECT_REF` trap unless `TREE_THIS_NOTRAP` / provably in-object; `-fnon-call-exceptions` widens what counts as throwing. LICM only moves such loads preserving execution: `tree-ssa-loop-im.cc:411-413` (`gimple_could_trap_p` ⇒ `MOVE_PRESERVE_EXECUTION`). So "deref UB" is *not* used to hoist loads past their guards.
- TBAA: oracle collapses without the flag — `alias.cc:405-407` (`flag_strict_aliasing && !alias_sets_conflict_p`), `:553-554`, `:717-719`; GIMPLE-level disambiguation `tree-ssa-alias.cc:2191-2193, 2368-2371, 2553-2557` all gated `flag_strict_aliasing`/`tbaa_p`. The exploited assumption: an access through one effective type never overlaps an access through an incompatible one.
- UBSan: `null` + `alignment` checks on MEM_REF (§2).

**(d) Confidence.** High.

### 1.7 LSHIFT_EXPR / RSHIFT_EXPR

**(a) Documented.** `tree.def:801-815` and `generic.texi:1493-1494`, identically: *"the result is undefined if the second operand is larger than or equal to the first operand's type size."* This is **narrower than C's UB**: nothing in the IR documentation makes signed-left-shift overflow or left-shifting a negative value undefined; and it says "result is undefined" (an unspecified *value*), which is weaker language than full UB — whether passes treat it as value-undefined or behavior-undefined is a Stage 3C question. Negative shift counts are not mentioned.

**(b) Verdict.** The most nuanced entry in batch 1 — three separate rulings:
1. **Count ≥ precision: value-undefined, exploited as "pick a convenient value" (0), never as behavior-UB.** Folds choose 0 (which disagrees with what most hardware would compute, e.g. x86 count masking — that divergence *is* the exploitation), ranges are computed assuming in-range counts, but no pass deletes paths or back-infers `count < prec`.
2. **Negative count: refused/inconsistent, not exploited.** Constant folding declines; some range code silently reinterprets it as an opposite-direction shift (flagged in §4).
3. **Signed left-shift overflow / shifting a negative value left: treated as fully defined (wrapping) by the middle end.** Only ubsan's `shift-base` check (a front-end, language-version-gated construct — off for C++20) calls it UB. This is a case where GCC's IR semantics are *wider* (more defined) than C's.

**(c) Evidence.**
- Oversized-count value choice: `match.pd:1344-1352` (verified) folds constant `count >= prec` shifts to `build_zero_cst`, gated `(GIMPLE || !sanitize_flags_p (SANITIZE_SHIFT_EXPONENT))` and deliberately *"Leave arithmetic right shifts of possibly negative values alone"*. The underlying `wide-int.h` shift helpers return 0 / sign-mask for `count >= prec` (`wide-int.h:3581-3586, 3629-3634`). Two-shift combination requires each count in `[0, prec)` and gives combined-overshoot a defined result (`match.pd:5223-5236`).
- Ranges assume in-range counts: `get_shift_range` intersects the count range with `[0, prec-1]` (`range-op.cc:591-594`); if nothing survives, the *result is set to zero* (`range-op.cc:2743-2749`, verified) — consistent with the fold, unsound vs. hardware masking semantics.
- Not behavior-UB: **no `op2_range`** exists for the shift operators (agent-verified negative result, `range-op.cc:2659-2719`) — VRP never concludes `c < prec` from `x << c`; `op1_range` just bails for out-of-range counts (`range-op.cc:2875-2880, 2940-2945`). CCP is conservative (`tree-ssa-ccp.cc:1637-1651`). But GCC *avoids introducing* count==prec itself: `tree-ssa-forwprop.cc:3109-3113` (verified): *"The above sequence isn't safe for Y being 0, because then one of the shifts triggers undefined behavior"* (rotate recognition demands the masked form).
- Negative count: `fold-const.cc:992-1004` (verified) — `if (wi::neg_p (arg2)) return false;` for both directions, with the remarkable comment *"It's unclear from the C standard whether shifts can overflow. The following code ignores overflow; perhaps a C standard interpretation ruling is needed."*
- Signed-lshift-overflow-is-wrapping: the only shift reassociation `(X + Y) << C`-style fold requires `TYPE_OVERFLOW_WRAPS` (`match.pd:1359-1362`); agent grep confirmed **no** shift transform in `match.pd`/`fold-const.cc` is gated on `TYPE_OVERFLOW_UNDEFINED` — there is nothing for `-fwrapv` to turn off. UBSan's `shift-base` check is skipped when `TYPE_OVERFLOW_WRAPS (type0)` or `cxx_dialect >= cxx20` (`c-ubsan.cc:195-201`, §2).
- `SHIFT_COUNT_TRUNCATED` is RTL-only, default 0 (`defaults.h:1061-1062`); no GIMPLE pass consults it (agent-verified negative).

**(d) Confidence.** High for all three rulings (each supported by both positive citations and verified negative sweeps).

### 1.8 TRUNC/CEIL/FLOOR/ROUND_DIV_EXPR

**(a) Documented.** `tree.def:742-752`, `generic.texi:1564-1576`: rounding directions; C/C++ use `TRUNC_DIV_EXPR`. Overflow (`INT_MIN / -1`) "controlled by the `flag_wrapv` and `flag_trapv` variables" (`generic.texi:1574-1576`). **Division by zero is not documented at all** — its UB status is purely emergent (Stage 3D).

**(b) Verdict.**
1. **Divisor == 0: UB with no opt-out flag**, but with a distinctive style of exploitation: GCC *deletes and narrows* (dead divisions removed, zero excluded from divisor ranges, `X/X → 1`) yet **never materializes a value** for `x/0` (constant folding refuses; `0/0` deliberately kept for diagnostics). Observability is restored only by `-fnon-call-exceptions` (the trap becomes an exception source and the UB-folds gain nonzero-proof guards).
2. **`INT_MIN / -1`: conditionally UB** via `TYPE_OVERFLOW_UNDEFINED` in range computation — **and `-ftrapv` does *not* protect it** (verified: the trap model ignores `honor_trapv` for division).
All four rounding variants share these rules via `match.pd` iterators and a shared range-op entry.

**(c) Evidence.**
- Executed-division ⇒ divisor≠0 (backward inference): `range-op.cc:2524-2535` (verified) `operator_div::op2_range`: *"Set OP2 to non-zero if the LHS isn't UNDEFINED"* — GORI propagates divisor-nonzero from any division whose result exists. (Nuance, agent-verified: this is GORI back-propagation only; the *on-exit* side-effect inferencer in `gimple-range-infer.cc` registers nothing for divisions — its range-ops path is disabled by default, `gimple-range-infer.cc:252-254`.)
- Range computation skips zero: `operator_div::wi_fold` splits a zero-containing divisor range into `[lb,-1] ∪ [1,ub]` (*"skip any division by zero"*, `range-op.cc:2588-2620`); divisor exactly `{0}` ⇒ `set_undefined` (empty range = "can't happen"). Mod analog: `range-op.cc:4327-4332` (*"Mod 0 is undefined"*).
- Folds assuming runtime nonzero: `X / X → 1` — `match.pd:585-592` (verified): gated `!integer_zerop (@0) && (!flag_non_call_exceptions || tree_expr_nonzero_p (@0))`; `X / bool_range → X` (`match.pd:578-583`, same `-fnon-call-exceptions` guard).
- Deletion of dead maybe-trapping division: `tree-eh.cc:2509-2519` (verified) — all nine div/mod codes possibly-trap iff divisor isn't a nonzero constant; but `stmt_could_throw_p` is false without `-fexceptions` (`tree-eh.cc:3050-3051`), so DCE's throw-based keep-alive (`tree-ssa-dce.cc:499-500`, verified) doesn't fire → a dead `x = a/b` with maybe-zero `b` is silently deleted. Div-by-zero is removable UB, not an observable trap, by default.
- Value never invented: `fold-const.cc:1042-1089` — every constant div/mod arm `if (arg2 == 0) return false;`; `0 / X → 0` keeps the `0/0` case (*"so that we can get the proper warnings and errors"*, `match.pd:567-571`).
- `INT_MIN / -1`: `range-op.cc:2566-2572` — the `-INF / -1 = +INF` overflow is treated as saturation, gated `TYPE_OVERFLOW_UNDEFINED`. `X / -1 → -X`: `match.pd:573-576` (verified) gated **only** `!TYPE_UNSIGNED` — see §4 for the `-ftrapv` wrinkle. Trap model ignores division overflow entirely: the div/mod arm of `operation_could_trap_helper_p` keys solely on the divisor; only PLUS/MINUS/MULT/NEGATE arms consult `honor_trapv` (`tree-eh.cc:2571-2591`, agent-verified negative).
- UBSan: `/0` under `integer-divide-by-zero`; `INT_MIN/-1` under `signed-integer-overflow` (§2).
- Variants: one iterator covers all rounding forms — `match.pd:565` `(for div (trunc_div ceil_div floor_div round_div exact_div) ...)`; shared `operator_div` registration `range-op.cc:2519-2522, 4859-4863`. CEIL/FLOOR forms are generated by layout/OpenMP/niter code with known-nonzero divisors; ROUND/CEIL_MOD essentially never generated.

**(d) Confidence.** High.

### 1.9 TRUNC/CEIL/FLOOR/ROUND_MOD_EXPR

**(a) Documented.** `tree.def:754-766`, `generic.texi:1578-1588`: defined as `a - (a/b)*b` with the corresponding division — so each MOD inherits its DIV's edge cases (zero divisor, `INT_MIN % -1`) by construction. Nothing further documented.

**(b) Verdict.** Same as §1.8: modulo-by-zero UB (no opt-out), `INT_MIN % -1` conditionally UB. One extra exploitation: `X % -1 → 0` *removes* the runtime trap that `INT_MIN % -1` raises on e.g. x86 — a concrete behavioral change licensed by the UB.

**(c) Evidence.** `X % X → 0` gated `!integer_zerop (@0)` + the `-fnon-call-exceptions` nonzero-proof guard (`match.pd:908-912`); `X % 1 → 0` unconditional (`match.pd:899-901`); `X % -1 → 0` gated only `!TYPE_UNSIGNED` (`match.pd:903-906`); `(X * C1) % C2 → 0` when `C1` is a multiple of `C2`, gated `TYPE_OVERFLOW_UNDEFINED` (`match.pd:918-924`); `mod 0` ⇒ empty range (`range-op.cc:4327-4332`); trap/DCE model identical to §1.8 (`tree-eh.cc:2509-2519`). All four variants share the `match.pd:891` iterator; unsigned FLOOR forms are canonicalized to TRUNC (`match.pd:614-620`).

**(d) Confidence.** High.

### 1.10 EXACT_DIV_EXPR

**(a) Documented.** `tree.def:771-773`: "Division which is not supposed to need rounding. Used for pointer subtraction in C." `generic.texi:1590-1594`: numerator "known to be an exact multiple of the denominator", letting the backend pick the cheapest rounding form. The exactness is a *promise made by the IR producer*; a nonzero remainder is the UB edge case unique to this code (plus the DIV edge cases it inherits).

**(b) Verdict.** **Inexact division (remainder ≠ 0) is unconditionally UB** — the exactness license is exploited by unguarded folds with no controlling flag. Plus everything from §1.8.

**(c) Evidence.** `(X /[ex] A) * A → X` — unconditional (`match.pd:5793-5796`). `A /[ex] B CMP C → A CMP B*C` (`match.pd:7885-7915`); `X /[ex] C1 < Y /[ex] C1 ⇔ X < Y` (`match.pd:2859-2881`). Successive-division combining bypasses the profitability/correctness check other div codes need: `div == EXACT_DIV_EXPR || optimize_successive_divisions_p (...)` (`match.pd:644-658`). `fold-const.cc:6826-6832` `extract_muldiv` cancels `MULT∘EXACT_DIV` under `TYPE_OVERFLOW_UNDEFINED && !TYPE_OVERFLOW_SANITIZED`. Producers: C/C++ pointer subtraction (`c/c-typeck.cc:5444-5446`, `cp/typeck.cc:7309`, verified — tying this code's exactness promise to POINTER_DIFF's same-object UB, §1.5) and loop-niter results with `control.no_overflow = true` (`tree-ssa-loop-niter.cc:1092-1093`).

**(d) Confidence.** High.

## 2. Stage 2 — UBSan instrumentation map

Everything below is opt-in (no check is inserted without `-fsanitize=...`); "in `undefined`" means the bare `-fsanitize=undefined` umbrella enables it. Umbrella membership: `flag-types.h:346-355` (`SANITIZE_UNDEFINED = SANITIZE_SHIFT | SANITIZE_DIVIDE | ... | SANITIZE_POINTER_OVERFLOW | SANITIZE_BUILTIN`); `float-divide-by-zero`, `float-cast-overflow`, `bounds-strict` are `SANITIZE_UNDEFINED_NONDEFAULT` — instrumented only when named explicitly. *(Citations verified by spot-check.)*

| Operation / tree code | Instrumenter | Runtime condition checked | Sub-flag | In `undefined`? |
|---|---|---|---|---|
| `PLUS/MINUS/MULT_EXPR` signed overflow | `ubsan.cc:1661` `instrument_si_overflow` → `IFN_UBSAN_CHECK_ADD/SUB/MUL` | result wraps | `signed-integer-overflow` | yes |
| `NEGATE_EXPR` | same, lowered as `UBSAN_CHECK_SUB (0, u)` (`ubsan.cc:1705`) | `-INT_MIN` | `signed-integer-overflow` | yes |
| `ABS_EXPR` | same (`ubsan.cc:1709-1716`) | `abs(INT_MIN)` | `signed-integer-overflow` | yes |
| `TRUNC_DIV/MOD_EXPR`, divisor 0 | `c-ubsan.cc:40` `ubsan_instrument_division`, gate at `:56-59` | `op1 == 0` | `integer-divide-by-zero` | yes |
| `TRUNC_DIV_EXPR`, `INT_MIN / -1` | same, `c-ubsan.cc:70-79` (*"We check INT_MIN / -1 only for signed types."*) | `op1 == -1 && op0 == TYPE_MIN` | **`signed-integer-overflow`** (not `-fsanitize=integer-divide-by-zero`!) | yes |
| `LSHIFT/RSHIFT_EXPR` count | `c-ubsan.cc:162` `ubsan_instrument_shift` | `count > prec-1` | `shift-exponent` | yes |
| `LSHIFT_EXPR` base | same, `c-ubsan.cc:195-231` | C99/C++<11: sign-bit shifted into/past; C++11-17: `x<0 \|\| overflow`; **skipped for C++≥20** and when `TYPE_OVERFLOW_WRAPS` | `shift-base` | yes |
| `POINTER_PLUS_EXPR` wrap | `ubsan.cc:1493/1505` → `IFN_UBSAN_PTR` | ptr+off wraps past 0 / address space | `pointer-overflow` | yes |
| `MEM_REF` deref null/misaligned | `ubsan.cc:1443/1478` → `IFN_UBSAN_NULL` | `ptr == 0`; `ptr & (align-1) != 0` | `null` / `alignment` | yes |
| `ARRAY_REF` OOB index | `c-ubsan.cc:424` → `IFN_UBSAN_BOUNDS` | `index >= bound` (`ubsan.cc:787`) | `bounds` (`bounds-strict` adds flex-array) | yes / no |
| VLA bound | `c-ubsan.cc:347` | `size <= 0` | `vla-bound` | yes |
| bool/enum load out of range | `ubsan.cc:1734` | value outside valid range | `bool` / `enum` | yes |
| `nonnull`/`returns_nonnull` violations | `ubsan.cc:2033/2139` | arg/retval null | `nonnull-attribute` etc. | yes |
| `clz/ctz(0)` | `ubsan.cc:2375` | arg == 0 | `builtin` | yes |
| float→int cast overflow | `ubsan.cc:1891` | value outside target range | `float-cast-overflow` | **no** |

Facts with direct bearing on later verdicts (all verified):

- **Zero pointer offset is defined by fiat**: `ubsan_expand_ptr_ifn` deletes the whole check when the offset is zero — `ubsan.cc:1132-1137`: `if (integer_zerop (off)) { gsi_remove (gsip, true); unlink_stmt_vdef (stmt); return true; }`. The front-end helper likewise returns early on zero total offset (`ubsan.cc:1567`). → feeds edge case §3(d).
- **`INT_MIN/-1` is classified as signed-overflow UB, not divide UB** (`c-ubsan.cc:70-79` gated on `SANITIZE_SI_OVERFLOW`).
- **Sanitizing disables the corresponding optimizer exploitation**: `TYPE_OVERFLOW_SANITIZED` (`tree.h:1007-1010`) is checked by folds (see §1 evidence) so checks aren't folded away first.
- **No unsigned-integer-overflow sanitizer exists** in GCC (absent from `opts.cc` sanitizer_opts table) and `instrument_si_overflow` returns early for `TYPE_OVERFLOW_WRAPS` types (`ubsan.cc:1671-1678`, verified quote: *"If this is not a signed operation, don't instrument anything here. Also punt on bit-fields."*) — consistent with unsigned wrap being fully defined.
- **C++≥20 signed-left-shift base is not instrumented** (`c-ubsan.cc:201`) — GCC tracks the language change (P0330-adjacent shift semantics), i.e. shift-base UB-ness is *front-end language-version dependent*, not an IR property.
- Rotates (`LROTATE/RROTATE_EXPR`): only a negative count is flagged (`c-ubsan.cc:181-185`); no exponent≥prec check.

## 3. Stage 6 — named edge cases

### (a) May a single-field store clobber adjacent padding? Is padding preserved across struct copies?

**Verdict.** Two different answers:
- **Single-field stores: no.** A byte-aligned field store is clamped to its own "bit region" under the C++11 memory model and may not write adjacent bytes — padding or otherwise. Exceptions: (i) bits inside the *same bitfield representative region* (which can include inter-bitfield padding bits) are legitimately RMW-clobbered; (ii) non-byte-aligned fields without `DECL_BIT_FIELD_TYPE` (packed Ada layouts) get *no* region restriction; (iii) `-fallow-store-data-races` (on at `-Ofast`) lifts the constraint deliberately.
- **Struct copies: padding is "don't care" content.** An opaque whole-aggregate copy does copy padding verbatim (full-size block move), but GCC reserves and exercises the right to drop it: SRA's total scalarization copies only the fields (padding is explicitly "not copied bits"), and IPA-ICF builds equivalence on exactly that principle. So the *contents* of padding are not preserved across optimization — indeterminate, per GCC's own documentation of `__builtin_clear_padding`.

**Evidence** (all verified). Region clamp: `expr.cc:6106-6114` — byte-aligned field ⇒ `bitregion = [bitpos, bitpos+bitsize-1]`, with the comment *"The C++ memory model naturally applies to byte-aligned fields. However, if we do not have a DECL_BIT_FIELD_TYPE but BITPOS or BITSIZE are not byte-aligned, there is no need to limit the range we can access"* (the Ada escape hatch). `expmed.cc:1199-1203`: *"Under the C++0x memory model, we must not touch bits outside the bit region."* `store_split_bit_field` shrinks the access unit near region edges except on registers (*"there can't be data races on a register"*, `expmed.cc:1441`). Inside-representative RMW: `optimize_bitfield_assignment_op`, `expr.cc:5806` (*"We may be accessing data outside the field, which means we can alias adjacent data"*). GCC even actively avoids clobbering trailing padding in an addressable-field block move: `expr.cc:8315-8321` (*"the block move will not clobber the padding that shouldn't be clobbered"*). Copies: full-size `emit_block_move` (`expr.cc:7005, 6443`); SRA drop: `tree-sra.cc:5311-5313` (*"…or if they have padding (i.e. not copied bits)"*), consumed by `ipa-icf-gimple.cc:393`; store-merging *skips* gap bytes rather than writing them (`gimple-ssa-store-merging.cc:3917-3920`). Padding is unspecified after construction unless `-fzero-init-padding-bits=all` (`expr.cc:7311`); `__builtin_clear_padding` docs: padding bits *"might have indeterminate values"* (`extend.texi:18332-18336`). DSE has **no** padding logic at all (agent-verified negative: zero hits; trimming is liveness-based only, `tree-ssa-dse.cc:405-441`).

**Confidence.** High. The two-part structure (stores bounded; copy contents unstable) is each supported by multiple verified citations including explicit comments.

### (b) Pointer lifetime-end zapping

**Verdict.** GCC's lifetime-end marker is the GIMPLE clobber (`CLOBBER_STORAGE_END` for scope exit / free-like events; `tree-core.h:1077-1091`, verified). GCC's **documented model is full value-indeterminacy** — *"All uses of indeterminate pointers are undefined"*, explicitly including arithmetic, relational, and (at `-Wuse-after-free=3`) equality uses — but its **optimizer exploitation is narrower**: storage reuse and deletion, not pointer-value folding. Concretely UB-exploiting mechanisms: (1) stack-slot sharing gives two non-overlapping-lifetime locals the same address, so a stale pointer physically aliases an unrelated live object; (2) `free()` kills the pointed-to memory for DSE; (3) unused malloc/free (and new/delete) pairs are deleted entirely, making the pointer value unobservable. No pass currently folds `p == q` based on lifetime state (verified negative), and points-to sets are *not* invalidated at clobber/free sites (verified negative).

**Evidence** (all verified). Markers: `clobber_kind` enum distinguishing `CLOBBER_OBJECT_END` (C++ dtor) from `CLOBBER_STORAGE_END` (scope/free), `tree-core.h:1077-1091`; emitted at scope exit only when `flag_stack_reuse != SR_NONE` (`gimplify.cc:1610-1618`). Slot sharing: `cfgexpand.cc:964-970` — live ranges end at *"the end-of-scope death clobber added by gimplify"*; `-fstack-reuse` docs (`invoke.texi:19133-19187`) warn *"Legacy code extending local lifetime is likely to break"* and pose the rhetorical *"What is the value of ap->i?"* for a dangling pointer into a reused slot. free() kills memory: `stmt_kills_ref_p` `BUILT_IN_FREE` case, `tree-ssa-alias.cc:3628-3637`. Allocation DCE: `tree-ssa-dce.cc:1010-1027, 1609-1624`, gated `flag_malloc_dce`/`flag_allocation_dce`. Diagnostic model: `gimple-ssa-warn-access.cc:4250-4253` tracks *"uses of the pointer … including its copies or others derived from it by pointer arithmetic"*; equality uses handled separately (`:4331-4340`); `invoke.texi:8482-8515` — deallocation renders pointers *"indeterminate"*; level 1 covers *"uses in arithmetic and relational expressions"*, level 3 adds equality. Negatives (agent-verified): no `BUILT_IN_FREE` handling in the points-to solver; `address_compare` (`fold-const.cc:16515-16521`, `match.pd:4704/8351`) folds address comparisons from *static object identity* only, never lifetime state.

**Confidence.** High — both for "value-indeterminate is the official model" (documentation is explicit) and for "exploitation today is via storage, not value folding" (explicit negative sweeps).

### (c) Partially-uninitialized union bytes across copies

**Verdict.** **The copy operation itself is defined, byte-exact, and preserving** — a union-typed assignment stays one opaque aggregate move (full `TYPE_SIZE` block copy), VN refuses to synthesize values across uninitialized gaps, and DSE trims only proven-dead bytes. **But uninitialized bytes have no stable identity once optimization looks *inside***: if SRA decomposes the union member-wise (unions are never *totally* scalarized, but member accesses are scalarized), uninitialized parts become fresh default-def SSA names — officially *"the compiler may assume any initial value"* — so two reads of the same uninitialized byte need not agree, and the "same" bytes need not survive a copy chain. GCC's documented stance matches: without `-ftrivial-auto-var-init`, the compiler *"will perform optimization as if the variable were uninitialized"*.

**Evidence** (all verified). Preservation: `expr.cc:7002-7007` BLKmode `emit_block_move` of `expr_size (exp)` (whole type incl. padding/uninit); `store_expr` path `expr.cc:6525`; VN partial-def coverage requirement `tree-ssa-sccvn.cc:2165-2166` (*"Continue looking for partial defs"* — declines, never invents); DSE liveness-only trimming `tree-ssa-dse.cc:405-441`. Instability: unions excluded from total scalarization (`tree-sra.cc:1090-1177`, verified — switch handles only `RECORD_TYPE`/`ARRAY_TYPE`, `default: return false`), but member-wise SRA walks *"just below the outermost union"* (`tree-sra.cc:1974`) and substitutes `get_repl_default_def_ssa_name` for *"an uninitialized part of an aggregate that is being loaded"* (`tree-sra.cc:4744, 4578`); CCP: *"Since 'V' is a local variable, the compiler may assume any initial value for it"* (`tree-ssa-ccp.cc:89-93`, verified) with UNDEFINED-absorbing meet (`:1058-1074`); VN treats reads overlapping a clobber as exploitable UB (*"While technically undefined behavior do not optimize a full read from a clobber"*, `tree-ssa-sccvn.cc:3196`, verified — note even here GCC self-limits). Docs: `invoke.texi:15400-15402` (`-ftrivial-auto-var-init`), `tree-ssa-uninit.cc:47-48` is warning-only.

**Confidence.** High for the mechanics on both sides. Medium for any end-to-end claim about a specific program, since whether bytes are preserved depends on whether SRA/SSA decomposition fires — that's exactly the boundary the evidence draws.

### (d) `p + 0` with null or dangling `p`

**Verdict.** **Definitely defined, unconditionally.** GCC folds `p + 0 → p` with no validity precondition, folds `NULL +p idx → (type)idx` (treating NULL as plain address 0), constant-folds `POINTER_PLUS` as a modular integer add, deletes the ubsan pointer-overflow check for zero offsets by fiat, and preserves points-to through zero offsets. No pass anywhere treats "invalid base + 0" as an exploitable precondition. The only UB-flavored inference from `POINTER_PLUS` concerns *nonzero* offsets (result-nonnull, §1.4), and even that never back-propagates to assert the *base* was valid.

**Evidence** (all verified). `match.pd:260-263`: `(op @0 integer_zerop) → @0` for `pointer_plus` among others — unconditional; `match.pd:265-268`: `0 +p index → (type)index`; `fold-const.cc:1368-1374`: constant `POINTER_PLUS` = `int_const_binop (PLUS_EXPR, ...)` (wrapping arithmetic, no trap); `ubsan.cc:1132-1137`: zero-offset `UBSAN_PTR` removed outright (§2); `range-op-ptr.cc:373-375`: *"If op1 refers to an object, op1 + 0 will also refer to the object"*. Negatives (agent-verified): `pointer_plus_operator` has no `op1_range` (`range-op-ptr.cc:300-321`) — nonnull results never imply nonnull bases; the statement-level inferencer only visits dereferences and call args, never bare `POINTER_PLUS` (`gimple-range-infer.cc:246-250`); no fold distinguishes a dangling base (the only pass caring — `gimple-ssa-warn-access.cc:4401-4409` — is diagnostics, and treats zero and nonzero offsets identically).

**Confidence.** High.

### (e) memcpy/memmove/memset with size 0

**Verdict.** **A zero-size call is a true no-op; the pointers need not be valid or non-null.** This is current, deliberate policy (C2y N3322, PR117023): the mem* builtins no longer carry unconditional `nonnull` — they carry `nonnull_if_nonzero` keyed on the size argument. Every consumer of that attribute (range inference, `-Wnonnull`, ubsan, RTL expansion validation) declines to assume/require non-null when the size is zero or possibly zero. Nonnull *is* still inferred when the size is provably nonzero — and that entire inference family is additionally gated on `flag_delete_null_pointer_checks`. Sub-cases: (i) constant 0 → folded to a plain copy of the dest pointer value, unconditionally; (ii) runtime possibly-zero size → no nonnull assumption (inference requires a proven-nonzero size range); (iii) null + literal 0 → valid, no warning, no sanitizer trap.

**Evidence** (all verified). Attributes: `builtins.def:893-896` — `BUILT_IN_MEMCPY/MEMMOVE/MEMPCPY` = `ATTR_NOTHROW_NONNULL_IF123_LEAF`, `MEMSET` = `..._IF13_LEAF`; expansion in `builtin-attrs.def:256-261`. Inference: `gimple.cc:3284-3297` — `integer_zerop (size)` ⇒ `return false` (no nonnull), proven-nonzero required otherwise; whole routine gated `flag_delete_null_pointer_checks` (`gimple.cc:3216-3221`); same pattern at the call-site inferencer (`gimple-range-infer.cc:210-234`). Folding: `gimple-fold.cc:910-928` — `size_must_be_zero_p (len)` ⇒ replace with `lhs = dest`/nop, no validity residue; memset analog `:1468-1473`; `size_must_be_zero_p` requires the range to be exactly `{0}` (`:870-893`). Expansion guard only rejects null+*nonzero*: `builtins.cc:1166-1172`. Intent tests: `gcc.dg/nonnull-11.c:24-34` (verified) — `__builtin_memcpy (NULL, p, 0)` etc. produce **no** warning while `__builtin_strncat (NULL, s, 0)` still warns (strncat's dest keeps plain `nonnull`); `c-c++-common/ubsan/nonnull-6.c` runs null+0 clean. History: commits `19fe55c4801` / `0d590d21586` (PR117023, N3322), `e33409a8325` (PR120520).

**Confidence.** High — the mechanism is explicit, recent, test-covered, and documented (`extend.texi:3885-3893`: *"it is valid to call my_memcpy2 (NULL, NULL, 0)"*).

## 4. Flagged for manual review

1. **`-Wstrict-overflow` machinery is gone from this tree.** `fold_overflow_warning` / the deferred-overflow-warning machinery has zero hits in `gcc/*.cc`/`*.h` (verified by direct grep), yet `Wstrict-overflow` / `Wstrict-overflow=` remain in `common.opt:783/787`. The fold-time "assuming signed overflow does not occur" diagnostics that historically documented each exploitation site no longer exist; the options appear vestigial at the fold level. Worth confirming against upstream history before relying on the warning.
2. **`X / -1 → -X` and `X % -1 → 0` are gated only on signedness** (`match.pd:573-576, 903-906`, verified), not on `TYPE_OVERFLOW_UNDEFINED`. Under `-fwrapv` this is value-consistent (both sides wrap to `INT_MIN` / 0), but under `-ftrapv` the rewrite exchanges a division overflow (which the trap model *ignores*, `tree-eh.cc` div arm) for a negation (which `-ftrapv` instruments) — a semantic seam between two edge-case regimes. Low practical impact; flagged rather than judged.
3. **Negative shift counts are handled inconsistently** (§1.7): constant folding refuses (`fold-const.cc:992-1004`), CCP is conservative, but `wi_op_overflows` in range-op silently reinterprets a negative count as an opposite-direction shift (`range-op.cc:2845-2854, 2995-3002`), with an in-tree comment admitting the semantics are unsettled (*"perhaps a C standard interpretation ruling is needed"*). No pass *exploits* negative counts as UB; the inconsistency is about which defined value they'd produce.
4. **POINTER_DIFF_EXPR's "difference does not fit the result type" UB** is documented in `tree.def` but no transform exploiting that specific facet was found (§1.5) — verdict rests on documentation alone.
5. **`INDIRECT_REF` scope note**: GENERIC-only; every middle-end verdict in §1.6 was gathered on `MEM_REF` (post-gimplification form). No divergent INDIRECT_REF-specific semantics were found, but the evidence base is MEM_REF's.

## 5. Stage 5 — testsuite cross-check

Each row cross-checks a conclusion from §§1–3 against actual tests. "No test" is reported honestly — those conclusions rest on the (verified) source evidence alone.

| Conclusion | Testsuite result | Key tests |
|---|---|---|
| §1.1 signed-overflow exploitation gated on overflow-is-UB | **Confirmed by a dual test pair** (verified): `gcc.dg/strict-overflow-1.c` — `return i - 5 < 10;` with `scan-tree-dump-not "-[ ]*5"` (folded, exploiting no-overflow); `gcc.dg/no-strict-overflow-1.c` — identical body, `scan-tree-dump` *expects* the `-5` to survive under wrapping semantics | + `c-c++-common/ubsan/overflow-add-4.c` (`j + i` at `INT_MAX` traps under `-fsanitize=signed-integer-overflow`) |
| §1.1 caveat: niter/scev ignore `TYPE_OVERFLOW_SANITIZED` | **No test found** — nearest is `gcc.dg/ubsan/pr81505.c` (compile-only ICE regression). Caveat rests on the verified zero-grep of the sources | — |
| §1.7 oversized shift count | ubsan side **confirmed**: `c-c++-common/ubsan/shift-1.c` (`1 << 154` → "shift exponent 154 is too large"). **No execution test** pins the middle-end's fold-to-0 value choice | — |
| §1.7 / §2 shift-base is language-version UB | **Confirmed**: `g++.dg/ubsan/cxx2a-shift-1.C` (verified) — `a <<= 31` under `-std=c++2a -fsanitize=shift`, *no* diagnostic expected; `cxx2a-shift-2.C` (`-42 << 1` clean in C++20); `gcc.dg/ubsan/c99-shift-3.c` expects both "left shift of negative value" and "cannot be represented" for C99 | — |
| §1.8 `INT_MIN / -1` is signed-overflow UB, not divide UB | **Confirmed** (verified): `c-c++-common/ubsan/overflow-div-1.c` runs `INT_MIN / -1` under `-fsanitize=integer-divide-by-zero` with **zero** `dg-output` lines; `overflow-div-3.c` catches the same expression under `signed-integer-overflow` | `overflow-div-2.c` covers both messages |
| §1.4/§3(d) pointer-plus: nonzero offset checked, zero offset defined, `-fno-delete-null-pointer-checks` respected | **Confirmed with one inference**: `c-c++-common/ubsan/ptr-overflow-2.c` — "applying **non-zero** offset … to null pointer" is the diagnostic trigger; `ptr-overflow-1.c` runs in-bounds and one-past-end cases clean; `ptr-overflow-sanitization-1.c` counts elided checks; `gcc.dg/pr49235.c` keeps null-derived arithmetic under `-fno-delete-null-pointer-checks`. No test *literally* exercises `p + 0` on NULL — the zero-offset verdict stands on `ubsan.cc:1132-1137` + the fold citations | — |
| §3(e) zero-size mem* with NULL valid | **Confirmed**: `c-c++-common/ubsan/nonnull-6.c` (`dg-do run`, `-fsanitize=undefined`) executes `bar (0, 0, &x, 0, 0)` cleanly; `gcc.dg/nonnull-11.c` (verified earlier); `gcc.dg/nonnull-13.c` — `f1_1 (p0, 0, 0)` silent vs. `f1_1 (p0, 42, 1)` warning *"null where non-null expected because arguments 2 and 3 are nonzero"* | — |
| §3(a) padding indeterminate; clear_padding defines it | **Confirmed**: `c-c++-common/torture/builtin-clear-padding-1.c` — memsets a struct to `-1`, field-assigns, `__builtin_clear_padding`, then `memcmp`s equal against a cleanly built copy. **No test** asserts padding *survives* a plain struct copy (consistent with "don't care") | `gcc.dg/gnu11-empty-init-warn-1.c` family for zero-init-padding warnings |
| §3(b) dangling-pointer *use* diagnosed; malloc/free pairs deleted | **Confirmed**: `gcc.dg/Wuse-after-free-2.c` (verified) warns on `sink (r)` / `sink (r + 1)` — pure value uses, no deref; `gcc.dg/tree-ssa/ssa-dce-7.c` — malloc/free + stores fully eliminated. **No test** exists for `-Wuse-after-free=3` equality diagnostics | — |
| §3(b) stack-slot reuse breaking out-of-lifetime pointers | **No test found** — only incidental `-fstack-reuse=none` uses (e.g. `gcc.dg/graphite/pr86865.c`). Conclusion rests on `cfgexpand.cc` + `invoke.texi` evidence | — |

Net: no conclusion was **contradicted** by the testsuite. Three conclusions (sanitizer-vs-loop-analysis caveat, oversized-shift value choice, stack-reuse breakage) have no direct test coverage and rest on verified source citations only — noted above and, where verdict-relevant, in §4.
