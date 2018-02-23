require"config"

rtypes   = "v"..types

math.randomseed(2342)
local sigs = { }
local id
for i = 1, ncases do
  id = math.random(#rtypes)
  local nargs = math.random(minargs,maxargs)
  local sig   = { rtypes:sub(id,id)}
  for j = 1, nargs do
    id = math.random(#types)
    sig[#sig+1] = types:sub(id,id)
  end
  io.write(table.concat(sig))
  io.write("\n")
end

