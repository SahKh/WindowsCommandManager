#include <CivetServer.h>
#include <CivetWeb.h>


class HtmlHandler : public CivetHandler {
public:
    bool handleGet(CivetServer* server, struct mg_connection* conn) {
        mg_send_http_ok(conn, "text/html", -1); 
        mg_send_file(conn, "server_control.html");  
        return true;  
    }
};
