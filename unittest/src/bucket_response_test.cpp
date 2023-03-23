#include "gtest/gtest.h"
#include "response/bucket_resp.h"
#include "response/data_process_resp.h"

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

}  // namespace qcloud_cos