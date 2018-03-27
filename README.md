# AWSSDK

A lightweight AWS message framework.

## Submodules:

### Reference counting GC with trait classes.

```C++
// Make a hashmap be referenceable.
    typedef REFStringMap REFWrapper<HashMap<String, String> >;
    REF<REFStringMap> mapRef = new REFStringMap();
    mapRef->set("key1", "value1");
    mapRef->set("key2", "value2");
    mapRef->set("key3", "value3");
    for (REFStringMap::PENTRY i = mapRef->getFirstEntry(); !!i; i = mapRef->getNextEntry(i)) {
            printf("entry[%s]=%s\n", i->key, i->value);
    }

    // Auto releasepool
    REFAutoreleasePool refPool;
    ...
    Object1* createObject1() {
        REF<Object1> obj1 = new Object1();
        ...
        obj1->autorelease();    // Will not release until refPool->drain()
        return obj1;
    }
```

### JSON library.

```C++
    // Build a json object.
    REF<JSONObject> body = new JSONObject();
    body->setProperty("key", new JSONString("value1"));
    body->setProperty("key2", new JSONNumber(2));
    REF<JSONArray> array = new JSONArray();
    array->addElement(JSONBool::True);
    array->addElement(JSONNull::Instance);
    array->addElement(new JSONNumber(3));
    body->setProperty("array", array);

    // Write to string.
    REF<StringWriter> stringWriter = new StringWriter();
    REF<JSONWriter> jsonWriter = new JSONWriter(_textWriter);
    jsonWriter->setCompactMode(true);
	JSONNode::toJSONWriter(jsonWriter, body);
    printf("%s\n", stringWriter->getString());

    // Read from string
    JSONNode* body1 = JSONNode::fromJSONReader(new JSONReader(new StringReader(stringWriter->getString())));

```
### AWS library
In progress, only SQS got work.

```C++
    // Create SQS client
    REF<AWSClientFactory> factory = new AWSClientFactory();
    factory->setRegion(AWSRegion::getRegion("cn-north-1"));
    SQSClient* client = factory->createSQSClient("my_accessid", "my_secretkey");
    SQSListQueuesResult* lqrst = client->listQueues("my_send_queue_name");

    // Resolve the queue URL.
    String queueUrl = lqrst->getQueueUrls()->getFirst();

    // Send SQS message.
    REF<SQSSendMessageParams> params = new SQSSendMessageParams(queueUrl);
    params->setMessageBody(stringWriter->getString());
    params->getMessageAttributes()->set("key1", "value1");
    SQSSendMessageResult* smrst = client->sendMessage(params);

    // Receive SQS message(s).
    SQSReceiveMessageResult* rmrst = receiveMessage(queueUrl);
    SQSMessageList* list = rmrst->getMessages();

```
