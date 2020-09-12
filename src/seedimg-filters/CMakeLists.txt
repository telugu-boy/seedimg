add_subdirectory(seedimg-filters-core)
if(OPENCL_ENABLED)
  add_subdirectory(seedimg-filters-ocl)
endif()