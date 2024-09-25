/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (©) 2019-2024 OpenTibiaBR <opentibiabr@outlook.com>
 * Repository: https://github.com/opentibiabr/canary
 * License: https://github.com/opentibiabr/canary/blob/main/LICENSE
 * Contributors: https://github.com/opentibiabr/canary/graphs/contributors
 * Website: https://docs.opentibiabr.com/
 */

#include "pch.hpp"

#include "server/network/message/networkmessage.hpp"
#include "items/containers/container.hpp"

int32_t NetworkMessage::decodeHeader() {
	int32_t newSize = buffer[0] | buffer[1] << 8;
	info.length = newSize;
	return info.length;
}

std::string NetworkMessage::getString(uint16_t stringLen /* = 0*/, const std::source_location &location) {
	if (stringLen == 0) {
		stringLen = get<uint16_t>();
	}

	if (!canRead(stringLen)) {
		g_logger().error("[{}] not enough data to read string of length: {}. Called from {}:{} in {}", __METHOD_NAME__, stringLen, location.line(), location.column(), location.function_name());
		return {};
	}

	if (stringLen > NETWORKMESSAGE_MAXSIZE) {
		g_logger().error("[{}] exceded NetworkMessage max size: {}, actually size: {}.  Called from '{}:{}' in '{}'", __METHOD_NAME__, NETWORKMESSAGE_MAXSIZE, stringLen, location.line(), location.column(), location.function_name());
		return {};
	}

	g_logger().debug("[{}] called from '{}:{}' in '{}'", __METHOD_NAME__, location.line(), location.column(), location.function_name());

	// Copy the string from the buffer
	std::string result(buffer.begin() + info.position, buffer.begin() + info.position + stringLen);
	info.position += stringLen;
	return result;
}

Position NetworkMessage::getPosition() {
	Position pos;
	pos.x = get<uint16_t>();
	pos.y = get<uint16_t>();
	pos.z = getByte();
	return pos;
}

void NetworkMessage::addString(const std::string &value, const std::source_location &location /*= std::source_location::current()*/, const std::string &function /* = ""*/) {
	size_t stringLen = value.length();
	if (value.empty()) {
		if (!function.empty()) {
			g_logger().debug("[{}] attempted to add an empty string. Called from '{}'", __METHOD_NAME__, function);
		} else {
			g_logger().debug("[{}] attempted to add an empty string. Called from '{}:{}' in '{}'", __METHOD_NAME__, location.line(), location.column(), location.function_name());
		}
		return;
	}
	if (!canAdd(stringLen + 2)) {
		if (!function.empty()) {
			g_logger().error("[{}] NetworkMessage size is wrong: {}. Called from '{}'", __METHOD_NAME__, stringLen, function);
		} else {
			g_logger().error("[{}] NetworkMessage size is wrong: {}. Called from '{}:{}' in '{}'", __METHOD_NAME__, stringLen, location.line(), location.column(), location.function_name());
		}
		return;
	}
	if (stringLen > NETWORKMESSAGE_MAXSIZE) {
		if (!function.empty()) {
			g_logger().error("[{}] exceeded NetworkMessage max size: {}, actual size: {}. Called from '{}'", __METHOD_NAME__, NETWORKMESSAGE_MAXSIZE, stringLen, function);
		} else {
			g_logger().error("[{}] exceeded NetworkMessage max size: {}, actual size: {}. Called from '{}:{}' in '{}'", __METHOD_NAME__, NETWORKMESSAGE_MAXSIZE, stringLen, location.line(), location.column(), location.function_name());
		}
		return;
	}

	if (!function.empty()) {
		g_logger().debug("[{}] called from '{}'", __METHOD_NAME__, function);
	} else {
		g_logger().debug("[{}] called from '{}:{}' in '{}'", __METHOD_NAME__, location.line(), location.column(), location.function_name());
	}

	uint16_t len = static_cast<uint16_t>(stringLen);
	add<uint16_t>(len);

	// Using std::copy to copy the string into the buffer
	std::copy(value.begin(), value.end(), buffer.begin() + info.position);
	info.position += stringLen;
	info.length += stringLen;
}

void NetworkMessage::addDouble(double value, uint8_t precision /*= 2*/) {
	addByte(precision);
	uint32_t scaledValue = static_cast<uint32_t>((value * std::pow(10.0, precision)) + static_cast<float>(std::numeric_limits<int32_t>::max()));
	add<uint32_t>(scaledValue);
}

void NetworkMessage::addBytes(const char* bytes, size_t size) {
	if (bytes == nullptr) {
		g_logger().error("[NetworkMessage::addBytes] - Bytes is nullptr");
		return;
	}
	if (!canAdd(size)) {
		g_logger().error("[NetworkMessage::addBytes] - NetworkMessage size is wrong: {}", size);
		return;
	}
	if (size > NETWORKMESSAGE_MAXSIZE) {
		g_logger().error("[NetworkMessage::addBytes] - Exceded NetworkMessage max size: {}, actually size: {}", NETWORKMESSAGE_MAXSIZE, size);
		return;
	}

	memcpy(buffer.data() + info.position, bytes, size);
	info.position += size;
	info.length += size;
}

void NetworkMessage::addPaddingBytes(size_t n) {
	if (!canAdd(n)) {
		g_logger().error("[NetworkMessage::addPaddingBytes] - Cannot add padding bytes, buffer overflow");
		return;
	}

	memset(buffer.data() + info.position, 0x33, n);
	info.position += n;
	info.length += n;
}

void NetworkMessage::addPosition(const Position &pos) {
	add<uint16_t>(pos.x);
	add<uint16_t>(pos.y);
	addByte(pos.z);
}
