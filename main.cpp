#include <iostream>
#include "Boost_basic_client.h"

int main()
{

    struct CommunicationConfig_t comm = { 80, "www.foobartester.free.beeceptor.com", 11, "https://foobartester.free.beeceptor.com"};
    std::string mime{"application/json"};
    Boost_basic_client client(comm, mime);
    client.connect();

    std::string resource{"RESOURCE"};
    std::string content{"CONTENT"};
    Boost_basic_client::BCSendMessageReturn_t x = client.sendMessage(boost::beast::http::verb::get, resource, content, mime);
    //std::cout << x.responseContent << std::endl;


    return 0;
}
