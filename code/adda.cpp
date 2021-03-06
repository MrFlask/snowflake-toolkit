//@ {
//@	 "targets":[{"name":"adda.o","type":"object"}]
//@	}

#include "adda.h"
#include "file_out.h"
#include "file_in.h"
#include "grid.h"

using namespace SnowflakeModel;

void SnowflakeModel::addaShapeWrite(const Grid& grid,FileOut&& dest)
	{
	auto min=grid.minGet();
	auto size=grid.sizeGet();
	auto step=grid.stepGet();
	dest.printf("# ADDA geometry file generated by snowflake-toolkit\n")
		.printf("# Bounding box starts at (%.7g,%.7g,%.7g)\n"
			,min.x,min.y,min.z)
		.printf("# Number of divisions are (%d,%d,%d)\n",size.x,size.y,size.z)
		.printf("# Step sizes are (%.7g,%.7g,%.7g)\n",step.x,step.y,step.z);

	grid.pointsVisit([&dest](int x,int y,int z,uint8_t value)
		{
		if(value)
			{dest.printf("%d %d %d\n",x,y,z);}
		});
	}

std::vector<Vector> SnowflakeModel::pointsLoad(FileIn&& source)
	{
	int ch_in=0;
	std::string buffer;
	int state=0;
	int index=0;
	glm::vec3 point;
	std::vector<glm::vec3> ret;
	while((ch_in=source.getc())!=EOF)
		{
		switch(state)
			{
			case 0:
				switch(ch_in)
					{
					case '#':
						state=1;
						if(index!=0)
							{throw "Uncompleted point";}
						break;
					case '\r':
						break;
					case'\n':
						point[index]=atof(buffer.c_str());
						ret.push_back(point);
						buffer.clear();
						index=0;
						break;
					case ' ':
						point[index]=atof(buffer.c_str());
						buffer.clear();
						++index;
						if(index==3)
							{throw "Too many values";}
						break;
					default:
						buffer+=ch_in;
					}
				break;
			case 1:
				switch(ch_in)
					{
					case '\n':
					case '\r':
						state=0;
						buffer.clear();
						break;
					default:
						break;
					}
				break;
			}
		}
	return std::move(ret);
	}


BoundingBox SnowflakeModel::boundingBoxGet(const std::vector<Vector>& points)
	{
	auto ptr=points.data();
	auto ptr_end=ptr + points.size();
	if(ptr==ptr_end)
		{return BoundingBox{{-1.0f,-1.0f,-1.0f,1.0f},{1.0f,1.0f,1.0f,1.0f}};}
	auto ret=BoundingBox{Point(*ptr,1.0f),Point(*ptr,1.0f)};
	++ptr;
	while(ptr!=ptr_end)
		{
		auto p=Point(*ptr,1.0f);
		ret.m_min=glm::min(p,ret.m_min);
		ret.m_max=glm::max(p,ret.m_max);
		++ptr;
		}
	ret.m_min-=Point(0.5f,0.5f,0.5f,0.0f);
	ret.m_max+=Point(0.5f,0.5f,0.5f,0.0f);
	return ret;
	}
