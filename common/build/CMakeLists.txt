install(DIRECTORY SCClassLibrary
	DESTINATION share/SuperCollider
	PATTERN *sc
)

install(DIRECTORY Help
	DESTINATION share/SuperCollider
	PATTERN *html
)

install(DIRECTORY examples
	DESTINATION share/SuperCollider
	PATTERN *scd
)
