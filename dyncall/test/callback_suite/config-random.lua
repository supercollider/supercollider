-- section 'main' --------------------------------------------------------------
                  
nsigs     = 800         -- number of test cases
mode      = "random"    -- generatore mode: 'random' or 'ordered' type sequences
minargs   = 0           -- minimum num. of supported arguments (>= 0)
maxargs   = 67          -- maximum num. of supported arguments (>= minargs)


-- section 'types' -------------------------------------------------------------

argtypes  = "BcCsSiIjJlLpfd"    -- supported argument types
rettypes  = argtypes    -- supported return types (currently no void support)


-- section 'ordered' -----------------------------------------------------------

offset    = 0           -- permutation index offset (default = 0)
step      = 1           -- permutation index increment (default = 1)


-- section 'random' ------------------------------------------------------------
                        
seed      = 40           -- random seed


-- section 'calling convention' (useful on Windows for now) --------------------
                        
api       = ""          -- calling convention ("__stdcall" or "__fastcall")
            -- for gcc use "__attribute__((__stdcall__))" or "__attribute__((__fastcall__))"
            -- for ms ?
ccprefix  = ""          -- signature prefix ("_s" (stdcall), "_f" (gcc fastcall) or "_F" (microsoft fastcall))
