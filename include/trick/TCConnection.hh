#ifndef TC_CONNECTION_HH
#define TC_CONNECTION_HH

/*
    PURPOSE: ( Encapsulate a connection with TrickComm. )
*/

#include "trick/ClientConnection.hh"
#include "trick/ClientListener.hh"
#include "tc.h"


namespace Trick {
    
    class ClientListener;

    class TCConnection : public ClientConnection {
        public:

            TCConnection ();

            int initialize() override;
            int establish_connection() override;

            int write (const std::string& message) override;
            int write (char * message, int size) override;

            std::string read  (int max_len) override;

            int disconnect () override;
            std::string get_client_tag () override;
            int set_client_tag(std::string tag) override;
            int get_socket();

            int set_block_mode(int block_mode) override;

            // Non-override functions
            int set_error_reporting (bool on);
            void set_listener (ClientListener * listener);

        private:
            TCDevice _device;   /**<  trick_io(**) */
            ClientListener * _listener;
    };
}

#endif