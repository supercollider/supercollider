#include <boost/test/unit_test.hpp>

#include "utilities/osc_server.hpp"
#include "nova-tt/semaphore.hpp"

using namespace nova;

namespace {
struct osc_test_responder:
    public osc_responder
{
    osc_test_responder(void):
        i(0), sem(0)
    {}

    void run(osc::ReceivedMessageArgumentIterator begin,
             osc::ReceivedMessageArgumentIterator const & end)
    {
        ++i;
        sem.post();
    }

    int i;
    semaphore sem;
};
}

BOOST_AUTO_TEST_CASE( network_thread_test )
{
    network_thread thread;
}

BOOST_AUTO_TEST_CASE( osc_server_test_1 )
{
    osc_server server(12321);
}

BOOST_AUTO_TEST_CASE( osc_server_test_2 )
{
    osc_server server(12321);
    std::unique_ptr<osc_test_responder> resp (new osc_test_responder());
    server.add_responder("/foo", resp.get());
    server.remove_responder("/foo", resp.get());
}

BOOST_AUTO_TEST_CASE( osc_server_test_3 )
{
    std::unique_ptr<osc_test_responder> resp (new osc_test_responder());
    {
        osc_server server(12321);

        server.add_responder("/foo", resp.get());

        {
            using namespace boost::asio::ip;

            boost::asio::io_service io_service;

            udp::resolver resolver(io_service);
            udp::resolver::query query(udp::v4(), "localhost", "12321");
            udp::endpoint receiver_endpoint = *resolver.resolve(query);

            udp::socket socket(io_service);
            socket.open(udp::v4());
            socket.send_to(boost::asio::buffer("/foo\0\0\0", 8), receiver_endpoint);
        }

        resp->sem.wait();

        server.remove_responder("/foo", resp.get());
    }

    BOOST_REQUIRE_EQUAL(resp->i, 1);
}
