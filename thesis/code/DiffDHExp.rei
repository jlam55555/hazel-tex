/* DiffDHExp.rei */

/* Structural diff judgment.
   - NoDiff: internal expressions are the same
   - NonFillDiff: diff is not rooted in a hole
   - FillDiff: diff is rooted in a hole, return fill parameters `d`, `u`
   */
[@deriving sexp]
type t =
  | NoDiff
  | NonFillDiff
  | FillDiff(DHExp.t, MetaVar.t);

/* Compare two `DHExp.t`s, returning a difference judgment of type `t`. */
let diff_dhexp: (DHExp.t, DHExp.t) => t;
