install(FILES bin/sclangpipe_app bin/scvim
		DESTINATION bin
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

install(DIRECTORY ftplugin
		DESTINATION share/vim/addons)

install(DIRECTORY indent
		DESTINATION share/vim/addons)

install(DIRECTORY syntax
		DESTINATION share/vim/addons)

configure_file(registry/supercollider-vim.yaml.in registry/supercollider-vim.yaml)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/registry/supercollider-vim.yaml
		DESTINATION share/vim/registry)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	install(FILES scclasses/SCVim.sc
			DESTINATION /Library/Application\ Support/SuperCollider/Extensions/scvim
			PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
ELSE()
	install(FILES scclasses/SCVim.sc
         DESTINATION share/SuperCollider/Extensions/scvim
	 PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
ENDIF()

install(FILES scvimrc SCVim.scd
		DESTINATION share/scvim)
