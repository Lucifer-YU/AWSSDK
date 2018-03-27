/*
 * JSONUtils.h
 *
 *  Created on: Jan 14, 2015
 *      Author: Lucifer
 */

#ifndef JSON_JSONCAST_H_
#define JSON_JSONCAST_H_

///////////////////////////////////////////////////////////////////////////////
/// JSON checked casters
/// @example
///		if (!JSONTryCast<int32_t>(node, value))
///			printf("cast value failed.\n");

template<typename T>
struct JSONTryCast;

template<>
struct JSONTryCast<int32_t> {
	JSONTryCast(JSONNode* node, int32_t& value) {
		checked = false;
		value = 0;
		if (node != NULL && node->getNodeType() == JSONNode::JNT_Number) {
			value = ((JSONNumber*) node)->getInt32Value();
			checked = true;
		}
	}
	operator bool() {
		return checked;
	}
	bool checked;
};

template<>
struct JSONTryCast<uint32_t> {
	JSONTryCast(JSONNode* node, uint32_t& value) {
		checked = false;
		value = 0;
		if (node != NULL && node->getNodeType() == JSONNode::JNT_Number) {
			value = ((JSONNumber*) node)->getUInt32Value();
			checked = true;
		}
	}
	operator bool() {
		return checked;
	}
	bool checked;
};

template<>
struct JSONTryCast<int64_t> {
	JSONTryCast(JSONNode* node, int64_t& value) {
		checked = false;
		value = 0;
		if (node != NULL && node->getNodeType() == JSONNode::JNT_Number) {
			value = ((JSONNumber*) node)->getInt64Value();
			checked = true;
		}
	}
	operator bool() {
		return checked;
	}
	bool checked;
};

template<>
struct JSONTryCast<uint64_t> {
	JSONTryCast(JSONNode* node, uint64_t& value) {
		checked = false;
		value = 0;
		if (node != NULL && node->getNodeType() == JSONNode::JNT_Number) {
			value = ((JSONNumber*) node)->getUInt64Value();
			checked = true;
		}
	}
	operator bool() {
		return checked;
	}
	bool checked;
};

template<>
struct JSONTryCast<float> {
	JSONTryCast(JSONNode* node, float& value) {
		checked = false;
		value = 0;
		if (node != NULL && node->getNodeType() == JSONNode::JNT_Number) {
			value = ((JSONNumber*) node)->getFloatValue();
			checked = true;
		}
	}
	operator bool() {
		return checked;
	}
	bool checked;
};

template<>
struct JSONTryCast<double> {
	JSONTryCast(JSONNode* node, double& value) {
		checked = false;
		value = 0;
		if (node != NULL && node->getNodeType() == JSONNode::JNT_Number) {
			value = ((JSONNumber*) node)->getDoubleValue();
			checked = true;
		}
	}
	operator bool() {
		return checked;
	}
	bool checked;
};

template<>
struct JSONTryCast<bool> {
	JSONTryCast(JSONNode* node, bool& value) {
		checked = false;
		value = 0;
		if (node != NULL && node->getNodeType() == JSONNode::JNT_Bool) {
			value = ((JSONBool*) node)->getValue();
			checked = true;
		}
	}
	operator bool() {
		return checked;
	}
	bool checked;
};

template<>
struct JSONTryCast<String> {
	JSONTryCast(JSONNode* node, String& value) {
		checked = false;
		value.setEmpty();
		if (node != NULL && node->getNodeType() == JSONNode::JNT_String) {
			value = ((JSONString*) node)->getValue();
			checked = true;
		}
	}
	operator bool() {
		return checked;
	}
	bool checked;
};

///////////////////////////////////////////////////////////////////////////////
/// JSON silent caster
/// @example
///		value = JSONCast<int32_t>(node);

#define LOG_TAG "JSONCast"

template<typename T>
struct JSONCast {
	JSONCast(JSONNode* node) {
		if (!JSONTryCast<T>(node, value)) {
			LOGW("JSON cast failed, set to initial value.\n");
		}
	}
	operator T() {
		return value;
	}
	T value;
};

#undef LOG_TAG

#endif /* JSON_JSONCAST_H_ */
