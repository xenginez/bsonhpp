/*!
 * \file	bson.hpp
 *
 * \author	ZhengYuanQing
 * \date	2022/01/24
 * \email	zhengyuanqing.95@gmail.com
 *
 */
#ifndef BSON_HPP__B008D87D_A662_47B8_8066_0121BFDF3473
#define BSON_HPP__B008D87D_A662_47B8_8066_0121BFDF3473
#pragma once

#include <array>
#include <chrono>
#include <limits>
#include <string>
#include <vector>
#include <iomanip>
#include <variant>
#include <sstream>
#include <cassert>

namespace bson
{
	enum class binary_type : std::uint8_t
	{
		binary = 0x00,
		function = 0x01,
		binary_old = 0x02,
		uuid_old = 0x03,
		uuid = 0x04,
		md5 = 0x05,
		encrypt = 0x06,
		user = 0x80,
	};
	enum class element_type : std::uint8_t
	{
		null_node = 0x0A,
		int32_node = 0x10,
		int64_node = 0x12,
		array_node = 0x04,
		double_node = 0x01,
		string_node = 0x02,
		binary_node = 0x05,
		boolean_node = 0x08,
		min_key_node = 0xFF,
		max_key_node = 0x7F,
		regular_node = 0x0B,
		datetime_node = 0x09,
		document_node = 0x03,
		timestamp_node = 0x11,
		object_id_node = 0x07,

		unknown_node = 0xEF,
	};

	template< element_type T > class element;

	template< typename ... T > void create_node( element_type val, std::variant< T... > & node );
	template< typename ... T > element_type get_node_type( const std::variant< T... > & node );
	template< typename ... T > std::size_t get_node_size( const std::variant< T... > & node );
	template< typename ... T > void node_deserialize( std::istream & is, std::variant< T... > & node );
	template< typename ... T > void node_serialize( std::ostream & os, const std::variant< T... > & node );
	template< typename ... T > void node_to_json( std::ostream & os, const std::variant< T... > & node );
	template< typename ... T > void node_from_json( std::istream & is, std::variant< T... > & node );

	template< typename ... T > struct overloaded : T... { using T::operator()...; };
	template< typename ... T > overloaded( T... )->overloaded< T... >;

	inline int sget( std::istream & is )
	{
		while( true )
		{
			assert( !is.eof() && "inline int sget( std::istream & is )" );
			int c = is.peek();

			if( c == ' ' || c == '\r' || c == '\n' || c == '\t' )
			{
				is.get();
			}
			else
			{
				return is.get();
			}
		}
	}
	inline int speek( std::istream & is )
	{
		while( true )
		{
			assert( !is.eof() && "inline int speek( std::istream & is )" );

			int c = is.peek();

			if( c == ' ' || c == '\r' || c == '\n' || c == '\t' )
			{
				is.get();
			}
			else
			{
				return c;
			}
		}
	}
	inline std::string sread( std::istream & is, std::size_t size )
	{
		std::string result;

		for( size_t i = 0; i < size; i++ )
		{
			result.push_back( sget( is ) );
		}

		return result;
	}
	inline bool smatch( std::istream & is, std::string_view delim )
	{
		auto pos = is.tellg();
		if( sread( is, delim.size() ) == delim )
		{
			return true;
		}
		is.seekg( pos );
		return false;
	}


	template<> class element< element_type::null_node >
	{
	public:
		element() = default;

		element( std::nullptr_t )
		{

		}

		element & operator=( std::nullptr_t )
		{
			return *this;
		}

		~element() = default;

	public:
		operator std::nullptr_t() const
		{
			return nullptr;
		}

	public:
		element_type get_type() const
		{
			return element_type::null_node;
		}

		std::size_t get_size() const
		{
			return 0;
		}

		void serialize( std::ostream & ) const
		{

		}

		void deserialize( std::istream & )
		{

		}

	public:
		void to_json( std::ostream & os ) const
		{
			os << "null";
		}

		void from_json( std::istream & is )
		{
			assert( smatch( is, "null" ) && "void from_json( std::istream & is )" );
		}
	};

	template<> class element< element_type::int32_node >
	{
	public:
		element() = default;

		element( std::int32_t val )
			:value( val )
		{

		}

		element & operator =( std::int32_t val )
		{
			value = val;

			return *this;
		}

		~element() = default;

	public:
		operator std::int32_t() const
		{
			return value;
		}

		std::int32_t get_value() const
		{
			return value;
		}

	public:
		element_type get_type() const
		{
			return element_type::int32_node;
		}

		std::size_t get_size() const
		{
			return sizeof( std::int32_t );
		}

		void serialize( std::ostream & os ) const
		{
			os.write( reinterpret_cast<const char *>( &value ), sizeof( value ) );
		}

		void deserialize( std::istream & is )
		{
			is.read( reinterpret_cast<char *>( &value ), sizeof( value ) );
		}

	public:
		void to_json( std::ostream & os ) const
		{
			os << std::to_string( value );
		}

		void from_json( std::istream & is )
		{
			std::string str;
			if( speek( is ) == '-' )
			{
				str.push_back( sget( is ) );
			}
			while( speek( is ) >= '0' && speek( is ) <= '9' )
			{
				str.push_back( sget( is ) );
			}
			value = std::stoi( str );
		}

	private:
		std::int32_t value;
	};

	template<> class element< element_type::int64_node >
	{
	public:
		element() = default;

		element( std::int64_t val )
			:value( val )
		{

		}

		element & operator =( std::int64_t val )
		{
			value = val;

			return *this;
		}

		~element() = default;

	public:
		operator std::int64_t() const
		{
			return value;
		}

		std::int64_t get_value() const
		{
			return value;
		}

	public:
		element_type get_type() const
		{
			return element_type::int64_node;
		}

		std::size_t get_size() const
		{
			return sizeof( std::int64_t );
		}

		void serialize( std::ostream & os ) const
		{
			os.write( reinterpret_cast<const char *>( &value ), sizeof( value ) );
		}

		void deserialize( std::istream & is )
		{
			is.read( reinterpret_cast<char *>( &value ), sizeof( value ) );
		}

	public:
		void to_json( std::ostream & os ) const
		{
			os << std::to_string( value );
		}

		void from_json( std::istream & is )
		{
			std::string str;
			if( speek( is ) == '-' )
			{
				str.push_back( sget( is ) );
			}
			while( speek( is ) >= '0' && speek( is ) <= '9' )
			{
				str.push_back( sget( is ) );
			}
			value = std::stoll( str );
		}

	private:
		std::int64_t value;
	};

