function install4infuse_base_boost_serialization {
if [[ ! -n $(find $PKG_DIR -name 'base_boost_serialization*') ]]; then
	fetchgit_function base_boost_serialization master https://github.com/envire/base-boost_serialization.git

	cmake \
	    -D CMAKE_BUILD_TYPE=RELEASE \
	    -D CMAKE_INSTALL_PREFIX=$INSTALL_DIR \
	    -D CMAKE_CXX_FLAGS:STRING=-std=c++11 \
	    $SOURCE_DIR/base_boost_serialization

	make --jobs=${CPUS}
	install_function base_boost_serialization master
	clean_function base_boost_serialization
fi
}
