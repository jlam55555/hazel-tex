/* FillAndResume.rei */

/* Utilities for fill-and-resume. Fill-and-resume is an alternative
   strategy for evaluation that can be performed if all changes in
   a recent update happen within a hole in an earlier state, avoiding
   re-evaluation of the program from the start. Originally described
   in the Hazelnut Live 2019 paper.

   See usage in `Model.update_program`.
   */

/* Perform the fill operation. Fill hole `u` with the provided
   DHExp.t in the result.

   fill(exp_to_fill, u, prev_program_result) => filled_program_result

   Note that this operates on the UHExp.t level, since hole filling is
   operationally on that level. */
let fill: (DHExp.t, MetaVar.t, Result.t) => Result.t;

/* Determine if a new program is a filled version of a past program.

   is_fill_viable(old_program, new_program) =>
     None if a fill is not viable (perform regular evaluation)
     Some((exp_to_fill, u)) if a fill is viable (perform fill-and-resume)

   Essentially performs a structural diff on the programs' elaborated
   expressions, and returns a hole if the root of the diff lies in a hole.

   See `DiffDHExp.rei` for more details about the diff-ing process.

   TODO: This can be applied on multiple past states. Currently, it is
   only applied on the most recent state. This should be a relatively
   inexpensive computation, so this should be reasonable. */
let is_fill_viable: (Program.t, Program.t) => option((DHExp.t, MetaVar.t));