	template<> class element< element_type::double_node >
	{
	public:
		element() = default;

		element( double val )
			:value( val )
		{

		}

		element( const element< element_type::double_node > & val )
			:value( val.value )
		{

		}

		element & operator =( double val )
		{
			value = val;

			return *this;
		}

		element & operator =( const element< element_type::double_node > & val )
		{
			value = val.value;

			return *this;
		}

		~element() = default;

	public:
		operator double() const
		{
			return value;
		}

		double get_value() const
		{
			return value;
		}

	public:
		element_type get_type() const
		{
			return element_type::double_node;
		}

		std::size_t get_size() const
		{
			return sizeof( double );
		}

		void serialize( std::ostream & os ) const
		{
			os.write( reinterpret_cast<const char *>( &value ), sizeof( value ) );
		}

		void deserialize( std::istream & is )
		{
			is.read( reinterpret_cast<char *>( &value ), sizeof( value ) );
		}

	public:
		void to_json( std::ostream & os ) const
		{
			if( std::isnan( value ) )
			{
				os << "\"NaN\"";
			}
			else if( std::isinf( value ) )
			{
				os << "\"" << ( value < 0 ? "-" : "" ) << "Infinity" << "\"";
			}
			else
			{
				os << std::to_string( value );
			}
		}

		void from_json( std::istream & is )
		{
			if( sget( is ) == '\"' )
			{
				bool neg = false;

				if( speek( is ) == '-' )
				{
					neg = true;
				}

				if( speek( is ) == 'N' )
				{
					smatch( is, "NaN" );

					value = std::numeric_limits< double >::quiet_NaN();
				}
				else if( speek( is ) == 'I' )
				{
					smatch( is, "Infinity" );

					value = neg ? -std::numeric_limits< double >::infinity() : std::numeric_limits< double >::infinity();
				}

				assert( sget( is ) == '\"' && "void from_json( std::istream & is )" );
			}
			else
			{
				std::string str;
				while( ( speek( is ) >= '0' && speek( is ) <= '9' ) || speek( is ) == '.' )
				{
					str.push_back( sget( is ) );
				}
				value = std::stod( str );
			}
		}

	private:
		double value;
	};

	template<> class element< element_type::string_node >
	{
	public:
		element() = default;

		element( element< element_type::string_node > && val )
		{
			swap( std::forward<element< element_type::string_node >>( val ) );
		}

		element( const element< element_type::string_node > & val )
			:value( val.value )
		{

		}

		element( std::string && val )
		{
			std::swap( value, val );
		}

		element( const std::string & val )
			:value( val )
		{

		}

		element & operator =( element< element_type::string_node > && val )
		{
			swap( std::forward<element< element_type::string_node >>( val ) );

			return *this;
		}

		element & operator =( const element< element_type::string_node > & val )
		{
			value = val.value;

			return *this;
		}

		element & operator =( std::string && val )
		{
			std::swap( value, val );

			return *this;
		}

		element & operator =( const std::string & val )
		{
			value = val;

			return *this;
		}

		~element() = default;

	public:
		void swap( element< element_type::string_node > & val )
		{
			std::swap( value, val.value );
		}

	public:
		operator const std::string & ( ) const
		{
			return value;
		}

		const std::string & get_value() const
		{
			return value;
		}

	public:
		element_type get_type() const
		{
			return element_type::string_node;
		}

		std::size_t get_size() const
		{
			return sizeof( std::int32_t ) + value.size() + 1;
		}

		void serialize( std::ostream & os ) const
		{
			std::int32_t sz = static_cast<std::int32_t>( value.size() + 1 );

			os.write( reinterpret_cast<const char *>( &sz ), sizeof( sz ) );

			os.write( value.c_str(), value.size() + 1 );
		}

		void deserialize( std::istream & is )
		{
			std::int32_t sz;

			is.read( reinterpret_cast<char *>( &sz ), sizeof( sz ) );

			value.resize( sz - 1 );

			is.read( value.data(), sz );
		}

	public:
		void to_json( std::ostream & os ) const
		{
			os << ( "\"" + value + "\"" );
		}

		void from_json( std::istream & is )
		{
			if( sget( is ) == '\"' )
			{
				while( is.peek() != '\"' )
				{
					value.push_back( is.get() );
				}

				assert( sget( is ) == '\"' && "void from_json( std::istream & is )" );
			}
		}

	private:
		std::string value;
	};

	template<> class element< element_type::binary_node >
	{
	public:
		element() = default;

		template< typename T > element( const T & container, binary_type type = binary_type::binary )
			:btype( type )
		{
			value.insert( value.end(), container.begin(), container.end() );
		}

		element( element< element_type::binary_node > && val )
		{
			swap( std::forward<element< element_type::binary_node >>( val ) );
		}

		element( const element< element_type::binary_node > & val )
			:btype( val.btype ), value( val.value )
		{

		}

		element & operator =( element< element_type::binary_node > && val )
		{
			swap( std::forward<element< element_type::binary_node >>( val ) );

			return *this;
		}

		element & operator =( const element< element_type::binary_node > & val )
		{
			btype = val.btype;
			value = val.value;

			return *this;
		}

		~element() = default;

	public:
		void swap( element< element_type::binary_node > & val )
		{
			std::swap( btype, val.btype );
			std::swap( value, val.value );
		}

	public:
		const std::vector<char> & get_value() const
		{
			return value;
		}

	public:
		element_type get_type() const
		{
			return element_type::binary_node;
		}

		std::size_t get_size() const
		{
			return value.size() + 5;
		}

		void serialize( std::ostream & os ) const
		{
			std::int32_t sz = static_cast<std::int32_t>( value.size() );

			os.write( reinterpret_cast<const char *>( &sz ), sizeof( sz ) );

			os.write( reinterpret_cast<const char *>( &btype ), sizeof( btype ) );

			os.write( reinterpret_cast<const char *>( value.data() ), value.size() );
		}

		void deserialize( std::istream & is )
		{
			std::int32_t sz;

			is.read( reinterpret_cast<char *>( &sz ), sizeof( sz ) );

			is.read( reinterpret_cast<char *>( &btype ), sizeof( btype ) );

			value.resize( static_cast<std::size_t>( sz ) );
			is.read( reinterpret_cast<char *>( value.data() ), static_cast< std::size_t >( sz ) );
		}

	public:
		void to_json( std::ostream & os ) const
		{
			static constexpr std::string_view encode_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

			std::string encode;

			std::size_t bytes = value.size();
			const char * current = value.data();

			while( bytes > 2 )
			{
				encode += encode_table[current[0] >> 2];
				encode += encode_table[( ( current[0] & 0x03 ) << 4 ) + ( current[1] >> 4 )];
				encode += encode_table[( ( current[1] & 0x0f ) << 2 ) + ( current[2] >> 6 )];
				encode += encode_table[current[2] & 0x3f];

				current += 3;
				bytes -= 3;
			}
			if( bytes > 0 )
			{
				encode += encode_table[current[0] >> 2];
				if( bytes % 3 == 1 )
				{
					encode += encode_table[( current[0] & 0x03 ) << 4];
					encode += "==";
				}
				else if( bytes % 3 == 2 )
				{
					encode += encode_table[( ( current[0] & 0x03 ) << 4 ) + ( current[1] >> 4 )];
					encode += encode_table[( current[1] & 0x0f ) << 2];
					encode += "=";
				}
			}

			os << R"({ "base64" : ")" << encode << R"(", "subType" : ")" << std::to_string( (std::uint8_t)btype ) << "\" }";
		}

