//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"ice_particle.o",
//@	            "type":"object"
//@	        }
//@	    ]
//@	}
#include "ice_particle.h"

using namespace SnowflakeModel;

namespace SnowflakeModel
	{
	template<>
	const DataDump::FieldDescriptor DataDump::MetaObject<IceParticle>::fields[]=
		{
		 {"velocity",offsetOf(&IceParticle::m_velocity),vectorObj().typeGet()}
		,{"density",offsetOf(&IceParticle::m_density),DataDump::MetaObject<decltype(IceParticle::m_density)>().typeGet()}
		,{"dead",offsetOf(&IceParticle::m_dead),DataDump::MetaObject<decltype(IceParticle::m_dead)>().typeGet()}
		};
	}

void IceParticle::solidGenerate() const
	{
	if(m_flags_dirty&DEFORMATIONS_DIRTY)
		{
		m_deformations=r_solid->deformationTemplatesGet();
		m_flags_dirty&=~DEFORMATIONS_DIRTY;
		}
	m_solid_generated.clear();
	auto subvolume=r_solid->subvolumesBegin();
	while(subvolume!=r_solid->subvolumesEnd())
		{
		VolumeConvex& vc=m_solid_generated.subvolumeAdd(*subvolume);
		auto deformation=m_deformations.data();
		auto deformations_end=deformation + m_deformations.size();
		while(deformation!=deformations_end)
			{
			if(*(deformation->nameGet().data())!='$')
				{
				vc.transformGroup(deformation->nameGet(),deformation->matrixGet());
				}
			else
			if(deformation->nameGet()=="$global")
				{vc.transform(deformation->matrixGet());}
			else
				{throw "Reserved group name";}
			++deformation;
			}
		++subvolume;
		}

	if(r_solid->mirrorFlagTest(Solid::MIRROR_HEADING))
		{
		auto bb=m_solid_generated.boundingBoxGet();
		Matrix trans;
		trans=glm::translate(trans,Vector(-bb.m_min.x,0,0));
		m_solid_generated.transform(trans,0);

		auto volume_copy=m_solid_generated;
		Matrix T;
		T[0][0]=-1;
		m_solid_generated.merge(T,volume_copy,1);
		}
	m_solid_generated.centerBoundingBoxAt(Point{0,0,0,1});
	m_flags_dirty&=~VOLUME_DIRTY;
	}

void IceParticle::parameterSet(const std::string& name,float value)
	{
	if(m_flags_dirty&DEFORMATIONS_DIRTY)
		{
		m_deformations=r_solid->deformationTemplatesGet();
		m_flags_dirty&=~DEFORMATIONS_DIRTY;
		}
	auto deformation=m_deformations.data();
	auto deformations_end=deformation+m_deformations.size();
	while(deformation!=deformations_end)
		{
		auto i=deformation->parameterFind(name);
		if(i!=nullptr)
			{
			auto index_current=i->data();
			auto index_end=index_current+i->size();
			while(index_current!=index_end)
				{
				(*deformation)[*index_current]=value;
				++index_current;
				}
			}
		++deformation;
		}
	m_flags_dirty|=VOLUME_DIRTY;
	}

void IceParticle::solidScale(float c)
	{
	if(m_flags_dirty&VOLUME_DIRTY)
		{solidGenerate();}

	Matrix S;
	S=glm::scale(S,Vector(c,c,c));
	m_solid_generated.transform(S,c<0);
	}

void IceParticle::write(const char* id,DataDump& dump) const
	{
	auto group=dump.groupCreate(id);
	std::string path(id);
	dump.write((path + "/data").c_str(),this,1);

		{
		auto deformation_group=path + "/deformations";
		auto deformations=dump.groupCreate(deformation_group.c_str());
		auto deformation=m_deformations.data();
		auto deformations_end=deformation + m_deformations.size();
		deformation_group+='/';
		size_t k=0;
		while(deformation!=deformations_end)
			{
			auto def_name=deformation_group + std::to_string(k);
			deformation->write(def_name.c_str(),dump);
			++deformation;
			++k;
			}
		}

		m_solid_generated.write((path+"/solid_generated").c_str(),dump);
	}

