/* EvalEnvId.re */

open Sexplib.Std;

[@deriving sexp]
type t = int;

let empty: t = 0;
