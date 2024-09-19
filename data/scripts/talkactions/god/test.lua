local testLog = TalkAction("/testlog")

function testLog.onSay(player, words, param)
	if param == "" then
		player:sendCancelMessage("Log level and message required.")
		logger.error("[testLog.onSay] - Log level and message not found")
		return true
	end

	local split = param:split(",")
	local logLevel = split[1]:trim():lower()
	local message = string.trimSpace(split[2])

	if message == "" then
		player:sendCancelMessage("Log message required.")
		return false
	end

	if logLevel == "info" then
		logger.info("[testLog] - {}", message)
	elseif logLevel == "warn" then
		logger.warn("[testLog] - {}", message)
	elseif logLevel == "error" then
		logger.error("[testLog] - {}", message)
	elseif logLevel == "debug" then
		logger.debug("[testLog] - {}", message)
	else
		player:sendTextMessage(MESSAGE_EVENT_ADVANCE, "Invalid log level. Use 'info', 'warn', 'error' or 'debug'.")
		return false
	end

	player:sendTextMessage(MESSAGE_EVENT_ADVANCE, "Logged message [" .. message .. "] at '" .. logLevel .. "' level.")
	return true
end

testLog:separator(" ")
testLog:groupType("god")
testLog:register()

local containerTalkAction = TalkAction("!testcontainer")

function containerTalkAction.onSay(player, words, param)
	local container = player:getSlotItem(CONST_SLOT_BACKPACK)
	if not container then
		player:sendCancelMessage("Your backpack does not contain a valid container.")
		logger.error("[!container] - Player: {} has a backpack without a valid container.", player:getName())
		return true
	end

	local items = container:getItems(true)
	local totalItems = 0
	for i, item in pairs(items) do
		if item then
			local itemName = item:getName()

			player:sendTextMessage(MESSAGE_EVENT_ADVANCE, "Found item: " .. itemName)
			item:remove()
		end
		-- Increment the item counter
		totalItems = totalItems + 1
	end

	logger.error("[!container] - Player: {} executed the '!container' command. Total items found: {}.", player:getName(), totalItems)

	return true
end

containerTalkAction:separator(" ")
containerTalkAction:groupType("god")
containerTalkAction:register()
