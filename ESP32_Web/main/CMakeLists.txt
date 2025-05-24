idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ".")
                    
# 嵌入 HTML 和 PNG 到可执行文件(将源文件转化成bin，二进制文件)
target_add_binary_data(${COMPONENT_TARGET} "index.html" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "pic.png" BINARY)
