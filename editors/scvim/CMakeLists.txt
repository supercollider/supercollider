install(FILES bin/sclangpipe_app bin/scvim bin/scvim_make_help
		DESTINATION bin
		PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

install(DIRECTORY ftplugin
		DESTINATION share/vim/addons)

install(DIRECTORY indent
		DESTINATION share/vim/addons)

install(DIRECTORY syntax
		DESTINATION share/vim/addons)

install(FILES scclasses/SCVim.sc
		DESTINATION share/SuperCollider/Extensions/scvim)

install(FILES scvimrc
		DESTINATION share/scvim)
