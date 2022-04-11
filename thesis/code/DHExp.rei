/* DHExp.rei */

[@deriving sexp]
type t =
  /* Hole types */
  | EmptyHole(MetaVar.t, HoleClosureId.t)
  | NonEmptyHole(ErrStatus.HoleReason.t, MetaVar.t, HoleClosureId.t, t)
  // TODO rename to ExpandingKeyword
  | Keyword(MetaVar.t, HoleClosureId.t, ExpandingKeyword.t)
  | FreeVar(MetaVar.t, HoleClosureId.t, Var.t)
  | InvalidText(MetaVar.t, HoleClosureId.t, string)
  | InconsistentBranches(MetaVar.t, HoleClosureId.t, case)
  /* Generalized closures */
  | Closure(evalenv, bool, t)
  /* Other expressions forms */
  | BoundVar(Var.t)
  | Let(DHPat.t, t, t)
  | FixF(Var.t, HTyp.t, t)
  | Fun(DHPat.t, HTyp.t, t)
  | Ap(t, t)
  | ApBuiltin(string, list(t))
  | BoolLit(bool)
  | IntLit(int)
  | FloatLit(float)
  | BinBoolOp(BinBoolOp.t, t, t)
  | BinIntOp(BinIntOp.t, t, t)
  | BinFloatOp(BinFloatOp.t, t, t)
  | ListNil(HTyp.t)
  | Cons(t, t)
  | Inj(HTyp.t, InjSide.t, t)
  | Pair(t, t)
  | Triv
  | ConsistentCase(case)
  | Cast(t, HTyp.t, HTyp.t)
  | FailedCast(t, HTyp.t, HTyp.t)
  | InvalidOperation(t, InvalidOperationError.t)
and case =
  | Case(t, list(rule), int)
and rule =
  | Rule(DHPat.t, t)
and environment = VarMap.t_(t)
and evalenv = (EvalEnvId.t, VarBstMap.t(result))
and result =
  | BoxedValue(t)
  | Indet(t);

/* (...) */

/* Used for faster structural equality checking. Structural
   checking may be slow when an expression is large,
   in particular when environments are repeated many times.
   We can optimize checking for structural equality of
   environments simply by checking equality of environment ID's.

   Note: assumes that environments with the same EvalEnvId.t
   within both expressions are equivalent. This assumption
   is true if comparing within a program evaluation (since
   EvalEnvId.t numbers don't get reused within a single program
   evaluation) or if all the environments are checked to be
   equal (see Result.fast_equals).
   */
let fast_equals: (t, t) => bool;
