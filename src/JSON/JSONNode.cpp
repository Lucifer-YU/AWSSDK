/*
 * JSONNode.cpp
 *
 *   Created on: Dec 13, 2014
 *      Author: Lucifer
 */

#include "JSON.h"
#include <stdlib.h>

#ifdef	WIN32
#define	atoll	_atoi64
#endif

#undef LOGI
#define LOGI(...)
#define LOG_TAG "JSONNode"

JSONNode* JSONNode::fromJSONReader(JSONReader* reader) {
	BFX_ASSERT(reader);

	switch (reader->next()) {
	case JSONReader::JT_ObjectBegin:
		return readObject(reader);
	case JSONReader::JT_Error:
		break;
	default:
		reader->setLastError(JRE_MissingRoot);
		LOGE(reader->getLastErrorMessage());
	}
	return NULL;
}

JSONObject* JSONNode::readObject(JSONReader* reader) {
	JSONReader::JSONTokenType type;
	String key;
	REF<JSONNode> value;
	REF<JSONObject> object = new JSONObject();

	for (;;) {
		type = reader->next();
		switch (type) {
		case JSONReader::JT_String:
			key = reader->getRawValue();
			break;
		case JSONReader::JT_ObjectEnd:	// '}'
			object->autorelease();
			return object;
		case JSONReader::JT_Error:
			return NULL;
		default:
			reader->setLastError(JRE_UnexpectedToken);
			LOGE(reader->getLastErrorMessage());
			return NULL;
		}

		// The key is followed by ':'
		type = reader->next();
		if (type != JSONReader::JT_MemberSeparator) {
			// Unexpected token, Expected a ':' after a key
			if (type != JSONReader::JT_Error) {
				reader->setLastError(JRE_UnexpectedToken);
				LOGE(reader->getLastErrorMessage());
			}
			return NULL;
		}
		type = reader->next();
		switch (type) {
		case JSONReader::JT_ObjectBegin:
			value = readObject(reader);
			break;
		case JSONReader::JT_ArrayBegin:
			value = readArray(reader);
			break;
		case JSONReader::JT_String:
			value = new JSONString(reader->getRawValue());
			break;
		case JSONReader::JT_Number:
			value = new JSONNumber(reader->getRawValue());
			break;
		case JSONReader::JT_True:
			value = JSONBool::True();
			break;
		case JSONReader::JT_False:
			value = JSONBool::False();
			break;
		case JSONReader::JT_Null:
			value = JSONNull::Instance();
			break;
		case JSONReader::JT_Error:
			return NULL;
		default:
			reader->setLastError(JRE_UnexpectedToken);
			LOGE(reader->getLastErrorMessage());
			return NULL;
		}
		// add child value
		if (value == NULL)
			return NULL;
		object->setProperty(key, value);

		// Pairs are separated by ','.
		switch (reader->next()) {
		case JSONReader::JT_ArraySeparator:	// ','
			break;
		case JSONReader::JT_ObjectEnd:	// '}'
			object->autorelease();
			return object;
		case JSONReader::JT_Error:
			return NULL;
		default:
			reader->setLastError(JRE_UnexpectedToken);
			LOGE(reader->getLastErrorMessage());
			return NULL;
		}
	}
	BFX_ASSERT(false);
	return NULL;
}

JSONArray* JSONNode::readArray(JSONReader* reader) {
	JSONReader::JSONTokenType type;
	REF<JSONNode> value;
	REF<JSONArray> array = new JSONArray();

	for (;;) {
		type = reader->next();
		switch (type) {
		case JSONReader::JT_ArrayEnd:	// ']'
			array->autorelease();
			return array;
		case JSONReader::JT_ObjectBegin:
			value = readObject(reader);
			break;
		case JSONReader::JT_ArrayBegin:
			value = readArray(reader);
			break;
		case JSONReader::JT_String:
			value = new JSONString(reader->getRawValue());
			break;
		case JSONReader::JT_Number:
			value = new JSONNumber(reader->getRawValue());
			break;
		case JSONReader::JT_True:
			value = JSONBool::True();
			break;
		case JSONReader::JT_False:
			value = JSONBool::False();
			break;
		case JSONReader::JT_Null:
			value = JSONNull::Instance();
			break;
		case JSONReader::JT_Error:
			return NULL;
		default:
			reader->setLastError(JRE_UnexpectedToken);
			LOGE(reader->getLastErrorMessage());
			return NULL;
		}

		// add child value
		if (value == NULL) {
			return NULL;
		}
		array->addElement(value);

		// separated by ','.
		switch (reader->next()) {
		case JSONReader::JT_ArraySeparator:	// ','
			break;
		case JSONReader::JT_ArrayEnd:	// ']'
			array->autorelease();
			return array;
		case JSONReader::JT_Error:
			return NULL;
		default:
			reader->setLastError(JRE_UnexpectedToken);
			LOGE(reader->getLastErrorMessage());
			return NULL;
		}
	}
	BFX_ASSERT(false);
	return NULL;
}

