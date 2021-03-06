//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"aggregate_edge.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_AGGREGATEEDGE_H
#define SNOWFLAKEMODEL_AGGREGATEEDGE_H

#include "vector.h"

namespace SnowflakeModel
	{
	class AggregateNode;
	
	struct AggregateEdge
		{
		AggregateEdge(AggregateNode* node_parent,const Vector& offset_parent
			,AggregateNode* node_child,const Vector& offset_child
			,float angle_x,float angle_y,float angle_z):
			 m_offset_parent(offset_parent),m_offset_child(offset_child)
			,r_node_parent(node_parent),r_node_child(node_child)
			,m_angle_x(angle_x),m_angle_y(angle_y),m_angle_z(angle_z)
			{}

		Vector m_offset_parent;
		Vector m_offset_child;
		AggregateNode* r_node_parent;
		AggregateNode* r_node_child;
		float m_angle_x;
		float m_angle_y;
		float m_angle_z;
		};
	}
#endif
