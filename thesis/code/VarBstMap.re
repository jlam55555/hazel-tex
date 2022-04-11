/* VarBstMap.re */

open Sexplib.Std;
module Sexp = Sexplib.Sexp;

include Map.Make(Var);

/* See IntMap */
[@deriving sexp]
type binding('v) = (Var.t, 'v);

let sexp_of_t = (sexp_of_v: 'v => Sexp.t, map: t('v)): Sexp.t =>
  map |> bindings |> sexp_of_list(sexp_of_binding(sexp_of_v));
let t_of_sexp = (v_of_sexp: Sexp.t => 'v, sexp: Sexp.t): t('v) =>
  sexp |> list_of_sexp(binding_of_sexp(v_of_sexp)) |> List.to_seq |> of_seq;