bool JSONNode::toJSONWriter(JSONWriter* writer, JSONNode* node) {
	BFX_ASSERT(writer != NULL);
	BFX_ASSERT(node != NULL);
	if (node->getNodeType() != JSONNode::JNT_Object) {
		writer->setLastError(JWE_MissingRoot);
		LOGE(writer->getLastErrorMessage());
		return false;
	}
	bool result = writeObject(writer, (JSONObject*) node);
	if (result) {
		writer->writeNewLine();
		if (writer->getLastError() != JWE_NoError)
			return false;
	}
	return result;
}

bool JSONNode::writeObject(JSONWriter* writer, JSONObject* object) {
	LOGI("JSONNode::writeObject(...)");

	// write the the leading '{' character.
	writer->writeObjectBegin();
	writer->writeNewLine();
	if (writer->getLastError() != JWE_NoError)
		return false;

	writer->pushIndent();
	// write all members
	for (int i = 0; i < object->getPropertyCount(); i++) {
		if (i > 0) {
			// each member separated by ',' character.
			writer->writeArraySeparator();
			writer->writeNewLine();
			if (writer->getLastError() != JWE_NoError)
				return false;
		}

		const JSONObject::Entry& kv = object->getPropertyEntry(i);
		const String& key = kv.name;
		JSONNode* value = kv.value;
		if (key.isEmpty()) {
			// TODO set error empty name
			return false;
		}
		BFX_ASSERT(value != NULL);

		// write the member key and followed ':' character.
		writer->writeIndent();
		writer->writeString(key);
		writer->writeSpace();
		writer->writeMemberSeparator();
		if (writer->getLastError() != JWE_NoError)
			return false;

		// write the member value
		writer->writeSpace();
		switch (value->getNodeType()) {
		case JSONNode::JNT_Object:
			writeObject(writer, (JSONObject*) value);
			break;
		case JSONNode::JNT_Array:
			writeArray(writer, (JSONArray*) value);
			break;
		case JSONNode::JNT_String:
			writer->writeString(((JSONString*) value)->getValue());
			break;
		case JSONNode::JNT_Number:
			writer->writeNumber(((JSONNumber*) value)->getRawValue());
			break;
		case JSONNode::JNT_Bool:
			writer->writeBool(((JSONBool*) value)->getValue());
			break;
		case JSONNode::JNT_Null:
			writer->writeNull();
			break;
		default:
			LOGI("We should never get to this point.");
			BFX_ASSERT(false);
		}
		if (writer->getLastError() != JWE_NoError)
			return false;
	}
	writer->popIndent();

	// write the tailing '}' character.
	writer->writeNewLine();
	writer->writeIndent();
	writer->writeEndObject();

	return (writer->getLastError() == JWE_NoError) ? true : false;
}

bool JSONNode::writeArray(JSONWriter* writer, JSONArray* array) {
	LOGI("JSONNode::writeArray(...)");

	// write the the leading '[' character.
	writer->writeStartArray();
	writer->writeNewLine();
	if (writer->getLastError() != JWE_NoError)
		return false;

	writer->pushIndent();
	// write all elements
	for (int i = 0; i < array->getElementCount(); i++) {
		if (i > 0) {
			// each element separated by ',' character.
			writer->writeArraySeparator();
			writer->writeNewLine();
		}
		if (writer->getLastError() != JWE_NoError)
			return false;

		JSONNode* value = array->getElement(i);
		BFX_ASSERT(value != NULL);

		// write the element value
		writer->writeIndent();
		switch (value->getNodeType()) {
		case JSONNode::JNT_Object:
			writeObject(writer, (JSONObject*) value);
			break;
		case JSONNode::JNT_Array:
			writeArray(writer, (JSONArray*) value);
			break;
		case JSONNode::JNT_String:
			writer->writeString(((JSONString*) value)->getValue());
			break;
		case JSONNode::JNT_Number:
			writer->writeNumber(((JSONNumber*) value)->getRawValue());
			break;
		case JSONNode::JNT_Bool:
			writer->writeBool(((JSONBool*) value)->getValue());
			break;
		case JSONNode::JNT_Null:
			writer->writeNull();
			break;
		default:
			LOGI("We should never get to this point.");
			BFX_ASSERT(false);
		}
		if (writer->getLastError() != JWE_NoError)
			return false;
	}
	writer->popIndent();

	// write the tailing ']' character.
	writer->writeNewLine();
	writer->writeIndent();
	writer->writeEndArray();

	return (writer->getLastError() == JWE_NoError) ? true : false;
}

