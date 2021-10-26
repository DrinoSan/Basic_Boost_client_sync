//
// Created by Sandrino Becirbegovic on 26.10.21.
//

#pragma once

#include <string>
#include <boost/asio.hpp>
#include <boost/beast.hpp>


using error_code = boost::system::error_code;
namespace http = boost::beast::http;
namespace beast = boost::beast;
namespace net = boost::asio;
using tcp = net::ip::tcp;

struct CommunicationConfig_t
{
    uint32_t        port;
    std::string     host;
    uint32_t        version;
    std::string     target;
};

class boost_basic_client
{

private:
    std::string                         name;
    uint32_t                            port;      // -> CommunicationConfig_t commConfig;
    std::string                         defaultMimeType;
    boost::asio::io_service             ioc;
    boost::asio::ip::tcp::socket        socket { ioc };

    // These objects perform our I/O
    boost::asio::ip::tcp::resolver      resolver{ ioc };
    boost::beast::tcp_stream            stream{ ioc };
    CommunicationConfig_t               commConfig;

public:


    // Constructor
    // Fancy text
    boost_basic_client( CommunicationConfig_t commConfig_, std::string& defaultMimeType_ );

    // Destructor
   ~boost_basic_client() ;

    // Function to send a mesage to the HTTPS - Server
    // @return weird tuple stuff
    struct BCSendMessageReturn_t
    {
        bool                        success;
        boost::beast::http::status  responseStatus;
        std::string                 responseContent;
    };

    BCSendMessageReturn_t sendMessage( const boost::beast::http::verb& method, const std::string& resource,
                                       const std::string& content, const std::string& mimeType);

    void connect();



};

