
FUNCTION(source_hash DIR DEPEND)

    ADD_CUSTOM_TARGET(source_hash
        DEPENDS ${DEPEND}
        COMMAND "dir=<${DIR}>; (find \"$dir\" -type f -exec md5sum {} +; find \"$dir\" -type d) | LC_ALL=C sort | md5sum"
    )

ENDFUNCTION(source_hash)
