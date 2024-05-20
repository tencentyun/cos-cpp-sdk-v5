#include "gtest/gtest.h"
#include "response/bucket_resp.h"
#include "response/data_process_resp.h"
#include "response/service_resp.h"

namespace qcloud_cos {

TEST(BucketRespTest, BucketDomainTest) {
    {
        std::string body;
        body += "<Error>\n";
        body += "    <Code>Code</Code>\n";
        body += "    <Message>Message</Message>\n";
        body += "    <RequestId>RequestId</RequestId>\n";
        body += "    <TraceId>TraceId</TraceId>\n";
        body += "</Error>";
        PutBucketDomainResp resp;
        resp.ParseFromXmlString(body);
        ASSERT_EQ(resp.GetDomainErrorMsg().GetCode(), "Code");
        ASSERT_EQ(resp.GetDomainErrorMsg().GetMessage(), "Message");
        ASSERT_EQ(resp.GetDomainErrorMsg().GetRequestid(), "RequestId");
        ASSERT_EQ(resp.GetDomainErrorMsg().GetTraceid(), "TraceId");
    }
    {
        std::string body;
        body += "<DomainConfiguration>\n";
        body += "    <DomainRule>\n";
        body += "        <Status>Status</Status>\n";
        body += "        <Name>Name</Name>\n";
        body += "        <Type>Type</Type>\n";
        body += "    </DomainRule>\n";
        body += "</DomainConfiguration>";
        GetBucketDomainResp resp;
        resp.ParseFromXmlString(body);
        ASSERT_EQ(resp.GetStatus(), "Status");
        ASSERT_EQ(resp.GetName(), "Name");
        ASSERT_EQ(resp.GetType(), "Type");
    }
    {
        GetBucketInventoryResp resp;
        std::string body;
        body += "<InventoryConfiguration>\n";
        body += "    <Destination>\n";
        body += "        <COSBucketDestination>\n";
        body += "            <Format>CSV</Format>\n";
        body += "            <AccountId>1250000000</AccountId>\n";
        body += "            <Bucket>qcs::cos:ap-guangzhou::examplebucket-1250000000</Bucket>\n";
        body += "            <Prefix>list1</Prefix>\n";
        body += "            <Encryption>\n";
        body += "                <SSE-COS></SSE-COS>\n";
        body += "            </Encryption>\n";
        body += "        </COSBucketDestination>\n";
        body += "    </Destination>\n";
        body += "    <Schedule>\n";
        body += "        <Frequency>Daily</Frequency>\n";
        body += "    </Schedule>\n";
        body += "    <Filter>\n";
        body += "         <Prefix>myPrefix</Prefix>\n";
        body += "    </Filter>\n";
        body += "    <OptionalFields>\n";
        body += "        <Field>Size</Field>\n";
        body += "        <Field>LastModifiedDate</Field>\n";
        body += "        <Field>ETag</Field>\n";
        body += "        <Field>StorageClass</Field>\n";
        body += "        <Field>IsMultipartUploaded</Field>\n";
        body += "        <Field>ReplicationStatus</Field>\n";
        body += "    </OptionalFields>\n";
        body += "    <IncludedObjectVersions>All</IncludedObjectVersions>\n";
        body += "    <Id>Id</Id>\n";
        body += "    <IsEnabled>true</IsEnabled>\n";
        body += "</InventoryConfiguration>";
        resp.ParseFromXmlString(body);
        ASSERT_EQ(resp.GetInventory().GetId(), "Id");
        ASSERT_TRUE(resp.GetInventory().GetIsEnable());
        ASSERT_EQ(resp.GetInventory().GetIncludedObjectVersions(), "All");
        ASSERT_TRUE(resp.GetInventory().GetOptionalFields().GetIsETag());
        ASSERT_TRUE(resp.GetInventory().GetOptionalFields().GetIsSize());
        ASSERT_TRUE(resp.GetInventory().GetOptionalFields().GetIsLastModified());
        ASSERT_TRUE(resp.GetInventory().GetOptionalFields().GetIsStorageClass());
        ASSERT_TRUE(resp.GetInventory().GetOptionalFields().GetIsMultipartUploaded());
        ASSERT_TRUE(resp.GetInventory().GetOptionalFields().GetIsReplicationStatus());
        ASSERT_EQ(resp.GetInventory().GetFilter(), "myPrefix");
        ASSERT_EQ(resp.GetInventory().GetFrequency(), "Daily");
        ASSERT_EQ(resp.GetInventory().GetCOSBucketDestination().GetAccountId(), "1250000000");
        ASSERT_EQ(resp.GetInventory().GetCOSBucketDestination().GetBucket(), "qcs::cos:ap-guangzhou::examplebucket-1250000000");
        ASSERT_TRUE(resp.GetInventory().GetCOSBucketDestination().GetEncryption());
        ASSERT_EQ(resp.GetInventory().GetCOSBucketDestination().GetFormat(), "CSV");
        ASSERT_EQ(resp.GetInventory().GetCOSBucketDestination().GetPrefix(), "list1");
    }

}

TEST(BucketRespTest, GetServiceTestRespTest) {
    {
        std::string body;
        body += "<ListAllMyBucketsResult>";
	    body += "<Owner>";
		body += "<ID>qcs::cam::uin/123:uin/123</ID>";
		body += "<DisplayName>123</DisplayName>";
        body += "<Unknown>123</Unknown>";
        body += "</Owner>";
        body += "<Marker/>";
        body += "<NextMarker/>";
        body += "<IsTruncated>false</IsTruncated>";
        body += "<Buckets>";
		body += "<Bucket>";
        body += "<Name>0-a-123</Name>";
        body += "<Location>ap-nanjing</Location>";
        body += "<CreationDate>2022-05-29T08:40:26Z</CreationDate>";
        body += "<BucketType>cos</BucketType>";
		body += "</Bucket>";
		body += "<Bucket>";
        body += "<Name>0001-230727-123</Name>";
        body += "<Location>ap-nanjing</Location>";
        body += "<CreationDate>2023-07-27T03:03:07Z</CreationDate>";
        body += "<BucketType>cos</BucketType>";
		body += "</Bucket>";
		body += "<Bucket>";
        body += "<Name>0001-batchtarget-123</Name>";
        body += "<Location>ap-nanjing</Location>";
        body += "<CreationDate>2023-09-26T06:54:56Z</CreationDate>";
        body += "<BucketType>cos</BucketType>";
		body += "</Bucket>";
		body += "<Bucket>";
        body += "<Name>0001-bigdata-123</Name>";
        body += "<Location>ap-nanjing</Location>";
        body += "<CreationDate>2024-04-22T08:07:30Z</CreationDate>";
        body += "<BucketType>cos</BucketType>";
		body += "</Bucket>";
        body += "<Unknown>123</Unknown>";
	    body += "</Buckets>";
        body += "</ListAllMyBucketsResult>";
        GetServiceResp resp;
        resp.ParseFromXmlString(body);
        const qcloud_cos::Owner& owner = resp.GetOwner();
        ASSERT_EQ(owner.m_id,"qcs::cam::uin/123:uin/123");
        ASSERT_EQ(owner.m_display_name,"123");
        const std::vector<qcloud_cos::Bucket>& buckets = resp.GetBuckets();
        std::vector<qcloud_cos::Bucket>::const_iterator itr = buckets.begin();
        ASSERT_EQ(itr->m_name, "0-a-123");
        ASSERT_EQ(itr->m_location, "ap-nanjing");
        ASSERT_EQ(itr->m_create_date, "2022-05-29T08:40:26Z");
        itr++;
        ASSERT_EQ(itr->m_name, "0001-230727-123");
        ASSERT_EQ(itr->m_location, "ap-nanjing");
        ASSERT_EQ(itr->m_create_date, "2023-07-27T03:03:07Z");
        itr++;
        ASSERT_EQ(itr->m_name, "0001-batchtarget-123");
        ASSERT_EQ(itr->m_location, "ap-nanjing");
        ASSERT_EQ(itr->m_create_date, "2023-09-26T06:54:56Z");
        itr++;
        ASSERT_EQ(itr->m_name, "0001-bigdata-123");
        ASSERT_EQ(itr->m_location, "ap-nanjing");
        ASSERT_EQ(itr->m_create_date, "2024-04-22T08:07:30Z");
    }
    //异常情况
    {
        GetServiceResp resp;
        bool result = resp.ParseFromXmlString("xsxsxxxs");
        ASSERT_TRUE(!result);
    }
    {
        std::string body = "<ListAllMyBucketsResulterror>error</ListAllMyBucketsResulterror>";
        GetServiceResp resp;
        bool result = resp.ParseFromXmlString(body);
        ASSERT_TRUE(!result);
    }
}

TEST(BucketRespTest, GetBucketLifecycleRespTest) {
    {
        std::string body;
        body += "<LifecycleConfiguration>";
        body += "	<Rule>";
        body += "		<ID>lifecycle_rule00</ID>";
        body += "		<Status>Enabled</Status>";
        body += "		<Prefix>test</Prefix>";
        body += "		<Filter>";
        body += "			<Prefix>sevenyou_e0</Prefix>";
        body += "		</Filter>";
        body += "		<Expiration>";
        body += "			<Days>31</Days>";
        body += "			<Date>60</Date>";
        body += "           <Unknown>sevenyou_e1</Unknown>";
        body += "			<ExpiredObjectDeleteMarker>true</ExpiredObjectDeleteMarker>";
        body += "		</Expiration>";
        body += "		<Transition>";
        body += "			<Days>30</Days>";
        body += "			<StorageClass>STANDARD_IA</StorageClass>";
        body += "		</Transition>";
        body += "		<NoncurrentVersionTransition>";
        body += "           <Unknown>sevenyou_e1</Unknown>";
        body += "			<NoncurrentDays>5</NoncurrentDays>";
        body += "			<StorageClass>STANDARD_IA</StorageClass>";
        body += "		</NoncurrentVersionTransition>";
        body += "		<NoncurrentVersionExpiration>";
        body += "			<NoncurrentDays>8</NoncurrentDays>";
        body += "           <Unknown>sevenyou_e1</Unknown>";
        body += "		</NoncurrentVersionExpiration>";
        body += "		<AbortIncompleteMultipartUpload>";
        body += "			<DaysAfterInitiation>3</DaysAfterInitiation>";
        body += "           <Unknown>sevenyou_e1</Unknown>";
        body += "		</AbortIncompleteMultipartUpload>";
        body += "       <Unknown>sevenyou_e1</Unknown>";
        body += "	</Rule>";
        body += "	<Rule>";
        body += "		<ID>lifecycle_rule01</ID>";
        body += "		<Status>Enabled</Status>";
        body += "		<Filter>";
        body += "			<Prefix>sevenyou_e1</Prefix>";
        body += "           <And>";
        body += "               <Prefix>sevenyou_e1</Prefix>";
        body += "               <Tag>";
        body += "                   <Key>sevenyou_e1</Key>";
        body += "                   <Value>sevenyou_e1</Value>";
        body += "                   <Unknown>sevenyou_e1</Unknown>";
        body += "               </Tag>";
        body += "               <Unknown>sevenyou_e1</Unknown>";
        body += "           </And>";
        body += "           <Tag>";
        body += "               <Key>sevenyou_e1</Key>";
        body += "               <Value>sevenyou_e1</Value>";
        body += "               <Unknown>sevenyou_e1</Unknown>";
        body += "           </Tag>";
        body += "		</Filter>";
        body += "		<Transition>";
        body += "			<Days>60</Days>";
        body += "			<StorageClass>STANDARD</StorageClass>";
        body += "			<Date>60</Date>";
        body += "           <Unknown>sevenyou_e1</Unknown>";
        body += "		</Transition>";
        body += "	</Rule>";
        body += "	<Rule>";
        body += "		<ID>lifecycle_rule02</ID>";
        body += "		<Status>Disabled</Status>";
        body += "		<Filter>";
        body += "			<Prefix>sevenyou_e2</Prefix>";
        body += "		</Filter>";
        body += "		<Transition>";
        body += "			<Days>30</Days>";
        body += "			<StorageClass>STANDARD_IA</StorageClass>";
        body += "		</Transition>";
        body += "	</Rule>";
        body += "   <Unknown>sevenyou_e1</Unknown>";
        body += "</LifecycleConfiguration>";
        GetBucketLifecycleResp resp;
        bool result = resp.ParseFromXmlString(body);
        ASSERT_TRUE(result);
    }
    //异常情况
    {
        GetBucketLifecycleResp resp;
        bool result = resp.ParseFromXmlString("xsxsxxxs");
        ASSERT_TRUE(!result);
    }
    {
        std::string body = "<null>error</null>";
        GetBucketLifecycleResp resp;
        bool result = resp.ParseFromXmlString(body);
        ASSERT_TRUE(!result);
    }
}


TEST(BucketRespTest, GetBucketReplicationRespTest) {
    {
        std::string body;
        body += "<ReplicationConfiguration>";
        body += "    <Role>qcs::cam::uin/12345678:uin/12345678</Role>";
        body += "    <Rule>";
        body += "        <Status>Enabled</Status>";
        body += "        <ID>12345678</ID>";
        body += "        <Prefix>test_prefix</Prefix>";
        body += "        <Destination>";
        body += "            <Bucket>testbucket-1250000000</Bucket>";
        body += "            <StorageClass>Standard</StorageClass>";
        body += "            <Unknown>sevenyou_e1</Unknown>";
        body += "        </Destination>";
        body += "        <Unknown>sevenyou_e1</Unknown>";
        body += "    </Rule>";
        body += "    <Unknown>sevenyou_e1</Unknown>";
        body += "</ReplicationConfiguration>";

        GetBucketReplicationResp resp;
        bool result = resp.ParseFromXmlString(body);
        ASSERT_TRUE(result);

        EXPECT_EQ(resp.GetRole(), "qcs::cam::uin/12345678:uin/12345678");

        const ReplicationRule& rule = resp.GetRules()[0];
        EXPECT_TRUE(rule.m_is_enable);
        EXPECT_EQ(rule.m_id, "12345678");
        EXPECT_EQ(rule.m_prefix, "test_prefix");
        EXPECT_EQ(rule.m_dest_bucket, "testbucket-1250000000");
        EXPECT_EQ(rule.m_dest_storage_class, "Standard");
    }
    //异常情况
    {
        GetBucketReplicationResp resp;
        bool result = resp.ParseFromXmlString("xsxsxxxs");
        ASSERT_TRUE(!result);
    }
    {
        std::string body = "<null>error</null>";
        GetBucketReplicationResp resp;
        bool result = resp.ParseFromXmlString(body);
        ASSERT_TRUE(!result);
    }
}

TEST(BucketRespTest, GetBucketRespTest) {
    {
        std::string body;
        body += "<ListBucketResult>";
        body += "<Name>coscppsdkv5ut-111</Name>";
        body += "<Prefix>prefix</Prefix>";
        body += "<Marker/>";
        body += "<CommonPrefixes>";
        body += "   <Prefix>string</Prefix>";
        body += "</CommonPrefixes>";
        body += "<MaxKeys>1000</MaxKeys>";
        body += "<IsTruncated>false</IsTruncated>";
        body += "<Contents>";
        body += "	<Key>prefixA_0</Key>";
        body += "	<LastModified>2024-05-20T06:42:19.000Z</LastModified>";
        body += "	<ETag>&quot;3be03ea31c1d6ce899f419c04cbf1ea9&quot;</ETag>";
        body += "	<Size>9</Size>";
        body += "	<Owner>";
        body += "		<ID>111</ID>";
        body += "		<DisplayName>111</DisplayName>";
        body += "	</Owner>";
        body += "	<StorageClass>STANDARD_IA</StorageClass>";
        body += "   <Unknown>sevenyou_e1</Unknown>";
        body += "</Contents>";
        body += "<Unknown>sevenyou_e1</Unknown>";
        body += "</ListBucketResult>";
        GetBucketResp resp;
        resp.ParseFromXmlString(body);
        ASSERT_EQ(resp.GetName(), "coscppsdkv5ut-111");
        ASSERT_EQ(resp.GetPrefix(), "prefix");
    }
    //异常情况
    {
        GetBucketResp resp;
        bool result = resp.ParseFromXmlString("xsxsxxxs");
        ASSERT_TRUE(!result);
    }
    {
        std::string body = "<null>error</null>";
        GetBucketResp resp;
        bool result = resp.ParseFromXmlString(body);
        ASSERT_TRUE(!result);
    }
}

TEST(BucketRespTest, GetBucketLocationRespTest) {
    {
        std::string body;
        body += "<LocationConstraint>error</LocationConstraint>";
        GetBucketLocationResp resp;
        resp.ParseFromXmlString(body);
        ASSERT_EQ(resp.GetLocation(), "error");
    }
    //异常情况
    {
        GetBucketLocationResp resp;
        bool result = resp.ParseFromXmlString("xsxsxxxs");
        ASSERT_TRUE(!result);
    }
    {
        std::string body = "<null>error</null>";
        GetBucketLocationResp resp;
        bool result = resp.ParseFromXmlString(body);
        ASSERT_TRUE(!result);
    }
}

}  // namespace qcloud_cos