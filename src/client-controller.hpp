/*
Copyright (c) 2013, David C Horton

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#ifndef __CLIENT_CONTROLLER_H__
#define __CLIENT_CONTROLLER_H__


#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>

#include <sofia-sip/nta.h>
#include <sofia-sip/sip.h>
#include <sofia-sip/sip_status.h>
#include <sofia-sip/sip_protos.h>
#include <sofia-sip/sip_extra.h>

#include "drachtio.h"
#include "client.hpp"
#include "sip-msg-matcher.hpp"

using namespace std ;

namespace drachtio {
    
    class ClientController : public boost::enable_shared_from_this<ClientController>  {
    public:
        ClientController( DrachtioController* pController, string& address, unsigned int port = 8022 ) ;
        ~ClientController() ;
        
		void start_accept() ;
		void threadFunc(void) ;

        void join( client_ptr client ) ;
        void leave( client_ptr client ) ;

        void wants_invites( client_ptr client, const string& matchId, const string& strMatch ) ;


        bool route_request( sip_t const *sip, string& msgId ) ;
        
        void respondToSipRequest( const string& msgId, boost::shared_ptr<JsonMsg> pMsg ) ;

    protected:

        class RequestSpecifier {
        public:
            RequestSpecifier( client_ptr client, const string& matchId ) : m_client(client), m_matchId(matchId) {}
            RequestSpecifier( client_ptr client, const string& matchId, const string& strMatch ) : m_client(client), m_matchId(matchId) {
                if( !strMatch.empty() ) m_matcher = boost::make_shared<SipMsgMatcher>( strMatch ) ;
            }
            ~RequestSpecifier() {}

            client_ptr client() {
                client_ptr p = m_client.lock() ;
                return p ;
            }

            bool matches( sip_t const *sip ) ;

            const string& getMatchId(void) { return m_matchId; }

            boost::shared_ptr<SipMsgMatcher> getMatcher(void)  { return m_matcher; }

        protected:
            client_weak_ptr m_client ;
            string m_matchId ;
            boost::shared_ptr<SipMsgMatcher> m_matcher ;
        } ;
 

    private:
        void accept_handler( client_ptr session, const boost::system::error_code& ec) ;
         void stop() ;

        DrachtioController*         m_pController ;
        boost::thread               m_thread ;
        boost::mutex                m_lock ;

        boost::asio::io_service m_ioservice;
        boost::asio::ip::tcp::endpoint  m_endpoint;
        boost::asio::ip::tcp::acceptor  m_acceptor ;

        typedef boost::unordered_set<client_ptr> set_of_clients ;
        set_of_clients m_clients ;

        typedef boost::unordered_multimap<string,client_weak_ptr> map_of_services ;
        map_of_services m_services ;

        typedef boost::unordered_multimap<string,RequestSpecifier> map_of_request_types ;
        map_of_request_types m_request_types ;


        
    } ;



}  



#endif