file(GLOB src *)
idf_component_register(SRCS ${src} REQUIRES smooth_component)