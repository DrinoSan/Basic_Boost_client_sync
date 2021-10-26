#include <iostream>
#include "boost_basic_client.h"

int main()
{

    struct CommunicationConfig_t comm = { 80, "www.foobartester.free.beeceptor.com", 11, "https://foobartester.free.beeceptor.com"};
    std::cout << "Hello, World!" << std::endl;
    std::string mime{"application/json"};
    boost_basic_client client(comm, mime);
    client.connect();

    std::string resource{"RESOURCE"};
    std::string content{"CONTENT"};
    boost_basic_client::BCSendMessageReturn_t x = client.sendMessage(boost::beast::http::verb::post, resource, content, mime);
    std::cout << x.responseContent << std::endl;


    return 0;
}
