DL=wget
all: lua-5.1.4

lua-5.1.4.tar.gz: 
	${DL} http://www.lua.org/ftp/lua-5.1.4.tar.gz
lua-5.1.4: lua-5.1.4.tar.gz
	gunzip -c $? | tar xf -

clean:
	rm -rf lua-5.1.4.tar.gz lua-5.1.4
