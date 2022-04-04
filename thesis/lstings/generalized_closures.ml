type t =
  (* Hole types *)
  | EmptyHole(/$u$/, /$i$/)
  | NonEmptyHole(/$u$/, /$i$/, /$d$/)
  | Keyword(/$u$/, /$i$/, ...)
  | InvalidText(/$u$/, /$i$/, ...)
  | FreeVar(/$u$/, /$i$/, ...)
  | InconsistentBranches(/$u$/, /$i$/, ...)
  (* /$\lambda$/ expressions and closures *)
  | Lam(/$x$/, /$\tau$/, /$d$/)
  (* Generalized closure *)
  | Closure(/$\env$/, /$d$/)
  (* ... *) ;
