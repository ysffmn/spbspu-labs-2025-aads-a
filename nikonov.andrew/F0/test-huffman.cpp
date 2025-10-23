#include <boost/test/unit_test.hpp>
#include <string>
#include <sstream>
#include "DataStorage.hpp"
#include "HuffmanCore.hpp"
#include "Commands.hpp"

BOOST_AUTO_TEST_CASE(storage_basic_operations)
{
  nikonov::Storage storage;

  BOOST_TEST(storage.addText("test1", "hello world") == true);
  BOOST_TEST(storage.textExists("test1") == true);
  BOOST_TEST(storage.textExists("nonexistent") == false);
  
  nikonov::Text* text = storage.getText("test1");
  BOOST_TEST(text != nullptr);
  BOOST_TEST(text->getOriginalContent() == "hello world");
  BOOST_TEST(text->isCompressed() == false);
}

BOOST_AUTO_TEST_CASE(storage_duplicate_ids)
{
  nikonov::Storage storage;
  
  BOOST_TEST(storage.addText("dup", "first") == true);
  BOOST_TEST(storage.addText("dup", "second") == false);
  
  std::unordered_map<char, std::string> codes = {{'a', "0"}};
  BOOST_TEST(storage.addEncoding("enc1", codes) == true);
  BOOST_TEST(storage.addEncoding("enc1", codes) == false);
}

BOOST_AUTO_TEST_CASE(huffman_core_basic_compression)
{
  std::string text = "aabacab";
  auto codes = nikonov::HuffmanCore::buildCodes(text);
  
  BOOST_TEST(codes.size() > 0);
  
  std::string compressed = nikonov::HuffmanCore::compress(text, codes);
  BOOST_TEST(!compressed.empty());
  
  std::unordered_map<std::string, char> reverseCodes;
  for (const auto& pair : codes) {
    reverseCodes[pair.second] = pair.first;
  }
  
  std::string decompressed = nikonov::HuffmanCore::decompress(compressed, reverseCodes);
  BOOST_TEST(decompressed == text);
}

BOOST_AUTO_TEST_CASE(huffman_core_empty_text)
{
  std::string empty_text = "";
  auto codes = nikonov::HuffmanCore::buildCodes(empty_text);
  
  BOOST_TEST(codes.empty() == true);
  
  std::string compressed = nikonov::HuffmanCore::compress(empty_text, codes);
  BOOST_TEST(compressed.empty() == true);
}

BOOST_AUTO_TEST_CASE(huffman_core_single_character)
{
  std::string text = "aaaa";
  auto codes = nikonov::HuffmanCore::buildCodes(text);
  
  BOOST_TEST(codes.size() == 1);
  BOOST_TEST(codes['a'] == "0");
  
  std::string compressed = nikonov::HuffmanCore::compress(text, codes);
  BOOST_TEST(compressed == "0000");
}

BOOST_AUTO_TEST_CASE(encoding_class_operations)
{
  std::unordered_map<char, std::string> codeTable = {
    {'a', "0"},
    {'b', "10"}, 
    {'c', "11"}
  };
  
  nikonov::Encoding encoding(codeTable, "source_text");
  
  const auto& retrievedTable = encoding.getCodeTable();
  BOOST_TEST(retrievedTable.at('a') == "0");
  BOOST_TEST(retrievedTable.at('b') == "10");
  BOOST_TEST(retrievedTable.at('c') == "11");
  
  BOOST_TEST(encoding.getFromTextId() == "source_text");
  
  auto reverseTable = encoding.getReverseTable();
  BOOST_TEST(reverseTable.at("0") == 'a');
  BOOST_TEST(reverseTable.at("10") == 'b');
  BOOST_TEST(reverseTable.at("11") == 'c');
}

BOOST_AUTO_TEST_CASE(text_class_properties)
{
  nikonov::Text uncompressed("hello", false, "", "");
  BOOST_TEST(uncompressed.getOriginalContent() == "hello");
  BOOST_TEST(uncompressed.isCompressed() == false);
  BOOST_TEST(uncompressed.getOriginalSizeBits() == 5 * 8);
  
  nikonov::Text compressed("hello", true, "enc1", "10101");
  BOOST_TEST(compressed.isCompressed() == true);
  BOOST_TEST(compressed.getCompressedContent() == "10101");
  BOOST_TEST(compressed.getEncodingId() == "enc1");
  BOOST_TEST(compressed.getCompressedSizeBits() == 5);
}

