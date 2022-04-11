/* Program.re */

/* (...) */

let get_result = (program: t): Result.t => {
  switch (program |> get_elaboration |> evaluate) {
  | (es, BoxedValue(d)) =>
    let (hci, d_postprocessed) =
      switch (d |> EvalPostprocess.postprocess) {
      | d => d
      | exception (EvalPostprocessError.Exception(reason)) =>
        raise(PostprocessError(reason))
      };
    Result.mk(BoxedValue(d_postprocessed), d, hci, es);
  | (es, Indet(d)) =>
    let (hci, d_postprocessed) =
      switch (d |> EvalPostprocess.postprocess) {
      | d => d
      | exception (EvalPostprocessError.Exception(reason)) =>
        raise(PostprocessError(reason))
      };
    Result.mk(BoxedValue(d_postprocessed), d, hci, es);
  | exception (EvaluatorError.Exception(reason)) => raise(EvalError(reason))
  };
};

/* (...) */