bool JSONArray::isPrimitive() const {
	return false;
}

JSONNode::JSONNodeType JSONArray::getNodeType() const {
	return JNT_Array;
}

void JSONArray::addElement(JSONNode* value) {
	_elements.add(value);
}
int JSONArray::getElementCount() const {
	return _elements.getSize();
}
JSONNode* JSONArray::getElement(int index) const {
	BFX_ASSERT(index < getElementCount());
	return _elements.getAt(index);
}

bool JSONObject::isPrimitive() const {
	return false;
}

JSONNode::JSONNodeType JSONObject::getNodeType() const {
	return JNT_Object;
}

void JSONObject::setProperty(const String& name, JSONNode* value) {
	for (int i = 0; i < _properties.getSize(); i ++) {
		Entry& pair = const_cast<Entry&>(_properties.getAt(i));
		if (pair.name == name) {
			pair.value = value;
			return;
		}
	}
	Entry pair = { name, value };
	_properties.add(pair);
}
int JSONObject::getPropertyCount() const {
	return _properties.getSize();
}

const JSONObject::Entry& JSONObject::getPropertyEntry(int index) const {
	BFX_ASSERT(index < getPropertyCount());
	return _properties.getAt(index);
}

JSONNode* JSONObject::getProperty(const String& key) const {
	for (int i = 0; i < _properties.getSize(); i++) {
		const Entry& kv = _properties.getAt(i);
		if (kv.name == key)
			return kv.value;
	}
	return NULL;
}

bool JSONPrimitive::isPrimitive() const {
	return true;
}

JSONString::JSONString(const String& value) {
	_value = value;
}

JSONNode::JSONNodeType JSONString::getNodeType() const {
	return JNT_String;
}

String JSONString::getValue() const {
	return _value;
}

JSONNumber::JSONNumber(int32_t value) {
	_rawValue = String::format("%d", value);
}
JSONNumber::JSONNumber(uint32_t value) {
	_rawValue = String::format("%u", value);
}
JSONNumber::JSONNumber(int64_t value) {
	_rawValue = String::format("%lld", value);
}
JSONNumber::JSONNumber(uint64_t value) {
	_rawValue = String::format("%llu", value);
}
JSONNumber::JSONNumber(float value) {
	_rawValue = String::format("%f", value);
}
JSONNumber::JSONNumber(double value) {
	_rawValue = String::format("%f", value);
}

JSONNumber::JSONNumber(const String& rawValue) {
	if (rawValue.isEmpty())
		_rawValue = "0";
	else
		_rawValue = rawValue;
}

JSONNode::JSONNodeType JSONNumber::getNodeType() const {
	return JSONNode::JNT_Number;
}

String JSONNumber::getRawValue() const {
	return _rawValue;
}

int32_t JSONNumber::getInt32Value() const {
	return (int32_t) getUInt64Value();

}
uint32_t JSONNumber::getUInt32Value() const {
	return (uint32_t) getUInt64Value();
}

int64_t JSONNumber::getInt64Value() const {
	return (int64_t) getUInt64Value();
}
uint64_t JSONNumber::getUInt64Value() const {
	const char *ptr = _rawValue.cstr();

	while (isspace(*ptr)) {
		ptr++;
	}
	if ('-' == *ptr) {
		return (uint64_t) atoll(ptr);
	} else {
		return strtoul(ptr, 0, 10);
	}
}

float JSONNumber::getFloatValue() const {
	return (float) getDoubleValue();
}
double JSONNumber::getDoubleValue() const {
	const char *ptr = _rawValue.cstr();

	while (isspace(*ptr)) {
		ptr++;
	}
	return strtod(ptr, NULL);
}

JSONBool::JSONBool(bool value) {
	_value = value;
}

JSONNode::JSONNodeType JSONBool::getNodeType() const {
	return JSONNode::JNT_Bool;
}

bool JSONBool::getValue() const {
	return _value;
}

JSONBool* JSONBool::True() {
	static JSONBool __trueValue(true);	// static allocation
	if (__trueValue.getRefCount() < 1) {
		__trueValue.addRef();
	}
	return &__trueValue;
}

JSONBool* JSONBool::False() {
	static JSONBool __falseValue(false); // static allocation
	if (__falseValue.getRefCount() < 1) {
		__falseValue.addRef();
	}
	return &__falseValue;
}

JSONNull::JSONNull() {
}

JSONNode::JSONNodeType JSONNull::getNodeType() const {
	return JSONNode::JNT_Null;
}

JSONNull* JSONNull::Instance() {
	static JSONNull _instance; // static allocation
	if (_instance.getRefCount() < 1) {
		_instance.addRef();
	}
	return &_instance;
}
