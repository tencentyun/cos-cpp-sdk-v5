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
}

}  // namespace qcloud_cos