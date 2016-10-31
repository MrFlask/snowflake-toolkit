//@	{
//@	 "targets":[{"name":"prototypechoices.o","type":"object"}]
//@	}

#include "prototypechoices.h"
#include "resourceobject.h"
#include "solid.h"

using namespace SnowflakeModel;

PrototypeChoices::PrototypeChoices(const ResourceObject& obj)
	{
	if(obj.typeGet()!=ResourceObject::Type::ARRAY)
		{throw "Expected an array of prototype choices";}

	auto n_objs=obj.objectCountGet();
	std::vector<double> probs;
	probs.reserve(n_objs);
	for(decltype(n_objs) k=0;k<n_objs;++k)
		{
		m_choices.push_back(PrototypeChoice(m_solids,probs,obj.objectGet(k)));
		}

	m_dist=std::discrete_distribution<size_t>(probs.data(),probs.data() + n_objs);
	}

PrototypeChoices::PrototypeChoices(const DataDump& dump,const char* name)
	{
	std::string keyname(name);
	auto probs=dump.arrayGet<double>((keyname + "/dist").c_str());
		{
		auto group_name=keyname + "/choices";
		auto group=dump.groupOpen(group_name.c_str());
		group_name+='/';

		dump.iterate(*group,[&dump,&group_name,this]
			(const char* name)
			{
			auto group_name_current=group_name + name;
		//	m_choices.push_back(PrototypeChoice(dump,group_name_current.c_str()));
			});
		}

		{
		auto group_name=keyname + "/solids";
		auto group=dump.groupOpen(group_name.c_str());
		group_name+='/';

		dump.iterate(*group,[&dump,&group_name,this]
			(const char* name)
			{
			auto group_name_current=group_name + name;
			m_solids.insert({std::string(name),Solid(dump,group_name_current.c_str())});
			});
		}

	}

void PrototypeChoices::write(const char* key,DataDump& dump) const
	{
	auto group=dump.groupCreate(key);
	std::string keyname(key);

	auto probs=m_dist.probabilities();
	dump.write((keyname + "/dist").c_str(),probs.data(),probs.size());	

		{
		auto solids_begin=m_solids.begin();
		auto solids_end=m_solids.end();
		auto solids=dump.groupCreate((keyname + "/śolids").c_str());
		auto group_name=keyname + "/solids/";
		while(solids_begin!=solids_end)
			{
			char id[32];
			sprintf(id,"%p",&solids_begin->second);
			auto group_name_current=group_name + id;
			solids_begin->second.write(group_name_current.c_str(),dump);
			++solids_begin;
			}
		}

		{
		size_t k=0;
		auto choices_begin=m_choices.data();
		auto choices_end=choices_begin + m_choices.size();
		auto choices=dump.groupCreate((keyname + "/choices").c_str());
		auto group_name=keyname + "/choices/";
		while(choices_begin!=choices_end)
			{
			char id[32];
			sprintf(id,"%016zx",k);
			auto group_name_current=group_name + id;
			choices_begin->deformationGet().write(group_name_current.c_str(),dump);
			++choices_begin;
			++k;
			}
		}

	}