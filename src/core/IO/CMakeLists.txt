CORE_ADD_SOURCE(
		FileSystem.h
		FileSystem.cpp

		ConfigManager.h
		ConfigManager.cpp

		SOIL2/common.cpp
		SOIL2/common.hpp
		SOIL2/image_DXT.c
		SOIL2/image_DXT.h
		SOIL2/image_helper.c
		SOIL2/image_helper.h
		SOIL2/pkm_helper.h
		SOIL2/pvr_helper.h
		SOIL2/SOIL2.c
		SOIL2/SOIL2.h
		SOIL2/stb_image_write.h
		SOIL2/stb_image.h
		SOIL2/stbi_DDS_c.h
		SOIL2/stbi_DDS.h
		SOIL2/stbi_ext_c.h
		SOIL2/stbi_ext.h
		SOIL2/stbi_pkm_c.h
		SOIL2/stbi_pkm.h
		SOIL2/stbi_pvr_c.h
		SOIL2/stbi_pvr.h
		SOIL2/stbi_qoi_c.h
		SOIL2/stbi_qoi_write.h
		SOIL2/stbi_qoi.h
		SOIL2/wfETC.c
		SOIL2/wfETC.h

		LogSystem/LogSystem.hpp
		LogSystem/LogSystem.cpp
	)

#add_subdirectory(SOIL2)

