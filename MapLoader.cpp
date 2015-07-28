#include "MapLoader.h"
#include "Vector.h"
#include <cstring>
#include <openvdb/openvdb.h>
#include <stdlib.h>
#include <fstream>
using namespace std;

/************************ This loads the map from openVDB.
                          You can use the native viewer once you complete this.
                          There are comments for each snippet of code.
                          Hint: http://www.openvdb.org/documentation/doxygen/codeExamples.html **************/



bool**
MapLoader::loadVDBMap(char* filename)
{
    //Initialize the OpenVDB library.
	openvdb::initialize();

    // Create a VDB file object.
	openvdb::io::File file(filename);


    // Open the file. This reads the file header, but not any grids.
		file.open();

    //Loop over all grids in the file and retrieve a shared pointer to "SDF". Close file
	
	openvdb::GridBase::Ptr baseGrid;
	for (openvdb::io::File::NameIterator nameIter = file.beginName(); nameIter != file.endName(); ++nameIter)
	{
    		// Read in only the grid we are interested in.
   			 if (nameIter.gridName() == "SDF") {
        			baseGrid = file.readGrid(nameIter.gridName());
    			} else {
        			std::cout << "skipping grid " << nameIter.gridName() << std::endl;
    			}
	}
	file.close();

    //cast the generic grid pointer to a FloatGrid pointer.
	openvdb::FloatGrid::Ptr grid = openvdb::gridPtrCast<openvdb::FloatGrid>(baseGrid);
    //Get metadata into strings, see .meta file 

		for (openvdb::MetaMap::MetaIterator iter = grid->beginMeta(); iter != grid->endMeta(); ++iter)
		{
		    const std::string& name = iter->first;
		    openvdb::Metadata::Ptr value = iter->second;
		    std::string valueAsString = value->str();
		    std::cout << name << " = " << valueAsString << std::endl;
		}

    //Convert strings to numbers
	
	x_boundary=stoi(grid->metaValue<std::string>("boundx"));
	y_boundary=stoi(grid->metaValue<std::string>("boundy"));
	endRadius=stof(grid->metaValue<std::string>("end_radius"));
	int endx=stoi(grid->metaValue<std::string>("endx"));
	int endy=stoi(grid->metaValue<std::string>("endy"));
	startRadius=stof(grid->metaValue<std::string>("start_radius"));
	int startx=stoi(grid->metaValue<std::string>("startx"));
	int starty=stoi(grid->metaValue<std::string>("starty"));
	//int file_mem_bytes=stoi(grid->metaValue<std::string>("file_mem_bytes"));
	//int file_voxel_count=stoi(grid->metaValue<std::string>("file_voxel_count"));
    // Create the map data - 2D map and SDF arrays. Allocate memory
	mapData = (bool**)malloc(sizeof(bool*)*x_boundary);
	grid_array = (float**)malloc(sizeof(float*)*x_boundary);

	for(int i=0;i<x_boundary;i++){
		mapData[i]=(bool*)malloc(sizeof(bool)*y_boundary);
		grid_array[i] = (float*)malloc(sizeof(float)*y_boundary);
	}

    //Get an accessor for coordinate-based access to voxels.
		openvdb::FloatGrid::Accessor accessor = grid->getAccessor();
		for(int i=0;i<x_boundary;i++){
			for(int j=0;j<y_boundary;j++){
				openvdb::Coord xyz(i,j,0);
				grid_array[i][j]=accessor.getValue(xyz);
				//std::cout << "Grid" << xyz << " = " << accessor.getValue(xyz) << std::endl;
				if (accessor.getValue(xyz)<5){
					mapData[i][j]=false;
				}
				else{
					mapData[i][j]=true;
				}
			}
		}
		for(int i=0;i<x_boundary;i++){
			for(int j=0;j<y_boundary;j++){
				std::cout<<mapData[i][j] << " ";
			}
			std::cout << "\n";
		}
    
    /* Iterate through SDF arrayusing the accessor to get SDF values. 
       Add true or false to map data according to the SDF value. */


    //Get values of Vectors posStart and posEnd
		posStart=Vec2f(startx,starty);
		posEnd=Vec2f(endx,endy);
	

    //Change "loaded" flag
	loaded=true;

    //Return map data array
	return mapData;
}

float**
MapLoader::getSDF(){
	return grid_array;
}

MapLoader::MapLoader() {

}

Vec2f
MapLoader::getStartPosition() {
	return posStart;
}

Vec2f
MapLoader::getEndPosition() {
	return posEnd;
}

unsigned int
MapLoader::gety_boundary() {
	return y_boundary;
}

unsigned int
MapLoader::getx_boundary() {
	return x_boundary;
}	

unsigned int
MapLoader::getStartRadius() {
	return startRadius;
}

unsigned int
MapLoader::getEndRadius() {
	return endRadius;
}
