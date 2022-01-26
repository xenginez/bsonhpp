#include <iostream>

#include "bson.hpp"

int main( int regc, char * argv[] )
{
	std::array<char, 25> data;
	for( size_t i = 0; i < data.size(); i++ ) data[i] = static_cast<char>( i + 'a' );

	bson::document_t doc{
		std::pair{ "null", nullptr },
		std::pair{ "int32", std::numeric_limits<std::int32_t>::min() },
		std::pair{ "int64_t", std::numeric_limits<std::int64_t>::max() },
		std::pair{ "float", 123456.654321f },
		std::pair{ "double", 123456.654321 },
		std::pair{ "double-NaN", std::numeric_limits<double>::quiet_NaN() },
		std::pair{ "double-Infinity", std::numeric_limits<double>::infinity() },
		std::pair{ "double--Infinity", -std::numeric_limits<double>::infinity() },
		std::pair{ "string", "hello world!" },
		std::pair{ "boolean", false },
		std::pair{ "min_key", bson::min_key_t() },
		std::pair{ "max_key", bson::max_key_t() },
		std::pair{ "binary", bson::binary_t{ data } },
		std::pair{ "regular", bson::regular_t{ "^H", "i" } },
		std::pair{ "datetime", std::chrono::system_clock::now() },
		std::pair{ "timestamp", std::numeric_limits<std::uint64_t>::max() },
		std::pair{ "object_id", bson::object_id_t{ "A1B2C3D4E5F66F5E4D3C2B1A" } },
		std::pair{ "array", bson::array_t{
											nullptr,
											std::numeric_limits<std::int32_t>::min(),
											std::numeric_limits<std::int64_t>::max(),
											123456.654321f,
											123456.654321,
											"hello world!",
											true,
											bson::min_key_t(),
											bson::max_key_t(),
											bson::regular_t{ "^H", "i" },
											std::chrono::system_clock::now(),
											std::numeric_limits<std::uint64_t>::max(),
											bson::object_id_t{ "A1B2C3D4E5F66F5E4D3C2B1A" },
											bson::binary_t{ data }
										}
		}
	};

	// serialize document
	{
		std::stringstream sstream;
		doc.serialize( sstream );

		bson::document_t doc2;
		doc2.deserialize( sstream );

		std::cout << std::endl;
		doc2.to_json( std::cout );
		std::cout << std::endl;
	}

	std::cout << "-----------------------------------" << std::endl;

	// serialize node
	{
		std::stringstream sstream;
		doc.serialize( sstream );

		bson::node_t node;
		bson::node_deserialize( sstream, node );

		std::cout << std::endl;
		bson::node_to_json( std::cout, node );
		std::cout << std::endl;
	}

	std::cout << "-----------------------------------" << std::endl;

	// json document
	{
		std::stringstream sstream;
		doc.to_json( sstream );

		bson::document_t doc2;
		doc2.from_json( sstream );

		std::cout << std::endl;
		doc2.to_json( std::cout );
		std::cout << std::endl;
	}

	std::cout << "-----------------------------------" << std::endl;

	// json node
	{
		std::stringstream sstream;
		doc.to_json( sstream );

		bson::node_t node;
		bson::node_from_json( sstream, node );

		std::cout << std::endl;
		bson::node_to_json( std::cout, node );
		std::cout << std::endl;
	}


	return 0;
}