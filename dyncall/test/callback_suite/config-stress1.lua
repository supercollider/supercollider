-- section 'main' --------------------------------------------------------------
                  
mode      = "designed"    -- generator mode: 'random', 'ordered' or 'designed' type sequences
designfile= "stress1.cfg" -- design file 

function nlines()
  local cnt = 0
  for l in io.open(designfile):lines() do
    cnt = cnt + 1
  end
  return cnt
end

nsigs     = nlines()    -- number of test cases

minargs   = 0           -- minimum num. of supported arguments (>= 0)
maxargs   = 20          -- maximum num. of supported arguments (>= minargs)

-- section 'types' (not used by 'designed') ------------------------------------

argtypes  = "BcCsSiIjJlLpfd" -- "BcCsSiIjJlLpfd"    -- supported argument types
rettypes  = argtypes    -- supported return types (currently no void support)


-- section 'ordered' -----------------------------------------------------------

offset    = 0           -- permutation index offset (default = 1)
step      = 1           -- permutation index increment (default = 1)


-- section 'random' ------------------------------------------------------------
                        
seed      = 1           -- random seed


-- section 'calling convention' (useful on Windows for now) --------------------
                        
api       = ""          -- calling convention ("__stdcall" or "__fastcall")
            -- for gcc use "__attribute__((__stdcall__))" or "__attribute__((__fastcall__))"
            -- for ms ?
ccprefix  = ""          -- signature prefix ("_s" (stdcall), "_f" (gcc fastcall) or "_F" (microsoft fastcall))
