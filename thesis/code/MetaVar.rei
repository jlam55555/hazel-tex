/* MetaVar.rei */

/* A `MetaVar.t` represents a hole number `u`.

   The name "metavar" comes from CMTT. */
[@deriving sexp]
type t = int;
let eq: (t, t) => bool;
