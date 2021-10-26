//
// Created by Sandrino Becirbegovic on 26.10.21.
//

#include "boost_basic_client.h"
#include <iostream>

boost_basic_client::boost_basic_client( CommunicationConfig_t commConfig_, std::string& defaultMimeType_)
                                       : name{ commConfig_.host }, commConfig{ std::move( commConfig_ )}
{

}

boost_basic_client::BCSendMessageReturn_t
boost_basic_client::sendMessage(const boost::beast::http::verb &method, const std::string &resource,
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

    // Receive the HTTP response
    http::read(stream, buffer, res);

    // Write the message to standard out
    //std::cout << res << std::endl;

    return {true , res.result() , res.body() };
}

//-------------------------------------------------------------------------------
void boost_basic_client::connect()
{
    // If http:://www.foo-bar.com is needed use resolver boost::asio
    //socket.connect( boost::asio::ip::tcp::endpoint( boost::asio::ip::address::from_string("127.0.0.1"), 1234 ));
    error_code error;


    //boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve("www.footer.free.beeceptor.com", "https", error);

    // Look up the domain name
    auto const results = resolver.resolve(commConfig.host, std::to_string(commConfig.port));
    //boost::asio::connect(socket,endpoints.begin(),endpoints.end(), error);
    stream.connect(results);
}

boost_basic_client::~boost_basic_client()
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

