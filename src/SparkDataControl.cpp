/*
 * SparkDataControl.cpp
 *
 *  Created on: 19.08.2021
 *      Author: stangreg
 */

#include "SparkDataControl.h"

SparkBLEControl *SparkDataControl::bleControl = nullptr;
SparkStreamReader SparkDataControl::spark_ssr;
SparkMessage SparkDataControl::spark_msg;
SparkPresetBuilder SparkDataControl::presetBuilder;
SparkDisplayControl *SparkDataControl::spark_display = nullptr;

Preset SparkDataControl::activePreset_;
Preset SparkDataControl::pendingPreset_ = activePreset_;

int SparkDataControl::activeBank_ = 0;
int SparkDataControl::pendingBank_ = 0;
Preset SparkDataControl::appReceivedPreset_;
int SparkDataControl::presetEditMode_ = PRESET_EDIT_NONE;

int SparkDataControl::presetNumToEdit_ = 0;
int SparkDataControl::presetBankToEdit_ = 0;

int SparkDataControl::activePresetNum_ = 1;
std::string SparkDataControl::responseMsg_ = "";

std::vector<ByteVector> SparkDataControl::ack_msg;
int SparkDataControl::operationMode_ = SPARK_MODE_APP;

int SparkDataControl::currentBTMode = BT_MODE_BLE;


SparkDataControl::SparkDataControl() {
	//init();
	bleControl = new SparkBLEControl(this);
}

SparkDataControl::~SparkDataControl() {
	// TODO Auto-generated destructor stub
	if (bleControl)
		delete bleControl;
	if (spark_display)
		delete spark_display;
}

void SparkDataControl::init(int opMode) {
	operationMode_ = opMode;
	// Creating vector of presets
	presetBuilder.initializePresetListFromFS();
	if (operationMode_ == SPARK_MODE_APP) {
		// initialize BLE
		bleKeyboard.setName("Ignitron BLE");
		bleKeyboard.begin();
		delay(1000);
		bleKeyboard.end();
		bleControl->initBLE(&notifyCB);

	} else if (operationMode_ == SPARK_MODE_AMP) {
		pendingBank_ = 1;
		activeBank_ = 1;
		std::string currentBTModeFile;
		fileSystem.openFromFile(btModeFileName.c_str(), currentBTModeFile);

		std::stringstream stream(currentBTModeFile);
		std::string line;
		while (std::getline(stream, line)) {
			currentBTMode = stoi(line);
		}

		if (currentBTMode == BT_MODE_BLE) {
			bleControl->startServer();
		} else if (currentBTMode == BT_MODE_SERIAL) {
			bleControl->startBTSerial();
		}
		activePreset_ = presetBuilder.getPreset(activePresetNum_, activeBank_);
		pendingPreset_ = presetBuilder.getPreset(activePresetNum_,
				pendingBank_);
	}

}


void SparkDataControl::switchOperationMode(int opMode) {
	operationMode_ = opMode;
	buttonMode_ = SWITCH_MODE_PRESET;
	if (opMode == SPARK_MODE_APP) {
		bleKeyboard.end();
	} else if (opMode == SPARK_MODE_LOOPER) {
		bleKeyboard.start();
	}
	updatePendingWithActive();
}

void SparkDataControl::setDisplayControl(SparkDisplayControl *display) {
	spark_display = display;
}

void SparkDataControl::checkForUpdates() {
	if (spark_ssr.isPresetNumberUpdated()) {
		spark_ssr.resetPresetNumberUpdateFlag();
		getCurrentPresetFromSpark();
	}
	// Check if active preset has been updated
	// If so, update the preset variables
	// TODO Check if we need to fix here if active preset name
	// is updated in AMP mode sometimes

	if (spark_ssr.isPresetUpdated()) {
		if (operationMode_ == SPARK_MODE_APP){
			Serial.println("DEBUG: MAYBE HERE IS THE ISSUE!");
			pendingPreset_  = spark_ssr.currentSetting();
			activePreset_ = pendingPreset_;
			spark_ssr.resetPresetUpdateFlag();
		}
	}
	// Checking if OTA server has been requested
	if (operationMode_ == SPARK_MODE_AMP) {

		while (bleControl && bleControl->byteAvailable()) {
			byte inputByte = bleControl->readByte();
			currentBTMsg.push_back(inputByte);
			int msgSize = currentBTMsg.size();
			if (msgSize > 0) {
				if (currentBTMsg[msgSize - 1] == 0xF7) {
					DEBUG_PRINTF("Free Heap size: %d\n", ESP.getFreeHeap()); DEBUG_PRINTF("Max free Heap block: %d\n",
							ESP.getMaxAllocHeap());

					DEBUG_PRINTLN("Received a message");
					DEBUG_PRINTVECTOR(currentBTMsg);
					DEBUG_PRINTLN();
					heap_caps_check_integrity_all(true) ;
					processSparkData(currentBTMsg);

					currentBTMsg.clear();

				}
				delay(10);
			}
		}

	}

}