BOOST_AUTO_TEST_CASE(compress_text_command)
{
  nikonov::Storage storage;
  storage.addText("orig", "test message");
  
  std::istringstream iss("orig compressed enc1");
  std::ostringstream oss;
  
  nikonov::compressText(storage, iss, oss);
  
  BOOST_TEST(storage.textExists("compressed") == true);
  BOOST_TEST(storage.encodingExists("enc1") == true);
  
  nikonov::Text* compressedText = storage.getText("compressed");
  BOOST_TEST(compressedText != nullptr);
  BOOST_TEST(compressedText->isCompressed() == true);
}

BOOST_AUTO_TEST_CASE(create_encoding_command)
{
  nikonov::Storage storage;
  storage.addText("source", "sample text");
  
  std::istringstream iss("source new_encoding");
  std::ostringstream oss;
  
  nikonov::createEncoding(storage, iss, oss);
  
  BOOST_TEST(storage.encodingExists("new_encoding") == true);
  
  nikonov::Encoding* encoding = storage.getEncoding("new_encoding");
  BOOST_TEST(encoding != nullptr);
  BOOST_TEST(encoding->getFromTextId() == "source");
}

BOOST_AUTO_TEST_CASE(show_encoding_command)
{
  nikonov::Storage storage;
  std::unordered_map<char, std::string> codes = {{'x', "101"}, {'y', "110"}};
  storage.addEncoding("show_test", codes);
  
  std::istringstream iss("show_test");
  std::ostringstream oss;
  
  nikonov::showEncoding(storage, iss, oss);
  
  std::string output = oss.str();
  BOOST_TEST(output.find("x") != std::string::npos);
  BOOST_TEST(output.find("101") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(text_stats_command)
{
  nikonov::Storage storage;
  storage.addText("stats_test", "abc");
  
  std::istringstream iss("stats_test");
  std::ostringstream oss;
  
  nikonov::textStats(storage, iss, oss);
  
  std::string output = oss.str();
  BOOST_TEST(output.find("Total characters: 3") != std::string::npos);
  BOOST_TEST(output.find("Unique characters: 3") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(commands_registration)
{
  std::map<std::string, std::function<void(nikonov::Storage&, std::istream&, std::ostream&)>> commands;
  nikonov::getCommands(commands);
  
  BOOST_TEST(commands.count("compress_text") > 0);
  BOOST_TEST(commands.count("decompress_text") > 0);
  BOOST_TEST(commands.count("create_encoding") > 0);
  BOOST_TEST(commands.count("show_encoding") > 0);
  BOOST_TEST(commands.count("text_stats") > 0);
  BOOST_TEST(commands.count("test_compression") > 0);
}

BOOST_AUTO_TEST_CASE(integration_compress_decompress)
{
  nikonov::Storage storage;
  

  storage.addText("original", "hello huffman");
  

  std::istringstream compress_iss("original compressed encoding1");
  std::ostringstream compress_oss;
  nikonov::compressText(storage, compress_iss, compress_oss);
  

  std::istringstream decompress_iss("compressed decompressed");
  std::ostringstream decompress_oss;
  nikonov::decompressText(storage, decompress_iss, decompress_oss);
  

  nikonov::Text* original = storage.getText("original");
  nikonov::Text* decompressed = storage.getText("decompressed");
  
  BOOST_TEST(decompressed != nullptr);
  BOOST_TEST(decompressed->getOriginalContent() == original->getOriginalContent());
  BOOST_TEST(decompressed->isCompressed() == false);
}

BOOST_AUTO_TEST_CASE(apply_encoding_command)
{
  nikonov::Storage storage;
  storage.addText("source", "test data");
  
  std::unordered_map<char, std::string> codes = {{'t', "0"}, {'e', "10"}, {'s', "110"}, {' ', "1110"}, {'d', "1111"}, {'a', "11101"}};
  storage.addEncoding("test_enc", codes);

  std::istringstream iss("source test_enc compressed_result");
  std::ostringstream oss;
  
  nikonov::applyEncoding(storage, iss, oss);
  
  BOOST_TEST(storage.textExists("compressed_result") == true);
  nikonov::Text* result = storage.getText("compressed_result");
  BOOST_TEST(result != nullptr);
  BOOST_TEST(result->isCompressed() == true);
}

BOOST_AUTO_TEST_CASE(error_handling_invalid_text)
{
  nikonov::Storage storage;
  
  std::istringstream iss("nonexistent compressed enc1");
  std::ostringstream oss;
  
  BOOST_CHECK_THROW(nikonov::compressText(storage, iss, oss), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(error_handling_invalid_encoding)
{
  nikonov::Storage storage;
  storage.addText("text", "some content");
  
  std::istringstream iss("text nonexistent_encoding result");
  std::ostringstream oss;
  
  BOOST_CHECK_THROW(nikonov::applyEncoding(storage, iss, oss), std::runtime_error);
}
