//
// Created by Sandrino Becirbegovic on 26.10.21.
//

#include "Boost_basic_client.h"
#include <iostream>



namespace
{
    //--------------------------------------------------------------------------------------
    /// Function to restart and run an io_context
    void runIOContext( boost::asio::io_context& ioc )
    {
        // Restart the io_context, as it may have been left in the "stopped" state
        // by a previous operation.
        ioc.restart();

        // Block until the asynchronous operation has completed, or timed out.
        ioc.run();
    }


    //--------------------------------------------------------------------------------------
    /// Function to get an stringified identifier of the remote endpoint
    std::string getEndpointName( const one::CommunicationConfig_t& commConfig )
    {
        return commConfig.hostName + ":" + one::toString( commConfig.portNumber );
    }

}

namespace one
{
        
    Boost_basic_client::Boost_basic_client( CommunicationConfig_t commConfig_, const std::string& defaultMimeType_)
                                        : name( commConfig_.hostName ), commConfig( std::move( commConfig_ ))
    {
        connect( std::chrono::seconds(commConfig.messageTimeout ) );
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
        {
            //throw beast::system_error{ec};
        }
          

        // If we get here then the connection is closed gracefully
    }


    Boost_basic_client::BCSendMessageReturn_t Boost_basic_client::sendMessage(
            const boost::beast::http::verb &method, const std::string &resource,
            const std::string &content, const std::string &mimeType)
    {

        
        // Set up an HTTP GET request message
        auto const version = 11;
        boost::beast::http::status  responseStatus;
        std::string                 responseContent;
        http::request<boost::beast::http::string_body> req{ method, resource, version };

        try
        {
            if( stream.socket().is_open() )
            {
                // Set a timeout on the operation
                stream.expires_after( std::chrono::seconds( commConfig.messageTimeout ) );
            }
            else
            {
                connect( std::chrono::seconds( commConfig.messageTimeout ) );
            }


            req.set(http::field::host, commConfig.hostName);
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            // Setting contenttype
            if ( ! mimeType.empty() )
                req.set( http::field::content_type, mimeType );


            req.body() = content;
            req.prepare_payload();
            one::traceRestRequest( LOG_DEBUG, "Send request", req );

            boost::system::error_code error;
            // Send the HTTP request to the remote host
            // Sollte eigentlich gehen
            http::write(stream, req, error);

            // Run the operation until it completes, or until the timeout.
            //runIOContext( ioc );

            // Determine whether request was successfully sent.
            if( error )
            {
                traceError("SAND write failed <%s>", error.message() );
                stream.close();
                throw OSException_t( ERRLOC, ErrorNumbers_T::OS_write,
                                    "Failed to write to " + getEndpointName( commConfig ), error.value(), error.message() );
            }



            // This buffer is used for reading and must be persisted
            beast::flat_buffer buffer;

            // Declare a container to hold the response
            http::response<http::string_body> res;
            http::response<http::dynamic_body> res2;
                
            // Receive the HTTP response
            //http::read(stream, buffer, res);
             http::read( stream, buffer, res2);
             for(auto const& field : res2)
                traceError("SAND FIELD failed <%s>", field.name() );

            // Write the message to standard out
            //std::cout << res << std::endl;
            traceError("SAND write failed <%s>", res.body() );
            traceError("SAND Bytes <%s>", error.message() );

            return { true , res.result() , res.body() };
        }
        catch( const ExceptionBase_t& kxe )
        {
            one::traceRestRequest( LOG_ERR, "HTTP Request failed", req );
            kxe.showError( ERRLOC );
            return { false, responseStatus, responseContent };
        }
        catch( std::exception const& e )
        {
            one::traceRestRequest( LOG_ERR, std::string( "HTTP Request failed <" ) + e.what() + ">", req );
            return { false, responseStatus, responseContent };
        }
    }

    //-------------------------------------------------------------------------------
    void Boost_basic_client::connect( std::chrono::steady_clock::duration timeout )
    {
        error_code error;
        // Look up the domain name
        //boost::asio::ip::tcp::resolver::query query(commConfig.hostName, std::to_string(commConfig.portNumber), tcp::resolver::query::canonical_name);
        //boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
        //boost::asio::ip::tcp::resolver::iterator end;
        //boost::asio::ip::tcp::endpoint endpoint;
        //while ( iter != end)
        //{
        //    endpoint = *iter++;
        //}
        auto endpoints = resolver.resolve( commConfig.hostName, one::toString( commConfig.portNumber ) );
        //auto const results = resolver.resolve(commConfig.hostName, std::to_string(commConfig.portNumber));

        // Set a timeout on the operation
        stream.expires_after( timeout );

        std::string test = "TEST";
        
        //boost::asio::connect(socket,endpoints.begin(),endpoints.end(), error);
        boost::beast::error_code error_conn;
        stream.async_connect( endpoints,
                            [&]( const auto& ec, const auto& /* endpoint unused */ )
                            {
                               error_conn = ec;
                            } );

        runIOContext( ioc );
    }



   

}
