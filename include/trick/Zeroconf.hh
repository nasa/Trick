
/*
    PURPOSE:
        (Test Zeroconf publishing.)
*/

#ifndef ZEROCONF_HH
#define ZEROCONF_HH

#include <string>
#ifdef HAVE_ZEROCONF
#if __linux__
#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-common/simple-watch.h>
#endif
#if __APPLE__
#include <dns_sd.h>
#endif
#endif

namespace Trick {

    class Zeroconf {
        public:
            Zeroconf() ;
            ~Zeroconf() {} ;

            int init() ;

            std::string get_name() ;
            void set_name(std::string) ;

            std::string get_type() ;
            void set_type(std::string) ;

        protected:
            std::string name ;
            std::string type ;
#ifdef HAVE_ZEROCONF
#if __linux__
            AvahiSimplePoll *simple_poll ;
            AvahiClient *client ;
            AvahiEntryGroup *group ;
#endif
#if __APPLE__
            DNSServiceRef dns_service_ref ;
#endif
#endif

    } ;

}

#endif