		void from_json( std::istream & is )
		{
			constexpr char decode_table[] =
			{
				-2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -2, -2, -1, -2, -2,
				-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
				-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 62, -2, -2, -2, 63,
				52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -2, -2, -2, -2, -2, -2,
				-2,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
				15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -2, -2, -2, -2, -2,
				-2, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -2, -2, -2, -2, -2,
				-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
				-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
				-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
				-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
				-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
				-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
				-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
				-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2
			};

			std::string encode;
			if( smatch( is, R"({"base64":")" ) )
			{
				while( is.peek() != '\"' )
				{
					encode.push_back( is.get() );
				}

				if( smatch( is, R"(","subType":")" ) )
				{
					std::string subt;
					while( speek( is ) != '\"' )
					{
						subt.push_back( is.get() );
					}

					assert( smatch( is, "\"}" ) && "void from_json( std::istream & is )" );

					{
						int bin = 0, i = 0;
						auto length = encode.size();
						const char * current = encode.c_str();
						char ch;
						while( ( ch = *current++ ) != '\0' && length-- > 0 )
						{
							if( ch == '=' )
							{
								if( *current != '=' && ( i % 4 ) == 1 )
								{
									return;
								}
								continue;
							}
							ch = decode_table[ch];
							if( ch < 0 )
							{
								continue;
							}
							switch( i % 4 )
							{
							case 0:
								bin = ch << 2;
								break;
							case 1:
								bin |= ch >> 4;
								value.push_back( bin );
								bin = ( ch & 0x0f ) << 4;
								break;
							case 2:
								bin |= ch >> 2;
								value.push_back( bin );
								bin = ( ch & 0x03 ) << 6;
								break;
							case 3:
								bin |= ch;
								value.push_back( bin );
								break;
							}
							i++;
						}
					}

					btype = static_cast<binary_type>( std::stoi( subt, nullptr, 16 ) );
				}
			}
		}

	private:
		binary_type btype = binary_type::binary;
		std::vector< char > value;
	};

	template<> class element< element_type::boolean_node >
	{
	public:
		element() = default;

		element( bool val )
			:value( val )
		{

		}

		element( const element< element_type::boolean_node > & val )
			:value( val.value )
		{

		}

		element & operator =( bool val )
		{
			value = val;

			return *this;
		}

		~element() = default;

	public:
		operator bool() const
		{
			return value;
		}

		bool get_value() const
		{
			return value;
		}

	public:
		element_type get_type() const
		{
			return element_type::boolean_node;
		}

		std::size_t get_size() const
		{
			return 1;
		}

		void serialize( std::ostream & os ) const
		{
			char c = value ? 1 : 0;

			os.write( &c, 1 );
		}

		void deserialize( std::istream & is )
		{
			char c;

			is.read( &c, 1 );

			value = c == 1;
		}

	public:
		void to_json( std::ostream & os ) const
		{
			os << ( value ? "true" : "false" );
		}

		void from_json( std::istream & is )
		{
			value = speek( is ) == 't' ? smatch( is, "true" ) : !smatch( is, "false" );
		}

	private:
		bool value;
	};

	template<> class element< element_type::min_key_node >
	{
	public:
		element() = default;

		element( std::nullptr_t )
		{

		}

		element & operator=( std::nullptr_t )
		{
			return *this;
		}

		~element() = default;

	public:
		element_type get_type() const
		{
			return element_type::min_key_node;
		}

		std::size_t get_size() const
		{
			return 0;
		}

		void serialize( std::ostream & ) const
		{

		}

		void deserialize( std::istream & )
		{

		}

	public:
		void to_json( std::ostream & os ) const
		{
			os << 1;
		}

		void from_json( std::istream & is )
		{
			assert( smatch( is, "1" ) && "void from_json( std::istream & is )" );
		}
	};

	template<> class element< element_type::max_key_node >
	{
	public:
		element() = default;

		element( std::nullptr_t )
		{

		}

		element & operator=( std::nullptr_t )
		{
			return *this;
		}

		~element() = default;

	public:
		element_type get_type() const
		{
			return element_type::max_key_node;
		}

		std::size_t get_size() const
		{
			return 0;
		}

		void serialize( std::ostream & ) const
		{

		}

		void deserialize( std::istream & )
		{

		}

	public:
		void to_json( std::ostream & os ) const
		{
			os << 1;
		}

		void from_json( std::istream & is )
		{
			assert( smatch( is, "1" ) && "void from_json( std::istream & is )" );
		}
	};

	template<> class element< element_type::regular_node >
	{
	public:
		element() = default;

		element( const std::string & pattern, const std::string & options )
			:pattern( pattern ), options( options )
		{

		}

		element( element< element_type::regular_node > && val )
		{
			swap( std::forward<element< element_type::regular_node >>( val ) );
		}

		element( const element< element_type::regular_node > & val )
			:pattern( val.pattern ), options( val.options )
		{

		}

		element & operator =( element< element_type::regular_node > && val )
		{
			swap( std::forward<element< element_type::regular_node >>( val ) );
			return *this;
		}

		element & operator =( const element< element_type::regular_node > & val )
		{
			pattern = val.pattern;
			options = val.options;
			return *this;
		}

		~element() = default;

	public:
		void swap( element<element_type::regular_node> & val )
		{
			std::swap( pattern, val.pattern );
			std::swap( options, val.options );
		}

		const std::string get_pattern() const
		{
			return pattern;
		}

		const std::string get_options() const
		{
			return options;
		}

	public:
		element_type get_type() const
		{
			return element_type::regular_node;
		}

		std::size_t get_size() const
		{
			return pattern.size() + options.size() + 2;
		}

		void serialize( std::ostream & os ) const
		{
			os.write( pattern.c_str(), pattern.size() + 1 );
			os.write( options.c_str(), options.size() + 1 );
		}

		void deserialize( std::istream & is )
		{
			while( !is.eof() )
			{
				char c = sget( is );

				if( c == 0 )
				{
					break;
				}

				pattern.push_back( c );
			}

			while( !is.eof() )
			{
				char c = sget( is );

				if( c == 0 )
				{
					break;
				}

				options.push_back( c );
			}
		}

	public:
		void to_json( std::ostream & os ) const
		{
			os << R"({ "pattern" : ")" + pattern + R"(", "options" : ")" + options + "\" }";
		}

		void from_json( std::istream & is )
		{
			if( smatch( is, R"({"pattern":")" ) )
			{
				while( is.peek() != '\"' )
				{
					pattern.push_back( is.get() );
				}
			}

			if( smatch( is, R"(","options":")" ) )
			{
				while( is.peek() != '\"' )
				{
					options.push_back( is.get() );
				}
			}

			assert( smatch( is, "\"}" ) && "void from_json( std::istream & is )" );
		}

	private:
		std::string pattern, options;
	};

