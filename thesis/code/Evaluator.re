/* Evaluator.re */

/* (...) */

let rec evaluate =
        (es: EvalState.t, env: EvalEnv.t, d: DHExp.t)
        : (EvalState.t, EvaluatorResult.t) => {
  /* Update evaluation statistics */
  let es = es |> EvalState.inc_steps;

  switch (d) {
  | BoundVar(x) =>
    let dr =
      x
      |> EvalEnv.lookup(env)
      |> OptUtil.get(_ =>
           raise(EvaluatorError.Exception(FreeInvalidVar(x)))
         );
    switch (dr) {
    | BoxedValue(FixF(_) as d) => evaluate(es, env, d)
    | _ => (es, dr)
    };
  | Let(dp, d1, d2) =>
    switch (evaluate(es, env, d1)) {
    | (es, BoxedValue(d1))
    | (es, Indet(d1)) =>
      switch (matches(dp, d1)) {
      | Indet
      | DoesNotMatch => (es, Indet(Closure(env, false, Let(dp, d1, d2))))
      | Matches(env') =>
        let (es, env) = extend_evalenv_with_env(es, env', env);
        evaluate(es, env, d2);
      }
    }
  | FixF(f, ty, d) =>
    switch (evaluate(es, env, d)) {
    | (es, BoxedValue(Closure(env', false, Fun(_) as d''') as d'')) =>
      let (es, env'') =
        EvalEnv.extend(es, env', (f, BoxedValue(FixF(f, ty, d''))));
      (es, BoxedValue(Closure(env'', false, d''')));
    | _ => raise(EvaluatorError.Exception(EvaluatorError.FixFWithoutLambda))
    }
  | Fun(_) => (es, BoxedValue(Closure(env, false, d)))
  | Ap(d1, d2) =>
    switch (evaluate(es, env, d1)) {
    | (es, BoxedValue(Closure(closure_env, false, Fun(dp, _, d3)) as d1)) =>
      switch (evaluate(es, env, d2)) {
      | (es, BoxedValue(d2))
      | (es, Indet(d2)) =>
        switch (matches(dp, d2)) {
        | DoesNotMatch
        | Indet => (es, Indet(Ap(d1, d2)))
        | Matches(env') =>
          // evaluate a closure: extend the closure environment with the
          // new bindings introduced by the function application.
          let (es, env) = extend_evalenv_with_env(es, env', closure_env);
          evaluate(es, env, d3);
        }
      }
    | (es, BoxedValue(Cast(d1', Arrow(ty1, ty2), Arrow(ty1', ty2'))))
    | (es, Indet(Cast(d1', Arrow(ty1, ty2), Arrow(ty1', ty2')))) =>
      switch (evaluate(es, env, d2)) {
      | (es, BoxedValue(d2'))
      | (es, Indet(d2')) =>
        /* ap cast rule */
        evaluate(es, env, Cast(Ap(d1', Cast(d2', ty1', ty1)), ty2, ty2'))
      }
    | (_, BoxedValue(d1')) =>
      raise(EvaluatorError.Exception(InvalidBoxedFun(d1')))
    | (es, Indet(d1')) =>
      switch (evaluate(es, env, d2)) {
      | (es, BoxedValue(d2'))
      | (es, Indet(d2')) => (es, Indet(Ap(d1', d2')))
      }
    }
  | ApBuiltin(ident, args) => evaluate_ap_builtin(es, env, ident, args)
  | ListNil(_)
  | BoolLit(_)
  | IntLit(_)
  | FloatLit(_)
  | Triv => (es, BoxedValue(d))
  | BinBoolOp(op, d1, d2) =>
    switch (evaluate(es, env, d1)) {
    | (es, BoxedValue(BoolLit(b1) as d1')) =>
      switch (eval_bin_bool_op_short_circuit(op, b1)) {
      | Some(b3) => (es, b3)
      | None =>
        switch (evaluate(es, env, d2)) {
        | (es, BoxedValue(BoolLit(b2))) => (
            es,
            BoxedValue(eval_bin_bool_op(op, b1, b2)),
          )
        | (_, BoxedValue(d2')) =>
          raise(EvaluatorError.Exception(InvalidBoxedBoolLit(d2')))
        | (es, Indet(d2')) => (es, Indet(BinBoolOp(op, d1', d2')))
        }
      }
    | (_, BoxedValue(d1')) =>
      raise(EvaluatorError.Exception(InvalidBoxedBoolLit(d1')))
    | (es, Indet(d1')) =>
      switch (evaluate(es, env, d2)) {
      | (es, BoxedValue(d2'))
      | (es, Indet(d2')) => (es, Indet(BinBoolOp(op, d1', d2')))
      }
    }
  | BinIntOp(op, d1, d2) =>
    switch (evaluate(es, env, d1)) {
    | (es, BoxedValue(IntLit(n1) as d1')) =>
      switch (evaluate(es, env, d2)) {
      | (es, BoxedValue(IntLit(n2))) =>
        switch (op, n1, n2) {
        | (Divide, _, 0) => (
            es,
            Indet(
              InvalidOperation(
                BinIntOp(op, IntLit(n1), IntLit(n2)),
                DivideByZero,
              ),
            ),
          )
        | _ => (es, BoxedValue(eval_bin_int_op(op, n1, n2)))
        }
      | (_, BoxedValue(d2')) =>
        raise(EvaluatorError.Exception(InvalidBoxedIntLit(d2')))
      | (es, Indet(d2')) => (es, Indet(BinIntOp(op, d1', d2')))
      }
    | (_, BoxedValue(d1')) =>
      raise(EvaluatorError.Exception(InvalidBoxedIntLit(d1')))
    | (es, Indet(d1')) =>
      switch (evaluate(es, env, d2)) {
      | (es, BoxedValue(d2'))
      | (es, Indet(d2')) => (es, Indet(BinIntOp(op, d1', d2')))
      }
    }
  | BinFloatOp(op, d1, d2) =>
    switch (evaluate(es, env, d1)) {
    | (es, BoxedValue(FloatLit(f1) as d1')) =>
      switch (evaluate(es, env, d2)) {
      | (es, BoxedValue(FloatLit(f2))) => (
          es,
          BoxedValue(eval_bin_float_op(op, f1, f2)),
        )
      | (_, BoxedValue(d2')) =>
        raise(EvaluatorError.Exception(InvalidBoxedFloatLit(d2')))
      | (es, Indet(d2')) => (es, Indet(BinFloatOp(op, d1', d2')))
      }
    | (_, BoxedValue(d1')) =>
      raise(EvaluatorError.Exception(InvalidBoxedFloatLit(d1')))
    | (es, Indet(d1')) =>
      switch (evaluate(es, env, d2)) {
      | (es, BoxedValue(d2'))
      | (es, Indet(d2')) => (es, Indet(BinFloatOp(op, d1', d2')))
      }
    }
  | Inj(ty, side, d1) =>
    switch (evaluate(es, env, d1)) {
    | (es, BoxedValue(d1')) => (es, BoxedValue(Inj(ty, side, d1')))
    | (es, Indet(d1')) => (es, Indet(Inj(ty, side, d1')))
    }
  | Pair(d1, d2) =>
    let (es, d1') = evaluate(es, env, d1);
    let (es, d2') = evaluate(es, env, d2);
    switch (d1', d2') {
    | (Indet(d1), Indet(d2))
    | (Indet(d1), BoxedValue(d2))
    | (BoxedValue(d1), Indet(d2)) => (es, Indet(Pair(d1, d2)))
    | (BoxedValue(d1), BoxedValue(d2)) => (es, BoxedValue(Pair(d1, d2)))
    };
  | Cons(d1, d2) =>
    let (es, d1') = evaluate(es, env, d1);
    let (es, d2') = evaluate(es, env, d2);
    switch (d1', d2') {
    | (Indet(d1), Indet(d2))
    | (Indet(d1), BoxedValue(d2))
    | (BoxedValue(d1), Indet(d2)) => (es, Indet(Cons(d1, d2)))
    | (BoxedValue(d1), BoxedValue(d2)) => (es, BoxedValue(Cons(d1, d2)))
    };
  | ConsistentCase(Case(d1, rules, n)) =>
    evaluate_case(es, env, None, d1, rules, n)

  /* Generalized closures evaluate to themselves. Only
     lambda closures are BoxedValues; other closures are all Indet. */
  | Closure(_, false, d') =>
    switch (d') {
    | Fun(_) => (es, BoxedValue(d))
    | _ => (es, Indet(d))
    }

  /* For purposes of fill-and-resume, `Closure` expressions may not be final.
     All closures are marked with a `re_eval` flag (the second parameter)
     before resuming evaluation during fill-and-resume. After a Closure is
     evaluated for the first time, then it does not need to be re-evaluated
     when encountered in the future.

     In addition, if a closure marks a filled hole, then the evaluation
     may be memoized by hole closure. (See TODO, below.)

     TODO: memoize the filling of the same hole instance. To do this, store
     the hole instance in the `re_eval` field (make `re_eval` not only a
     boolean flag) and store a mapping from ids to results in the
     `EvalState.t` (replacing `FARInfo.t`).
     */
  | Closure(env', true, d') => evaluate(es, env', d')

  /* Hole expressions. Wrap in closure. */
  | EmptyHole(_)
  | FreeVar(_)
  | Keyword(_)
  | InvalidText(_) => (es, Indet(Closure(env, false, d)))
  | InconsistentBranches(u, i, Case(d1, rules, n)) =>
    evaluate_case(es, env, Some((u, i)), d1, rules, n)
  | NonEmptyHole(reason, u, i, d1) =>
    switch (evaluate(es, env, d1)) {
    | (es, BoxedValue(d1'))
    | (es, Indet(d1')) => (
        es,
        Indet(Closure(env, false, NonEmptyHole(reason, u, i, d1'))),
      )
    }

  /* Cast calculus */
  | Cast(d1, ty, ty') =>
    switch (evaluate(es, env, d1)) {
    | (es, BoxedValue(d1') as result) =>
      switch (ground_cases_of(ty), ground_cases_of(ty')) {
      | (Hole, Hole) => (es, result)
      | (Ground, Ground) =>
        /* if two types are ground and consistent, then they are eq */
        (es, result)
      | (Ground, Hole) =>
        /* can't remove the cast or do anything else here, so we're done */
        (es, BoxedValue(Cast(d1', ty, ty')))
      | (Hole, Ground) =>
        /* by canonical forms, d1' must be of the form d<ty'' -> ?> */
        switch (d1') {
        | Cast(d1'', ty'', Hole) =>
          if (HTyp.eq(ty'', ty')) {
            (es, BoxedValue(d1''));
          } else {
            (es, Indet(FailedCast(d1', ty, ty')));
          }
        | _ =>
          raise(EvaluatorError.Exception(CastBVHoleGround(d1')))
        }
      | (Hole, NotGroundOrHole(ty'_grounded)) =>
        /* ITExpand rule */
        let d' = DHExp.Cast(Cast(d1', ty, ty'_grounded), ty'_grounded, ty');
        evaluate(es, env, d');
      | (NotGroundOrHole(ty_grounded), Hole) =>
        /* ITGround rule */
        let d' = DHExp.Cast(Cast(d1', ty, ty_grounded), ty_grounded, ty');
        evaluate(es, env, d');
      | (Ground, NotGroundOrHole(_))
      | (NotGroundOrHole(_), Ground) =>
        /* can't do anything when casting between diseq, non-hole types */
        (es, BoxedValue(Cast(d1', ty, ty')))
      | (NotGroundOrHole(_), NotGroundOrHole(_)) =>
        /* they might be eq in this case, so remove cast if so */
        if (HTyp.eq(ty, ty')) {
          (es, result);
        } else {
          (es, BoxedValue(Cast(d1', ty, ty')));
        }
      }
    | (es, Indet(d1') as result) =>
      switch (ground_cases_of(ty), ground_cases_of(ty')) {
      | (Hole, Hole) => (es, result)
      | (Ground, Ground) =>
        /* if two types are ground and consistent, then they are eq */
        (es, result)
      | (Ground, Hole) =>
        /* can't remove the cast or do anything else here, so we're done */
        (es, Indet(Cast(d1', ty, ty')))
      | (Hole, Ground) =>
        switch (d1') {
        | Cast(d1'', ty'', Hole) =>
          if (HTyp.eq(ty'', ty')) {
            (es, Indet(d1''));
          } else {
            (es, Indet(FailedCast(d1', ty, ty')));
          }
        | _ => (es, Indet(Cast(d1', ty, ty')))
        }
      | (Hole, NotGroundOrHole(ty'_grounded)) =>
        /* ITExpand rule */
        let d' = DHExp.Cast(Cast(d1', ty, ty'_grounded), ty'_grounded, ty');
        evaluate(es, env, d');
      | (NotGroundOrHole(ty_grounded), Hole) =>
        /* ITGround rule */
        let d' = DHExp.Cast(Cast(d1', ty, ty_grounded), ty_grounded, ty');
        evaluate(es, env, d');
      | (Ground, NotGroundOrHole(_))
      | (NotGroundOrHole(_), Ground) =>
        /* can't do anything when casting between diseq, non-hole types */
        (es, Indet(Cast(d1', ty, ty')))
      | (NotGroundOrHole(_), NotGroundOrHole(_)) =>
        /* it might be eq in this case, so remove cast if so */
        if (HTyp.eq(ty, ty')) {
          (es, result);
        } else {
          (es, Indet(Cast(d1', ty, ty')));
        }
      }
    }
  | FailedCast(d1, ty, ty') =>
    switch (evaluate(es, env, d1)) {
    | (es, BoxedValue(d1'))
    | (es, Indet(d1')) => (es, Indet(FailedCast(d1', ty, ty')))
    }
  | InvalidOperation(d, err) => (es, Indet(InvalidOperation(d, err)))
  };
}
and evaluate_case =
    (
      es: EvalState.t,
      env: EvalEnv.t,
      inconsistent_info: option(HoleClosure.t),
      scrut: DHExp.t,
      rules: list(DHExp.rule),
      current_rule_index: int,
    )
    : (EvalState.t, EvaluatorResult.t) =>
  switch (evaluate(es, env, scrut)) {
  | (es, BoxedValue(scrut))
  | (es, Indet(scrut)) =>
    switch (List.nth_opt(rules, current_rule_index)) {
    | None =>
      let case = DHExp.Case(scrut, rules, current_rule_index);
      (
        es,
        switch (inconsistent_info) {
        | None => Indet(Closure(env, false, ConsistentCase(case)))
        | Some((u, i)) =>
          Indet(Closure(env, false, InconsistentBranches(u, i, case)))
        },
      );
    | Some(Rule(dp, d)) =>
      switch (matches(dp, scrut)) {
      | Indet =>
        let case = DHExp.Case(scrut, rules, current_rule_index);
        (
          es,
          switch (inconsistent_info) {
          | None => Indet(Closure(env, false, ConsistentCase(case)))
          | Some((u, i)) =>
            Indet(Closure(env, false, InconsistentBranches(u, i, case)))
          },
        );
      | Matches(env') =>
        // extend environment with new bindings introduced
        let (es, env) = extend_evalenv_with_env(es, env', env);
        evaluate(es, env, d);
      // by the rule and evaluate the expression.
      | DoesNotMatch =>
        evaluate_case(
          es,
          env,
          inconsistent_info,
          scrut,
          rules,
          current_rule_index + 1,
        )
      }
    }
  }

/* This function extends an EvalEnv.t with new bindings
   (an Environment.t from match()). We need to wrap the new bindings
   in a final judgment (BoxedValue or Indet), so we call evaluate()
   on it again, but it shouldn't change the value of the expression. */
and extend_evalenv_with_env =
    (es: EvalState.t, new_bindings: Environment.t, to_extend: EvalEnv.t)
    : (EvalState.t, EvalEnv.t) => {
  let (es, ei) = es |> EvalState.next_evalenvid;
  let result_map =
    List.fold_left(
      (new_env, (x, d)) => {
        /* The value of environment doesn't matter here */
        let (_, dr) = evaluate(es, EvalEnv.empty, d);
        VarBstMap.add(x, dr, new_env);
      },
      EvalEnv.result_map_of_evalenv(to_extend),
      new_bindings,
    );
  (es, (ei, result_map));
}

/* Evaluate the application of a built-in function. */
and evaluate_ap_builtin =
    (es: EvalState.t, env: EvalEnv.t, ident: string, args: list(DHExp.t))
    : (EvalState.t, EvaluatorResult.t) => {
  switch (Builtins.lookup_form(ident)) {
  | Some((eval, _)) => eval(es, env, args, evaluate)
  | None => raise(EvaluatorError.Exception(InvalidBuiltin(ident)))
  };
};
