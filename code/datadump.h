//@	{
//@	"targets":[{"name":"datadump.h","type":"include"}]
//@	,"dependencies_extra":[{"ref":"datadump.o","rel":"implementation"}]
//@	}

#ifndef SNOWFLAKEMODEL_DATADUMP_H
#define SNOWFLAKEMODEL_DATADUMP_H

#include <memory>
#include <string>
#include <type_traits>
#include <cstdint>
#include <vector>

namespace H5
	{
	class H5File;
	class DataType;
	class Group;
	class StrType;
	}

namespace SnowflakeModel
	{
    template <typename T, std::size_t N>
    constexpr std::size_t countOf(T const (&)[N]) noexcept
		{return N;}

	template<typename T, typename U> constexpr size_t offsetOf(U T::*member)
		{
		return (char*)&((T*)nullptr->*member) - (char*)nullptr;
		}

	class DataDump
		{
		public:
			class StringHolder
				{
				public:
					StringHolder():m_buffer(nullptr)
						{}

					~StringHolder()
						{free(m_buffer);}

					StringHolder(const StringHolder&)=delete;

					StringHolder& operator=(const StringHolder&)=delete;

					StringHolder(StringHolder&& x) noexcept:
						m_buffer(x.m_buffer)
						{x.m_buffer=nullptr;}

					StringHolder& operator=(StringHolder&& x) noexcept
						{
						std::swap(x.m_buffer,m_buffer);
						return *this;
						}

					operator const char*() const
						{return m_buffer;}

					const char* begin() const
						{return m_buffer;}

				private:
					char* m_buffer;
				};

			class GroupIterateCallback
				{
				public:
					virtual void groupVisit(const char* group_name)=0;
				};
				

			class ArrayReaderImpl;
			typedef std::unique_ptr<ArrayReaderImpl,void(*)(ArrayReaderImpl*)>
				ArrayReaderHandle;

			static size_t dataRead(ArrayReaderImpl& reader,void* buffer,size_t n_elems);
			static uintmax_t size(const ArrayReaderImpl& impl);

			ArrayReaderHandle arrayReaderCreate(const H5::DataType& type,const char* objname) const;
			ArrayReaderHandle arrayReaderCreate(H5::DataType&& type,const char* objname) const=delete;

			template<class T>
			class ArrayReader
				{
				public:
					ArrayReader(DataDump&& dump,const char* objname)=delete;

					ArrayReader(const DataDump& dump,const char* objname):
						m_impl(dump.arrayReaderCreate(MetaObject<T>().typeGet(),objname))
						{}
					
					size_t dataRead(T* data,size_t n_elems)
						{return DataDump::dataRead(*m_impl,data,n_elems);}

					uintmax_t size() const noexcept
						{return DataDump::size(*m_impl);}

					std::vector<T> dataRead()
						{
						std::vector<T> ret;
						ret.resize(size());
						dataRead(ret.data(),ret.size());
						return std::move(ret);
						}

				private:
					ArrayReaderHandle m_impl;
				};

			class MatrixReaderImpl;
			typedef std::unique_ptr<MatrixReaderImpl,void(*)(MatrixReaderImpl*)>
				MatrixReaderHandle;
			static void dataRead(MatrixReaderImpl& reader,void* buffer,size_t n_rows,size_t n_cols);
			static uintmax_t nRowsGet(const MatrixReaderImpl& impl);
			static uintmax_t nColsGet(const MatrixReaderImpl& impl);
			MatrixReaderHandle matrixReaderCreate(const H5::DataType& type,const char* objname) const;
			MatrixReaderHandle matrixReaderCreate(H5::DataType&& type,const char* objname) const=delete;


			typedef std::unique_ptr<H5::DataType,void(*)(H5::DataType*)> DataTypeHandle;
			typedef std::unique_ptr<H5::Group,void(*)(H5::Group*)> GroupHandle;

			enum class IOMode:int{READ,WRITE};

			explicit DataDump(const char* filename,IOMode mode);

			~DataDump();

			struct FieldDescriptor
				{
				const char* name;
				size_t offset;
				const H5::DataType& type;
				};

			template<class T,class Enable=void>
			class MetaObject
				{
				public:
					const H5::DataType& typeGet() const noexcept;
				};

			template<class T>
			class MetaObject<T,typename std::enable_if<std::is_same<T,const char*>::value
				|| std::is_same<T,StringHolder>::value>::type>
				{
				public:
					MetaObject();
					const H5::DataType& typeGet() const noexcept
						{return r_cstr;}
				private:
					const H5::DataType& r_cstr;
				};

			template<class T>
			class MetaObject<T
				,typename std::enable_if<std::is_class<T>::value 
					&& !std::is_same<T,StringHolder>::value>::type>
				{
				public:
					static const DataDump::FieldDescriptor fields[];
					static const size_t field_count;
					MetaObject():m_type(compoundMake(fields,field_count,sizeof(T)))
						{}

					const H5::DataType& typeGet() const noexcept
						{return *m_type;}

				private:
					DataTypeHandle m_type;
				};

			template<class T>
			void write(const char* objname,const T* data,size_t n_elems)
				{dataWrite(MetaObject<T>().typeGet(),objname,n_elems,data);}

			template<class T>
			void write(const char* objname,const T* data,size_t n_rows,size_t n_cols)
				{dataWrite(MetaObject<T>().typeGet(),objname,n_rows,n_cols,data);}

			template<class T>
			ArrayReader<T> arrayRead(const char* objname) const
				{return ArrayReader<T>(*this,objname);}

			template<class T>
			std::vector<T> arrayGet(const char* objname) const
				{return arrayRead<T>(objname).dataRead();}

			template<class T>
			std::pair<std::vector<T>,size_t> matrixGet(const char* objname) const
				{
				auto reader=matrixReaderCreate(MetaObject<T>().typeGet(),objname);
				std::vector<T> ret;
				auto n_rows=nRowsGet(*reader);
				auto n_cols=nColsGet(*reader);
				ret.resize(n_rows*n_cols);
				dataRead(*reader,ret.data(),n_rows,n_cols);
				return {std::move(ret),n_cols};
				}

			template<class T>
			void matrixGet(const char* objname,T* data,size_t n_rows,size_t n_cols) const
				{
				auto reader=matrixReaderCreate(MetaObject<T>().typeGet(),objname);
				dataRead(*reader,data,n_rows,n_cols);				
				}

			static 
			DataTypeHandle compoundMake(const FieldDescriptor* fields
				,size_t n_fields,size_t struct_size);

			GroupHandle groupCreate(const char* name);

			GroupHandle groupOpen(const char* name) const;

			template<class T>
			static void iterate(const H5::Group& group,T&& callback)
				{iterate_impl(group,GroupIterateCallbackImpl<T>(callback));}
			
			static size_t objectsCount(const H5::Group& group);

		private:
			static void deleter(H5::DataType* obj);
			static void deleter(H5::Group* obj);
			static void deleter(ArrayReaderImpl* obj);
			static void deleter(MatrixReaderImpl* obj);

			void dataWrite(const H5::DataType& type,const char* objname
				,size_t n_elems,const void* data);

			void dataWrite(const H5::DataType& type,const char* objname
				,size_t n_rows,size_t n_cols,const void* data);

			std::unique_ptr<H5::H5File> m_file;
			static H5::StrType s_cstr;

			template<class T>
			class GroupIterateCallbackImpl:public GroupIterateCallback
				{
				public:
					explicit GroupIterateCallbackImpl(T& obj):
						r_object(obj)
						{}
					void groupVisit(const char* group_name)
						{
						r_object(group_name);
						}

				private:
					T& r_object;
				};

			static void iterate_impl(const H5::Group& group,GroupIterateCallback&& cb);
		};

	template<>
	void DataDump::write<std::string>(const char* objname,const std::string* data,size_t n_elems);
	}

#endif