void SparkDataControl::startBLEServer() {
	bleControl->startServer();
}

bool SparkDataControl::checkBLEConnection() {
	if (bleControl->isAmpConnected()) {
		return true;
	} else if (bleControl->isConnectionFound()) {
		if (bleControl->connectToServer()) {
			bleControl->subscribeToNotifications(&notifyCB);
			Serial.println("BLE connection to Spark established.");
			return true;
		} else {
			Serial.println("Failed to connect, starting scan");
			bleControl->startScan();
			return false;
		}
	}
	return false;
}


Preset SparkDataControl::getPreset(int bank, int pre) {
	return presetBuilder.getPreset(bank, pre);
}

int SparkDataControl::getNumberOfBanks() {
	return presetBuilder.getNumberOfBanks();
}

bool SparkDataControl::isAmpConnected() {
	return bleControl->isAmpConnected();
}

bool SparkDataControl::isAppConnected() {
	return bleControl->isAppConnected();
}

void SparkDataControl::notifyCB(
		NimBLERemoteCharacteristic *pRemoteCharacteristic, uint8_t *pData,
		size_t length, bool isNotify) {

	// Transform data into ByteVetor and process
	ByteVector chunk(&pData[0], &pData[length]);
	processSparkData(chunk);

}

int SparkDataControl::processSparkData(ByteVector blk) {

	bool ackNeeded;
	byte seq, sub_cmd;

	DEBUG_PRINTLN("Received data:");
	DEBUG_PRINTVECTOR(blk);
	DEBUG_PRINTLN();
	// Check if ack needed. In positive case the sequence number and command
	// are also returned to send back to requester
	std::tie(ackNeeded, seq, sub_cmd) = spark_ssr.needsAck(blk);
	if (ackNeeded) {
		if (operationMode_ == SPARK_MODE_AMP) {
			ack_msg = spark_msg.send_ack(seq, sub_cmd, DIR_FROM_SPARK);
		} else {
			ack_msg = spark_msg.send_ack(seq, sub_cmd, DIR_TO_SPARK);
		}

		DEBUG_PRINTLN("Sending acknowledgement");
		if (operationMode_ == SPARK_MODE_APP) {
			bleControl->writeBLE(ack_msg);
		} else if (operationMode_ == SPARK_MODE_AMP) {
			bleControl->notifyClients(ack_msg);
		}
	}
	int retCode = spark_ssr.processBlock(blk);
	if (retCode == MSG_PROCESS_RES_REQUEST && operationMode_ == SPARK_MODE_AMP) {

		std::vector<ByteVector> msg;
		std::vector<CmdData> currentMessage = spark_ssr.lastMessage();
		byte currentMessageNum = spark_ssr.lastMessageNum();
		byte sub_cmd = currentMessage.back().subcmd;

		switch (sub_cmd) {

		case 0x23:
			DEBUG_PRINTLN("Found request for serial number");
			msg = spark_msg.send_serial_number(
					currentMessageNum);
			break;
		case 0x2F:
			DEBUG_PRINTLN("Found request for firmware version");
			msg = spark_msg.send_firmware_version(
					currentMessageNum);
			break;
		case 0x2A:
			DEBUG_PRINTLN("Found request for hw checksum");
			msg = spark_msg.send_hw_checksums(currentMessageNum);
			break;
		case 0x10:
			DEBUG_PRINTLN("Found request for hw preset number");
			msg = spark_msg.send_hw_preset_number(currentMessageNum);
			break;
		case 0x01:
			DEBUG_PRINTLN("Found request for current preset");
			msg = spark_msg.create_preset(activePreset_, DIR_FROM_SPARK,
					currentMessageNum);
			break;
		default:
			break;
		}

		bleControl->notifyClients(msg);
	}
	if (retCode == MSG_PROCESS_RES_COMPLETE && operationMode_ == SPARK_MODE_AMP) {
		std::string msgStr = spark_ssr.getJson();
		if (msgStr.length() > 0) {
			Serial.println("Message processed:");
			Serial.println(msgStr.c_str());
		}
		if (spark_ssr.lastMessageType() == MSG_TYPE_PRESET) {
			presetEditMode_ = PRESET_EDIT_STORE;
			appReceivedPreset_ = presetBuilder.getPresetFromJson(&msgStr[0]);
			DEBUG_PRINTLN("received from app:");
			DEBUG_PRINTLN(appReceivedPreset_.json.c_str());
			spark_ssr.resetPresetUpdateFlag();
			spark_ssr.resetPresetNumberUpdateFlag();
			presetNumToEdit_ = 0;
		}
	}
	// if last Ack was for preset change (0x38) or effect switch (0x15),
	// confirm pending preset into active
	byte lastAck = spark_ssr.getLastAckAndEmpty();
	if ((lastAck == 0x38 && activeBank_ != 0) || lastAck == 0x15) {
		Serial.println("OK!");
		activePreset_ = pendingPreset_;
		pendingPreset_ = activePreset_;
	}
	return retCode;
}

