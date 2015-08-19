#ifdef __WAND__
target[name[volume_convex.h] type[include]]
dependency[volume_convex.o]
#endif

#ifndef SNOWFLAKEMODEL_VOLUMECONVEX_H
#define SNOWFLAKEMODEL_VOLUMECONVEX_H

#include "vector.h"
#include "bounding_box.h"
#include <vector>
#include <map>

namespace SnowflakeModel
	{
	class VoxelBuilder;
	class VolumeConvex
		{
		public:
 			typedef uint16_t VertexIndex;
			typedef uint16_t FaceIndex;
			static constexpr uint16_t VERTEX_COUNT=3;
			typedef Point Vertex;
			class Face
				{
				public:
					Face(VertexIndex v0,VertexIndex v1,VertexIndex v2
						,const VolumeConvex& vc):r_vc(&vc)
						{
						m_verts[0]=v0;
						m_verts[1]=v1;
						m_verts[2]=v2;
						}

					const Vertex& vertexGet(int index) const
						{return r_vc->vertexGet(m_verts[index]);}

					mutable Point m_mid;
					mutable Vector m_normal;
					mutable Vector m_normal_raw;

					void directionChange()
						{std::swap(m_verts[0],m_verts[1]);}

					VertexIndex vertexIndexGet(int index) const
						{return m_verts[index];}

					void parentSet(const VolumeConvex& vc)
						{r_vc=&vc;}

				private:
					VertexIndex m_verts[VERTEX_COUNT];
					const VolumeConvex* r_vc;
				};

			VolumeConvex():
				m_flags_dirty(BOUNDINGBOX_DIRTY|MIDPOINT_DIRTY
					|FACES_NORMAL_DIRTY|FACES_MIDPOINT_DIRTY|VOLUME_DIRTY
					|AREA_VISIBLE_DIRTY)
				{}

			VolumeConvex(const VolumeConvex& vc);

			size_t vertexAdd(const Matrix& T,const Point& v)
				{
				m_vertices.push_back( T*v );
				return m_vertices.size();
				}

			size_t vertexAdd(const Vertex& v)
				{
				m_vertices.push_back(v);
				return m_vertices.size();
				}

			const Vertex* verticesBegin() const
				{return m_vertices.data();}

			const Vertex* verticesEnd() const
				{return m_vertices.data()+m_vertices.size();}

			Vertex* verticesBegin()
				{return m_vertices.data();}

			Vertex* verticesEnd()
				{return m_vertices.data()+m_vertices.size();}

			size_t verticesCount() const
				{return m_vertices.size();}

			const Vertex& vertexGet(size_t index) const
				{return m_vertices[index];}



			void faceAdd(const Face& face)
				{m_faces.push_back(face);}

			const Face* facesBegin() const
				{return m_faces.data();}

			const Face* facesEnd() const
				{return m_faces.data()+m_faces.size();}

			Face* facesBegin()
				{return m_faces.data();}

			Face* facesEnd()
				{return m_faces.data()+m_faces.size();}


			const Face& faceGet(FaceIndex i) const
				{return m_faces[i];}

			size_t facesCount() const
				{return m_faces.size();}

			void vertexGroupSet(const std::string& name,VertexIndex index)
				{m_vertex_groups[name].push_back(index);}



			const BoundingBox& boundingBoxGet() const
				{
				if(m_flags_dirty&BOUNDINGBOX_DIRTY)
					{boundingBoxCompute();}
				return m_bounding_box;
				}

			const Point& midpointGet() const
				{
				if(m_flags_dirty&MIDPOINT_DIRTY)
					{midpointCompute();}
				return m_mid;
				}

			float volumeGet() const
				{
				if(m_flags_dirty&VOLUME_DIRTY)
					{volumeCompute();}
				return m_volume;
				}

			float areaVisibleGet() const
				{
				if(m_flags_dirty&AREA_VISIBLE_DIRTY)
					{areaVisibleCompute();}
				return m_area_visible;
				}


			void transform(const Matrix& T);
			void transformGroup(const std::string& name,const Matrix& T);

			bool inside(const Point& v) const;

			const Face* cross(const Face& face) const;

			void geometrySample(VoxelBuilder& builder) const;

			void normalsFlip();

			void facesNormalCompute() const;
			void facesMidpointCompute() const;
			void boundingBoxCompute() const;
			void midpointCompute() const;
			void volumeCompute() const;
			void areaVisibleCompute() const;

			FaceIndex facesOutCount() const
				{return m_faces_out.size();}

			const FaceIndex* facesOutBegin() const
				{return m_faces_out.data();}

			const FaceIndex* facesOutEnd() const
				{return facesOutBegin()+facesOutCount();}

			void faceOutAdd(FaceIndex i)
				{m_faces_out.push_back(i);}

			void faceOutRemove(size_t i)
				{
				assert(i<m_faces_out.size());
				m_faces_out.erase(m_faces_out.begin()+i);
				m_flags_dirty|=AREA_VISIBLE_DIRTY;
				}

			const Face& faceOutGet(size_t i) const
				{return m_faces[m_faces_out[i]];}

			bool normalsDirty() const
				{return m_flags_dirty&FACES_NORMAL_DIRTY;}


		private:
			std::vector< Vertex > m_vertices;
			std::vector< Face > m_faces;
			std::vector<FaceIndex> m_faces_out;
			std::map< std::string, std::vector<VertexIndex> > m_vertex_groups;

			mutable BoundingBox m_bounding_box;
			mutable Point m_mid;
			mutable float m_volume;
			mutable float m_area_visible;
			mutable uint32_t m_flags_dirty;

			static constexpr uint32_t BOUNDINGBOX_DIRTY=1;
			static constexpr uint32_t MIDPOINT_DIRTY=2;
			static constexpr uint32_t FACES_NORMAL_DIRTY=4;
			static constexpr uint32_t FACES_MIDPOINT_DIRTY=8;
			static constexpr uint32_t VOLUME_DIRTY=16;
			static constexpr uint32_t AREA_VISIBLE_DIRTY=32;
		};
	}

#endif
