#include <iostream>

#include "bson.hpp"

int main( int regc, char * argv[] )
{
	std::array<char, 25> data;
	for( size_t i = 0; i < data.size(); i++ ) data[i] = static_cast<char>( i + 'a' );

	bson::document_t doc1, doc2{
		std::pair{ "null", nullptr },
		std::pair{ "int32", std::numeric_limits<std::int32_t>::min() },
		std::pair{ "int64_t", std::numeric_limits<std::int64_t>::max() },
		std::pair{ "float", 123456.654321f },
		std::pair{ "double", 123456.654321 },
		std::pair{ "double-NaN", std::numeric_limits<double>::quiet_NaN() },
		std::pair{ "double-Infinity", std::numeric_limits<double>::infinity() },
		std::pair{ "string", "hello world!" },
		std::pair{ "boolean", false },
		std::pair{ "min_key", bson::min_key_t() },
		std::pair{ "max_key", bson::max_key_t() },
		std::pair{ "binary", bson::binary_t{ data } },
		std::pair{ "regular", bson::regular_t{ "^H", "i" } },
		std::pair{ "datetime", std::chrono::system_clock::now() },
		std::pair{ "timestamp", std::numeric_limits<std::uint64_t>::max() },
		std::pair{ "object_id", bson::object_id_t{ "A1B2C3D4E5F66F5E4D3C2B1A" } },
		//std::pair{ "decimal128", bson::decimal128_t{ 100,200 } },
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
											//bson::decimal128_t{ 100,200 },
											bson::binary_t{ data }
										}
		}
	};

	std::cout << std::endl;
	doc2.to_json( std::cout );
	std::cout << std::endl;

	std::stringstream sstream;
	doc2.to_json( sstream );

// 	std::cout << std::endl;
// 	std::cout << sstream.str() << std::endl;
// 	std::cout << std::endl;

	doc1.from_json( sstream );

	std::cout << std::endl;
	doc1.to_json( std::cout );
	std::cout << std::endl;

	return 0;
}