cmake_minimum_required (VERSION 2.6)
project (tmcBitDecoder)

find_package( Boost 1.40 COMPONENTS program_options REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

add_executable(tmcBitDecoder bitDecoder.cpp   
				Event.cpp     
				Locations.cpp   
				tmcInfoBlock.cpp
				bitsetqueue.cpp  
				Events.cpp    
				rdsDecoder.cpp  
				tmcMsg.cpp
				enum.cpp         
				Location.cpp  
				tmcDecoder.cpp
				)

target_link_libraries( tmcBitDecoder ${Boost_LIBRARIES} )
