/* EvalPostprocess.re */

/* Postprocess outside evaluation boundary */
let rec pp_uneval =
        (
          hci: HoleClosureInfo_.t,
          env: EvalEnv.t,
          d: DHExp.t,
          parent: HoleClosureParents.t_,
        )
        : (HoleClosureInfo_.t, DHExp.t) => {
  switch (d) {
  /* Bound variables should be looked up within the closure
     environment. If lookup fails, then variable is not bound. */
  | BoundVar(x) =>
    switch (EvalEnv.lookup(env, x)) {
    | Some(Indet(d'))
    | Some(BoxedValue(d')) =>
      let (hci, d'') = pp_eval(hci, d', parent);
      (hci, d'');
    | None => (hci, d)
    }

  /* Non-hole expressions: expand recursively */
  | BoolLit(_)
  | IntLit(_)
  | FloatLit(_)
  | ListNil(_)
  | Triv => (hci, d)
  | Let(dp, d1, d2) =>
    let (hci, d1') = pp_uneval(hci, env, d1, parent);
    let (hci, d2') = pp_uneval(hci, env, d2, parent);
    (hci, Let(dp, d1', d2'));
  | FixF(f, ty, d1) =>
    let (hci, d1') = pp_uneval(hci, env, d1, parent);
    (hci, FixF(f, ty, d1'));
  | Fun(dp, ty, d') =>
    let (hci, d'') = pp_uneval(hci, env, d', parent);
    (hci, Fun(dp, ty, d''));
  | Ap(d1, d2) =>
    let (hci, d1') = pp_uneval(hci, env, d1, parent);
    let (hci, d2') = pp_uneval(hci, env, d2, parent);
    (hci, Ap(d1', d2'));
  | ApBuiltin(f, args) =>
    let (hci, args') =
      List.fold_right(
        (arg, (hci, args)) => {
          let (hci, arg') = pp_uneval(hci, env, arg, parent);
          (hci, [arg', ...args]);
        },
        args,
        (hci, []),
      );
    (hci, ApBuiltin(f, args'));
  | BinBoolOp(op, d1, d2) =>
    let (hci, d1') = pp_uneval(hci, env, d1, parent);
    let (hci, d2') = pp_uneval(hci, env, d2, parent);
    (hci, BinBoolOp(op, d1', d2'));
  | BinIntOp(op, d1, d2) =>
    let (hci, d1') = pp_uneval(hci, env, d1, parent);
    let (hci, d2') = pp_uneval(hci, env, d2, parent);
    (hci, BinIntOp(op, d1', d2'));
  | BinFloatOp(op, d1, d2) =>
    let (hci, d1') = pp_uneval(hci, env, d1, parent);
    let (hci, d2') = pp_uneval(hci, env, d2, parent);
    (hci, BinFloatOp(op, d1', d2'));
  | Cons(d1, d2) =>
    let (hci, d1') = pp_uneval(hci, env, d1, parent);
    let (hci, d2') = pp_uneval(hci, env, d2, parent);
    (hci, Cons(d1', d2'));
  | Inj(ty, side, d') =>
    let (hci, d'') = pp_uneval(hci, env, d', parent);
    (hci, Inj(ty, side, d''));
  | Pair(d1, d2) =>
    let (hci, d1') = pp_uneval(hci, env, d1, parent);
    let (hci, d2') = pp_uneval(hci, env, d2, parent);
    (hci, Pair(d1', d2'));
  | Cast(d', ty1, ty2) =>
    let (hci, d'') = pp_uneval(hci, env, d', parent);
    (hci, Cast(d'', ty1, ty2));
  | FailedCast(d', ty1, ty2) =>
    let (hci, d'') = pp_uneval(hci, env, d', parent);
    (hci, FailedCast(d'', ty1, ty2));
  | InvalidOperation(d', reason) =>
    let (hci, d'') = pp_uneval(hci, env, d', parent);
    (hci, InvalidOperation(d'', reason));
  | ConsistentCase(Case(scrut, rules, i)) =>
    let (hci, scrut') = pp_uneval(hci, env, scrut, parent);
    let (hci, rules') = pp_uneval_rules(hci, env, rules, parent);
    (hci, ConsistentCase(Case(scrut', rules', i)));

  /* Closures shouldn't exist inside other closures */
  | Closure(_) => raise(EvalPostprocessError.Exception(ClosureInsideClosure))

  /* Hole expressions:
     - Use the closure environment as the hole environment.
     - Number the hole closure appropriately.
     - Recurse through inner expression (if any).

     Note: we still have to recurse through the hole boundary in order
     to set the correct hole parents; however, this is only at most
     one depth of repeated traversal through the environment
     */
  | EmptyHole(u, _) =>
    let hc_id_res = HoleClosureInfo_.get_hc_id(hci, u, env, parent);
    switch (hc_id_res) {
    | ExistClosure(hci, i, env) => (
        hci,
        Closure(env, false, EmptyHole(u, i)),
      )
    | NewClosure(hci, i) =>
      let (hci, env) = pp_eval_hole_env(hci, env, (u, i));
      let hci = HoleClosureInfo_.update_hc_env(hci, u, env);
      (hci, Closure(env, false, EmptyHole(u, i)));
    };
  | NonEmptyHole(reason, u, _, d') =>
    let (hci, d'') = pp_uneval(hci, env, d', parent);
    let hc_id_res = HoleClosureInfo_.get_hc_id(hci, u, env, parent);
    switch (hc_id_res) {
    | ExistClosure(hci, i, env) => (
        hci,
        Closure(env, false, NonEmptyHole(reason, u, i, d'')),
      )
    | NewClosure(hci, i) =>
      let (hci, env) = pp_eval_hole_env(hci, env, (u, i));
      let hci = HoleClosureInfo_.update_hc_env(hci, u, env);
      (hci, Closure(env, false, NonEmptyHole(reason, u, i, d'')));
    };
  | Keyword(u, _, kw) =>
    let hc_id_res = HoleClosureInfo_.get_hc_id(hci, u, env, parent);
    switch (hc_id_res) {
    | ExistClosure(hci, i, env) => (
        hci,
        Closure(env, false, Keyword(u, i, kw)),
      )
    | NewClosure(hci, i) =>
      let (hci, env) = pp_eval_hole_env(hci, env, (u, i));
      let hci = HoleClosureInfo_.update_hc_env(hci, u, env);
      (hci, Closure(env, false, Keyword(u, i, kw)));
    };
  | FreeVar(u, _, x) =>
    let hc_id_res = HoleClosureInfo_.get_hc_id(hci, u, env, parent);
    switch (hc_id_res) {
    | ExistClosure(hci, i, env) => (
        hci,
        Closure(env, false, FreeVar(u, i, x)),
      )
    | NewClosure(hci, i) =>
      let (hci, env) = pp_eval_hole_env(hci, env, (u, i));
      let hci = HoleClosureInfo_.update_hc_env(hci, u, env);
      (hci, Closure(env, false, FreeVar(u, i, x)));
    };
  | InvalidText(u, _, text) =>
    let hc_id_res = HoleClosureInfo_.get_hc_id(hci, u, env, parent);
    switch (hc_id_res) {
    | ExistClosure(hci, i, env) => (
        hci,
        Closure(env, false, InvalidText(u, i, text)),
      )
    | NewClosure(hci, i) =>
      let (hci, env) = pp_eval_hole_env(hci, env, (u, i));
      let hci = HoleClosureInfo_.update_hc_env(hci, u, env);
      (hci, Closure(env, false, InvalidText(u, i, text)));
    };
  | InconsistentBranches(u, _, Case(d', rules, i)) =>
    let (hci, d'') = pp_uneval(hci, env, d', parent);
    let (hci, rules') = pp_uneval_rules(hci, env, rules, parent);
    let case' = DHExp.Case(d'', rules', i);
    let hc_id_res = HoleClosureInfo_.get_hc_id(hci, u, env, parent);
    switch (hc_id_res) {
    | ExistClosure(hci, i, env) => (
        hci,
        Closure(env, false, InconsistentBranches(u, i, case')),
      )
    | NewClosure(hci, i) =>
      let (hci, env) = pp_eval_hole_env(hci, env, (u, i));
      let hci = HoleClosureInfo_.update_hc_env(hci, u, env);
      (hci, Closure(env, false, InconsistentBranches(u, i, case')));
    };
  };
}

and pp_uneval_rules =
    (
      hci: HoleClosureInfo_.t,
      env: EvalEnv.t,
      rules: list(DHExp.rule),
      parent: HoleClosureParents.t_,
    )
    : (HoleClosureInfo_.t, list(DHExp.rule)) =>
  List.fold_right(
    (DHExp.Rule(dp, d), (hci, rules)) => {
      let (hci, d') = pp_uneval(hci, env, d, parent);
      (hci, [DHExp.Rule(dp, d'), ...rules]);
    },
    rules,
    (hci, []),
  )

/* Postprocess inside evaluation boundary */
and pp_eval =
    (hci: HoleClosureInfo_.t, d: DHExp.t, parent: HoleClosureParents.t_)
    : (HoleClosureInfo_.t, DHExp.t) =>
  switch (d) {
  /* Non-hole expressions: recurse through subexpressions */
  | BoolLit(_)
  | IntLit(_)
  | FloatLit(_)
  | ListNil(_)
  | Triv => (hci, d)
  | FixF(f, ty, d1) =>
    let (hci, d1') = pp_eval(hci, d1, parent);
    (hci, FixF(f, ty, d1'));
  | Ap(d1, d2) =>
    let (hci, d1') = pp_eval(hci, d1, parent);
    let (hci, d2') = pp_eval(hci, d2, parent);
    (hci, Ap(d1', d2'));
  | ApBuiltin(f, args) =>
    let (hci, args') =
      List.fold_right(
        (arg, (hci, args)) => {
          let (hci, arg') = pp_eval(hci, arg, parent);
          (hci, [arg', ...args]);
        },
        args,
        (hci, []),
      );
    (hci, ApBuiltin(f, args'));
  | BinBoolOp(op, d1, d2) =>
    let (hci, d1') = pp_eval(hci, d1, parent);
    let (hci, d2') = pp_eval(hci, d2, parent);
    (hci, BinBoolOp(op, d1', d2'));
  | BinIntOp(op, d1, d2) =>
    let (hci, d1') = pp_eval(hci, d1, parent);
    let (hci, d2') = pp_eval(hci, d2, parent);
    (hci, BinIntOp(op, d1', d2'));
  | BinFloatOp(op, d1, d2) =>
    let (hci, d1') = pp_eval(hci, d1, parent);
    let (hci, d2') = pp_eval(hci, d2, parent);
    (hci, BinFloatOp(op, d1', d2'));
  | Cons(d1, d2) =>
    let (hci, d1') = pp_eval(hci, d1, parent);
    let (hci, d2') = pp_eval(hci, d2, parent);
    (hci, Cons(d1', d2'));
  | Inj(ty, side, d') =>
    let (hci, d'') = pp_eval(hci, d', parent);
    (hci, Inj(ty, side, d''));
  | Pair(d1, d2) =>
    let (hci, d1') = pp_eval(hci, d1, parent);
    let (hci, d2') = pp_eval(hci, d2, parent);
    (hci, Pair(d1', d2'));
  | Cast(d', ty1, ty2) =>
    let (hci, d'') = pp_eval(hci, d', parent);
    (hci, Cast(d'', ty1, ty2));
  | FailedCast(d', ty1, ty2) =>
    let (hci, d'') = pp_eval(hci, d', parent);
    (hci, FailedCast(d'', ty1, ty2));
  | InvalidOperation(d', reason) =>
    let (hci, d'') = pp_eval(hci, d', parent);
    (hci, InvalidOperation(d'', reason));

  /* Bound variables should not appear outside holes or closures */
  | BoundVar(x) =>
    raise(EvalPostprocessError.Exception(BoundVarOutsideClosure(x)))

  /* Lambda should not appear outside closure in evaluated result */
  | Let(_)
  | ConsistentCase(_)
  | Fun(_)
  | EmptyHole(_)
  | NonEmptyHole(_)
  | Keyword(_)
  | FreeVar(_)
  | InvalidText(_)
  | InconsistentBranches(_) =>
    raise(EvalPostprocessError.Exception(UnevalOutsideClosure))

  /* Closure */
  | Closure(env', _, d') =>
    switch (d') {
    | Fun(dp, ty, d'') =>
      let (hci, d'') = pp_uneval(hci, env', d'', parent);
      (hci, Fun(dp, ty, d''));
    | Let(dp, d1, d2) =>
      /* d1 should already be evaluated, d2 is not */
      let (hci, d1') = pp_eval(hci, d1, parent);
      let (hci, d2') = pp_uneval(hci, env', d2, parent);
      (hci, Let(dp, d1', d2'));
    | ConsistentCase(Case(scrut, rules, i)) =>
      /* scrut should already be evaluated, rule bodies are not */
      let (hci, scrut') = pp_eval(hci, scrut, parent);
      let (hci, rules') = pp_uneval_rules(hci, env', rules, parent);
      (hci, ConsistentCase(Case(scrut', rules', i)));

    /* Holes: should be left in closures in the result */
    | EmptyHole(u, _) =>
      let hc_id_res = HoleClosureInfo_.get_hc_id(hci, u, env', parent);
      switch (hc_id_res) {
      | ExistClosure(hci, i, env') => (
          hci,
          Closure(env', false, EmptyHole(u, i)),
        )
      | NewClosure(hci, i) =>
        let (hci, env) = pp_eval_hole_env(hci, env', (u, i));
        let hci = HoleClosureInfo_.update_hc_env(hci, u, env);
        (hci, Closure(env', false, EmptyHole(u, i)));
      };
    | NonEmptyHole(reason, u, _, d') =>
      let (hci, d'') = pp_eval(hci, d', parent);
      let hc_id_res = HoleClosureInfo_.get_hc_id(hci, u, env', parent);
      switch (hc_id_res) {
      | ExistClosure(hci, i, env') => (
          hci,
          Closure(env', false, NonEmptyHole(reason, u, i, d'')),
        )
      | NewClosure(hci, i) =>
        let (hci, env') = pp_eval_hole_env(hci, env', (u, i));
        let hci = HoleClosureInfo_.update_hc_env(hci, u, env');
        (hci, Closure(env', false, NonEmptyHole(reason, u, i, d'')));
      };
    | Keyword(u, _, kw) =>
      let hc_id_res = HoleClosureInfo_.get_hc_id(hci, u, env', parent);
      switch (hc_id_res) {
      | ExistClosure(hci, i, env') => (
          hci,
          Closure(env', false, Keyword(u, i, kw)),
        )
      | NewClosure(hci, i) =>
        let (hci, env') = pp_eval_hole_env(hci, env', (u, i));
        let hci = HoleClosureInfo_.update_hc_env(hci, u, env');
        (hci, Closure(env', false, Keyword(u, i, kw)));
      };
    | FreeVar(u, _, x) =>
      let hc_id_res = HoleClosureInfo_.get_hc_id(hci, u, env', parent);
      switch (hc_id_res) {
      | ExistClosure(hci, i, env') => (
          hci,
          Closure(env', false, FreeVar(u, i, x)),
        )
      | NewClosure(hci, i) =>
        let (hci, env') = pp_eval_hole_env(hci, env', (u, i));
        let hci = HoleClosureInfo_.update_hc_env(hci, u, env');
        (hci, Closure(env', false, FreeVar(u, i, x)));
      };
    | InvalidText(u, _, text) =>
      let hc_id_res = HoleClosureInfo_.get_hc_id(hci, u, env', parent);
      switch (hc_id_res) {
      | ExistClosure(hci, i, env') => (
          hci,
          Closure(env', false, InvalidText(u, i, text)),
        )
      | NewClosure(hci, i) =>
        let (hci, env') = pp_eval_hole_env(hci, env', (u, i));
        let hci = HoleClosureInfo_.update_hc_env(hci, u, env');
        (hci, Closure(env', false, InvalidText(u, i, text)));
      };
    | InconsistentBranches(u, _, Case(d', rules, case_i)) =>
      let (hci, d'') = pp_eval(hci, d', parent);
      let hc_id_res = HoleClosureInfo_.get_hc_id(hci, u, env', parent);
      switch (hc_id_res) {
      | ExistClosure(hci, i, env') =>
        let (hci, rules') = pp_uneval_rules(hci, env', rules, parent);
        (
          hci,
          Closure(
            env',
            false,
            InconsistentBranches(u, i, Case(d'', rules', case_i)),
          ),
        );
      | NewClosure(hci, i) =>
        let (hci, env') = pp_eval_hole_env(hci, env', (u, i));
        let (hci, rules') = pp_uneval_rules(hci, env', rules, parent);
        let hci = HoleClosureInfo_.update_hc_env(hci, u, env');
        (
          hci,
          Closure(
            env',
            false,
            InconsistentBranches(u, i, Case(d'', rules', case_i)),
          ),
        );
      };

    /* Fill-and-resume: may be possible to have other expression
       types inside a closure. */
    | _ => pp_uneval(hci, env', d', parent)
    }
  /* Hole expressions:
     - Fix environment recursively.
     - Number the hole closure appropriately.
     - Recurse through subexpressions if applicable. */
  }

/* Apply pp_eval to each expression in sigma,
   threading hci throughout.

   hc is the current HoleClosure.t, which will be set as the
   parent of any holes directly in the subexpressions
   */
and pp_eval_hole_env =
    (hci: HoleClosureInfo_.t, sigma: EvalEnv.t, parent_hc: HoleClosure.t)
    : (HoleClosureInfo_.t, EvalEnv.t) => {
  let ei = sigma |> EvalEnv.id_of_evalenv;
  let (hci, result_map) =
    VarBstMap.fold(
      (x, dr: EvaluatorResult.t, (hci, new_env)) => {
        let (hci, dr: EvaluatorResult.t) =
          switch (dr) {
          | BoxedValue(d) =>
            let (hci, d) = pp_eval(hci, d, (x, parent_hc));
            (hci, BoxedValue(d));
          | Indet(d) =>
            let (hci, d) = pp_eval(hci, d, (x, parent_hc));
            (hci, Indet(d));
          };
        (hci, VarBstMap.add(x, dr, new_env));
      },
      sigma |> EvalEnv.result_map_of_evalenv,
      (hci, VarBstMap.empty),
    );
  (hci, (ei, result_map));
};

/* Postprocessing driver.

   See also HoleClosureInfo.rei/HoleClosureInfo_.rei.
   */
let postprocess = (d: DHExp.t): (HoleClosureInfo.t, DHExp.t) => {
  let (hci, d) =
    pp_eval(HoleClosureInfo_.empty, d, ("", HoleClosure.result_hc));
  (hci |> HoleClosureInfo_.to_hole_closure_info, d);
};
