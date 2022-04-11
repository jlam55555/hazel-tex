/* DHExp.re */

/* (...) */

let rec fast_equals = (d1: t, d2: t): bool => {
  switch (d1, d2) {
  /* Primitive forms: regular structural equality */
  | (BoundVar(_), _)
  | (BoolLit(_), _)
  | (IntLit(_), _)
  | (FloatLit(_), _)
  | (ListNil(_), _)
  | (Triv, _) => d1 == d2

  /* Non-hole forms: recurse */
  | (Let(dp1, d11, d21), Let(dp2, d12, d22)) =>
    dp1 == dp2 && fast_equals(d11, d12) && fast_equals(d21, d22)
  | (FixF(f1, ty1, d1), FixF(f2, ty2, d2)) =>
    f1 == f2 && ty1 == ty2 && fast_equals(d1, d2)
  | (Fun(dp1, ty1, d1), Fun(dp2, ty2, d2)) =>
    dp1 == dp2 && ty1 == ty2 && fast_equals(d1, d2)
  | (Ap(d11, d21), Ap(d12, d22))
  | (Cons(d11, d21), Cons(d12, d22))
  | (Pair(d11, d21), Pair(d12, d22)) =>
    fast_equals(d11, d12) && fast_equals(d21, d22)
  | (ApBuiltin(f1, args1), ApBuiltin(f2, args2)) =>
    f1 == f2 && List.for_all2(fast_equals, args1, args2)
  | (BinBoolOp(op1, d11, d21), BinBoolOp(op2, d12, d22)) =>
    op1 == op2 && fast_equals(d11, d12) && fast_equals(d21, d22)
  | (BinIntOp(op1, d11, d21), BinIntOp(op2, d12, d22)) =>
    op1 == op2 && fast_equals(d11, d12) && fast_equals(d21, d22)
  | (BinFloatOp(op1, d11, d21), BinFloatOp(op2, d12, d22)) =>
    op1 == op2 && fast_equals(d11, d12) && fast_equals(d21, d22)
  | (Inj(ty1, side1, d1), Inj(ty2, side2, d2)) =>
    ty1 == ty2 && side1 == side2 && fast_equals(d1, d2)
  | (Cast(d1, ty11, ty21), Cast(d2, ty12, ty22))
  | (FailedCast(d1, ty11, ty21), FailedCast(d2, ty12, ty22)) =>
    fast_equals(d1, d2) && ty11 == ty12 && ty21 == ty22
  | (InvalidOperation(d1, reason1), InvalidOperation(d2, reason2)) =>
    fast_equals(d1, d2) && reason1 == reason2
  | (ConsistentCase(case1), ConsistentCase(case2)) =>
    fast_equals_case(case1, case2)
  /* We can group these all into a `_ => false` clause; separating
     these so that we get exhaustiveness checking. */
  | (Let(_), _)
  | (FixF(_), _)
  | (Fun(_), _)
  | (Ap(_), _)
  | (ApBuiltin(_), _)
  | (Cons(_), _)
  | (Pair(_), _)
  | (BinBoolOp(_), _)
  | (BinIntOp(_), _)
  | (BinFloatOp(_), _)
  | (Inj(_), _)
  | (Cast(_), _)
  | (FailedCast(_), _)
  | (InvalidOperation(_), _)
  | (ConsistentCase(_), _) => false

  /* Hole forms: when checking environments, only check that
     environment ID's are equal, don't check structural equality.

     (This resolves a performance issue with many nested holes.) */
  | (EmptyHole(u1, i1), EmptyHole(u2, i2)) => u1 == u2 && i1 == i2
  | (NonEmptyHole(reason1, u1, i1, d1), NonEmptyHole(reason2, u2, i2, d2)) =>
    reason1 == reason2 && u1 == u2 && i1 == i2 && fast_equals(d1, d2)
  | (Keyword(u1, i1, kw1), Keyword(u2, i2, kw2)) =>
    u1 == u2 && i1 == i2 && kw1 == kw2
  | (FreeVar(u1, i1, x1), FreeVar(u2, i2, x2)) =>
    u1 == u2 && i1 == i2 && x1 == x2
  | (InvalidText(u1, i1, text1), InvalidText(u2, i2, text2)) =>
    u1 == u2 && i1 == i2 && text1 == text2
  | (Closure((ei1, _), _, d1), Closure((ei2, _), _, d2)) =>
    /* Cannot use EvalEnv.equals here because it will create a dependency loop. */
    ei1 == ei2 && fast_equals(d1, d2)
  | (
      InconsistentBranches(u1, i1, case1),
      InconsistentBranches(u2, i2, case2),
    ) =>
    u1 == u2 && i1 == i2 && fast_equals_case(case1, case2)
  | (EmptyHole(_), _)
  | (NonEmptyHole(_), _)
  | (Keyword(_), _)
  | (FreeVar(_), _)
  | (InvalidText(_), _)
  | (Closure(_), _)
  | (InconsistentBranches(_), _) => false
  };
}
and fast_equals_case = (Case(d1, rules1, i1), Case(d2, rules2, i2)) => {
  fast_equals(d1, d2)
  && List.length(rules1) == List.length(rules2)
  && List.for_all2(
       (Rule(dp1, d1), Rule(dp2, d2)) => dp1 == dp2 && fast_equals(d1, d2),
       rules1,
       rules2,
     )
  && i1 == i2;
};
