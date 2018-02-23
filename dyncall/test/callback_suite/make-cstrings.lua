require "config"
for line in io.lines() do
  io.write( '"' .. ccprefix .. line .. '",\n' )
end
io.flush()

