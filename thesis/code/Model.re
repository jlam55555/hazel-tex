/* Model.re */

/* (In `Model.update_program`) */

/* Decide between regular evaluation or fill-and-resume. */
let old_result = Program.get_result(old_program);
let new_result =
  switch (FillAndResume.is_fill_viable(old_program, new_program)) {
  | None => new_program |> Program.get_result
  | Some((e, u)) => old_result |> FillAndResume.fill(e, u)
  };

/* (End `Model.update_program`) */
