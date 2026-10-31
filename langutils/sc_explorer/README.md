# sc_explorer

A web page that shows the AST, IR, LIR, bytecode, and diagnostics for any given inpuit.


## Use

Build the `sc_explorer` target, then run:

```
python3 langutils/sc_explorer/serve.py [--port 8000] [--backend path] [--prelude file.sc]... [--no-prelude]
```

Open `http://localhost:8000`.
