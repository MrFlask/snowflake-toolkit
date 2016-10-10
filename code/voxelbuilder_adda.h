//@	{
//@	    "dependencies_extra":[
//@	        {
//@	            "ref":"voxelbuilder_adda.o",
//@	            "rel":"implementation"
//@	        }
//@	    ],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"voxelbuilder_adda.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_VOXELBUILDERADDA_H
#define SNOWFLAKEMODEL_VOXELBUILDERADDA_H

#include "voxel_builder.h"
#include "bounding_box.h"

namespace SnowflakeModel
	{
	class FileOut;	
	
	class VoxelbuilderAdda:public VoxelBuilder
		{
		public:
			VoxelbuilderAdda(FileOut& dest,unsigned int n_x
				,unsigned int n_y,unsigned int n_z
				,const BoundingBox& bounding_box);

			VoxelbuilderAdda(FileOut& dest,double dx,double dy,double dz
				,const BoundingBox& bounding_box);
			
			~VoxelbuilderAdda();
			bool fill(const PointInt& v);
			PointInt quantize(const Point& p) const;
			Point dequantize(const PointInt& p) const;

			VectorInt gridSizeGet() const noexcept
				{return {m_n_x,m_n_y,m_n_z};}
			
		private:
			FileOut& r_dest;
			double m_dx;
			double m_dy;
			double m_dz;
			int m_n_x;
			int m_n_y;
			int m_n_z;
			BoundingBox m_bounding_box;
		};
	}

#endif
