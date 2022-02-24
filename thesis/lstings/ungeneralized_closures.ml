type t =
  (* Hole types *)
  | EmptyHole(/$u$/, /$i$/, /$\env$/)
  | NonEmptyHole(/$u$/, /$i$/, /$\env$/, /$d$/)
  | Keyword(/$u$/, /$i$/, /$\env$/, ...)
  | InvalidText(/$u$/, /$i$/, /$\env$/, ...)
  | FreeVar(/$u$/, /$i$/, /$\env$/, ...)
  | InconsistentBranches(/$u$/, /$i$/, /$\env$/, ...)
  (* Lambda expressions and /$\lambda$/ closures *)
  | Lam(/$x$/, /$\tau$/, /$d$/)
  | FnClosure(/$\env$/, /$x$/, /$\tau$/, /$d$/)
  (* ... *) ;
