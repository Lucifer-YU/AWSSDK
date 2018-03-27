/*
 * JSONNode.h
 *
 *   Created on: Dec 13, 2014
 *      Author: Lucifer
 */

#ifndef __TEST_JSON_JSONNODE_H_
#define __TEST_JSON_JSONNODE_H_

class JSONObject;
class JSONArray;

/// Represents a single node in the JSON document.
class JSONNode: public REFObject {
public:
	/// Specifies the type of JSON node.
	enum JSONNodeType {
		// Complex types
		JNT_Object,	/// JSON object
		JNT_Array,	/// JSON array'
		// Primitive types
		JNT_String,	/// JSON string value
		JNT_Number,	/// JSON numeric value
		JNT_Bool,	/// JSON boolean value
		JNT_Null	/// JSON null value
	};
public:
	/// Gets a value indicating whether the Type is one of the JSON primitive types.
	virtual bool isPrimitive() const = 0;
	/// Specifies the type of JSON node
	virtual JSONNodeType getNodeType() const = 0;

	/// Reads the specified JSONReader to build a node
	static JSONNode* fromJSONReader(JSONReader* reader);
	/// Saves the given node to the specified JSONWriter
	static bool toJSONWriter(JSONWriter* writer, JSONNode* node);

private:
	static JSONObject* readObject(JSONReader* reader);
	static JSONArray* readArray(JSONReader* reader);

	static bool writeObject(JSONWriter* writer, JSONObject* object);
	static bool writeArray(JSONWriter* writer, JSONArray* array);
};

/// Represents a JSON array.
class JSONArray : public JSONNode {
public:
	virtual bool isPrimitive() const;
	virtual JSONNodeType getNodeType() const;

	/// Adds the specified node to the end of the list of element nodes
	void addElement(JSONNode* value);
	/// Gets the total count of the list of element nodes
	int getElementCount() const;
	/// Gets the node in the list of element nodes with the specified index
	JSONNode* getElement(int index) const;

private:
	ArrayListT<REF<JSONNode> > _elements;
};

/// Represents a JSON object.
class JSONObject : public JSONNode {
public:
	/// A structure to stores both name / value of a JSON property.
	struct Entry {
		String name;
		REF<JSONNode> value;
	};
public:
	virtual bool isPrimitive() const;
	virtual JSONNodeType getNodeType() const;

	/// Adds a pair of name / value to properties
	void setProperty(const String& key, JSONNode* value);
	/// Gets the total count of properties
	int getPropertyCount() const;
	/// Gets the entry for the property with the specified index
	const Entry& getPropertyEntry(int index) const;
	/// Gets the value for the property with the specified name
	JSONNode* getProperty(const String& key) const;

private:
	ArrayListT<Entry> _properties;
};

/// Represents the base class of all JSON primitive types.
class JSONPrimitive : public JSONNode {
public:
	virtual bool isPrimitive() const;
};

/// Represents a JSON string value.
class JSONString : public JSONPrimitive {
public:
	/// Creates a new instance by specified string value.
	JSONString(const String& value);

	virtual JSONNodeType getNodeType() const;
	/// Gets the string value
	String getValue() const;

private:
	String _value;
};

/// Represents a JSON numeric value.
class JSONNumber : public JSONPrimitive {
public:
	/// Creates a new instance by specified 32 bits integer value.
	JSONNumber(int32_t value);
	/// Creates a new instance by specified 32 bits unsigned integer value.
	JSONNumber(uint32_t value);
	/// Creates a new instance by specified 64 bits integer value.
	JSONNumber(int64_t value);
	/// Creates a new instance by specified 64 bits unsigned integer value.
	JSONNumber(uint64_t value);
	/// Creates a new instance by specified float value.
	JSONNumber(float value);
	/// Creates a new instance by specified double float value.
	JSONNumber(double value);
	/// Creates a new instance by specified raw string value.
	JSONNumber(const String& rawValue);

	virtual JSONNodeType getNodeType() const;
	/// Gets the raw string value
	String getRawValue() const;

	/// Gets the value, in 32 bits integer
	int32_t getInt32Value() const;
	/// Gets the value, in 32 bits unsigned integer
	uint32_t getUInt32Value() const;

	/// Gets the value, in 64 bits integer
	int64_t getInt64Value() const;
	/// Gets the value, in 64 bits unsigned integer
	uint64_t getUInt64Value() const;

	/// Gets the value, in float
	float getFloatValue() const;
	/// Gets the value, in double float
	double getDoubleValue() const;

private:
	String _rawValue;
};

/// Represents a JSON boolean value.
class JSONBool : public JSONPrimitive {
public:
	/// Creates a new instance by specified boolean value
	JSONBool(bool value);

	virtual JSONNodeType getNodeType() const;
	/// Gets the value
	bool getValue() const;

	/// Returns the pre-created boolean true JSON value
	static JSONBool* True();
	/// Returns the pre-created boolean false JSON value
	static JSONBool* False();

private:
	bool _value;
};

/// Represents a JSON null value.
class JSONNull : public JSONPrimitive {
public:
	/// Creates a new instance, should use JSONNull::Instance() to instead.
	JSONNull();

	virtual JSONNodeType getNodeType() const;

	/// Returns the pre-created JSON null value
	static JSONNull* Instance();
};

#endif /* __TEST_JSON_JSONNODE_H_ */
