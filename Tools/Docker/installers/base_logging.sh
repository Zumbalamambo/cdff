depends_tools_plugin_manager=base_cmake;

function install4infuse_base_logging {
if [[ ! -n $(find $PKG_DIR -name 'base_logging*') ]]; then
	fetchgit_function base_logging master https://github.com/rock-core/base-logging 860adb7495ef90af321c3e0041d441c8f648d3c6
	cmake \
	    -D CMAKE_BUILD_TYPE=RELEASE \
	    -D CMAKE_INSTALL_PREFIX=$INSTALL_DIR \
	    $SOURCE_DIR/base_logging

	make --jobs=${CPUS}
	install_function base_logging master
	clean_function base_logging
fi
}