bool SparkDataControl::getCurrentPresetFromSpark() {
	current_msg = spark_msg.get_current_preset();
	DEBUG_PRINTLN("Getting current preset from Spark");
	if (bleControl->writeBLE(current_msg)) {
		return true;
	}
	return false;
}

void SparkDataControl::updatePendingPreset(int bnk) {
	pendingPreset_ = getPreset(bnk, activePresetNum_);
}

void SparkDataControl::updatePendingWithActive() {
	pendingBank_ = activeBank_;
	pendingPreset_ = activePreset_;
}
bool SparkDataControl::switchPreset(int pre, bool isInitial) {
	bool retValue = false;
	int bnk = pendingBank_;
	if (operationMode_ == SPARK_MODE_APP || operationMode_ == SPARK_MODE_LOOPER) {
		if (pre == activePresetNum_ && activeBank_ == pendingBank_
				&& !(activePreset_.isEmpty) && !isInitial) {
			Pedal drivePedal = activePreset_.pedals[2];
			std::string drivePedalName = drivePedal.name;
			bool isDriveEnabled = drivePedal.isOn;
			if (switchEffectOnOff(drivePedalName, !isDriveEnabled)) {
				retValue = true;
			}
		} else {
			if (bnk == 0) { // for bank 0 switch hardware presets
				current_msg = spark_msg.change_hardware_preset(pre);
				Serial.printf("Changing to HW preset %d\n", pre);
				if (bleControl->writeBLE(current_msg)
						&& getCurrentPresetFromSpark()) {
					// For HW presets we always need to get the preset from Spark
					// as we don't know the parameters
					retValue = true;
				}
			} else {
				pendingPreset_ = presetBuilder.getPreset(bnk, pre);
				current_msg = spark_msg.create_preset(pendingPreset_);
				Serial.printf("Changing to preset %2d-%d...", bnk, pre);
				if (bleControl->writeBLE(current_msg)) {
					// This is the final message with actually switches over to the
					//previously sent preset
					current_msg = spark_msg.change_hardware_preset(128);
					if (bleControl->writeBLE(current_msg)) {
						retValue = true;
					}
				}
			}
		}
	}
	if (retValue == true) {
		activeBank_ = bnk;
		activePresetNum_ = pre;
	}
	return retValue;
}

bool SparkDataControl::switchEffectOnOff(std::string fx_name, bool enable) {

	Serial.printf("Switching %s effect %s...", enable ? "On" : "Off",
			fx_name.c_str());
	for (int i = 0; i < pendingPreset_.pedals.size(); i++) {
		Pedal currentPedal = pendingPreset_.pedals[i];
		if (currentPedal.name == fx_name) {
			pendingPreset_.pedals[i].isOn = enable;
			break;
		}
	}
	current_msg = spark_msg.turn_effect_onoff(fx_name, enable);
	if (bleControl->writeBLE(current_msg)) {
		return true;
	}
	return false;
}

void SparkDataControl::processPresetEdit(int presetNum) {
	if (presetNum == 0) {
		processDeletePresetRequest();
	} else if (presetEditMode_ == PRESET_EDIT_STORE) {
		processStorePresetRequest(presetNum);
	} else {
		resetPresetEdit(true, true);
		activePresetNum_ = presetNum;
		activePreset_ = presetBuilder.getPreset(activeBank_, activePresetNum_);
		pendingPreset_ = activePreset_;

	}
}

