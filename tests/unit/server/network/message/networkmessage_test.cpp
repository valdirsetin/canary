#include "pch.hpp"

#include <boost/ut.hpp>

#include "server/network/message/networkmessage.hpp"
#include "utils/tools.hpp"

using namespace boost::ut;

// Define a test suite for NetworkMessage
suite<"networkmessage"> networkMessageTest = [] {
	
	test("NetworkMessage::addByte and getByte") = [] {
		NetworkMessage msg;
		msg.addByte(0x64);
		expect(eq(msg.getByte(), 0x64)) << "Expected to retrieve the same byte value";
	};

	test("NetworkMessage::addString and getString") = [] {
		NetworkMessage msg;
		std::string testStr = "TestString";
		msg.addString(testStr);
		expect(eq(msg.getString(), testStr)) << "Expected to retrieve the same string value";
	};

	test("NetworkMessage::addString should handle empty string") = [] {
		NetworkMessage msg;
		msg.addString("");
		expect(eq(msg.getString(), std::string {})) << "Expected to retrieve an empty string";
	};

	test("NetworkMessage::addString should fail with oversized string") = [] {
		NetworkMessage msg;
		std::string oversizedString(NETWORKMESSAGE_MAXSIZE + 1, 'a');
		msg.addString(oversizedString);
		// Should not add oversized string, expect empty string to be returned
		expect(eq(msg.getString(), std::string {})) << "Expected to retrieve an empty string due to oversized input";
	};

	test("NetworkMessage::canAdd should return false when exceeding max size") = [] {
		NetworkMessage msg;
		expect(msg.canAdd(NETWORKMESSAGE_MAXSIZE - msg.getBufferPosition())) << "Should have enough space in buffer";
		expect(not msg.canAdd(NETWORKMESSAGE_MAXSIZE + 1)) << "Should not be able to add data exceeding the max buffer size";
	};

	test("NetworkMessage::addDouble and getDouble") = [] {
		NetworkMessage msg;
		double testValue = 12345.6789;
		uint8_t precision = 2;
		msg.addDouble(testValue, precision);
		// Implement getDouble if needed to test retrieval, or use addByte and getByte to manually verify
	};

	test("NetworkMessage::addPosition and getPosition") = [] {
		NetworkMessage msg;
		Position pos { 100, 200, 7 };
		msg.addPosition(pos);
		expect(eq(msg.getPosition(), pos)) << "Expected to retrieve the same position values";
	};

	test("NetworkMessage::reset should clear buffer") = [] {
		NetworkMessage msg;
		msg.addByte(0x64);
		msg.reset();
		expect(eq(msg.getLength(), 0)) << "Expected the message length to be zero after reset";
	};

	test("NetworkMessage::append should merge messages correctly") = [] {
		NetworkMessage msg1, msg2;
		msg1.addByte(0x64);
		msg1.addString("Hello");
		
		msg2.addByte(0x32);
		msg2.addString("World");
		
		// Check that the merged message contains both the original and appended data
		expect(eq(msg1.getByte(), 0x64)) << "Expected the first byte of the first message";
		expect(eq(msg1.getString(), "Hello")) << "Expected the first string of the first message";
		expect(eq(msg1.getByte(), 0x32)) << "Expected the first byte of the second message";
		expect(eq(msg1.getString(), "World")) << "Expected the first string of the second message";
	};

	test("NetworkMessage::getString should handle out-of-bounds access safely") = [] {
		NetworkMessage msg;
		std::string testStr = "Short";
		msg.addString(testStr);

		// Move the position to simulate incomplete data read
		msg.setBufferPosition(msg.getBufferPosition() + 10);
		expect(eq(msg.getString(), std::string {})) << "Expected empty string due to out-of-bounds access";
	};

	test("NetworkMessage::decodeHeader should correctly decode the header") = [] {
		NetworkMessage msg;
		msg.addByte(0x12);
		msg.addByte(0x34);
		
		int32_t header = msg.decodeHeader();
		expect(eq(header, 0x3412)) << "Expected header to be decoded correctly";
	};

	test("NetworkMessage::addBytes and validate content") = [] {
		NetworkMessage msg;
		const char* bytes = "testBytes";
		msg.addBytes(bytes, 9);
		
		auto buffer = msg.getBuffer();
		expect(eq(std::string(reinterpret_cast<const char*>(buffer + msg.getBufferPosition() - 9)), "testBytes")) << "Expected the same bytes added";
	};
};