	template<> class element< element_type::datetime_node >
	{
	public:
		element() = default;

		element( std::time_t val )
			:value( val )
		{

		}

		element( const element< element_type::datetime_node > & val )
			:value( val.value )
		{

		}

		element & operator =( bool val )
		{
			value = val;

			return *this;
		}

		~element() = default;

	public:
		operator std::time_t() const
		{
			return value;
		}

		std::time_t get_value() const
		{
			return value;
		}

	public:
		element_type get_type() const
		{
			return element_type::datetime_node;
		}

		std::size_t get_size() const
		{
			return sizeof( value );
		}

		void serialize( std::ostream & os ) const
		{
			os.write( reinterpret_cast<const char *>( &value ), sizeof( value ) );
		}

		void deserialize( std::istream & is )
		{
			is.read( reinterpret_cast<char *>( &value ), sizeof( value ) );
		}

	public:
		void to_json( std::ostream & os ) const
		{
			struct tm _tm;
			auto _tt = value / 1000;

#ifdef WIN32
			gmtime_s( &_tm, &_tt );
#else
			gmtime_r( &_tt, &_tm );
#endif // WIN32

			os << "\"" << std::put_time( &_tm, "%Y-%m-%dT%H:%M:%S" ) << "." << std::setfill( '0' ) << std::setw( 3 ) << ( value % 1000 ) << "Z\"";
		}

		void from_json( std::istream & is )
		{
			assert( smatch( is, "\"" ) && "void from_json( std::istream & is )" );
			{
				std::string date_t = sread( is, 24 );

				struct tm _tm; std::int32_t millsec;

				sscanf_s( date_t.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d.%3dZ", &_tm.tm_year, &_tm.tm_mon, &_tm.tm_mday, &_tm.tm_hour, &_tm.tm_min, &_tm.tm_sec, &millsec );

				_tm.tm_year -= 1900;
				_tm.tm_mon -= 1;

				value = mktime( &_tm ) * 1000 + millsec;
			}
			assert( smatch( is, "\"" ) && "void from_json( std::istream & is )" );
		}

	private:
		std::time_t value = 0;
	};

	template<> class element< element_type::timestamp_node >
	{
	public:
		element() = default;

		element( std::uint64_t val )
			:value( val )
		{

		}

		template< typename ... T > element( const std::chrono::time_point< T... > & val )
		{
			value = std::chrono::duration_cast<std::chrono::milliseconds>( val ).count();
		}

		element( const element< element_type::timestamp_node > & val )
			:value( val.value )
		{

		}

		element & operator =( bool val )
		{
			value = val;

			return *this;
		}

		~element() = default;

	public:
		operator std::uint64_t() const
		{
			return value;
		}

		std::uint64_t get_value() const
		{
			return value;
		}

	public:
		element_type get_type() const
		{
			return element_type::timestamp_node;
		}

		std::size_t get_size() const
		{
			return sizeof( value );
		}

		void serialize( std::ostream & os ) const
		{
			os.write( reinterpret_cast<const char *>( &value ), sizeof( value ) );
		}

		void deserialize( std::istream & is )
		{
			is.read( reinterpret_cast<char *>( &value ), sizeof( value ) );
		}

	public:
		void to_json( std::ostream & os ) const
		{
			os << R"({ "t" : )" << std::to_string( value ) << R"(, "i" : 1 })";
		}

		void from_json( std::istream & is )
		{
			assert( smatch( is, R"({"t":)" ) && "void from_json( std::istream & is )" );
			{
				std::string str;
				while( speek( is ) >= '0' && speek( is ) <= '9' )
				{
					str.push_back( sget( is ) );
				}
				value = std::stoull( str );
			}
			assert( smatch( is, R"(,"i":1})" ) && "void from_json( std::istream & is )" );
		}

	private:
		std::uint64_t value = 0;
	};

	template<> class element< element_type::object_id_node >
	{
	public:
		element() = default;

		element( element< element_type::object_id_node > && val )
		{
			swap( std::forward<element< element_type::object_id_node >>( val ) );
		}

		element( const element< element_type::object_id_node > & val )
			:value( val.value )
		{

		}

		element( std::string_view val )
		{
			for( size_t i = 0; i < value.size(); i++ )
			{
				value[i] = std::stoi( std::string( val.begin() + ( i * 2 ), val.begin() + ( i * 2 ) + 2 ), nullptr, 16 );
			}
		}

		element( const std::array<char, 12> & val )
			:value( val )
		{

		}

		element & operator =( element< element_type::object_id_node > && val )
		{
			swap( std::forward<element< element_type::object_id_node >>( val ) );

			return *this;
		}

		element & operator =( const element< element_type::object_id_node > & val )
		{
			value = val.value;

			return *this;
		}

		element & operator =( const std::array<char, 12> & val )
		{
			value = val;

			return *this;
		}

		~element() = default;

	public:
		void swap( element< element_type::object_id_node > & val )
		{
			std::swap( value, val.value );
		}

	public:
		operator const std::array<char, 12> & ( ) const
		{
			return value;
		}

		const std::array<char, 12> & get_value() const
		{
			return value;
		}

	public:
		element_type get_type() const
		{
			return element_type::object_id_node;
		}

		std::size_t get_size() const
		{
			return 12;
		}

		void serialize( std::ostream & os ) const
		{
			os.write( value.data(), 12 );
		}

		void deserialize( std::istream & is )
		{
			is.read( value.data(), 12 );
		}

	public:
		void to_json( std::ostream & os ) const
		{
			os << "\"";
			for( auto c : value )
			{
				os << std::setfill( '0' ) << std::setw( 2 ) << std::hex << static_cast<std::uint32_t>( static_cast<std::uint8_t>( c ) );
			}
			os << "\"";
		}

		void from_json( std::istream & is )
		{
			assert( smatch( is, "\"" ) && "void from_json( std::istream & is )" );
			{
				std::string hex;
				for( size_t i = 0; i < value.size(); i++ )
				{
					hex.push_back( is.get() ); hex.push_back( is.get() );
					value[i] = std::stoi( hex, nullptr, 16 );
					hex.clear();
				}
			}
			assert( smatch( is, "\"" ) && "void from_json( std::istream & is )" );
		}

	private:
		std::array<char, 12> value;
	};

	template< element_type T > class element
	{
	public:
		using node_t = std::variant< std::monostate,
			element< element_type::null_node >,
			element< element_type::int32_node >,
			element< element_type::int64_node >,
			element< element_type::array_node >,
			element< element_type::double_node >,
			element< element_type::string_node >,
			element< element_type::binary_node >,
			element< element_type::boolean_node >,
			element< element_type::min_key_node >,
			element< element_type::max_key_node >,
			element< element_type::regular_node >,
			element< element_type::datetime_node >,
			element< element_type::document_node >,
			element< element_type::timestamp_node >,
			element< element_type::object_id_node >
		>;

	public:
		using value_type = node_t;
		using mapped_type = std::pair< std::string, node_t >;
		using iterator = std::vector< mapped_type >::iterator;
		using const_iterator = std::vector< mapped_type >::const_iterator;

	public:
		element() = default;

		template< typename ... T > element( T &&... args )
		{
			unpack( args... );
		}

		element( element<T> && val )
		{
			swap( std::forward<element>( val ) );
		}

		element( const element<T> & val )
			:nodes( val.nodes )
		{

		}

		element & operator =( element<T> && val )
		{
			swap( std::forward<element<T>>( val ) );

			return *this;
		}

		element & operator =( const element<T> & val )
		{
			nodes = val.nodes;

			return *this;
		}

		~element() = default;

	public:
		void swap( element<T> & val )
		{
			std::swap( nodes, val.nodes );
		}

	public:
		value_type & operator[]( std::size_t i )
		{
			assert( get_type() == element_type::array_node && "element_list< type::array_node >" );

			return nodes[i].second;
		}

		const value_type & operator[]( std::size_t i ) const
		{
			assert( get_type() == element_type::array_node && "element_list< type::array_node >" );

			return nodes[i].second;
		}

		value_type & operator[]( const std::string & key )
		{
			assert( get_type() == element_type::array_node && "element_list< type::document_node >" );

			auto it = std::find_if( begin(), end(), [&]( const auto & it ) { return it.first == key; } );
			if( it != end() )
			{
				return it->second;
			}

			nodes.push_back( { key, value_type{} } );

			return nodes.back().second;
		}

		const value_type & operator[]( const std::string & key ) const
		{
			assert( get_type() == element_type::array_node && "element_list< type::document_node >" );

			auto it = std::find_if( begin(), end(), [&]( const auto & it ) { return it.first == key; } );
			if( it != end() )
			{
				return it->second;
			}

			throw std::out_of_range( "const value_type & operator[]( const std::string & key ) const" );
		}

	public:
		bool empty() const
		{
			return nodes.empty();
		}

	public:
		iterator begin()
		{
			return nodes.begin();
		}

		iterator end()
		{
			return nodes.end();
		}

		const_iterator begin() const
		{
			return nodes.begin();
		}

		const_iterator end() const
		{
			return nodes.end();
		}

	public:
		iterator find( const std::string & key )
		{
			return std::find_if( begin(), end(), [&]( const auto & it ) { return it.first == key; } );
		}

		const_iterator find( const std::string & key ) const
		{
			return std::find_if( begin(), end(), [&]( const auto & it ) { return it.first == key; } );
		}

	public:
		void erase( const_iterator val )
		{
			nodes.erase( val );

			if( T == element_type::array_node )
			{
				for( size_t i = 0; i < nodes.size(); i++ )
				{
					nodes[i].first = std::to_string( i );
				}
			}
		}

	public:
		void push_back( std::nullptr_t )
		{
			assert( get_type() == element_type::array_node && "element_list< type::array_node >" );

			nodes.push_back( { std::to_string( nodes.size() ), element< element_type::null_node >() } );
		}
		void push_back( bool val )
		{
			assert( get_type() == element_type::array_node && "element_list< type::array_node >" );

			nodes.push_back( { std::to_string( nodes.size() ), element< element_type::boolean_node >( val ) } );
		}
		void push_back( std::int32_t val )
		{
			assert( get_type() == element_type::array_node && "element_list< type::array_node >" );

			nodes.push_back( { std::to_string( nodes.size() ), element< element_type::int32_node >( val ) } );
		}
		void push_back( std::int64_t val )
		{
			assert( get_type() == element_type::array_node && "element_list< type::array_node >" );

			nodes.push_back( { std::to_string( nodes.size() ), element< element_type::int64_node >( val ) } );
		}
		void push_back( std::uint64_t val )
		{
			assert( get_type() == element_type::array_node && "element_list< type::array_node >" );

			nodes.push_back( { std::to_string( nodes.size() ), element< element_type::timestamp_node >( val ) } );
		}
		void push_back( float val )
		{
			assert( get_type() == element_type::array_node && "element_list< type::array_node >" );

			nodes.push_back( { std::to_string( nodes.size() ), element< element_type::double_node >( val ) } );
		}
		void push_back( double val )
		{
			assert( get_type() == element_type::array_node && "element_list< type::array_node >" );

			nodes.push_back( { std::to_string( nodes.size() ), element< element_type::double_node >( val ) } );
		}
		void push_back( const char * val )
		{
			assert( get_type() == element_type::array_node && "element_list< type::array_node >" );

			nodes.push_back( { std::to_string( nodes.size() ), element< element_type::string_node >( val ) } );
		}
		void push_back( std::string_view val )
		{
			assert( get_type() == element_type::array_node && "element_list< type::array_node >" );

			nodes.push_back( { std::to_string( nodes.size() ), element< element_type::string_node >( std::string{ val.data(), val.size() } ) } );
		}
		void push_back( const std::string & val )
		{
			assert( get_type() == element_type::array_node && "element_list< type::array_node >" );

			nodes.push_back( { std::to_string( nodes.size() ), element< element_type::string_node >( val ) } );
		}
		template< element_type T > void push_back( const element< T > & val )
		{
			assert( get_type() == element_type::array_node && "element_list< type::array_node >" );

			nodes.push_back( { std::to_string( nodes.size() ), val } );
		}
		template< typename K, typename V > void push_back( const std::pair< K, V > & val )
		{
			assert( get_type() == element_type::document_node && "element_list< type::document_node >" );

			insert( val.first, val.second );
		}
		template< typename ... T > void push_back( const std::chrono::time_point< T... > & val )
		{
			assert( get_type() == element_type::array_node && "element_list< type::array_node >" );

			nodes.push_back( { std::to_string( nodes.size() ), element< element_type::datetime_node >( std::chrono::time_point< T... >::clock::to_time_t( val ) * 1000 ) } );
		}

	public:
		void unpack() {}
		template< typename T, typename ... Args > void unpack( T && val, Args &&... args )
		{
			push_back( std::forward<T>( val ) );

			unpack( args... );
		}

	public:
		void insert( const std::string & key, std::nullptr_t )
		{
			assert( get_type() == element_type::document_node && "element_list< type::document_node >" );

			auto it = find( key );
			if( it != end() )
			{
				it->second = element< element_type::null_node >();
			}
			else
			{
				nodes.push_back( { key, element< element_type::null_node >() } );
			}
		}
		void insert( const std::string & key, bool val )
		{
			assert( get_type() == element_type::document_node && "element_list< type::document_node >" );

			auto it = find( key );
			if( it != end() )
			{
				it->second = element< element_type::boolean_node >( val );
			}
			else
			{
				nodes.push_back( { key, element< element_type::boolean_node >( val ) } );
			}
		}
		void insert( const std::string & key, std::int32_t val )
		{
			assert( get_type() == element_type::document_node && "element_list< type::document_node >" );

			auto it = find( key );
			if( it != end() )
			{
				it->second = element< element_type::int32_node >( val );
			}
			else
			{
				nodes.push_back( { key, element< element_type::int32_node >( val ) } );
			}
		}
		void insert( const std::string & key, std::int64_t val )
		{
			assert( get_type() == element_type::document_node && "element_list< type::document_node >" );

			auto it = find( key );
			if( it != end() )
			{
				it->second = element< element_type::int64_node >( val );
			}
			else
			{
				nodes.push_back( { key, element< element_type::int64_node >( val ) } );
			}
		}
		void insert( const std::string & key, std::uint64_t val )
		{
			assert( get_type() == element_type::document_node && "element_list< type::document_node >" );

			auto it = find( key );
			if( it != end() )
			{
				it->second = element< element_type::timestamp_node >( val );
			}
			else
			{
				nodes.push_back( { key, element< element_type::timestamp_node >( val ) } );
			}
		}
		void insert( const std::string & key, float val )
		{
			assert( get_type() == element_type::document_node && "element_list< type::document_node >" );

			auto it = find( key );
			if( it != end() )
			{
				it->second = element< element_type::double_node >( val );
			}
			else
			{
				nodes.push_back( { key, element< element_type::double_node >( val ) } );
			}
		}
		void insert( const std::string & key, double val )
		{
			assert( get_type() == element_type::document_node && "element_list< type::document_node >" );

			auto it = find( key );
			if( it != end() )
			{
				it->second = element< element_type::double_node >( val );
			}
			else
			{
				nodes.push_back( { key, element< element_type::double_node >( val ) } );
			}
		}
		void insert( const std::string & key, const char * val )
		{
			assert( get_type() == element_type::document_node && "element_list< type::document_node >" );

			auto it = find( key );
			if( it != end() )
			{
				it->second = element< element_type::string_node >( val );
			}
			else
			{
				nodes.push_back( { key, element< element_type::string_node >( val ) } );
			}
		}
		void insert( const std::string & key, std::string_view val )
		{
			assert( get_type() == element_type::document_node && "element_list< type::document_node >" );

			auto it = find( key );
			if( it != end() )
			{
				it->second = element< element_type::string_node >( val );
			}
			else
			{
				nodes.push_back( { key, element< element_type::string_node >( val ) } );
			}
		}
		void insert( const std::string & key, const std::string & val )
		{
			assert( get_type() == element_type::document_node && "element_list< type::document_node >" );

			auto it = find( key );
			if( it != end() )
			{
				it->second = element< element_type::string_node >( val );
			}
			else
			{
				nodes.push_back( { key, element< element_type::string_node >( val ) } );
			}
		}
		template< element_type T > void insert( const std::string & key, const element< T > & val )
		{
			assert( get_type() == element_type::document_node && "element_list< type::document_node >" );

			auto it = find( key );
			if( it != end() )
			{
				it->second = val;
			}
			else
			{
				nodes.push_back( { key, val } );
			}
		}
		template< typename ... T > void insert( const std::string & key, const std::chrono::time_point< T... > & val )
		{
			assert( get_type() == element_type::document_node && "element_list< type::document_node >" );

			auto it = find( key );
			if( it != end() )
			{
				it->second = element< element_type::datetime_node >( std::chrono::time_point< T... >::clock::to_time_t( val ) * 1000 );
			}
			else
			{
				nodes.push_back( { key, element< element_type::datetime_node >( std::chrono::time_point< T... >::clock::to_time_t( val ) * 1000 ) } );
			}
		}

	public:
		element_type get_type() const
		{
			return T;
		}

		std::size_t get_size() const
		{
			std::size_t result = 4;
			for( const auto & it : nodes )
			{
				result += 1 + it.first.size() + 1 + get_node_size( it.second );
			}
			return result + 1;
		}

		void serialize( std::ostream & os ) const
		{
			std::int32_t sz = static_cast<std::int32_t>( get_size() );

			os.write( reinterpret_cast<const char *>( &sz ), sizeof( sz ) );

			for( auto it = begin(); it != end(); ++it )
			{
				char t = static_cast<char>( get_node_type( it->second ) );
				os.write( &t, sizeof( t ) );

				os.write( it->first.c_str(), it->first.size() + 1 );

				node_serialize( os, it->second );
			}

			os.write( "\0", 1 );
		}

		void deserialize( std::istream & is )
		{
			std::int32_t sz = 0;

			is.read( reinterpret_cast<char *>( &sz ), sizeof( sz ) );

			while( true )
			{
				element_type t = element_type::unknown_node;

				is.read( reinterpret_cast<char *>( &t ), sizeof( t ) );

				if( static_cast<std::uint8_t>( t ) == 0 )
				{
					return;
				}

				std::string key;

				while( !is.eof() )
				{
					char c = sget( is );

					if( c == 0 )
					{
						break;
					}

					key.push_back( c );
				}

				node_t value; create_node( t, value );

				node_deserialize( is, value );

				nodes.push_back( { std::move( key ), std::move( value ) } );
			}
		}

	public:
		void to_json( std::ostream & os ) const
		{
			if( get_type() == element_type::array_node )
			{
				os << "[ ";
				{
					for( size_t i = 0; i < nodes.size(); i++ )
					{
						node_to_json( os, nodes[i].second );
						if( i < nodes.size() - 1 )
						{
							os << ", ";
						}
					}
				}
				os << " ]";
			}
			else
			{
				os << "{ ";
				{
					for( size_t i = 0; i < nodes.size(); i++ )
					{
						os << ( "\"" + nodes[i].first + "\" : " );

						node_to_json( os, nodes[i].second );
						if( i < nodes.size() - 1 )
						{
							os << ", ";
						}
					}
				}
				os << " }";
			}
		}

		void from_json( std::istream & is )
		{
			if( get_type() == element_type::array_node )
			{
				assert( !is.eof() && sget( is ) == '[' && "void from_json( std::istream & is )" );
				{
					bool exit = false;
					while( !exit )
					{
						node_t node;

						node_from_json( is, node );

						nodes.push_back( { std::to_string( nodes.size() ), node } );

						switch( speek( is ) )
						{
						case ']':
							exit = true;
							break;
						case ',':
							sget( is );
							break;
						default:
							assert( false && "void from_json( std::istream & is )" );
							break;
						}
					}
				}
				assert( !is.eof() && sget( is ) == ']' && "void from_json( std::istream & is )" );
			}
			else
			{
				assert( !is.eof() && sget( is ) == '{' && "void from_json( std::istream & is )" );
				{
					bool exit = false;
					while( !exit )
					{
						element< element_type::string_node > key;

						key.from_json( is );

						assert( !is.eof() && sget( is ) == ':' && "void from_json( std::istream & is )" );

						node_t value;

						node_from_json( is, value );

						nodes.push_back( { key.get_value(), value } );

						switch( speek( is ) )
						{
						case '}':
							exit = true;
							break;
						case ',':
							sget( is );
							break;
						default:
							assert( false && "void from_json( std::istream & is )" );
							break;
						}
					}
				}
				assert( !is.eof() && sget( is ) == '}' && "void from_json( std::istream & is )" );
			}
		}

	private:
		std::vector< mapped_type > nodes;
	};

	template<> class element< element_type::unknown_node >;

	template< typename ... T > void create_node( element_type val, std::variant< T... > & node )
	{
		switch( val )
		{
		case bson::element_type::null_node:
			node = element< element_type::null_node >();
			break;
		case bson::element_type::int32_node:
			node = element< element_type::int32_node >();
			break;
		case bson::element_type::int64_node:
			node = element< element_type::int64_node >();
			break;
		case bson::element_type::array_node:
			node = element< element_type::array_node >();
			break;
		case bson::element_type::double_node:
			node = element< element_type::double_node >();
			break;
		case bson::element_type::string_node:
			node = element< element_type::string_node >();
			break;
		case bson::element_type::binary_node:
			node = element< element_type::binary_node >();
			break;
		case bson::element_type::boolean_node:
			node = element< element_type::boolean_node >();
			break;
		case bson::element_type::min_key_node:
			node = element< element_type::min_key_node >();
			break;
		case bson::element_type::max_key_node:
			node = element< element_type::max_key_node >();
			break;
		case bson::element_type::regular_node:
			node = element< element_type::regular_node >();
			break;
		case bson::element_type::datetime_node:
			node = element< element_type::datetime_node >();
			break;
		case bson::element_type::document_node:
			node = element< element_type::document_node >();
			break;
		case bson::element_type::timestamp_node:
			node = element< element_type::timestamp_node >();
			break;
		case bson::element_type::object_id_node:
			node = element< element_type::object_id_node >();
			break;
		default:
			throw std::runtime_error( "bson::type unknown" );
			break;
		}
	}
	template< typename ... T > element_type get_node_type( const std::variant< T... > & node )
	{
		return std::visit( overloaded
						   {
							   []( const std::monostate & val ) { return element_type::unknown_node; },
							   []( const element< element_type::null_node > & val ) { return val.get_type(); },
							   []( const element< element_type::int32_node > & val ) { return val.get_type(); },
							   []( const element< element_type::int64_node > & val ) { return val.get_type(); },
							   []( const element< element_type::array_node > & val ) { return val.get_type(); },
							   []( const element< element_type::double_node > & val ) { return val.get_type(); },
							   []( const element< element_type::string_node > & val ) { return val.get_type(); },
							   []( const element< element_type::binary_node > & val ) { return val.get_type(); },
							   []( const element< element_type::boolean_node > & val ) { return val.get_type(); },
							   []( const element< element_type::min_key_node > & val ) { return val.get_type(); },
							   []( const element< element_type::max_key_node > & val ) { return val.get_type(); },
							   []( const element< element_type::regular_node > & val ) { return val.get_type(); },
							   []( const element< element_type::datetime_node > & val ) { return val.get_type(); },
							   []( const element< element_type::document_node > & val ) { return val.get_type(); },
							   []( const element< element_type::timestamp_node > & val ) { return val.get_type(); },
							   []( const element< element_type::object_id_node > & val ) { return val.get_type(); },
						   }, node );
	}
	template< typename ... T > std::size_t get_node_size( const std::variant< T... > & node )
	{
		return std::visit( overloaded
						   {
							   []( const std::monostate & val ) ->std::size_t { return 0; },
							   []( const element< element_type::null_node > & val ) { return val.get_size(); },
							   []( const element< element_type::int32_node > & val ) { return val.get_size(); },
							   []( const element< element_type::int64_node > & val ) { return val.get_size(); },
							   []( const element< element_type::array_node > & val ) { return val.get_size(); },
							   []( const element< element_type::double_node > & val ) { return val.get_size(); },
							   []( const element< element_type::string_node > & val ) { return val.get_size(); },
							   []( const element< element_type::binary_node > & val ) { return val.get_size(); },
							   []( const element< element_type::boolean_node > & val ) { return val.get_size(); },
							   []( const element< element_type::min_key_node > & val ) { return val.get_size(); },
							   []( const element< element_type::max_key_node > & val ) { return val.get_size(); },
							   []( const element< element_type::regular_node > & val ) { return val.get_size(); },
							   []( const element< element_type::datetime_node > & val ) { return val.get_size(); },
							   []( const element< element_type::document_node > & val ) { return val.get_size(); },
							   []( const element< element_type::timestamp_node > & val ) { return val.get_size(); },
							   []( const element< element_type::object_id_node > & val ) { return val.get_size(); },
						   }, node );
	}
	template< typename ... T > void node_deserialize( std::istream & is, std::variant< T... > & node )
	{
		if( node.index() == 0 )
		{
			node = element<element_type::document_node>();
		}

		std::visit( overloaded
					{
						[&]( std::monostate & val ){},
						[&]( element< element_type::null_node > & val ) { val.deserialize( is ); },
						[&]( element< element_type::int32_node > & val ) { val.deserialize( is ); },
						[&]( element< element_type::int64_node > & val ) { val.deserialize( is ); },
						[&]( element< element_type::array_node > & val ) { val.deserialize( is ); },
						[&]( element< element_type::double_node > & val ) { val.deserialize( is ); },
						[&]( element< element_type::string_node > & val ) { val.deserialize( is ); },
						[&]( element< element_type::binary_node > & val ) { val.deserialize( is ); },
						[&]( element< element_type::boolean_node > & val ) { val.deserialize( is ); },
						[&]( element< element_type::min_key_node > & val ) { val.deserialize( is ); },
						[&]( element< element_type::max_key_node > & val ) { val.deserialize( is ); },
						[&]( element< element_type::regular_node > & val ) { val.deserialize( is ); },
						[&]( element< element_type::datetime_node > & val ) { val.deserialize( is ); },
						[&]( element< element_type::document_node > & val ) { val.deserialize( is ); },
						[&]( element< element_type::timestamp_node > & val ) { val.deserialize( is ); },
						[&]( element< element_type::object_id_node > & val ) { val.deserialize( is ); },
					}, node );
	}
	template< typename ... T > void node_serialize( std::ostream & os, const std::variant< T... > & node )
	{
		std::visit( overloaded
					{
						[&]( const std::monostate & val ) {},
						[&]( const element< element_type::null_node > & val ) { val.serialize( os ); },
						[&]( const element< element_type::int32_node > & val ) { val.serialize( os ); },
						[&]( const element< element_type::int64_node > & val ) { val.serialize( os ); },
						[&]( const element< element_type::array_node > & val ) { val.serialize( os ); },
						[&]( const element< element_type::double_node > & val ) { val.serialize( os ); },
						[&]( const element< element_type::string_node > & val ) { val.serialize( os ); },
						[&]( const element< element_type::binary_node > & val ) { val.serialize( os ); },
						[&]( const element< element_type::boolean_node > & val ) { val.serialize( os ); },
						[&]( const element< element_type::min_key_node > & val ) { val.serialize( os ); },
						[&]( const element< element_type::max_key_node > & val ) { val.serialize( os ); },
						[&]( const element< element_type::regular_node > & val ) { val.serialize( os ); },
						[&]( const element< element_type::datetime_node > & val ) { val.serialize( os ); },
						[&]( const element< element_type::document_node > & val ) { val.serialize( os ); },
						[&]( const element< element_type::timestamp_node > & val ) { val.serialize( os ); },
						[&]( const element< element_type::object_id_node > & val ) { val.serialize( os ); },
					}, node );
	}
	template< typename ... T > void node_to_json( std::ostream & os, const std::variant< T... > & node )
	{
		std::visit( overloaded
					{
						[&os]( const std::monostate & val ) {},
						[&os]( const element< element_type::null_node > & val ) { val.to_json( os ); },
						[&os]( const element< element_type::int32_node > & val ) { val.to_json( os ); },
						[&os]( const element< element_type::int64_node > & val ) { val.to_json( os ); },
						[&os]( const element< element_type::array_node > & val ) { val.to_json( os ); },
						[&os]( const element< element_type::double_node > & val ) { val.to_json( os ); },
						[&os]( const element< element_type::string_node > & val ) { val.to_json( os ); },
						[&os]( const element< element_type::binary_node > & val ) { os << R"({ "$binary" : )"; val.to_json( os ); os << " }"; },
						[&os]( const element< element_type::boolean_node > & val ) { val.to_json( os ); },
						[&os]( const element< element_type::min_key_node > & val ) { os << R"({ "$minKey" : )"; val.to_json( os ); os << " }"; },
						[&os]( const element< element_type::max_key_node > & val ) { os << R"({ "$maxKey" : )"; val.to_json( os ); os << " }"; },
						[&os]( const element< element_type::regular_node > & val ) { os << R"({ "$regularExpression" : )"; val.to_json( os ); os << " }"; },
						[&os]( const element< element_type::datetime_node > & val ) { os << R"({ "$date" : )"; val.to_json( os ); os << " }"; },
						[&os]( const element< element_type::document_node > & val ) { val.to_json( os ); },
						[&os]( const element< element_type::timestamp_node > & val ) { os << R"({ "$timestamp" : )"; val.to_json( os ); os << " }"; },
						[&os]( const element< element_type::object_id_node > & val ) { os << R"({ "$oid" : )"; val.to_json( os ); os << " }"; },
					}, node );
	}
	template< typename ... T > void node_from_json( std::istream & is, std::variant< T... > & node )
	{
		switch( speek( is ) )
		{
		case '\"':
		{
			auto elem = element< element_type::string_node >();

			elem.from_json( is );

			if( elem.get_value().front() == '$' )
			{
				assert( !is.eof() && sget( is ) == ':' && "template< typename ... T > void node_from_json( std::istream & is, std::variant< T... > & node )" );

				if( elem.get_value() == "$oid" )
				{
					auto oid = element< element_type::object_id_node >();
					oid.from_json( is );
					node = std::move( oid );
				}
				else if( elem.get_value() == "$date" )
				{
					auto date = element< element_type::datetime_node >();
					date.from_json( is );
					node = std::move( date );
				}
				else if( elem.get_value() == "$numberDouble" )
				{
					auto dou = element< element_type::double_node >();
					dou.from_json( is );
					node = std::move( dou );
				}
				else if( elem.get_value() == "$minKey" )
				{
					auto min = element< element_type::min_key_node >();
					min.from_json( is );
					node = std::move( min );
				}
				else if( elem.get_value() == "$maxKey" )
				{
					auto max = element< element_type::max_key_node >();
					max.from_json( is );
					node = std::move( max );
				}
				else if( elem.get_value() == "$timestamp" )
				{
					auto time = element< element_type::timestamp_node >();
					time.from_json( is );
					node = std::move( time );
				}
				else if( elem.get_value() == "$binary" )
				{
					auto time = element< element_type::binary_node >();
					time.from_json( is );
					node = std::move( time );
				}
				else if( elem.get_value() == "$regularExpression" )
				{
					auto reg = element< element_type::regular_node >();
					reg.from_json( is );
					node = std::move( reg );
				}
				else
				{
					assert( false && "template< typename ... T > void node_from_json( std::istream & is, std::variant< T... > & node )" );
				}
			}
			else if( elem.get_value() == "NaN" )
			{
				node = element<element_type::double_node>( std::numeric_limits<double>::quiet_NaN() );
			}
			else if( elem.get_value() == "Infinity" )
			{
				node = element<element_type::double_node>( std::numeric_limits<double>::infinity() );
			}
			else if( elem.get_value() == "-Infinity" )
			{
				node = element<element_type::double_node>( -std::numeric_limits<double>::infinity() );
			}
			else
			{
				node = std::move( elem );
			}
		}
		break;
		case '{':
		{
			auto pos = is.tellg();

			if( sget( is ) == '{' && sget( is ) == '\"' && sget( is ) == '$' )
			{
				pos += 1;
				is.seekg( pos );

				node_from_json( is, node );

				assert( smatch( is, "}" ) && "template< typename ... T > void node_from_json( std::istream & is, std::variant< T... > & node )" );
			}
			else
			{
				is.seekg( pos );

				auto doc = element< element_type::document_node >();
				doc.from_json( is );
				node = std::move( doc );
			}
		}
		break;
		case '[':
		{
			auto arr = element< element_type::array_node >();
			arr.from_json( is );
			node = std::move( arr );
		}
		break;
		case 'n':
		{
			auto elem = element<element_type::null_node>();
			elem.from_json( is );
			node = std::move( elem );
		}
		break;
		case 't':
		case 'f':
		{
			auto elem = element<element_type::boolean_node>();
			elem.from_json( is );
			node = std::move( elem );
		}
		break;
		default:
			if( !is.eof() && ( speek( is ) >= '0' && speek( is ) <= '9' ) || speek( is ) == '.' || speek( is ) == '-' )
			{
				std::string num;

				while( !is.eof() && ( speek( is ) >= '0' && speek( is ) <= '9' ) || speek( is ) == '.' || speek( is ) == '-' )
				{
					num.push_back( sget( is ) );
				}

				if( num.find( '.' ) != std::string::npos )
				{
					node = std::move( element< element_type::double_node >( std::stod( num ) ) );
				}
				else
				{
					std::int64_t n = std::stoll( num );
					if( n > std::numeric_limits<std::int32_t>::max() ||
						n < std::numeric_limits<std::int32_t>::min() )
					{
						node = std::move( element< element_type::int64_node >( n ) );
					}
					else
					{
						node = std::move( element< element_type::int32_node >( static_cast<std::int32_t>( n ) ) );
					}
				}
			}
			else
			{
				throw std::runtime_error( "template< typename ... T > void node_from_json( std::istream & is, std::variant< T... > & node )" );
			}
			break;
		}
	}

	using null_t = element< element_type::null_node >;
	using int32_t = element< element_type::int32_node >;
	using int64_t = element< element_type::int64_node >;
	using array_t = element< element_type::array_node >;
	using double_t = element< element_type::double_node >;
	using string_t = element< element_type::string_node >;
	using binary_t = element< element_type::binary_node >;
	using boolean_t = element< element_type::boolean_node >;
	using min_key_t = element< element_type::min_key_node >;
	using max_key_t = element< element_type::max_key_node >;
	using regular_t = element< element_type::regular_node >;
	using datetime_t = element< element_type::datetime_node >;
	using document_t = element< element_type::document_node >;
	using timestamp_t = element< element_type::timestamp_node >;
	using object_id_t = element< element_type::object_id_node >;

	using node_t = std::variant< 
		std::monostate,
		null_t,
		int32_t,
		int64_t,
		array_t,
		double_t,
		string_t,
		binary_t,
		boolean_t,
		min_key_t,
		max_key_t,
		regular_t,
		datetime_t,
		document_t,
		timestamp_t,
		object_id_t
	>;
}

#endif//BSON_HPP__B008D87D_A662_47B8_8066_0121BFDF3473