void SparkDataControl::processStorePresetRequest(int presetNum) {
	int responseCode;
	responseMsg_ = "";
	if (presetEditMode_ == PRESET_EDIT_STORE) {
		if (presetNumToEdit_ == presetNum
				&& presetBankToEdit_ == pendingBank_) {
			responseCode = presetBuilder.storePreset(appReceivedPreset_,
					pendingBank_, presetNum);
			if (responseCode == STORE_PRESET_OK) {
				Serial.println("Successfully stored preset");
				resetPresetEdit(true, true);
				appReceivedPreset_ = { };
				activePresetNum_ = presetNum;
				activePreset_ = presetBuilder.getPreset(activeBank_,
						activePresetNum_);
				pendingPreset_ = activePreset_;
				responseMsg_ = "SAVE OK";
			}
			if (responseCode == STORE_PRESET_FILE_EXISTS) {
				responseMsg_ = "PRST EXIST";
			}
			if (responseCode == STORE_PRESET_ERROR_OPEN
					|| responseCode == STORE_PRESET_UNKNOWN_ERROR) {
				responseMsg_ = "SAVE ERROR";
			}
		} else {
			activePresetNum_ = presetNum;
			activePreset_ = presetBuilder.getPreset(activeBank_,
					activePresetNum_);
			pendingPreset_ = activePreset_;
			presetNumToEdit_ = presetNum;
			presetBankToEdit_ = pendingBank_;
		}
	}

}

void SparkDataControl::resetPresetEdit(bool resetEditMode, bool resetPreset) {
	presetNumToEdit_ = 0;
	presetBankToEdit_ = 0;

	if (resetPreset) {
		appReceivedPreset_ = { };
	}
	if (resetEditMode) {
		presetEditMode_ = PRESET_EDIT_NONE;
	}

}

void SparkDataControl::resetPresetEditResponse() {
	responseMsg_ = "";
}

void SparkDataControl::processDeletePresetRequest() {
	int responseCode;
	responseMsg_ = "";
	if (presetEditMode_ == PRESET_EDIT_DELETE && activeBank_ > 0) {
		responseCode = presetBuilder.deletePreset(activeBank_,
				activePresetNum_);
		if (responseCode == DELETE_PRESET_OK
				|| responseCode == DELETE_PRESET_FILE_NOT_EXIST) {
			Serial.printf("Successfully deleted preset %d-%d\n", pendingBank_,
					activePresetNum_);
			presetNumToEdit_ = 0;
			presetBankToEdit_ = 0;
			activePreset_ = presetBuilder.getPreset(pendingBank_,
					activePresetNum_);
			pendingPreset_ = activePreset_;
			if (responseCode == DELETE_PRESET_OK) {
				responseMsg_ = "DELETE OK";
			} else {
				responseMsg_ = "FILE NOT EXITS";
			}
		}
		if (responseCode == DELETE_PRESET_ERROR_OPEN
				|| responseCode == STORE_PRESET_UNKNOWN_ERROR) {
			responseMsg_ = "DELETE ERROR";
		}
		resetPresetEdit(true, true);
	} else {
		setPresetDeletionFlag();
		presetNumToEdit_ = activePresetNum_;
		presetBankToEdit_ = activeBank_;
	}

}

void SparkDataControl::setPresetDeletionFlag() {
	presetEditMode_ = PRESET_EDIT_DELETE;
}

void SparkDataControl::updateActiveWithPendingPreset() {
	activePreset_ = pendingPreset_;
}

void SparkDataControl::sendButtonPressAsKeyboard(uint8_t c) {
	if (bleKeyboard.isConnected()) {
		Serial.printf("Sending button: %d\n", c);
		bleKeyboard.write(c);
	}
	else {
		Serial.println("Keyboard not connected");
	}

}

void SparkDataControl::toggleBTMode() {

	Serial.print("Switching Bluetooth mode to ");
	if (currentBTMode == BT_MODE_BLE) {
		Serial.println("Serial");
		currentBTMode = BT_MODE_SERIAL;
	} else if (currentBTMode == BT_MODE_SERIAL) {
		Serial.println("BLE");
		currentBTMode = BT_MODE_BLE;
	}
	// Save new mode to file
	fileSystem.saveToFile(btModeFileName.c_str(), currentBTMode);
	Serial.println("Restarting in new BT mode");
	ESP.restart();
}
