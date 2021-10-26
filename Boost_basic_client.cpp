//
// Created by Sandrino Becirbegovic on 26.10.21.
//

#include "Boost_basic_client.h"
#include <iostream>


Boost_basic_client::Boost_basic_client( CommunicationConfig_t commConfig_, std::string& defaultMimeType_)
                                       : name{ commConfig_.host }, commConfig{ std::move( commConfig_ )}
{

}

Boost_basic_client::~Boost_basic_client()
{
    // Gracefully close the socket
    beast::error_code ec;
    stream.socket().shutdown(tcp::socket::shutdown_both, ec);

    // not_connected happens sometimes
    // so don't bother reporting it.
    //
    if(ec && ec != beast::errc::not_connected)
        throw beast::system_error{ec};

    // If we get here then the connection is closed gracefully
}


Boost_basic_client::BCSendMessageReturn_t Boost_basic_client::sendMessage(
        const boost::beast::http::verb &method, const std::string &resource,
        const std::string &content, const std::string &mimeType)
{
    // Set up an HTTP GET request message
    http::request<boost::beast::http::string_body> req{ method, commConfig.target, commConfig.version};
    req.set(http::field::host, commConfig.host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    // Setting contenttype
    if ( ! mimeType.empty() )
        req.set( http::field::content_type, mimeType );

    req.body() = content;
    req.prepare_payload();
    //std::cout << "SAND: " << req.body() << std::endl;

    boost::system::error_code error;
    // Send the HTTP request to the remote host
    http::write(stream, req, error);

    // This buffer is used for reading and must be persisted
    beast::flat_buffer buffer;

    // Declare a container to hold the response
    http::response<http::string_body> res;
    http::response<http::dynamic_body> res2;

    // Receive the HTTP response
    http::read(stream, buffer, res2);
    for( auto const& x : res2 )
    {
        std::cout << x.name_string() << " and " << x.value() << std::endl;
    }

    std::cout << boost::beast::buffers_to_string(res2.body().data()) << std::endl;

    // Write the message to standard out
    std::cout << res.body() << std::endl;

    return {true , res.result() , res.body() };
}

//-------------------------------------------------------------------------------
void Boost_basic_client::connect()
{
    error_code error;
    // Look up the domain name
    auto const results = resolver.resolve(commConfig.host, std::to_string(commConfig.port));
    //boost::asio::connect(socket,endpoints.begin(),endpoints.end(), error);
    stream.connect(results);
}
