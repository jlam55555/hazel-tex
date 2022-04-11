/* EvalEnv.rei */

/* EvalEnv is an environment (mapping variables to expressions) that
   are used during evaluation. It is different from Environment.t in two ways:

   1. It maps Var.t to Evaluator.result (rather than to DHExp.t)
   2. Each EvalEnv has an ID associated with it (if evaluation reaches it),
      or is unreachable. (i.e., a placeholder environment)

   Environment.t may be useful in certain cases, namely pattern matching,
   when an evaluated result is not needed. EvalEnv is used for environents
   during evaluation, including in closures. EvalEnvs are numbered
   so that operations on them (e.g., during hole numbering) can be memoized;
   the id allows for quick equality checking and allows environments to be
   comparable (e.g., so that they can be stored in a map).

   Both EvalEnv.t and Environment.t are often named sigma (usually for hole
   environments) or env.

   This mimicks the VarMap interface on the extended EvalEnv.t type. Most
   operations require an EvalState.t parameter, which is used to generate
   unique ID's for each environment, and is created using EvalEnv.empty
   (at the beginning of evaluation).
   */

[@deriving sexp]
type t = DHExp.evalenv
and result_map = VarBstMap.t(EvaluatorResult.t);

/* Special environment to begin evaluation at the top level
   (empty environment). */
let empty: t;

let id_of_evalenv: t => EvalEnvId.t;
let result_map_of_evalenv: t => result_map;
let environment_of_evalenv: t => Environment.t;
let alist_of_evalenv: t => list((Var.t, EvaluatorResult.t));

let is_empty: t => bool;
let length: t => int;
let to_list: t => list((Var.t, EvaluatorResult.t));
let lookup: (t, Var.t) => option(EvaluatorResult.t);
let contains: (t, Var.t) => bool;

/* Equals only needs to check environment ID's.
   (faster than structural equality checking.) */
let equals: (t, t) => bool;

/* these functions require an `EvalState.t` because they generate a new
   `EvalEnvId.t` */
let extend: (EvalState.t, t, (Var.t, EvaluatorResult.t)) => (EvalState.t, t);
let map:
  (EvalState.t, (Var.t, EvaluatorResult.t) => EvaluatorResult.t, t) =>
  (EvalState.t, t);
let filter:
  (EvalState.t, (Var.t, EvaluatorResult.t) => bool, t) => (EvalState.t, t);

/* union(new_env, env) extends env with new_env (same argument order
   as in VarMap.union) */
let union: (EvalState.t, t, t) => (EvalState.t, t);

/* same as map, but doesn't assign a new ID. (This is used when
   transforming an environment, such as in the closure->lambda stage
   after evaluation. More functions may be added like this as-needed
   for similar purposes.) */
let map_keep_id: ((Var.t, EvaluatorResult.t) => EvaluatorResult.t, t) => t;
