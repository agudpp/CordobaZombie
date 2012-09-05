
IF(NOT TINYXPATH_PATH)
	set(TINYXPATH_PATH .)
endif()

set(TINYXPATH_SOURCES
${TINYXPATH_PATH}/action_store.cpp  
	${TINYXPATH_PATH}/tinystr.cpp
	${TINYXPATH_PATH}/xml_util.cpp          
	${TINYXPATH_PATH}/xpath_stream.cpp
	${TINYXPATH_PATH}/tinyxml.cpp       
	${TINYXPATH_PATH}/xpath_expression.cpp  
	${TINYXPATH_PATH}/xpath_syntax.cpp
	${TINYXPATH_PATH}/lex_util.cpp      
	${TINYXPATH_PATH}/tinyxmlerror.cpp   
	${TINYXPATH_PATH}/xpath_processor.cpp
	${TINYXPATH_PATH}/tinyxmlparser.cpp  
	${TINYXPATH_PATH}/xpath_stack.cpp
	${TINYXPATH_PATH}/node_set.cpp      
	${TINYXPATH_PATH}/tokenlist.cpp      
	${TINYXPATH_PATH}/xpath_static.cpp
)

set(TINYXPATH_HEADERS
	${TINYXPATH_PATH}/action_store.h  
	${TINYXPATH_PATH}/node_set.h        
	${TINYXPATH_PATH}/xml_util.h          
	${TINYXPATH_PATH}/xpath_stream.h
	${TINYXPATH_PATH}/byte_stream.h   
	${TINYXPATH_PATH}/tinystr.h        
	${TINYXPATH_PATH}/xpath_expression.h  
	${TINYXPATH_PATH}/xpath_syntax.h
	${TINYXPATH_PATH}/htmlutil.h      
	${TINYXPATH_PATH}/tinyxml.h         
	${TINYXPATH_PATH}/xpath_processor.h
	${TINYXPATH_PATH}/lex_token.h     
	${TINYXPATH_PATH}/tinyxpath_conf.h  
	${TINYXPATH_PATH}/xpath_stack.h
	${TINYXPATH_PATH}/lex_util.h      
	${TINYXPATH_PATH}/tokenlist.h       
	${TINYXPATH_PATH}/xpath_static.h
)
