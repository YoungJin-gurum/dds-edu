#include <string.h>
#include <time.h>
#include <gurumdds/dcps.h>
#include <gurumdds/typesupport.h>

#include <Hello/MsgTypeSupport.h>

int main(int argc, char** argv) {
	dds_ReturnCode_t ret = dds_RETCODE_OK;
	dds_DomainParticipantFactory* factory = NULL;
	dds_DomainId_t domain_id = 0;
	dds_DomainParticipant* participant = NULL;

	// Get entity of DomainParticipantFactory.
	factory = dds_DomainParticipantFactory_get_instance();
	if(factory == NULL)
		return 1;

	// A default value is used to initialize a QoS of DomainParticipant.
	dds_DomainParticipantQos participant_qos;
	ret = dds_DomainParticipantFactory_get_default_participant_qos(factory, &participant_qos);
	if(ret != dds_RETCODE_OK)
		return 2;
	// Create a DomainParticipant entity.
	participant = dds_DomainParticipantFactory_create_participant(factory, domain_id, &participant_qos, NULL, 0);
	if(participant == NULL)
		return 3;

	// Get type name(Hello.msg)
	const char* type_name = Hello_MsgTypeSupport_get_type_name();
	if(type_name == NULL)
		return 4;
	// Register type with DomainParticipant entity.
	ret = Hello_MsgTypeSupport_register_type(participant, type_name);
	if(ret != dds_RETCODE_OK)
		return 5;

	// A default value is used to initialize a QoS of Topic.
	dds_TopicQos topic_qos;
	ret = dds_DomainParticipant_get_default_topic_qos(participant, &topic_qos);
	if(ret != dds_RETCODE_OK)
		return 6;
	// Create a Topic entity.
	// And specify a Topic name as 'Profile'
	dds_Topic* topic = dds_DomainParticipant_create_topic(participant, "Profile", type_name, &topic_qos, NULL, 0);
	if(topic == NULL)
		return 7;

	// A default value is used to initialize a QoS of Publisher.
	dds_PublisherQos pub_qos;
	ret = dds_DomainParticipant_get_default_publisher_qos(participant, &pub_qos);
	if(ret != dds_RETCODE_OK)
		return 8;
	// Create a Publisher entity.
	dds_Publisher* pub = dds_DomainParticipant_create_publisher(participant, &pub_qos, NULL, 0);
	if(pub == NULL)
		return 9;

	ret = dds_PublisherQos_finalize(&pub_qos);
	if(ret != dds_RETCODE_OK)
		return 10;

	// A default value is used to initialize a QoS of DataWriter.
	dds_DataWriterQos writer_qos;
	ret = dds_Publisher_get_default_datawriter_qos(pub, &writer_qos);
	if(ret != dds_RETCODE_OK)
		return 11;
	// Overwrite QoS of Publisher with Qos of Topic.
	ret = dds_Publisher_copy_from_topic_qos(pub, &writer_qos, &topic_qos);
	if(ret != dds_RETCODE_OK)
		return 12;
	// Create a DataWriter entity.
	dds_DataWriter* dw = dds_Publisher_create_datawriter(pub, topic, &writer_qos, NULL, 0);
	if(dw == NULL)
		return 13;
	ret = dds_DataWriterQos_finalize(&writer_qos);
	if(ret != dds_RETCODE_OK)
		return 14;
	ret = dds_TopicQos_finalize(&topic_qos);
	if(ret != dds_RETCODE_OK)
		return 15;

	// Create a sample
	Hello_Msg* sample = Hello_MsgTypeSupport_alloc();
	if(sample == NULL)
		return 16;

	// Set values for a sample.
    sample->name = dds_strdup("YYJ");
    sample->birth = 921104;
    sample->team = 0x02;

	dds_Time_t delay = { 1, 500 * 1000 * 1000 };
	dds_Time_sleep(&delay);

	// Publish a sample.
	ret = Hello_MsgDataWriter_write(dw, sample, dds_HANDLE_NIL);
	if(ret != dds_RETCODE_OK)
		return 17;

	// Release a sample.
	Hello_MsgTypeSupport_free(sample);

	dds_Time_sleep(&delay);

	// Terminate the GurumDDS middleware.
	// This API will send a messages that left a communication.
	// And Dispose of any resources you used in the GurumDDS.
	dds_DomainParticipantFactory_shutdown();

	return 0;
}
