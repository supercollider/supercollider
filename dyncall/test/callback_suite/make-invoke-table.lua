require "config"

local i
for i = 1, nsigs do
  line = "&f" .. i .. ",\n"
  io.write(line)
end

