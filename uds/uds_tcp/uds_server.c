#include <stdio.h>    
#include <sys/socket.h>    
#include <sys/un.h>    
#include <string.h>
#include <unistd.h>
    
int main(void)  
{    
    int i_listenfd = 0, i_clientfd = 0;       
    struct sockaddr_un addr_server, addr_client;    
    char psz_path[32] = "./server_unixsocket_file";
    int i_caddr_len = sizeof(struct sockaddr_un); 
    int i_saddr_len = sizeof(struct sockaddr_un);    
    char psz_rbuf[32] = {0};    
    char psz_wbuf[32] = "i am server.";
    int i_readlen = 0;
  
    
    if ( ( i_listenfd = socket( AF_UNIX, SOCK_STREAM, 0 ) ) < 0 )  
    {    
        perror( "socket" );
        return -1;
    }
  

    unlink( psz_path );    
   
    memset( &addr_server, 0, sizeof( addr_server ) );    
    addr_server.sun_family = AF_UNIX;
    strncpy( addr_server.sun_path, psz_path, sizeof( addr_server.sun_path ) - 1 );    
  

    if ( bind( i_listenfd, ( struct sockaddr * )&addr_server, i_saddr_len ) < 0 )  
    {
        perror( "bind" );    
        return -1;    
    }    
  
    if ( listen( i_listenfd, 10 ) < 0 )
    {    
        perror( "listen" );
        return -1;
    }    
 
    while(1)
    {
        if ( ( i_clientfd = accept( i_listenfd, ( struct sockaddr * )&addr_client,
            ( socklen_t * )&i_caddr_len ) ) < 0 )  
        {    
            perror("accept");
            return -1;
        }    
        
        printf( "client is: %s\n", addr_client.sun_path );    
      
        if ( ( i_readlen = read( i_clientfd, psz_rbuf, sizeof( psz_rbuf ) - 1 ) ) < 0 )
        {
            perror( "read" );
            return -1;
        }
        psz_rbuf[i_readlen] = '\0';
        printf( "receive msg:%s\n", psz_rbuf );    
      
        if ( write( i_clientfd, psz_wbuf, strlen( psz_wbuf ) + 1 ) < 0 )  
        {    
            perror("write");    
            return -1;    
        }
    }
  
    unlink( psz_path );    
    return 0;    
}   

